#include "server.h"

/*
 * Read the content send through the header.
 * If successful, return OK. Otherwise, an appropriate code error is returned.
*/

static int			receive_content(int fd, t_http *data, uint8_t *str, ssize_t size)
{
	uint8_t		buff[BUFF_SOCKET];
	uint8_t		*content;
	ssize_t		total;

	total = size;
	if ((data->content = (uint8_t *)malloc(sizeof(uint8_t) * data->content_length)) == NULL)
		return (create_partial_answer(fd, data, INTERNAL_SERVER_ERR));
	bzero(data->content, sizeof(uint8_t) * data->content_length);
	memcpy(data->content, str, size);
	content = data->content + size;
	while (total < data->content_length && (size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		total += size;
		if (total > data->content_length)
			break ;
		memcpy(content, buff, size);
		content = content + size;
	}
	if (total != data->content_length)
		return (create_partial_answer(fd, data, BAD_REQUEST));
	return (202);
}

/*
 * Copy the buffer into a malloced pointer. 
 * If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int			header_malloc(uint8_t *buff, uint8_t **header, uint8_t **end, ssize_t size)
{
	if ((*header = (uint8_t *)malloc(sizeof(uint8_t) * size)) == NULL)
		return (0);
	memcpy(*header, buff, size);
	*end = (uint8_t *)strstr((const char *)*header, "\r\n\r\n");
	bzero(*end, sizeof(uint8_t) * 4);
	*end = *end + 4;	
	return (1);
}

/*
 * Receive the header from the client.
 * If successful, the number of bytes actually read is returned.
 * Upon reading end-of-file, zero is returned. Otherwise, a -1 is returned to indicate an error.
*/

static ssize_t		receive_header(int fd, uint8_t **header, uint8_t **end, int *status)
{
	ssize_t		size;
	uint8_t		buff[BUFF_SOCKET];

	bzero(buff, BUFF_SOCKET);
	if ((size = recv(fd, buff, (BUFF_SOCKET - 1), 0)) > 0)
	{
		if (strstr((const char *)buff, "\r\n\r\n") == NULL)
		{
			*status = create_partial_answer(fd, NULL, ENTITY_TOO_LARGE);
			return (-1);
		}
	}
	else if (size == 0)
		*status = create_partial_answer(fd, NULL, NO_CONTENT);
	else
		*status = create_partial_answer(fd, NULL, REQUEST_TIME_OUT);
	if (header_malloc(buff, header, end, size) == 0)
		*status = create_partial_answer(fd, NULL, INTERNAL_SERVER_ERR);
	if (*status != 200)
		return ((size == 0) ? 0 : -1);
	return (size);
}

/*
 * Build the http header and send the appropriate response to the client.
 * If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

int					receive(int fd, int *status)
{
	ssize_t		size;
	uint8_t		*buf;
	uint8_t		*end;
	t_http		*request;

	buf = NULL;
	if ((size = receive_header(fd, &buf, &end, status)) <= 0)
		return (0);
	// write(1, buf, (end - buf));
	if ((request = header((char *)buf, status)) == NULL)
		return (0);
	if (request && request->content_length)
	{
		if ((*status = receive_content(fd, request, end, size - (end - buf))) != 202)
		{
			free(buf);
			return (0);
		}
	}
	free(buf);
	*status = response(request, fd);
	return (1);
}
