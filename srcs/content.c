#include "server.h"

static t_content		*contentInit(char *key, char *value, uint8_t flag)
{
	t_content	*node;

	if ((node = (t_content *)malloc(sizeof(t_content))) == NULL)
		return (NULL);
	memset(node, 0, sizeof(t_content));
	node->key = key;
	node->value = value;
	node->flag = flag;
	return (node);
}

void					contentFree(t_content *node)
{
	if (node == NULL)
		return ;
	free(node->key);
	free(node->value);
	free(node);
}

static int				contentAdd(t_http *data, char *key, char *value, uint8_t flag)
{
	t_content	*node;

	if (data == NULL)
		return (0);
	if (data->content == NULL)
		data->content = queueInit();
	if ((node = contentInit(key, value, flag)) == NULL)
		return (0);
	if (enqueue(data->content, node) == 0)
	{
		contentFree(node);
		return (0);
	}
	return (1);
}

static int				contentSplit(t_http *data, char *content)
{
	char		*key;
	char		*value;
	uint32_t	i;

	i = 0;
	while (content[i] && content[i] != '=')
		++i;
	if ((key = strndup(content, i)) == NULL)
		return (0);
	content = content + i + 1;
	i = 0;
	while (content[i])
		++i;
	if ((value = strndup(content, i)) == NULL)
		return (0);
	if (contentAdd(data, key, value, 0) == 0)
		return (0);
	return (1);
}

int						contentParse(t_http *data, char *content)
{
	int			ret;
	char		*ptr;
	char		*tmp;

	ret = 1;
	tmp = content;
	while ((ptr = strchr(tmp, '&')) != NULL)
	{
		*ptr = '\0';
		if ((ret = contentSplit(data, tmp)) == 0)
			break ;
		tmp = ptr + 1;
	}
	if (ret != 0 && *tmp != '\0')
		ret = contentSplit(data, tmp);
	free(content);
	return (ret);
}