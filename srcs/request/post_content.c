#include "server.h"

/*
** Create a pointer to the CACHE_FILE then parse the content
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		request_process(int fd, t_http *data)
{
	size_t			size;
	const void		*mapped;

	size = data->content_length;
	mapped = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED)
		return (0);
	return (parse_multipart(data, (const uint8_t *)mapped, size));
}

/*
** Copy the content receive into a CACHE_FILE then parse the content
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		post_multipart(int fd, t_http *data, uint8_t *str, size_t size)
{
	int			cache;
	size_t		total;
	size_t		length;
	uint8_t		buff[PAGE_SIZE];

	total = size;
	length = data->content_length;
	if ((cache = open(CACHE_FILE, O_RDWR | O_CREAT | O_TRUNC, 0)) == -1)
		return (INTERNAL_SERVER_ERROR);
	(size) ? write(cache, str, size) : 0;
	while (total < length && (size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		write(cache, buff, size);
	}
	if (total == length)
		request_process(cache, data);
	close(cache);
	remove(CACHE_FILE);
	if (total != length)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	return (ACCEPTED);
}

/*
** Read the content send through the header.
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		content_copy(int fd, t_http *data, char *content, size_t total)
{
	char		buf[PAGE_SIZE];
	char		*tmp;
	size_t		size;
	size_t		maximum;

	tmp = content + total;
	maximum = data->content_length;
	while (total < maximum && (size = recv(fd, buf, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		memcpy(tmp, buf, size);
		tmp = tmp + size;
	}
	if (total != maximum)
	{
		free(content);
		return (0);
	}
	return (1);
}

static int		post_form(int fd, t_http *data, char *str, size_t size)
{
	char		*content;

	content = (char *)malloc(sizeof(char) * (data->content_length + 1));
	if (content == NULL)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	memset(content, 0, sizeof(char) * (data->content_length + 1));
	(size != 0) ? memcpy(content, str, size) : NULL;
	if (content_copy(fd, data, content, size) == 0)
		return (response_error(fd, data, BAD_REQUEST));
	if (content_parse(data, content) == 0)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	return (ACCEPTED);
}

/*
** Dispatch to the appropriate POST type
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

int				post_request(int fd, t_http *data, uint8_t *ptr, size_t size)
{
	if (data->boundary == NULL)
		return (post_form(fd, data, (char *)ptr, size));
	else
		return (post_multipart(fd, data, ptr, size));
}
