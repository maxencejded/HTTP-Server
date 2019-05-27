#include "server.h"

/*
# define FIELDS_SIZE	35

static t_fields		fields[FIELDS_SIZE] = {
	{"A-IM:", &fields_a_im},
	{"Accept:", &fields_accept},
	{"Accept-Charset:", &fields_accept_charset},
	{"Accept-Encoding:", &fields_accept_encoding},
	{"Accept-Language:", &fields_accept_language},
	{"Accept-Datetime:", &fields_accept_datetime},
	{"Access-Control-Request-Method:", &fields_acces_control},
	{"Authorization:", &fields_authorization},
	{"Cache-Control:", &fields_cache_control},
	{"Connection:", &fields_connection},
	{"Content-Length:", &fields_content_length},
	{"Content-Type:", &fields_content_type},
	{"Cookie:", &fields_cookie},
	{"Date:", &fields_date},
	{"Expect:", &fields_expect},
	{"Forwarded:", &fields_forwarded},
	{"From:", &fields_from},
	{"Host:", &fields_host},
	{"HTTP2-Settings:", &fields_http_settings},
	{"If-Match:", &fields_if_match},
	{"If-Modified-Since:", &fields_if_modified_since},
	{"If-None-Match:", &fields_if_non_match},
	{"If-Range:", &fields_if_range},
	{"If-Unmodified-Since:", &fields_if_unmodified},
	{"Max-Forwards:", &fields_max_forwards},
	{"Origin:", &fields_origin},
	{"Pragma:", &fields_pragma},
	{"Proxy-Authorization:", &fields_proxy_authorization},
	{"Range:", &fields_range},
	{"Referer:", &fields_referer},
	{"TE:", &fields_te},
	{"User-Agent:", &fields_user_agent},
	{"Upgrade:", &fields_upgrade},
	{"Via:", &fields_via},
	{"Warning:", &fields_warning}
};
*/

# define FIELDS_SIZE	4

uint8_t		fields_dispatch(char *str, t_http *data)
{
	uint8_t				i;
	uint8_t				len;
	static t_fields		fields[FIELDS_SIZE] = {
		{"Accept:", &fields_accept},	
		{"Content-Length:", &fields_content_length},
		{"Content-Type:", &fields_content_type},	
		{"Cookie:", &fields_cookie}
	};

	i = 0;
	while (i < FIELDS_SIZE)
	{
		len = strlen(fields[i].name);
		if (strncmp(str, fields[i].name, len) == 0)
		{
			fields[i].fct(str + len, data);
			return (1);
		}
		++i;
	}
	return (0);
}