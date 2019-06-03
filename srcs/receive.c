#include "server.h"

/*
** Read the content send through the header.
** If successful, return OK. Otherwise, an appropriate code error is returned.
*/

// static int		request_data(int fd, t_http *data, uint8_t *str, ssize_t size)
// {
// 	uint8_t		buff[PAGE_SIZE];
// 	uint8_t		*content;
// 	ssize_t		total;

// 	total = size;
// 	data->content = (uint8_t *)malloc(sizeof(uint8_t) * data->content_length);
// 	if (data->content == NULL)
// 		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
// 	memset(data->content, 0, sizeof(uint8_t) * data->content_length);
// 	memcpy(data->content, str, size);
// 	content = data->content + size;
// 	if (total < data->content_length)
// 	{
// 		while ((size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
// 		{
// 			total += size;
// 			if (total > data->content_length)
// 				break ;
// 			memcpy(content, buff, size);
// 			content = content + size;
// 		}
// 	}
// 	if (total != data->content_length)
// 		return (response_error(fd, data, BAD_REQUEST));
// 	return (ACCEPTED);
// }

#define CACHE_FILENAME "./cache.tmp"

static uint32_t		header_next(uint8_t *str)
{
	uint32_t		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			str[i] = '\0';
			str[i + 1] = '\0';
			return (i + 2);
		}
		++i;
	}
	return (0);
}

static int		get_element(char **str, char *search)
{
	int		i;
	char	*element;
	char	*name;

	if ((element = strstr((const char *)*str, search)) == NULL)
		return (0);
	element = element + strlen(search);
	if (*element == '\"')
		++element;
	i = 0;
	while (element[i] && element[i] != '\"')
		++i;
	name = strndup(element, i);
	printf("NAME: |%s|\n", name);
	free(name);
	if (element[i] == '\"')
		++i;
	*str = element + i;
	return (1);
}

static int		parse_boundary(uint8_t *buff)
{
	char		*str;

	str = (char *)buff;
	if (get_element(&str, "name=") == 0)
		return (0);
	if (*str == '\0')
		return (2);
	if (get_element(&str, "filename=") == 0)
		return (0);
	return (3);
}

static int		read_file(int fd, char *boundary)
{
	size_t		size;
	uint8_t		buff[PAGE_SIZE];
	uint8_t		*line;

	int fd_new = open("file.copy", O_RDWR | O_CREAT | O_TRUNC, 0660);

	memset(buff, 0, PAGE_SIZE);
	while ((size = read(fd, buff, PAGE_SIZE - 1)) > 0)
	{
		if ((line = (uint8_t *)strstr((const char *)buff, boundary)) != NULL)
			write(fd_new, buff, (line - buff) - 2);
		else
			write(fd_new, buff, size);
	}
	close(fd_new);
	printf("DONE\n");
	return (1);
}

static int		get_line(int fd, t_http *data)
{
	size_t		size;
	size_t		len_boundary;
	uint8_t		buff[PAGE_SIZE];
	uint8_t		*line;

	len_boundary = strlen(data->boundary);
	memset(buff, 0, PAGE_SIZE);
	while ((size = read(fd, buff, PAGE_SIZE - 1)) > 0)
	{
		if ((line = (uint8_t *)strstr((const char *)buff, "\r\n")) != NULL)
		{
			if (strncmp((const char *)buff, data->boundary, len_boundary) == 0)
			{
				line = line + 2;
				uint32_t i = header_next(line);
				int ret = parse_boundary(line);
				line = line + i;
				while (strncmp((const char *)line, "\r\n", 2) != 0)
					line = line + header_next(line);
				line = line + 2;
				lseek(fd, line - buff, SEEK_SET);
				if (ret == 3)
					read_file(fd, data->boundary);
			}
		}
	}
	return (1);
}

static int		request_data(int fd, t_http *data, uint8_t *str, ssize_t size)
{
	uint8_t		buff[PAGE_SIZE];
	size_t		total;
	int			fd_out;

	total = size;
	if ((fd_out = open(CACHE_FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0660)) == -1)
		return (INTERNAL_SERVER_ERROR);
	(size != 0) ? write(fd_out, str, size) : 0;
	while (total < data->content_length && (size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
	{
		total += size;
		if (total > data->content_length)
			break ;
		write(fd_out, buff, size);
	}
	if (lseek(fd_out, 0, SEEK_SET) != 0)
	{
		close(fd_out);
		return (response_error(fd, data, INTERNAL_SERVER_ERROR));
	}
	get_line(fd_out, data);
	close(fd_out);
	if (total != data->content_length)
		return (response_error(fd, data, BAD_REQUEST));
	return (ACCEPTED);
}

/*
** Copy the buffer into a malloced pointer.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		copy(uint8_t *buff, uint8_t **data, uint8_t **end, ssize_t size)
{
	if ((*data = (uint8_t *)malloc(sizeof(uint8_t) * size)) == NULL)
		return (0);
	memcpy(*data, buff, size);
	*end = (uint8_t *)strstr((const char *)*data, "\r\n\r\n");
	memset(*end, 0, sizeof(uint8_t) * 4);
	*end = *end + 4;
	return (1);
}

/*
** Receive the header from the client.
** If successful, the number of bytes actually read is returned.
** Upon reading end-of-file, zero is returned.
** Otherwise, a -1 is returned to indicate an error.
*/

static ssize_t	request_read(int fd, uint8_t **data, uint8_t **end, int *status)
{
	ssize_t		size;
	uint8_t		buff[PAGE_SIZE];

	memset(buff, 0, PAGE_SIZE);
	if ((size = recv(fd, buff, (PAGE_SIZE - 1), 0)) > 0)
	{
		write(1, buff, size);
		if (strstr((const char *)buff, "\r\n\r\n") == NULL)
		{
			*status = response_error(fd, NULL, ENTITY_TOO_LARGE);
			return (-1);
		}
	}
	else
	{
		if (size == 0)
			*status = response_error(fd, NULL, NO_CONTENT);
		else
			*status = response_error(fd, NULL, REQUEST_TIME_OUT);
		return (-1);
	}
	if (copy(buff, data, end, size) == 0)
		*status = response_error(fd, NULL, INTERNAL_SERVER_ERROR);
	if (*status != 200)
		return ((size == 0) ? 0 : -1);
	return (size);
}

/*
** Build the http header and send the appropriate response to the client.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
**
** DEBUG: write(1, buf, (end - buf));
*/

int				receive(int fd, int *status)
{
	ssize_t		size;
	uint8_t		*buf;
	uint8_t		*end;
	t_http		*data;

	buf = NULL;
	if ((size = request_read(fd, &buf, &end, status)) <= 0)
		return (0);
	if ((data = header(fd, (char *)buf, status)) == NULL)
		return (0);
	if (data && data->method == METHOD_POST && data->content_length)
	{
		*status = request_data(fd, data, end, size - (end - buf));
		if (*status != ACCEPTED)
		{
			free(buf);
			return (0);
		}
	}
	free(buf);
	*status = response(data, fd);
	return (1);
}
