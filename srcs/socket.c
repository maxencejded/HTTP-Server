#include "server.h"

int			socket_int(void)
{
	int					num;
	int					sock_fd;
	struct protoent		*proto;

	if ((proto = getprotobyname("TCP")) == 0)
	{
		perror("ERROR: Protocol");
		return (-1);
	}
	if ((sock_fd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
	{
		perror("ERROR: Socket");
		return (-1);
	}
	num = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(int)) == -1)
	{
		perror("ERROR: Setsockopt(SO_REUSEADDR)");
		return (-1);
	}
	return (sock_fd);
}

int			socket_bind(int fd, int port, char **address)
{
	struct sockaddr_in	bind_s;

	bzero(&bind_s, sizeof(struct sockaddr_in));
	bind_s.sin_family = AF_INET;
	bind_s.sin_port = htons(port);
	bind_s.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (struct sockaddr *)&bind_s, sizeof(bind_s)) == -1)
	{
		perror("ERROR: Bind");
		return (0);
	}
	*address = strdup(inet_ntoa(bind_s.sin_addr));
	return (1);
}

int			socket_accept(int fd, char **address)
{
	int					sock;
	int					sigpipe;
	socklen_t			sock_len;
	struct timeval		timeout;      
	struct sockaddr_in	sock_init;

	sock_len = sizeof(struct sockaddr_in);
	bzero(&sock_init, sizeof(struct sockaddr_in));
	sock = accept(fd, (struct sockaddr *)&sock_init, &sock_len);
	if (sock == -1)
	{
		perror("ERROR: Accept");
		return (-1);
	}
	bzero(&timeout, sizeof(struct timeval));
    timeout.tv_sec = 10;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) == -1)
	{
		perror("ERROR: Setsockopt(SO_RCVTIMEO)");
		return (-1);
	}
	if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &sigpipe, sizeof(int)) == -1)
	{
		perror("ERROR: Setsockopt(SO_NOSIGPIPE)");
		return (-1);
	}
	*address = strdup(inet_ntoa(sock_init.sin_addr));
	return (sock);
}
