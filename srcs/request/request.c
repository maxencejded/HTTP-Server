#include "server.h"

static t_http	*http_init(void)
{
	t_http	*data;

	if ((data = (t_http *)malloc(sizeof(t_http))) == NULL)
		return (NULL);
	bzero(data, sizeof(t_http));
	return (data);
}

void			http_free(t_http *data)
{
	if (data == NULL)
		return ;
	free(data->path);
	free(data->accept);
	free(data->content);
	free(data);
	data = NULL;
}

t_http			*header(char *request)
{
	uint8_t		i;
	char		**fields;
	t_http		*header;

	if ((header = http_init()) == NULL)
		return (NULL);
	if ((fields = splitheader(request)) == NULL)
		return (NULL);
	if (fields[0] == NULL || method(fields[0], header) == 0)
	{
		http_free(header);
		return (NULL);
	}
	i = 0;
	while (fields[++i] != NULL)
		fields_dispatch(fields[i], header);
	ptrdel(fields);
	return (header);
}