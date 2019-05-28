#include "server.h"

static int			receive_content(int fd, t_http *data, char *str, ssize_t size)
{
	char		buff[BUFF_SOCKET];
	uint8_t		*content;
	ssize_t		total;

	total = size;
	if ((data->content = (uint8_t *)malloc(sizeof(uint8_t) * data->content_length)) == NULL)
	{
		// Send Response: "500 Internal Server Error"
		printf("ERROR: '500 Internal Server Error'\n");
		http_free(data);
		return (0);
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
		return (0);
	}
	return (1);
}

static ssize_t		receive_header(int fd, char **header, char **end)
{
	ssize_t		size;
	char		buff[BUFF_SOCKET];

	if ((size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		if ((*end = strstr((const char *)buff, "\r\n\r\n")) != NULL)
			*end = *end + 4;
		else
		{
			// Send Response: "413 Entity Too Large"
			printf("ERROR: '413 Entity Too Large'\n");
			return (-1);
		}	
	}
	else if (size == 0)
	{
		// Send Response: "204 No Content"
		printf("ERROR: '204 No Content'\n");
		return (0);
	}
	else
	{
		// Send Response: "408 Request Time-out"
		perror("ERROR: '408 Request Time-out'");
		return (-1);
	}
	*header = buff;
	return (size);
}

int					receive(int fd)
{
	ssize_t		size;
	int 		status;
	char		*buf;
	char		*end;
	t_http		*request;

	if ((size = receive_header(fd, &buf, &end)) < 0)
		return (0);
	// write(1, buf, size);
	request = header(buf, ((end - 4) - buf));
	if (request->content_length)
		if (receive_content(fd, request, end, size - (end - buf)) == 0)
			return (0);
	status = response(request, fd);
	http_free(request);
	return (status);
}