#ifndef HTTP_H
# define HTTP_H

# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/stat.h>

# define PORT 6060
# define CONNECTION_NBR 4
# define BUFF_SOCKET	getpagesize()

// # pragma pack(2)

# define PROTOCOL_1_0	1
# define PROTOCOL_1_1	2
# define PROTOCOL_2_0	3

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

extern int			sock_fd;

int					socket_int(void);
int					socket_bind(int fd, int port, char **address);
int					socket_accept(int fd, char **address);

int					receive(int fd);

void				exit_server(void);
void				sigstop(int sig);
void				sigchld(int sig);

#endif
