#include "libft.h"
#include <fcntl.h>
#include "http.h"

uint8_t		*get_file_content(uint64_t *file_size, char *filename)
{
	uint8_t		*str;
	struct stat	st;

	if (stat(filename, &st) != 0 && (*file_size = 0) == 0)
		return (NULL);
	*file_size = st.st_size;
	if ((str = malloc(sizeof(uint8_t) * 2)) == NULL)
		return (NULL);
	str[0] = '1';
	str[1] = '\0';
	return (str);
}
