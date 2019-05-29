#include "server.h"
#include "response.h"

char	*protocol_version(uint8_t protocol)
{
	if (protocol == 1)
		return ("1.0");
	else if (protocol == 2)
		return ("1.1");
	else
		return ("2.0");
}
