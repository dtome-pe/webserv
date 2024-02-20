NAME = webserv

#FILES = main.cpp Socket.cpp handle_client.cpp start_aux.cpp utils_print.cpp parse_config.cpp \
		utils_list_socket.cpp parse_aux.cpp utils_list_server.cpp Server.cpp poll.cpp Response.cpp

FILES = main  Socket  handle_client  start_aux  Server  poll  \
		Response  Request  HeaderHTTP  ConfFile find_serv_block utils_ip \
		Locations find_loc_block response_utils

SRC = $(addprefix $(DIR_SRC), $(addsuffix .cpp, $(FILES)))
FLAGS = -std=c++98 -Wall -Wextra -Werror -Iinc/
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
