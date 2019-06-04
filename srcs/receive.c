#include "server.h"

/*
** Read the content send through the header.
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int		request_data(int fd, t_http *data, char *str, ssize_t size)
{
	char		buf[PAGE_SIZE];
	char		*content;
	char		*tmp;
	size_t		total;

	total = size;
	if ((content = (char *)malloc(sizeof(char) * (data->content_length + 1))) == NULL)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	memset(content, 0, sizeof(char) * (data->content_length + 1));
	(size != 0) ? memcpy(content, str, size) : NULL;
	tmp = content + size;
	while (total < data->content_length && (size = recv(fd, buf, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		memcpy(tmp, buf, size);
		tmp = tmp + size;
	}
	if (total != data->content_length)
	{
		free(content);
		return (response_error(fd, data, BAD_REQUEST));
	}
	if (contentParse(data, content) == 0)
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	return (ACCEPTED);
}

/*
** Copy the buffer into a malloced pointer.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		copy(uint8_t *buff, uint8_t **data, uint8_t **end, ssize_t size)
{
	if ((*data = (uint8_t *)malloc(sizeof(uint8_t) * size)) == NULL)
		return (0);
	memcpy(*data, buff, size);
	*end = (uint8_t *)strstr((const char *)*data, "\r\n\r\n");
	memset(*end, 0, sizeof(uint8_t) * 4);
	*end = *end + 4;
	return (1);
}

/*
** Receive the header from the client.
** If successful, the number of bytes actually read is returned.
** Upon reading end-of-file, zero is returned.
** Otherwise, a -1 is returned to indicate an error.
*/

static ssize_t	request_read(int fd, uint8_t **data, uint8_t **end, int *status)
{
	ssize_t		size;
	uint8_t		buff[PAGE_SIZE];

	memset(buff, 0, PAGE_SIZE);
	if ((size = recv(fd, buff, (PAGE_SIZE - 1), 0)) > 0)
	{
		write(1, buff, size);
		if (strstr((const char *)buff, "\r\n\r\n") == NULL)
		{
			*status = response_error(fd, NULL, ENTITY_TOO_LARGE);
			return (-1);
		}
	}
	else
	{
		if (size == 0)
			*status = response_error(fd, NULL, NO_CONTENT);
		else
			*status = response_error(fd, NULL, REQUEST_TIME_OUT);
		return (-1);
	}
	if (copy(buff, data, end, size) == 0)
		*status = response_error(fd, NULL, INTERNAL_SERVER_ERROR);
	if (*status != 200)
		return ((size == 0) ? 0 : -1);
	return (size);
}

/*
** Build the http header and send the appropriate response to the client.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
**
** DEBUG: write(1, buf, (end - buf));
*/

int				receive(int fd, int *status)
{
	ssize_t		size;
	uint8_t		*buf;
	uint8_t		*end;
	t_http		*data;

	buf = NULL;
	if ((size = request_read(fd, &buf, &end, status)) <= 0)
		return (0);
	if ((data = header(fd, (char *)buf, status)) == NULL)
		return (0);
	if (data && data->method == METHOD_POST && data->content_length)
	{
		if (data->boundary)
			*status = request_multipart(fd, data, end, size - (end - buf));
		else
			*status = request_data(fd, data, (char *)end, size - (end - buf));
		if (*status != ACCEPTED)
		{
			free(buf);
			return (0);
		}
	}
	free(buf);
	*status = response(data, fd);
	return (1);
}
