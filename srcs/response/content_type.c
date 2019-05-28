#include "server.h"

/*
 * If the content type is correctly formated and match the file format, returns 0
 * Otherwise will return an error and provoq a BAD_REQUEST exception (erroror 400)
 *
 *
 * Examples for request->accept :
 * [text/html,application/xhtml+xml,application/xml;q=0.9,*\*;q=0.8]
 * [audio/webm,audio/ogg,audio/wav,audio\*;q=0.9,application/ogg;q=0.7,video\*;q=0.6,*\*;q=0.5]
 *
 * \ are to be replaced by / unfortunately cant comment with these.
*/

static int		ft_free(char *accepted)
{
	free(accepted);
	return (0);
}

int				check_content_type(t_http *request, char *complete_path)
{
	char		*accepted; /* Duplicated string of accept to simplify the parsing */
	char		*file_extension;
	int			i, n, accept_size; /* Index of the accept string inside request */

	i = -1;
	n = -1;

/*
 * Initialising all my different variables, if one fails, returns an error
*/

	if (!request || !request->accept)
		return (-1);
	if ((accept_size = strlen(request->accept)) <= 0)
		return (-1);
	if ((accepted = malloc(sizeof(char) * (strlen(request->accept) + 1))) == NULL)
		return (-1);

/*
 * Filling the accepted string with the well parsed request->accept
*/

	while (++i < accept_size)
	{
		if (request->accept[i] != ',' && request->accept[i] != ';')
			accepted[++n] = request->accept[i];
		else if ((accepted[++n] = ' ') == ' ')
			while (i < accept_size && request->accept[i] != '\0' && request->accept[i] != ',')
				++i;
	}
	accepted[++n] = '\0';

/*
 * Accepted string has this format now:
 * 
 * [audio/webm audio/ogg audio/wav audio\* application/ogg video\* *\*]
 * [audio/webm audio/ogg audio/wav audio\* application/ogg video\* *\*]
 *
 * The best format are sorted by order and separated by spaces, we can now
 * check the extension of the path to check if the format is corresponding
*/

/*
 * Getting the file_extension of the file called
*/

	i = -1;
	while (complete_path[++i] != '\0')
		;
	while (complete_path[--i] != '/')
		;
	i += 1;
	if ((file_extension = strdup(&complete_path[i])) == NULL)
		return (-1);

/*
 * Getting the file_extension accepted in best order and comparing them
 * with the file_extension of the file called
*/

	i = -1;
	while (++i < n) /* N is the size of the accepted string */
	{
		while (accepted[i] != '\0' && accepted[i] != ' ')
			++i;
		accept_size = i; /* Using accept_size in a new may, will be our memory to remember the last index visited */
		while (accepted[--i] != '/')
			;
		i += 1;
		if (strncmp(&accepted[i], file_extension, accept_size - i) == 0 && ft_free(accepted) == 0 &&
				ft_free(file_extension) == 0)
			return (0); /* File extension is correct */
		else if (accepted[i] == '*' && ft_free(accepted) == 0 && ft_free(file_extension) == 0)
			return (0); /* Handle Star in file_extension */
		i = accept_size;
	}
	free(file_extension);
	free(accepted);
	return (-1);
}

char			*get_content_type(t_http *request, char *complete_path)
{
	char		*file_extension;
	int			i, n;

	n = strlen(complete_path);
	while (complete_path[--n] != '/')
		;
	n += 1;
	if ((file_extension = strdup(&complete_path[n])) == NULL)
		return (NULL);
	i = -1;
	while (strncmp(file_extension, &request->accept[++i], strlen(file_extension)) != 0)
		if (request->accept[i] == '*')
			break ;
	free(file_extension);
	n = i;
	while (n > 0 && request->accept[n] != ',' && request->accept[n] != ';')
		--n;
	if (n != 0)
		n += 1;
	while (request->accept[i] != '\0' && request->accept[i] != ',' && request->accept[i] != ';')
		++i;
	if (i != '\0')
		i -= 1;
	if ((file_extension = strndup(&request->accept[n], (i - n + 1))) == NULL)
		return (NULL);
	return (file_extension);
}
