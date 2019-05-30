#include "server.h"
#include "response.h"

#include <time.h>
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

static char			*get_date()
{
	char			*date;
	time_t			t;
	struct tm		*tm;
	
	time(&t);
	tm = localtime(&t);
	if ((date = malloc(sizeof(char) * 30)) == NULL)
		return (NULL);
	bzero(date, sizeof(char) * 30);
	sprintf(date, "%d-%d-%d %d:%d:%d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return (date);
}

/*
 * Freeing response structure
*/

static int			reponse_free(t_reponse *answer)
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

static int			ft_free(char *to_free)
{
	if (to_free)
		free(to_free);
	return (0);
}

/*
 * Initializing response structure not in use right now
*/

static t_reponse	*reponse_init(void)
{
	t_reponse	*answer;

	if ((answer = (t_reponse *)malloc(sizeof(t_reponse))) == NULL)
		return (NULL);
	bzero(answer, sizeof(t_reponse));
	answer->file_fd = -1;
	return (answer);
}

/*
 * print the reponse message linked to the value
*/

static char		*get_reponse_message(int reponse)
{
	if (reponse < 300)
		return ("Success connecting");
	else if (reponse < 400)
		return ("Redirection");
	else if (reponse < 500)
		return ("Request Error");
	else
		return ("Server Error");
}

/*
 * Function to write the content of the error page linked to the error in 
 * case of unsuccesful connection
*/

static int			write_connection_error(t_reponse *answer)
{
	int		size;
	int		reponse;
	char	buff[PAGE_SIZE];

	if (answer->protocol)
		dprintf(answer->fd, "HTTP/%s ", protocol_version(answer->protocol));
	if (answer->reponse)
		dprintf(answer->fd, "%d %s\r\n", answer->reponse, get_reponse_message(answer->reponse));
	dprintf(answer->fd, "Connexion: close\r\n");
	if (answer->date)
		dprintf(answer->fd, "Date: %s\r\n", answer->date);
	if (answer->date)
		dprintf(answer->fd, "Last-Modified: %s\r\n", answer->date);
	if (answer->content_type)
		dprintf(answer->fd, "Content-Type: %s\r\n", answer->content_type);
	if (answer->file_size)
		dprintf(answer->fd, "Content-Length: %lu\r\n\r\n", (long unsigned int)answer->file_size);
	if (answer->file_fd != -1)
	{
		while ((size = read(answer->file_fd, buff, PAGE_SIZE)) > 0)
			write(answer->fd, buff, size);
	}
	reponse = answer->reponse;
	reponse_free(answer);
	return (reponse);
}

/*
 * Handling bad responses closing the connection
 * Return the error status implemented
*/

static int			end_connection_error(t_http *request, int reponse, int fd, t_reponse *answer)
{
	char			str[4];

	answer->reponse = reponse;
	answer->protocol = (request == NULL) ? 1 : request->protocol;
	answer->fd = fd;
	sprintf(str, "%d", reponse);
	if ((answer->complete_path = concat(ERROR_FOLDER_PATH, str)) == NULL && http_free(request) == 1)
		return (write_connection_error(answer));
	if ((answer->complete_path = concat(answer->complete_path, ".html")) == NULL && http_free(request) == 1)
		return (write_connection_error(answer));
	if ((answer->file_fd = open(answer->complete_path, O_RDONLY)) < 0 && http_free(request) == 1)
		return (write_connection_error(answer));
	if ((get_file_content(&answer->file_size, answer->complete_path)) < 0 && http_free(request) == 1)
		return (write_connection_error(answer));
	if (check_content_type(request, answer->complete_path) != 0 && http_free(request) == 1)
		return (write_connection_error(answer));
	if ((answer->content_type = get_content_type(request, answer->complete_path)) == NULL && http_free(request) == 1)
		return (write_connection_error(answer));
	if ((answer->date = get_date()) == NULL && http_free(request) == 1)
		return (write_connection_error(answer));
	write_connection_error(answer);
	return (reponse);
}

/*
 * Function to write the content to the fd of the socket in case of 
 * succesful connection
*/

static void			write_connection_success(t_reponse *answer)
{
	int		size;
	char	buff[PAGE_SIZE];

	if (answer->protocol)
		dprintf(answer->fd, "HTTP/%s ", protocol_version(answer->protocol));
	if (answer->reponse)
		dprintf(answer->fd, "%d %s\r\n", answer->reponse, get_reponse_message(answer->reponse));
	dprintf(answer->fd, "Connexion: close\r\n");
	if (answer->date)
		dprintf(answer->fd, "Date: %s\r\n", answer->date);
	if (answer->date)
		dprintf(answer->fd, "Last-Modified: %s\r\n", answer->date);
	if (answer->content_type)
		dprintf(answer->fd, "Content-Type: %s\r\n", answer->content_type);
	if (answer->file_size)
		dprintf(answer->fd, "Content-Length: %lu\r\n\r\n", (long unsigned int)answer->file_size);
	while ((size = read(answer->file_fd, buff, PAGE_SIZE)) > 0)
		write(answer->fd, buff, size);
}

/*
 * Handling succesful responses and closing the connection
 * In case of error, will link the connection success to the connection error 
 * function.
*/

static int			end_connection_success(t_http *request, int reponse, int fd, t_reponse *answer)
{
	answer->reponse = reponse;
	answer->protocol = (request == NULL) ? 1 : request->protocol;
	answer->fd = fd;
	ft_free(answer->complete_path);
	if ((answer->complete_path = concat(WEBSITE_FOLDER_PATH, request->path)) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERROR, fd, answer));
	if (!request->path || request->path[0] == '\0' || strcmp(request->path, "/") == 0)
		if (ft_free(answer->complete_path) == 0 && (answer->complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL)
			return (end_connection_error(request, INTERNAL_SERVER_ERROR, fd, answer));
	if ((answer->file_fd = open(answer->complete_path, O_RDONLY)) < 0)
		return (end_connection_error(request, INTERNAL_SERVER_ERROR, fd, answer));
	if ((get_file_content(&answer->file_size, answer->complete_path)) < 0)
		return (end_connection_error(request, SERVICE_UNAVAILABLE, fd, answer));
	if (check_content_type(request, answer->complete_path) != 0)
		return (end_connection_error(request, BAD_REQUEST, fd, answer));
	if ((answer->content_type = get_content_type(request, answer->complete_path)) == NULL)
		return (end_connection_error(request, BAD_REQUEST, fd, answer));
	if ((answer->date = get_date()) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERROR, fd, answer));
	write_connection_success(answer);
	reponse_free(answer);
	http_free(request);
	return (reponse);
}

