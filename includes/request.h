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

char				**splitheader(char *str);

t_http				*header(char *request, int32_t size);
uint8_t		        method(char *method, t_http *data);

typedef struct		s_fields
{
	char			*name;
	void			(*fct) (char *str, t_http *data);
}					t_fields;

uint8_t				fields_dispatch(char *str, t_http *data);

void				fields_a_im(char *str, t_http *data);
void				fields_accept(char *str, t_http *data);
void				fields_accept_charset(char *str, t_http *data);
void				fields_accept_encoding(char *str, t_http *data);
void				fields_accept_language(char *str, t_http *data);

void				fields_accept_datetime(char *str, t_http *data);
void				fields_acces_control(char *str, t_http *data);
void				fields_authorization(char *str, t_http *data);
void				fields_cache_control(char *str, t_http *data);
void				fields_connection(char *str, t_http *data);

void				fields_content_length(char *str, t_http *data);
void				fields_content_type(char *str, t_http *data);
void				fields_cookie(char *str, t_http *data);
void				fields_date(char *str, t_http *data);
void				fields_expect(char *str, t_http *data);

void				fields_forwarded(char *str, t_http *data);
void				fields_from(char *str, t_http *data);
void				fields_host(char *str, t_http *data);
void				fields_http_settings(char *str, t_http *data);
void				fields_if_match(char *str, t_http *data);

void				fields_if_modified_since(char *str, t_http *data);
void				fields_if_non_match(char *str, t_http *data);
void				fields_if_range(char *str, t_http *data);
void				fields_if_unmodified(char *str, t_http *data);
void				fields_max_forwards(char *str, t_http *data);

void				fields_origin(char *str, t_http *data);
void				fields_pragma(char *str, t_http *data);
void				fields_proxy_authorization(char *str, t_http *data);
void				fields_range(char *str, t_http *data);
void				fields_referer(char *str, t_http *data);

void				fields_te(char *str, t_http *data);
void				fields_user_agent(char *str, t_http *data);
void				fields_upgrade(char *str, t_http *data);
void				fields_via(char *str, t_http *data);
void				fields_warning(char *str, t_http *data);

#endif