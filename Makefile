NAME = webserv
SRC = $(addprefix $(DIR_SRC), main.cpp Server.cpp ip_addr_utils.cpp handle_client.cpp)
FLAGS = -std=c++98 -Wall -Wextra -Werror
DEPFLAGS = -MMD -MP
DIR_OBJ = tmp/
DIR_SRC = src/
LIBFT_DIR	= libft
LIBFT 		= $(LIBFT_DIR)/libft.a
OBJ = $(addprefix $(DIR_OBJ), $(notdir $(SRC:.cpp=.o)))
DEP = $(OBJ:.o=.d)

all: make_libft $(DIR_OBJ) $(NAME)

$(DIR_OBJ):
	mkdir -p $@

$(NAME): $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp Makefile 
	c++ $(FLAGS) $(DEPFLAGS) -c $< -o $@

make_libft:
	@make -C $(LIBFT_DIR)

clean:
	rm -fr $(DIR_OBJ)
	@make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	@make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP)