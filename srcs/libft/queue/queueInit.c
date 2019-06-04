#include "queue.h"

t_queue			*queueInit(void)
{
	t_queue		*queue;

	if ((queue = (t_queue *)malloc(sizeof(t_queue))) == NULL)
		return (NULL);
	queue->first = NULL;
	queue->last = NULL;
	return (queue);
}
