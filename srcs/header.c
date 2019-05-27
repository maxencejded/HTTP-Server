#include "http.h"

static t_request	*header_init(void)
{
	t_request	*data;

	if ((data = (t_request *)malloc(sizeof(t_request))) == NULL)
		return (NULL);
	bzero(data, sizeof(t_request));
	return (data);
}

void			header_free(t_request *data)
{
	if (data == NULL)
		return ;
	free(data->file);
	free(data);
}

t_request			*header_parse(char *request, int32_t size)
{
	uint8_t		i;
	char		**fields;
	t_request	*header;

	if ((header = header_init()) == NULL)
		return (NULL);
	request[size] = '\0';
	if ((fields = ft_strsplit(request, '\n')) == NULL)
		return (NULL);
	if (fields[0] == NULL || header_method(fields[0], header) == 0)
	{
		header_free(header);
		return (NULL);
	}
	i = 1;
	while (fields[i] != NULL)
		++i;
	ft_ptrdel(fields);
	return (header);
}