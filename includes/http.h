#ifndef HTTP_H
# define HTTP_H

# include <netdb.h>
# include <signal.h>
# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>

/*
** HTTP Server configuration
*/

# define PORT			6060
# define CONNECTION		4
# define TIME_OUT		10

# define PAGE_SIZE		getpagesize()

typedef struct		s_http
{
	uint8_t			method;
	char			*path;
	uint8_t			protocol;

	char			*accept;
	uint8_t			content_type;

	uint32_t		content_length;
	uint8_t			*content;
}					t_http;

extern int			g_fd;

int					socket_int(void);
int					socket_bind(int fd, int port, char **address);
int					socket_accept(int fd, char **address);

int					receive(int fd, int *status);

void				exit_server(void);
void				sigstop(int sig);
void				sigchld(int sig);

#endif
