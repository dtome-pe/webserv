NAME = webserv
SRC = $(addprefix $(DIR_SRC), main.cpp Server.cpp handle_client.cpp start_aux.cpp print_utils.cpp parse_config.cpp \
								utils_list_server.cpp parse_aux.cpp poll.cpp)
FLAGS = -std=c++98 -Wall -Wextra -Werror
DEPFLAGS = -MMD -MP
DIR_OBJ = tmp/
DIR_SRC = src/
OBJ = $(addprefix $(DIR_OBJ), $(notdir $(SRC:.cpp=.o)))
DEP = $(OBJ:.o=.d)

all: $(DIR_OBJ) $(NAME)

$(DIR_OBJ):
	mkdir -p $@

$(NAME): $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp Makefile 
	c++ $(FLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -fr $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP)