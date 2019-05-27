CC			= gcc
RM			= rm -f
SERVER		= server
CFLAGS		= -Wall -Wextra -Werror -g
INCLUDES	= -I includes

BASE		= server.c
COMMON		= socket.c header.c header_method.c
PRINT		= print_memory.c
LIBFT		= strsplit.c strdel.c ptrdel.c

LIBFT_FCT	= $(addprefix libft/, $(LIBFT))
FUNCTIONS	= $(addprefix srcs/, $(BASE) $(COMMON) $(PRINT) $(LIBFT_FCT))
OBJECTS		= $(FUNCTIONS:.c=.o)

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
