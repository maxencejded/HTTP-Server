#include "server.h"
#include "response.h"

/*
** Freeing response structure
*/

int				reponse_free(t_reponse *answer)
{
	if (answer == NULL)
		return (0);
	if (answer->complete_path)
		free(answer->complete_path);
	if (answer->content_type)
		free(answer->content_type);
	if (answer->date)
		free(answer->date);
	close(answer->file_fd);
	free(answer);
	return (0);
}

/*
** Free a char * if it exists
** Always return 0
*/

int				ft_free(char *to_free)
{
	if (to_free)
		free(to_free);
	return (0);
}
