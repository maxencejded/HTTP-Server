CC				= gcc
RM				= rm -f
SERVER			= server
CFLAGS			= -Wall -Wextra -Werror -g
INCLUDES		= -I includes

BASE			= server.c socket.c receive.c signal.c
REQUEST			= request.c method.c
FIELDS			= fields.c fields_first.c fields_second.c fields_sixth.c fields_fifth.c\
					fields_fourth.c fields_seventh.c fields_third.c
RESPONSE		= response.c protocol.c content_type.c
LIBFT			= strsplit.c strdel.c ptrdel.c print_memory.c concat.c get_file_content.c

REQUEST_FCT		= $(addprefix request/, $(REQUEST) $(FIELDS))
RESPONSE_FCT	= $(addprefix response/, $(RESPONSE))
LIBFT_FCT		= $(addprefix libft/, $(LIBFT))
FUNCTIONS		= $(addprefix srcs/, $(BASE) $(REQUEST_FCT) $(RESPONSE_FCT) $(LIBFT_FCT))
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
