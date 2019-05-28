#include "server.h"

void		header_print(t_http *data)
{
	if (data == NULL)
		return ;
	if (data->method == 1)
		printf("METHOD: GET\n");
	else if (data->method == 2)
		printf("METHOD: HEAD\n");
	else if (data->method == 3)
		printf("METHOD: POST\n");
	else if (data->method == 4)
		printf("METHOD: PUT\n");
	else
		printf("METHOD: NOT HANDLE\n");
	printf("PATH: %s\n", data->path);
	printf("PROTOCOL: %s\n", (data->protocol == 2) ? "HTTP/1.1" : "HTTP/2.0");
	printf("CONTENT_TYPE: %d\n", data->content_type);
	printf("CONTENT_LENGTH: %u\n", data->content_length);
	printf("CONTENT: %s\n", data->content);
}

int			read_data(int fd)
{
	int32_t		size;
	char		*end;
	char		buff[BUFF_SOCKET];
	t_http		*request;

	size = 0;
	bzero(buff, BUFF_SOCKET);
	while ((size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
//		write(1, buff, size);
		if ((end = strstr((const char *)buff, "\r\n\r\n")) != NULL)
		{
			end = end + 4;
			break ;
		}
	}
	if (size - (end - buff))
		printf("More Data to read: %ld bytes\n", size - (end - buff));
	request = header(buff, ((end - 4) - buff));
	if (request->content_length != 0)
	{
		if ((request->content = (uint8_t *)malloc(request->content_length)) == NULL)
			perror("ERROR: Malloc");
		memcpy(request->content, end, size - (end - buff));
	}
	header_print(request);
	printf("Reponse value = %d\n", response(request, fd));
	http_free(request);
	return (1);
}

static void		sigchld(int sig)
{
	int	status;

	(void)sig;
	while (waitpid(-1, &status, WNOHANG) > 0)
		(void)status;
}

int				connection_add(int fd, char *address, uint16_t connect)
{
	pid_t		pid;

	if ((pid = fork()) == 0)
	{
		printf("[%d] At Address: %s\n\n", connect, address);
		read_data(fd);
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

static void		sigstop(int sig)
{
	(void)sig;
	printf("Stoping Server\n");
	exit(close(sock_fd));
}

int			sock_fd;

int			main(int argc, char **argv)
{
	char	*address;

	(void)argv;
	if (argc != 1)
	{
		printf("Usage: ./server\n");
		exit(EXIT_FAILURE);
	}
	if ((sock_fd = socket_int()) == -1)
		exit(EXIT_FAILURE);
	address = NULL;
	if (socket_bind(sock_fd, PORT, &address) == 0)
	{
		close(sock_fd);
		exit(EXIT_FAILURE);
	}
	if (listen(sock_fd, CONNECTION_NBR) == -1)
	{
		close(sock_fd);
		exit(EXIT_FAILURE);
	}
	printf("Starting new Server\nAdrress: %s:%d\n", address, PORT);
	strdel(&address);
	signal(SIGINT, sigstop);
	loop(sock_fd);
	return (0);
}
