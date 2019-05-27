CC				= gcc
RM				= rm -f
SERVER			= server
CFLAGS			= -Wall -Wextra -Werror -g
INCLUDES		= -I includes

BASE			= server.c socket.c
REQUEST			= request.c method.c
RESPONSE		=
LIBFT			= strsplit.c strdel.c ptrdel.c print_memory.c

REQUEST_FCT		= $(addprefix request/, $(REQUEST))
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
