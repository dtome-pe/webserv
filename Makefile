NAME = webserv

FILES = main  Socket Server   \
		Response  Request  HeaderHTTP  ConfFile find_serv_block utils cgi \
		Location find_loc_block response_utils cgi_utils poll_utils Cluster

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
	c++ $(FLAGS)  $(OBJ) -o $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp Makefile 
	c++ $(FLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -fr $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP)
