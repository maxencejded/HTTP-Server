#include "server.h"

static int			receive_content(int fd, t_http *data, uint8_t *str, ssize_t size)
{
	uint8_t		buff[BUFF_SOCKET];
	uint8_t		*content;
	ssize_t		total;

	total = size;
	if ((data->content = (uint8_t *)malloc(sizeof(uint8_t) * data->content_length)) == NULL)
	{
		// Send Response: "500 Internal Server Error"
		printf("ERROR: '500 Internal Server Error'\n");
		http_free(data);
		return (500);
	}
	bzero(data->content, sizeof(uint8_t) * data->content_length);
	memcpy(data->content, str, size);
	content = data->content + size;
	while (total < data->content_length && (size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		memcpy(content, buff, size);
		content = content + size;
		total += size;
	}
	if (total != data->content_length)
	{
		// Send Response: "400 Bad Request"
		printf("ERROR: '400 Bad Request'\n");
		http_free(data);
		return (400);
	}
	return (202);
}

static ssize_t		header_malloc(uint8_t *buff, uint8_t **header, uint8_t **end, ssize_t size)
{
	if ((*header = (uint8_t *)malloc(sizeof(uint8_t) * size)) == NULL)
		return (-1);
	memcpy(*header, buff, size);
	*end = (uint8_t *)strstr((const char *)*header, "\r\n\r\n");
	bzero(*end, sizeof(uint8_t) * 4);
	*end = *end + 4;	
	return (size);
}

static ssize_t		receive_header(int fd, uint8_t **header, uint8_t **end, int *status)
{
	ssize_t		size;
	uint8_t		buff[BUFF_SOCKET];

	bzero(buff, BUFF_SOCKET);
	if ((size = recv(fd, buff, (BUFF_SOCKET - 1), 0)) > 0)
	{
		if (strstr((const char *)buff, "\r\n\r\n") == NULL)
		{
			// Send Response: "413 Entity Too Large"
			printf("ERROR: '413 Entity Too Large'\n");
			*status = 413;
			return (-1);
		}	
	}
	else if (size == 0)
	{
		// Send Response: "204 No Content"
		printf("ERROR: '204 No Content'\n");
		*status = 204;
		return (0);
	}
	else
	{
		// Send Response: "408 Request Time-out"
		perror("ERROR: '408 Request Time-out'");
		*status = 408;
		return (-1);
	}
	if ((size = header_malloc(buff, header, end, size)) == -1)
	{
		// Send Response: "500 Internal Server Error"
		printf("ERROR: '500 Internal Server Error'\n");
		*status = 500;
		return (-1);
	}
	return (size);
}

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
	http_free(request);
	return (1);
}
