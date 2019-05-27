#include "libft.h"

void	ptrdel(char **ptr)
{
	size_t	i;

	if (ptr)
	{
		i = -1;
		while (ptr[++i])
			strdel(&ptr[i]);
		free(ptr);
	}
}