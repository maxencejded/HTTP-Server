#include "server.h"

/*
** Create a pointer to cache the content form the request
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		request_memory(size_t size, void **mapped, uint8_t **tmp)
{
	*mapped = mmap(0, size,
				PROT_READ | PROT_WRITE,
				MAP_ANON | MAP_PRIVATE,
				-1, 0);
	if (mapped == MAP_FAILED)
		return (0);
	*tmp = (uint8_t *)(*mapped);
	return (1);
}

/*
** Copy the content receive into a cache pointer
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		post_multipart(int fd, t_http *data, uint8_t *str, size_t size)
{
	size_t		total;
	size_t		length;
	uint8_t		buff[PAGE_SIZE];
	uint8_t		*tmp;
	void		*mapped;
	
	total = size;
	length = data->content_length;
	if (request_memory(length, &mapped, &tmp) == 0)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	(size) ? memcpy(tmp, str, size) : 0;
	tmp = (size) ? tmp + size : tmp;
	while (total < length && (size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		memcpy(tmp, buff, size);
		tmp = tmp + size;
	}
	if (total == length)
		if (parse_multipart(data, (const uint8_t *)mapped, length) == 0)
			total = 0;
	munmap(mapped, length);
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
