#include "server.h"

int				connection_add(int fd, char *address, uint16_t connect)
{
	int			response;
	pid_t		pid;

	if ((pid = fork()) == 0)
	{
		printf("[%d] At Address: %s\n", connect, address);
		response = receive(fd);
		printf("[%d] Close with status: %d\n", connect, response);
		_exit(close(fd));
	}
	else if (pid < 0)
	{
		perror("ERROR: Fork");
		strdel(&address);
		return (0);
	}
	strdel(&address);
	return (1);
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
		if (connection_add(sock_new, address, connect) == 0)
			return (0);
		close(sock_new);
		++connect;
	}
	if (sock_new == -1)
	{
		perror("ERROR: Accept");
		exit_server();
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
	if (loop(sock_fd) == 0)
		exit_server();
	close(sock_fd);
	return (0);
}
