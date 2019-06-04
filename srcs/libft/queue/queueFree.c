#include "queue.h"

void			queueFree(t_queue *queue)
{
	t_node		*tmp;

	if (queue == NULL)
		return ;
	tmp = NULL;
	while (queue->first)
	{
		tmp = queue->first;
		queue->first = queue->first->next;
		// Call free elem
		free(tmp);
	}
	free(queue);
}