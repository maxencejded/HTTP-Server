#include "server.h"
#include "api.h"
#include "response.h"

#include <time.h>
#include <fcntl.h>

/*
** Return value of the different method implemented
** Get method -> 1
** Head method -> 2
** Post method -> 3
** Put method -> 4
**
** Ip of the server of dev:
** 127.0.0.1:6060
**
** Function to get the current date for the response header
** In case of malloc error will return an error 500
** Returns an allocated char *
*/

char				*get_date(void)
{
	char			*date;
	time_t			t;
	struct tm		*tm;

	time(&t);
	tm = localtime(&t);
	if ((date = malloc(sizeof(char) * 30)) == NULL)
		return (NULL);
	memset(date, 0, sizeof(char) * 30);
	sprintf(date, "%d-%d-%d %d:%d:%d", tm->tm_year + 1900, tm->tm_mon + 1,
			tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return (date);
}

/*
** Initializing response structure not in use right now
*/

t_reponse			*reponse_init(void)
{
	t_reponse	*answer;

	if ((answer = (t_reponse *)malloc(sizeof(t_reponse))) == NULL)
		return (NULL);
	memset(answer, 0, sizeof(t_reponse));
	answer->file_fd = -1;
	return (answer);
}

/*
** Print the reponse message linked to the value of the reponse sent
*/

t_status			g_status[STATUS_COUNT] = {

	{200, "Ok"},
	{201, "Created"},
	{202, "Accepted"},
	{204, "No-content"},
	{206, "Partial content"},

/*
** Error messages for error 300
*/

	{300, "Multiple choices"},
	{301, "Moved permanently"},
	{302, "Found"},
	{308, "Permanently redirected"},

/*
** Error messages for error 400
*/

	{400, "Bad request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not found"},
	{405, "Method not allowed"},
	{408, "Request timed out"},
	{413, "Entity too large"},

/*
** Error messages for error 500
*/

	{500, "Internal server error"},
	{501, "Not implemented"},
	{502, "Bad gateway"},
	{503, "Service unavailable"},

};

char				*get_reponse_message(int reponse)
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
** Checking if the called URL is not present in the API folder
*/

t_api				g_api[API_COUNT] = {
	{"demo", &demo}
};

static int			start_api_response(t_http *request, t_reponse *answer)
{
	char			*concatted;
	char			*complete_path;
	struct stat		sb;
	int				n;
	int				i;

	concatted = concat(API_FOLDER_PATH, request->path);
	complete_path = concat(concatted, ".c");
	if ((!concatted || !complete_path || stat(complete_path, &sb) == -1)
			&& ft_free(complete_path) == 0 && ft_free(concatted) == 0)
		return (end_connection_error(request, IER, answer->fd, answer));
	ft_free(complete_path);
	n = strlen(concatted);
	while (n > 0 && concatted[n] != '/')
		--n;
	n += 1;
	i = -1;
	while (++i < API_COUNT)
		if (strcmp(&concatted[n], g_api[i].name) == 0)
			answer->reponse = g_api[i].fct(request, answer);
	return (answer->reponse);
}

/*
** Main thread for every call to the server, will transform the request to
** a reponse after having perfeomed it
** Returns an int matching the response answer.
*/

int					response(t_http *request, int fd)
{
	struct stat		sb;
	t_reponse		*answer;
	char			*concatted;

	if (!request)
		return (end_connection_error(request, BAD_REQUEST, fd, NULL));
	if ((answer = reponse_init()) == NULL)
		return (end_connection_error(request, IER, fd, NULL));
	answer->fd = fd;
	if (request->method == 0 || request->method > METHOD_PUT)
		return (end_connection_error(request, NOT_IMPLEMENTED, fd, answer));
	concatted = concat(WEBSITE_FOLDER_PATH, request->path);
	if ((!request->path || stat(concatted, &sb) == -1) && ft_free(concatted)
			== 0)
		return (start_api_response(request, answer));
	ft_free(concatted);
	return (end_connection_success(request, OK, fd, answer));
}
