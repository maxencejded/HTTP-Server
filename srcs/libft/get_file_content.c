#include "libft.h"
#include <fcntl.h>
#include "http.h"

uint8_t		*get_file_content(uint64_t *file_size, char *filename)
{
	struct stat	st;

	if (stat(filename, &st) != 0 && (*file_size = 0) == 0)
		return (NULL);
	*file_size = st.st_size;
	return ((void *)1);
}
