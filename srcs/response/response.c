#include "server.h"
#include "response.h"

#include <fcntl.h>

/*
 * Return value of the different method implemented
 * Get method -> 1
 * Head method -> 2
 * Post method -> 3
 * Put method -> 4
*/

/*
 * Ip of the server of dev:
 * 127.0.0.1:6060
*/

/*
 * Initializing response structure not in use right now
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
 * Return the error status implemented
*/

static int			end_connection_error(t_http *request, int reponse, int fd)
{
	size_t			content_length;
	char			*message;

	content_length = snprintf(NULL, 0, "HTTP/%s %d OK\nContent-Type: text/plain\nConnection: close\nContent-Length: %lu\n\nError People!", protocol_version(request), reponse, strlen("Error People!"));
	if ((message = malloc(sizeof(char) * (content_length + 1))) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	bzero(message, sizeof(char) * (content_length + 1));
	sprintf(message, "HTTP/%s %d OK\nContent-Type: text/plain\nConnection: close\nContent-Length: %lu\n\nError People!", protocol_version(request), reponse, strlen("Error People!"));
	write(fd, message, content_length);
	strdel(&message);
	return (reponse);
}

/*
 * Allowing me to free a char* while returning an int
 * Mostly used in error handling to free the complete path
*/

static int		ft_free(char *path)
{
	free(path);
	return (0);
}

/*
 * Handling succesful responses and closing the connection
 * In case of error, will link the connection success to the connection error 
 * function.
*/

static int			end_connection_success(t_http *request, int reponse, int fd)
{
	char	*complete_path;
	int		file_fd;
	int		file_size;
	char	*content;
	char	*content_type;

	if ((strcmp(request->path, "/") == 0) && ((complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL))
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	else if ((complete_path = concat(WEBSITE_FOLDER_PATH, request->path)) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));

	if (strcmp(request->path, "/") == 0)
		if ((complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL)
			return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	if ((file_fd = open(complete_path, O_RDONLY)) < 0 && ft_free(complete_path) == 0)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	if ((content = get_file_content(file_fd, &file_size, complete_path)) == NULL && ft_free(complete_path) == 0)
		return (end_connection_error(request, SERVICE_UNAVAILABLE, fd));
	if (check_content_type(request, complete_path) != 0 && ft_free(complete_path) == 0)
		return (end_connection_error(request, BAD_REQUEST, fd));
	if ((content_type = get_content_type(request, complete_path)) == NULL)
		return (end_connection_error(request, BAD_REQUEST, fd));
	printf("HTTP/%s %d OK\nContent-Type: %s\n", protocol_version(request), reponse, content_type);
	dprintf(fd, "HTTP/%s %d OK\nContent-Type: %s\nConnection: close\nContent-Length: %d\n\n%s", protocol_version(request), reponse, content_type, file_size, content);
	free(complete_path);
	free(content_type);
	return (reponse);
}

/*
 * Freeing response structure, not implemented yet
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
