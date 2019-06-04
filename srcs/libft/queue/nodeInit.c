#include "queue.h"

t_node			*nodeInit(void *elem)
{
	t_node		*node;

	if ((node = (t_node *)malloc(sizeof(t_node))) == NULL)
		return (NULL);
	node->elem = elem;
	node->next = NULL;
	return (node);
}
