#include "server.h"
#include "response.h"

/*
 * Get method -> 1
 * Head method -> 2
 * Post method -> 3
 * Put method -> 4
*/

/*
 * 127.0.0.1:6060
*/

/*
 * Initializing response structure
*/

static t_reponse	*reponse_init(void)
{
	t_reponse	*answer;

	if ((answer = (t_reponse *)malloc(sizeof(t_reponse))) == NULL)
		return (NULL);
	bzero(answer, sizeof(t_reponse));
	return (answer);
}

/*
 * Handling bad responses closing the connection
*/

static int			end_connection_error(t_http *request, int reponse, int fd)
{
	size_t			content_length;
	char			*message;

	content_length = snprintf(NULL, 0, "HTTP/%s %d OK\nContent-Type: text/plain\nConnection: close\nContent-Length: %lu\n\nError People!", protocol_version(request), reponse, strlen("Error People!"));
	if ((message = malloc(sizeof(char) * (content_length + 1))) == NULL)
		return (end_connection_error(request, 500, fd));
	bzero(message, sizeof(char) * (content_length + 1));
	sprintf(message, "HTTP/%s %d OK\nContent-Type: text/plain\nConnection: close\nContent-Length: %lu\n\nError People!", protocol_version(request), reponse, strlen("Error People!"));
	write(fd, message, content_length);
	strdel(&message);
	return (reponse);
}

/*
 * Handling succesful responses and closing the connection
*/

static int			end_connection_success(t_http *request, int reponse, int fd)
{
	char	*complete_path;

	if (strcmp(request->path, "/") == 0
	&& (complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL)
		return (end_connection_error(request, 500, fd));
	else if ((complete_path = concat(WEBSITE_FOLDER_PATH, request->path)) == NULL)
		return (end_connection_error(request, 500, fd));

	dprintf(fd, "HTTP/%s %d OK\nContent-Type: text/plain\nConnection: close\nContent-Length: 16\n\nSuccess People!\n", protocol_version(request), reponse);
	return (reponse);
}

/*
 * Freeing response structure
*/

static void			reponse_free(t_reponse *answer)
{
	if (answer == NULL)
		return ;
	if (answer && answer->content)
		free(answer->content);
	if (answer)
		free(answer);
	return ;
}

/*
 * Main thread for every call to the server, will transform the request to
 * a reponse after having perfeomed it
*/

int		response(t_http *request, int fd)
{
	struct stat		sb;
	t_reponse		*answer;

	(void)reponse_free;
	if (!request)
		return (end_connection_error(request, BAD_REQUEST, fd));
	if ((answer = reponse_init()) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	if (request->method < 0 || request->method > 4)
		return (end_connection_error(request, NOT_IMPLEMENTED, fd));
	if (!request->path || stat(concat(WEBSITE_FOLDER_PATH, request->path), &sb) == -1)
		return (end_connection_error(request, NOT_FOUND, fd));
	return (end_connection_success(request, OK, fd));
}
