CC				= gcc
RM				= rm -f
SERVER			= server
CFLAGS			= -Wall -Wextra -Werror -g
INCLUDES		= -I includes

BASE			= server.c socket.c receive.c signal.c
REQUEST			= request.c method.c post_content.c post_form.c post_multipart.c

FIELD			= field_accept.c field_content_length.c field_content_type.c\
					field_expect.c field_host.c
FIELDS			= fields.c $(FIELD)

RESPONSE		= response.c protocol.c content_type.c ft_free.c\
					response_error.c response_success.c

QUEUE			= queueInit.c nodeInit.c queueFree.c dequeue.c enqueue.c\
					isEmpty.c peek.c
LIBFT			= strdel.c concat.c get_file_content.c $(addprefix queue/, $(QUEUE))
API				= demo.c

REQUEST_FCT		= $(addprefix request/, $(REQUEST) $(FIELDS))
RESPONSE_FCT	= $(addprefix response/, $(RESPONSE))
LIBFT_FCT		= $(addprefix libft/, $(LIBFT))
API_FCT			= $(addprefix api/, $(API))
FUNCTIONS		= $(addprefix srcs/, $(BASE) $(REQUEST_FCT) $(RESPONSE_FCT)\
				  $(LIBFT_FCT)) $(API_FCT)
OBJECTS			= $(FUNCTIONS:.c=.o)

.PHONY: all $(SERVER) clean fclean re

all: $(SERVER)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(SERVER): $(OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER) $(OBJECTS)

clean:
	@$(RM) $(OBJECTS)

fclean: clean
	@$(RM) $(SERVER)

re: fclean all
