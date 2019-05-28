#include "server.h"
#include "response.h"

char	*protocol_version(t_http *request)
{
	if (!request)
		return (NULL);
	if (request->protocol == 1)
		return ("1.0");
	else if (request->protocol == 2)
		return ("1.1");
	else
		return ("2.0");
}
