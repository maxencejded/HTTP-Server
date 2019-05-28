#include "http.h"
#include "libft.h"

char* concat(const char *s1, const char *s2)
{
	char			*result;
	size_t			len1;
	size_t			len2;

	len1 = strlen(s1);
	len2 = strlen(s2);
	if ((result = malloc(len1 + len2 + 1)) == NULL)
		return (NULL);
	bzero(result, len1 + len2 + 1);
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);
	return (result);
}
