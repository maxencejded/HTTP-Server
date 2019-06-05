#include "server.h"

/*
** Create a pointer to cache the content form the request
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static size_t	cache(t_http *data, void **mapped, uint8_t *ptr, size_t size)
{
	size_t		total;
	size_t		length;
	uint8_t		*tmp;
	uint8_t		buff[PAGE_SIZE];

	length = data->content_length;
	*mapped = mmap(0, length,
					PROT_READ | PROT_WRITE, 
					MAP_ANON | MAP_PRIVATE,
					-1, 0);
	if (mapped == MAP_FAILED)
		return (0);
	tmp = (uint8_t *)(*mapped);
	(size) ? memcpy(tmp, ptr, size) : 0;
	tmp = (size) ? tmp + size : tmp;
	total = size;
	while (total < length && (size = recv(data->fd, buff, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		memcpy(tmp, buff, size);
		tmp = tmp + size;
	}
	return (total);
}

/*
** Copy the content receive into a cache pointer
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		post_multipart(t_http *data, uint8_t *str, size_t size)
{
	size_t		total;
	size_t		length;
	void		*mapped;
	
	length = data->content_length;
	if ((total = cache(data, &mapped, str, size)) == 0)
		return (response_error(data->fd, data, INTERNAL_SERVER_ERROR));
	if (total == length)
	{
		if (parse_multipart(data, (const uint8_t *)mapped, length) == 0)
		{
			munmap(mapped, length);
			return (response_error(data->fd, data, INTERNAL_SERVER_ERROR));
		}
	}
	munmap(mapped, length);
	if (total != length)
		return (response_error(data->fd, data, INTERNAL_SERVER_ERROR));
	return (ACCEPTED);
}

/*
** Read the content send through the header.
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		content_copy(t_http *data, char *content, size_t total)
{
	char		buf[PAGE_SIZE];
	char		*tmp;
	size_t		size;
	size_t		maximum;

	tmp = content + total;
	maximum = data->content_length;
	while (total < maximum && (size = recv(data->fd, buf, PAGE_SIZE, 0)) > 0)
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

static int		post_form(t_http *data, char *str, size_t size)
{
	char		*content;

	content = (char *)malloc(sizeof(char) * (data->content_length + 1));
	if (content == NULL)
		return (response_error(data->fd, data, INTERNAL_SERVER_ERROR));
	memset(content, 0, sizeof(char) * (data->content_length + 1));
	(size != 0) ? memcpy(content, str, size) : NULL;
	if (content_copy(data, content, size) == 0)
		return (response_error(data->fd, data, BAD_REQUEST));
	if (content_parse(data, content) == 0)
		return (response_error(data->fd, data, INTERNAL_SERVER_ERROR));
	return (ACCEPTED);
}

/*
** Dispatch to the appropriate POST type
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

int				post_request(t_http *data, uint8_t *ptr, size_t size)
{
	if (data->boundary == NULL)
		return (post_form(data, (char *)ptr, size));
	else
		return (post_multipart(data, ptr, size));
}
