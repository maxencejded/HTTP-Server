#include "server.h"

#define CACHE_FILENAME "./cache.tmp"

static char		*locate(const char *content, size_t size, const char *needle)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < size)
	{
		if (content[i] == needle[0])
		{
			j = 0;
			while ((i + j) < size && content[i + j] == needle[j])
				++j;
			if (needle[j] == '\0')
				return ((char *)(content + i));
		}
		++i;
	}
	return (NULL);
}

static int		get_element(char **str, char *search, char **key)
{
	int		i;
	char	*element;

	if ((element = strstr((const char *)*str, search)) == NULL)
		return (0);
	element = element + strlen(search);
	if (*element == '\"')
		++element;
	i = 0;
	while (element[i] && element[i] != '\"')
		++i;
	*key = strndup(element, i);
	if (element[i] == '\"')
		++i;
	*str = element + i;
	return (1);
}

static int		parse_boundary(t_http *data, uint8_t *buff, size_t size)
{
	char		*str;
	char		*key;
	char		*value;
	uint8_t		*content;

	str = (char *)buff;
	if ((content = (uint8_t *)locate((const char *)buff, size, "\r\n\r\n")) == NULL)
		return (0);
	memset(content, 0, sizeof(uint8_t) * 4);
	content = content + 4;
	if (get_element(&str, "name=", &key) == 0)
		return (0);
	if (*str == '\0')
	{
		value = strndup((char *)content, (size - (content - buff)));
		if (contentAdd(data, key, value, 0) == 0)
			return (0);
		return (2);
	}
	if (get_element(&str, "filename=", &value) == 0)
		return (0);
	if (contentAdd(data, key, value, 1) == 0)
		return (0);
	int		fd;

	if ((fd = open(value, O_WRONLY | O_CREAT | O_TRUNC, 0660)) == -1)
		return (0);
	write(fd, content, (size - (content - buff)));
	close(fd);
	return (3);
}

static int		isBoundaryEnd(char *ptr)
{
	if (ptr[0] == '-' && ptr[1] == '-')
		return (1);
	return (0);
}

static int		get_line(t_http *data, const uint8_t *ptr, size_t size)
{
	size_t		len;
	uint8_t		*start;
	uint8_t		*end;

	end = (uint8_t *)ptr;
	len = strlen(data->boundary);
	while ((start = (uint8_t *)locate((const char *)end, size, data->boundary)) != NULL)
	{
		if (isBoundaryEnd((char *)start + len) == 1)
			return (1);
		if ((end = (uint8_t *)locate((const char *)start + len, size, data->boundary)) == NULL)
			return (0);
		memset(end - 2, 0, sizeof(uint8_t) * 2);
		parse_boundary(data, start, ((end - 2) - start));
	}
	return (1);
}

static int		request_process(int fd, t_http *data)
{
	const void		*mapped;
	const uint8_t	*ptr;
	size_t			size;

	size = data->content_length;
	if ((mapped = mmap (0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	ptr = (uint8_t *)mapped;
	get_line(data, ptr, size);
	return (1);
}

int				request_multipart(int fd, t_http *data, uint8_t *str, ssize_t size)
{
	uint8_t		buff[PAGE_SIZE];
	size_t		total;
	int			fd_out;

	total = size;
	if ((fd_out = open(CACHE_FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0)) == -1)
		return (INTERNAL_SERVER_ERROR);
	(size != 0) ? write(fd_out, str, size) : 0;
	while (total < data->content_length && (size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		if (total > data->content_length)
			break ;
		write(fd_out, buff, size);
	}
	request_process(fd_out, data);
	close(fd_out);
	remove(CACHE_FILENAME);
	if (total != data->content_length)
		return (response_error(fd, data, BAD_REQUEST));
	return (ACCEPTED);
}