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
	struct tm tm = *localtime(&t);

	t = time(NULL);
	if ((date = malloc(sizeof(char) * 30)) == NULL)
		return (NULL);
	bzero(date, sizeof(31));
	sprintf(date, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return (date);
}

/*
 * Freeing response structure, not implemented yet
*/

static int			reponse_free(t_reponse *answer)
{
	if (answer == NULL)
		return (0);
	if (answer->complete_path)
		free(answer->complete_path);
	if (answer->protocol)
		free(answer->protocol);
	if (answer->content_type)
		free(answer->content_type);
	if (answer->date)
		free(answer->date);
	free(answer);
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
		dprintf(answer->fd, "HTTP/%s ", answer->protocol);
	if (answer->reponse)
		dprintf(answer->fd, "%d %s\r\n", answer->reponse, get_reponse_message(answer->reponse));
	if (answer->date)
		dprintf(answer->fd, "Date: %s\r\n", answer->date);
	if (answer->date)
		dprintf(answer->fd, "Last Modified: %s\r\n", answer->date);
	if (answer->content_type)
		dprintf(answer->fd, "Content-Type: %s\r\n", answer->content_type);
	if (answer->file_size)
		dprintf(answer->fd, "Content-Length: %lld\r\n\r\n", answer->file_size);
	while ((size = read(answer->file_fd, buff, PAGE_SIZE)) > 0)
		write(answer->fd, buff, size);
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
	answer->protocol = strdup(protocol_version(request));
	answer->fd = fd;
	sprintf(str, "%d", reponse);
	if ((answer->complete_path = concat(ERROR_FOLDER_PATH, str)) == NULL)
		return (write_connection_error(answer));
	if ((answer->complete_path = concat(answer->complete_path, ".html")) == NULL)
		return (write_connection_error(answer));
	if ((answer->file_fd = open(answer->complete_path, O_RDONLY)) < 0)
		return (write_connection_error(answer));
	if ((get_file_content(&answer->file_size, answer->complete_path)) < 0)
		return (write_connection_error(answer));
	if (check_content_type(request, answer->complete_path) != 0)
		return (write_connection_error(answer));
	if ((answer->content_type = get_content_type(request, answer->complete_path)) == NULL)
		return (write_connection_error(answer));
	if ((answer->date = get_date()) == NULL)
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
		dprintf(answer->fd, "HTTP/%s ", answer->protocol);
	if (answer->reponse)
		dprintf(answer->fd, "%d %s\r\n", answer->reponse, get_reponse_message(answer->reponse));
	if (answer->date)
		dprintf(answer->fd, "Date: %s\r\n", answer->date);
	if (answer->date)
		dprintf(answer->fd, "Last Modified: %s\r\n", answer->date);
	if (answer->content_type)
		dprintf(answer->fd, "Content-Type: %s\r\n", answer->content_type);
	if (answer->file_size)
		dprintf(answer->fd, "Content-Length: %lld\r\n\r\n", answer->file_size);
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
	answer->protocol = strdup(protocol_version(request));
	answer->fd = fd;
	if ((strcmp(request->path, "/") == 0) && ((answer->complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL))
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, answer));
	else if ((answer->complete_path = concat(WEBSITE_FOLDER_PATH, request->path)) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, answer));

	if (strcmp(request->path, "/") == 0)
		if ((answer->complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL)
			return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, answer));
	if ((answer->file_fd = open(answer->complete_path, O_RDONLY)) < 0)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, answer));
	if ((get_file_content(&answer->file_size, answer->complete_path)) < 0)
		return (end_connection_error(request, SERVICE_UNAVAILABLE, fd, answer));
	if (check_content_type(request, answer->complete_path) != 0)
		return (end_connection_error(request, BAD_REQUEST, fd, answer));
	if ((answer->content_type = get_content_type(request, answer->complete_path)) == NULL)
		return (end_connection_error(request, BAD_REQUEST, fd, answer));
	if ((answer->date = get_date()) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, answer));
	write_connection_success(answer);
	reponse_free(answer);
	return (reponse);
}

/*
 * Main thread for every call to the server, will transform the request to
 * a reponse after having perfeomed it
*/

int		response(t_http *request, int fd)
{
	struct stat		sb;
	t_reponse		*answer;

	if (!request)
		return (end_connection_error(request, BAD_REQUEST, fd, NULL));
	if ((answer = reponse_init()) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd, NULL));
	answer->fd = fd;
	if ((request->method < 0 || request->method > 4))
		return (end_connection_error(request, NOT_IMPLEMENTED, fd, answer));
	if ((!request->path || stat(concat(WEBSITE_FOLDER_PATH, request->path), &sb) == -1))
		return (end_connection_error(request, NOT_FOUND, fd, answer));
	return (end_connection_success(request, OK, fd, answer));
}
