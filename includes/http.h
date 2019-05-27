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

# include "libft.h"

# define PORT 6060
# define CONNECTION_NBR 4
# define BUFF_SOCKET	getpagesize()

# pragma pack(2)

# define METHOD_GET		1
# define METHOD_HEAD	2
# define METHOD_POST	3

typedef struct		s_method
{
	char			*method;
	uint8_t			length;
	uint8_t			number;
}					t_method;

typedef struct		s_request
{
	uint8_t			method;
	int8_t			*file;
	uint8_t			http;
}					t_request;

# define HTTP_1	"HTTP/1.1"
# define HTTP_2	"HTTP/2.0"

typedef struct		s_response
{
	int8_t			*get;
	uint16_t		code;
	int8_t			*content_type;
	uint64_t		length;
}					t_response;

extern int			sock_fd;

int					socket_int(void);
int					socket_bind(int fd, int port, char **address);
int					socket_accept(int fd, char **address);

t_request			*header_parse(char *request, int32_t size);
uint8_t		        header_method(char *method, t_request *header);
void				header_free(t_request *data);

void				print_memory(void *addr, size_t size);

#endif
