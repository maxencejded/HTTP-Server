#include "server.h"

#define END(x) (x == '\r' || x == '\n')

static size_t	wordnumber(char *str)
{
	size_t	i;
	size_t	size;

	size = 0;
	i = -1;
	while (str[++i])
	{
		if (i == 0 && !END(str[i]))
			size += 1;
		else if (!END(str[i]) && END(str[i - 1]))
			size += 1;
	}
	return (size);
}

static size_t	wordlen(char *str)
{
	size_t	size;

	size = 0;
	while (str[size] && !END(str[size]))
		size++;
	return (size);
}

char			**splitheader(char *str)
{
	size_t	i;
	size_t	len;
	size_t	size;
	char	**tab;

	size = wordnumber(str);
	if (!(tab = (char **)malloc(sizeof(char *) * (size + 1))))
		return (NULL);
	i = -1;
	while (++i < size)
	{
		while (*str && END(*str))
			str++;
		len = wordlen(str);
		tab[i] = strndup(str, len);
		str += len;
	}
	tab[i] = 0;
	return (tab);
}