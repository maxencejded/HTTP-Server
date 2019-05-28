#include "server.h"

void		exit_server(void)
{
	close(sock_fd);
	exit(EXIT_FAILURE);
}

void		sigstop(int sig)
{
	(void)sig;

	printf("Stoping Server\n");
	exit(close(sock_fd));
}

void		sigchld(int sig)
{
	int	status;

	(void)sig;
	while (waitpid(-1, &status, WNOHANG) > 0)
		(void)status;
}