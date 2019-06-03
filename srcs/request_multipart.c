#include "server.h"

#define CACHE_FILENAME "./cache.tmp"

static uint32_t		header_next(uint8_t *str)
{
	uint32_t		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			str[i] = '\0';
			str[i + 1] = '\0';
			return (i + 2);
		}
		++i;
	}
	return (0);
}

static int		get_element(char **str, char *search)
{
	int		i;
	char	*element;
	char	*name;

	if ((element = strstr((const char *)*str, search)) == NULL)
		return (0);
	element = element + strlen(search);
	if (*element == '\"')
		++element;
	i = 0;
	while (element[i] && element[i] != '\"')
		++i;
	name = strndup(element, i);
	printf("NAME: |%s|\n", name);
	free(name);
	if (element[i] == '\"')
		++i;
	*str = element + i;
	return (1);
}

static int		parse_boundary(uint8_t *buff)
{
	char		*str;

	str = (char *)buff;
	if (get_element(&str, "name=") == 0)
		return (0);
	if (*str == '\0')
		return (2);
	if (get_element(&str, "filename=") == 0)
		return (0);
	return (3);
}

static int		read_file(int fd, char *boundary, size_t len_boundary)
{
	size_t		size;
	uint8_t		buff[PAGE_SIZE];
	uint8_t		*line;

	int fd_new = open("file.copy", O_RDWR | O_CREAT | O_TRUNC, 0660);

	memset(buff, 0, PAGE_SIZE);
	while ((size = read(fd, buff, PAGE_SIZE - 1)) > 0)
	{
		if ((line = (uint8_t *)strstr((const char *)buff, boundary)) != NULL)
		{
			write(fd_new, buff, (line - buff) - 2);
			line = line + len_boundary;
			break ;
		}
		else
			write(fd_new, buff, size);
	}
	close(fd_new);
	printf("DONE\n");
	if (line[0] == '-' && line[1] == '-')
		return (1);
	return (1);
}

static int		get_line(int fd, t_http *data)
{
	size_t		size;
	size_t		len_boundary;
	uint8_t		buff[PAGE_SIZE];
	uint8_t		*line;

	len_boundary = strlen(data->boundary);
	memset(buff, 0, PAGE_SIZE);
	while ((size = read(fd, buff, PAGE_SIZE - 1)) > 0)
	{
		if ((line = (uint8_t *)strstr((const char *)buff, "\r\n")) != NULL)
		{
			if (strncmp((const char *)buff, data->boundary, len_boundary) == 0)
			{
				line = line + 2;
				uint32_t i = header_next(line);
				int ret = parse_boundary(line);
				line = line + i;
				while (strncmp((const char *)line, "\r\n", 2) != 0)
					line = line + header_next(line);
				line = line + 2;
				lseek(fd, line - buff, SEEK_SET);
				if (ret == 3)
					read_file(fd, data->boundary, len_boundary);
			}
		}
	}
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
	lseek(fd_out, 0, SEEK_SET);
	get_line(fd_out, data);
	close(fd_out);
	remove(CACHE_FILENAME);
	if (total != data->content_length)
		return (response_error(fd, data, BAD_REQUEST));
	return (ACCEPTED);
}