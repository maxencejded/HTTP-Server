CC			= gcc
RM			= rm -f
SERVER		= server
CFLAGS		= -Wall -Wextra -Werror -g
LIBFT 		= libft/libft.a
INCLUDES	= -I includes\
			  -I libft/includes

BASE		= server.c
COMMON		= socket.c header.c header_method.c
PRINT		= print_memory.c

FUNCTIONS	= $(addprefix srcs/, $(BASE) $(COMMON) $(PRINT))
OBJECTS		= $(FUNCTIONS:.c=.o)

.PHONY: all $(SERVER) clean fclean re

all: $(SERVER)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	@make -C libft

$(SERVER): $(LIBFT) $(OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER) $(OBJECTS) $(LIBFT)

clean:
	@$(RM) $(OBJECTS)
	@make clean -C libft

fclean: clean
	@$(RM) $(SERVER)
	@make fclean -C libft

re: fclean all
