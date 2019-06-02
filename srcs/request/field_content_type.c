#include "server.h"

static int		boundary(char *str, t_http *data)
{
	if (*str == ';')
		++str;
	while (*str && *str == ' ')
		++str;
	if (strncmp(str, "boundary=", 9) == 0)
	{
		if ((data->boundary = strdup(str + 9)) == NULL)
			return (0);
		return (1);
	}
	return (0);
}

int				field_content_type(char *str, t_http *data)
{
	int8_t					i;
	size_t					len;
	static t_content_type	type[POST_CONTENT_NUM] = {
		{"text/plain", POST_TYPE_1},
		{"application/x-www-form-urlencoded", POST_TYPE_2},
		{"multipart/form-data", POST_TYPE_3},
	};

	if (data->method != METHOD_POST)
		return (0);
	while (*str && *str == ' ')
		++str;
	i = 0;
	while (i < POST_CONTENT_NUM)
	{
		len = strlen(type[i].name);
		if (strncmp(str, type[i].name, len) == 0)
		{
			data->content_type = type[i].code;
			break ;
		}
		++i;
	}
	return ((type[i].code == POST_TYPE_3) ? boundary(str + len, data) : 1);
}
