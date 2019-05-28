#include "libft.h"
#include <fcntl.h>
#include "http.h"

char		*get_file_content(const int fd, int *file_size, char *filename)
{
	char		*buffer;
	struct stat	st;

	if (stat(filename, &st) != 0 && (*file_size = 0) == 0)
		return (NULL);
	*file_size = st.st_size;
	if ((buffer = malloc(sizeof(char) * (*file_size + 1))) == NULL)
		return (NULL);
	if (read(fd, buffer, *file_size) < 0)
		return (NULL);
	close(fd);
	return (buffer);
}
