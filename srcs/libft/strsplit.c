#include "libft.h"

static size_t	count_letters(char const *str, char c)
{
	size_t	count;

	count = 0;
	while (str[count] != c && str[count])
		count++;
	return (count);
}

static size_t	count_words(char const *str, char c)
{
	size_t	count;

	count = 0;
	while (*str)
	{
		if (*str != c)
		{
			if (*(str + 1) == c || *(str + 1) == '\0')
				count++;
		}
		str++;
	}
	return (count);
}

char			**strsplit(char const *s, char c)
{
	size_t		i;
	size_t		j;
	size_t		words;
	char		**tab;

	i = 0;
	if (!s || !c)
		return (NULL);
	words = count_words(s, c);
	if (!(tab = (char **)malloc(sizeof(char *) * (words + 1))))
		return (NULL);
	while (i < words)
	{
		while (*s == c && *s)
			s++;
		tab[i] = (char *)malloc(sizeof(char) * (count_letters(s, c) + 1));
		j = 0;
		while (*s != c && *s)
			tab[i][j++] = *s++;
		tab[i][j] = '\0';
		i++;
	}
	tab[i] = 0;
	return (tab);
}