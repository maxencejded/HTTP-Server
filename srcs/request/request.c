#include "server.h"

static t_http		*http_init(void)
{
	t_http	*data;

	if ((data = (t_http *)malloc(sizeof(t_http))) == NULL)
		return (NULL);
	bzero(data, sizeof(t_http));
	return (data);
}

void				http_free(t_http *data)
{
	if (data == NULL)
		return ;
	free(data->path);
	free(data->accept);
	free(data->content);
	free(data);
}

static uint32_t		header_next(char *str)
{
	uint32_t		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\r')
		{
			str[i] = '\0';
			if (str[++i] == '\n')
			{
				str[i] = '\0';
				++i;
			}
			return (i);
		}
		++i;
	}
	return (i);
}

t_http				*header(char *request, int *status)
{
	uint32_t		index;
	t_http		*header;

	if (request == NULL)
		return (NULL);
	if ((header = http_init()) == NULL)
	{
		// Send Response: "500 Internal Server Error"
		printf("ERROR: '500 Internal Server Error'\n");
		*status = 500;
		return (NULL);
	}
	index = header_next(request);
	if (method(request, header) == 0)
	{
		// Send Response: "501 Not Implemented"
		printf("ERROR: '501 Not Implemented'\n");
		http_free(header);
		*status = 501;
		return (NULL);
	}
	request = request + index;
	while (*request)
	{
		index = header_next(request);
		fields_dispatch(request, header);
		request = request + index;
	}
	return (header);
}
