#include "server.h"

void		exit_server(void)
{
	close(sock_fd);
	exit(EXIT_FAILURE);
}

// Need to check if total is more than content_length -> Handle error
int			receive_content(int fd, t_http *data, char *str, ssize_t size)
{
	char		buff[BUFF_SOCKET];
	uint8_t		*content;
	ssize_t		total;

	total = size;
	bzero(buff, BUFF_SOCKET);
	if ((data->content = (uint8_t *)malloc(sizeof(uint8_t) * data->content_length)) == NULL)
	{
		perror("ERROR: Malloc");
		http_free(data);
		return (0);
	}
	bzero(data->content, sizeof(uint8_t) * data->content_length);
	content = data->content;
	memcpy(content, str, size);
	content = content + size;
	while (total < data->content_length && (size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		memcpy(content, buff, size);
		content = content + size;
		total += size;
	}
	return (1);
}

int			receive(int fd)
{
	ssize_t		size;
	char		*end;
	char		buff[BUFF_SOCKET];
	t_http		*request;

	size = 0;
	bzero(buff, BUFF_SOCKET);
	while ((size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		write(1, buff, size);
		if ((end = strstr((const char *)buff, "\r\n\r\n")) != NULL)
		{
			end = end + 4;
			break ;
		}
	}
	if (size == -1)
	{
		perror("ERROR: Timeout");
		return (0);
	}
	request = header(buff, ((end - 4) - buff));
	if (request->content_length != 0)
		if (receive_content(fd, request, end, size - (end - buff)) == 0)
			return (0);
	printf("Reponse value = %d\n", response(request, fd));
	http_free(request);
	return (1);
}

int				connection_add(int fd, char *address, uint16_t connect)
{
	pid_t		pid;

	if ((pid = fork()) == 0)
	{
		printf("[%d] At Address: %s\n", connect, address);
		receive(fd);
		printf("[%d] Close\n", connect);
		_exit(close(fd));
	}
	else if (pid < 0)
	{
		perror("ERROR: Fork");
		strdel(&address);
		return (1);
	}
	strdel(&address);
	return (0);
}

int				loop(int fd)
{
	int			sock_new;
	char		*address;
	uint16_t	connect;

	connect = 0;
	address = NULL;
	signal(SIGCHLD, sigchld);
	while ((sock_new = socket_accept(fd, &address)) > 0)
	{
		if (connection_add(sock_new, address, connect) == 1)
			return (0);
		close(sock_new);
		++connect;
	}
	close(fd);
	if (sock_new == -1)
	{
		perror("ERROR: Accept");
		exit(EXIT_FAILURE);
	}
	return (1);
}

int			sock_fd;

int			main(int argc, char **argv)
{
	char	*address;

	(void)argv;
	address = NULL;
	if (argc != 1)
	{
		printf("Usage: ./server\n");
		exit(EXIT_FAILURE);
	}
	if ((sock_fd = socket_int()) == -1)
		exit(EXIT_FAILURE);
	if (socket_bind(sock_fd, PORT, &address) == 0)
		exit_server();
	if (listen(sock_fd, CONNECTION_NBR) == -1)
		exit_server();
	printf("Starting new Server\nAdrress: %s:%d\n", address, PORT);
	strdel(&address);
	signal(SIGINT, sigstop);
	loop(sock_fd);
	close(sock_fd);
	return (0);
}
