#include "http.h"

static uint8_t		header_http_version(char *method, t_request *header)
{
	if (ft_strncmp(method, "HTTP/", 5) != 0)
		return (0);
	method = method + 5;
	if (method[0] == '1')
		header->http = 1;
	else if (method[0] == '2')
		header->http = 2;
	else
		return (0);
	return (1);
}

static uint8_t		header_file(char *method, t_request *header)
{
	uint32_t	i;
	uint32_t	len;

	i = 0;
	while (method[i] && method[i] != ' ')
		++i;
	if (method[i] == '\0')
		return (0);
	++i;
	len = 0;
	while (method[i + len] && method[i + len] != ' ')
		++len;
	header->file = (int8_t *)ft_strmcpy((char *)&method[i], len);
	method = method + i + len + 1;
	if (header_http_version(method, header) == 0)
		return (0);
	return (1);
}

#define METHOD_NUMBER 9

uint8_t				header_method(char *method, t_request *header)
{
	uint8_t			i;
	static t_method	m[METHOD_NUMBER] = {
		{"GET", 3, METHOD_GET},
		{"HEAD", 4, METHOD_HEAD},
		{"POST", 4, METHOD_POST},
		{"PUT", 3, 3},
		{"DELETE", 6, 4},
		{"CONNECT", 7, 5},
		{"OPTIONS", 7, 6},
		{"TRACE", 5, 7},
		{"PATCH", 5, 8}
	};

	i = 0;
	while (i < METHOD_NUMBER)
	{
		if (ft_strncmp(method, m[i].method, m[i].length) == 0)
		{
			header->method = m[i].number;
			header_file(method, header);
			return (1);
		}
		++i;
	}
	return (0);
}
