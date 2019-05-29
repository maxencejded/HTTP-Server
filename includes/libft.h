#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>

void		strdel(char **str);
void		ptrdel(char **ptr);
char		**strsplit(char const *s, char c);
char		*concat(const char *s1, const char *s2);
uint8_t		*concat_uint8_t(const uint8_t *s1, const uint8_t *s2);
uint8_t		*get_file_content(uint64_t *file_size, char *filename);

void		print_memory(void *addr, size_t size);

#endif
