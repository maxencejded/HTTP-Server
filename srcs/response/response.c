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
 * Allowing me to free a char* while returning an int
 * Mostly used in error handling to free the complete path
*/

static int		ft_free(void *path)
{
	if (path)
		free(path);
	return (0);
}

/*
 * Function to write the content of the error page linked to the error in 
 * case of unsuccesful connection
*/

static int			write_connection_error(char *protocol, int reponse, char *date, char *content_type, uint64_t file_size, int file_fd, int fd)
{
	int		size;
	char	buff[4096];

	dprintf(fd, "HTTP/%s %d Error\r\nDate: %s\r\nServer: Mine\r\nLast Modified: %s\r\nContent-Type: %s\r\nConnection: close\r\nContent-Length: %lld\r\n\r\n", protocol, reponse, date, date, content_type, file_size - 1);
	while ((size = read(file_fd, buff, 4096)) > 0)
		write(fd, buff, size);
	return (reponse);
}

/*
 * Handling bad responses closing the connection
 * Return the error status implemented
*/

static int			end_connection_error(t_http *request, int reponse, int fd)
{
	char			*complete_path;
	int				file_fd;
	uint64_t		file_size;
	uint8_t			*content;
	char			*content_type;
	char			*date;
	char			str[12];
	
	sprintf(str, "%d", reponse);
	if ((complete_path = concat(ERROR_FOLDER_PATH, str)) == NULL)
		return (write_connection_error(protocol_version(request), INTERNAL_SERVER_ERR, NULL, NULL, 0, -1, fd));
	if ((complete_path = concat(complete_path, ".html")) == NULL)
		return (write_connection_error(protocol_version(request), INTERNAL_SERVER_ERR, NULL, NULL, 0, -1, fd));
	if ((file_fd = open(complete_path, O_RDONLY)) < 0 && ft_free(complete_path) == 0)
		return (write_connection_error(protocol_version(request), NOT_FOUND, NULL, NULL, 0, -1, fd));
	if ((content = get_file_content(&file_size, complete_path)) == NULL && ft_free(complete_path) == 0)
		return (write_connection_error(protocol_version(request), NOT_FOUND, NULL, NULL, 0, -1, fd));
	if (check_content_type(request, complete_path) != 0 && ft_free(complete_path) == 0 && ft_free(content) == 0)
		return (write_connection_error(protocol_version(request), NOT_FOUND, NULL, NULL, 0, -1, fd));
	if ((content_type = get_content_type(request, complete_path)) == NULL && ft_free(complete_path) == 0 && ft_free(content) == 0)
		return (write_connection_error(protocol_version(request), NOT_FOUND, NULL, NULL, 0, -1, fd));
	if ((date = get_date()) == NULL && ft_free(complete_path) == 0 && ft_free(content) == 0 && ft_free(content_type) == 0)
		return (write_connection_error(protocol_version(request), NOT_FOUND, NULL, NULL, 0, -1, fd));
	write_connection_error(protocol_version(request), reponse, date, content_type, file_size, file_fd, fd);
	free(complete_path);
	free(content);
	free(content_type);
	free(date);
	return (reponse);
}

/*
 * Function to write the content to the fd of the socket in case of 
 * succesful connection
*/

static void			write_connection_success(char *protocol, int reponse, char *date, char *content_type, uint64_t file_size, int file_fd, int fd)
{
	int		size;
	char	buff[4096];

	dprintf(fd, "HTTP/%s %d OK\r\nDate: %s\r\nServer: Mine\r\nLast Modified: %s\r\nContent-Type: %s\r\nConnection: close\r\nContent-Length: %lld\r\n\r\n", protocol, reponse, date, date, content_type, file_size - 1);
	while ((size = read(file_fd, buff, 4096)) > 0)
		write(fd, buff, size);
}

/*
 * Handling succesful responses and closing the connection
 * In case of error, will link the connection success to the connection error 
 * function.
*/

static int			end_connection_success(t_http *request, int reponse, int fd)
{
	char		*complete_path;
	int			file_fd;
	uint64_t	file_size;
	uint8_t		*content;
	char		*content_type;
	char		*date;

	if ((strcmp(request->path, "/") == 0) && ((complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL))
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	else if ((complete_path = concat(WEBSITE_FOLDER_PATH, request->path)) == NULL)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));

	if (strcmp(request->path, "/") == 0)
		if ((complete_path = concat(WEBSITE_FOLDER_PATH, "/index.html")) == NULL)
			return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	if ((file_fd = open(complete_path, O_RDONLY)) < 0 && ft_free(complete_path) == 0)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	if ((content = get_file_content(&file_size, complete_path)) == NULL && ft_free(complete_path) == 0)
		return (end_connection_error(request, SERVICE_UNAVAILABLE, fd));
	if (check_content_type(request, complete_path) != 0 && ft_free(complete_path) == 0 && ft_free(content) == 0)
		return (end_connection_error(request, BAD_REQUEST, fd));
	if ((content_type = get_content_type(request, complete_path)) == NULL && ft_free(complete_path) == 0 && ft_free(content) == 0)
		return (end_connection_error(request, BAD_REQUEST, fd));
	if ((date = get_date()) == NULL && ft_free(complete_path) == 0 && ft_free(content) == 0 && ft_free(content_type) == 0)
		return (end_connection_error(request, INTERNAL_SERVER_ERR, fd));
	write_connection_success(protocol_version(request), reponse, date, content_type, file_size, file_fd, fd);
	free(complete_path);
	free(content);
	free(content_type);
	free(date);
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
