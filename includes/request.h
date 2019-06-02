#ifndef REQUEST_H
# define REQUEST_H

# include <stdlib.h>

# include "libft.h"
# include "http.h"

# define PROTOCOL_1_0	1
# define PROTOCOL_1_1	2
# define PROTOCOL_2_0	3

# define METHOD_GET		1
# define METHOD_HEAD	2
# define METHOD_POST	3
# define METHOD_PUT		4

typedef struct		s_method
{
	char			*method;
	uint8_t			length;
	uint8_t			number;
}					t_method;

int					http_free(t_http *data);

t_http				*header(int fd, char *request, int *status);
uint8_t				method(char *method, t_http *data);

typedef struct		s_fields
{
	char			*name;
	int				(*fct) (char *str, t_http *data);
}					t_fields;

#define FIELDS_SIZE		5

uint8_t				fields_dispatch(char *str, t_http *data);

int					field_accept(char *str, t_http *data);
int					field_content_length(char *str, t_http *data);
int					field_content_type(char *str, t_http *data);
int					field_expect(char *str, t_http *data);
int					field_host(char *str, t_http *data);

typedef struct		s_content_type
{
	char			*name;
	uint8_t			code;
}					t_content_type;

# define POST_CONTENT_NUM	3
# define POST_TYPE_1		1
# define POST_TYPE_2		2
# define POST_TYPE_3		3

#endif
