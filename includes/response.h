#ifndef RESPONSE_H
# define RESPONSE_H

# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

# include "libft.h"
# include "http.h"

typedef struct		s_response
{
	char			*complete_path;
	int				reponse;
	char			*protocol;
	int				fd;
	int				file_fd;
	uint64_t		file_size;
	char			*content_type;
	char			*date;
}					t_reponse;

/*
 * Environment variables
*/

# define WEBSITE_FOLDER_PATH "site"
# define ERROR_FOLDER_PATH "error_pages/"

/*
 * SUCCESS
*/

# define OK 200
# define CREATED 201
# define ACCEPTED 202
# define NO_CONTENT 204
# define PARTIAL_CONTENT 206

/*
 * Redirections
*/

# define MULTIPLE_CHOICES 300
# define MOVED_DEF 301
# define MOVED_TMP 302
# define REDIR_DEF 308

/*
 * Request Error
*/

# define BAD_REQUEST 400
# define UNAUTHORIZED 401
# define PAYMENT_REQ 402
# define FORBIDDEN 403
# define NOT_FOUND 404
# define METHOD_NOT_ALLOWED 405
# define REQUEST_TIME_OUT 408
# define ENTITY_TOO_LARGE 413

/*
 * Server Error
*/

# define INTERNAL_SERVER_ERR 500
# define NOT_IMPLEMENTED 501
# define BAD_GATEWAY 502
# define SERVICE_UNAVAILABLE 503

/*
 * Response function definition
*/

int		response(t_http *request, int fd);
char	*protocol_version(t_http *request);
int		check_content_type(t_http *request, char *complete_path);
char	*get_content_type(t_http *request, char *complete_path);
int		create_partial_answer(int fd, t_http *data, int reponse);

#endif /* RESPONSE_H */
