#include "http.h"

int			read_data(int fd)
{
	int32_t		size;
	char		*end;
	char		buff[BUFF_SOCKET];
	t_request	*request;

	size = 0;
	bzero(buff, BUFF_SOCKET);
	while ((size = recv(fd, buff, BUFF_SOCKET, 0)) > 0)
	{
		write(1, buff, size);
		if ((end = (char *)strstr((const char *)buff, "\r\n\r\n")) != NULL)
		{
			end = end + 4;
			break ;
		}
	}
	if (end - buff > size)
		printf("More Data to read: %ld bytes", size - (end - buff));
	request = header_parse(buff, ((end - 4) - buff));
	//    write(fd, "HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\nContent-Length: 12\n\nHello world!", 91);
	write(fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: 640\n\n<!DOCTYPE html><html><head><meta charset=\"utf-8\"><script src=\"script.js\"></script><title>Pokequiz</title><link href=\"style.css\" type=\"text/css\" rel=\"stylesheet\"></head><body><h1>Who's that Pokémon ?</h1><div class=\"all\"><img id=\"imgpoke\"><img><p id=\"pokemon_name\"></p><br><div class=\"input\"><select id=\"lang\"><option type=\"radio\" value=\"pokemon_g2_fr\">French</option><option type=\"radio\" value=\"pokemon_g2_en\">English</option></select><input id=\"namepoke\" type=\"text\" placeholder=\"Pokemon name\"></input><input id=\"submit\" type=\"submit\" value=\"Submit\"></input></div><p>Score : <b id=\"score\">0</b>/<b id=\"total\">0</b></p></div></body></html>", 727);
	header_free(request);
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
		return (1);
	}
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
		strdel(&address);
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
