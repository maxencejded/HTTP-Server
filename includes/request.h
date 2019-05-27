#ifndef REQUEST_H
# define REQUEST_H

# include <stdlib.h>

# include "libft.h"
# include "http.h"

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

void				http_free(t_http *data);

t_http				*header(char *request, int32_t size);
uint8_t		        method(char *method, t_http *data);

#endif