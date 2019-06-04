#include "queue.h"

int				isEmpty(t_queue *queue)
{
	if (queue == NULL || queue->first == NULL)
		return (1);
	return (0);
}
