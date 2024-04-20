NAME = webserv

FILES = main  Socket SocketGetSet Server   \
		Response ResponseUtils  Request RequestGetSet HeaderHTTP  ConfFile utils cgi \
		Location response_utils cgiUtils pollUtils Cluster MIME addToClientUtils

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
