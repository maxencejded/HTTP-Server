#include "server.h"

void				fields_content_length(char *str, t_http *data)
{
	uint32_t	len;

	len = strtol(str, &str, 10);
	if (*str == '\0')
		data->content_length = len;
}

void				fields_content_type(char *str, t_http *data)
{
	uint8_t			i;
	t_post_content	content_type[POST_CONTENT_NUM] = {
		{"text/plain", POST_1},
		{"application/x-www-form-urlencoded", POST_2},
		{"multipart/form-data", POST_3},
	};

	while (*str && *str == ' ')
		++str;	
	if (data->method == METHOD_POST)
	{
		i = 0;
		while(i < POST_CONTENT_NUM)
		{
			if (strncmp(str, content_type[i].name, strlen(content_type[i].name)) == 0)
			{
				data->content_type = i;
				break ;
			}
			++i;
		}
		// Read the data
	}
}

void				fields_cookie(char *str, t_http *data)
{
	(void)str;
	(void)data;
}

void				fields_date(char *str, t_http *data)
{
	(void)str;
	(void)data;	
}

void				fields_expect(char *str, t_http *data)
{
	(void)str;
	(void)data;
}
