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
	uint8_t			code;
	uint8_t			protocol;
	uint32_t		header_size;
	uint32_t		content_length;
	uint8_t			*content;
}					t_reponse;

/*
 * Environment variables
*/

# define WEBSITE_FOLDER_PATH "site"

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

#endif /* RESPONSE_H */