/*
 * Function which can be called by the server to print an error
*/

int				create_partial_answer(int fd, t_http *request, int reponse)
{
	t_reponse	*answer;

	if (reponse == INTERNAL_SERVER_ERROR || (answer = reponse_init()) == NULL)
	{
		dprintf(fd, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
		close (fd);
		return (INTERNAL_SERVER_ERROR);
	}
	answer->fd = fd;
	answer->reponse = reponse;
	answer->protocol = (request == NULL) ? 1 : request->protocol;
	answer->date = get_date();
	http_free(request);
	return (write_connection_error(answer));
}

/*
 * Main thread for every call to the server, will transform the request to
 * a reponse after having perfeomed it
*/

int		response(t_http *request, int fd)
{
	struct stat		sb;
	t_reponse		*answer;
	char			*concatted;

	if (!request)
		return (end_connection_error(request, BAD_REQUEST, fd, NULL));
	if ((answer = reponse_init()) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERROR, fd, NULL));
	answer->fd = fd;
	if (request->method == 0 || request->method > METHOD_PUT)
		return (end_connection_error(request, NOT_IMPLEMENTED, fd, answer));
	concatted = concat(WEBSITE_FOLDER_PATH, request->path);
	if ((!request->path || stat(concatted, &sb) == -1) && ft_free(concatted) == 0)
		return (end_connection_error(request, NOT_FOUND, fd, answer));
	ft_free(concatted);
	return (end_connection_success(request, OK, fd, answer));
}
