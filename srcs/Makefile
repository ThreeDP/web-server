# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 15:04:07 by dapaulin          #+#    #+#              #
#    Updated: 2023/11/03 19:30:27 by dapaulin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


######################## CONFIG ##############################
NAME										= 	webServer
######################## PATHS ###############################
OBJS_PATH									= 	objs/
VPATH										= 	./Server	\
												./Parser	

######################## FLAGS ###############################
FLAGS										= 	-Wall -Wextra -Werror -std=c++98
######################## SOURCES #############################
SRC_SERVER									=	Route.cpp Server.cpp
SRC_PARSER									=	Parser.cpp
SRC											= 	test_methods_rewrite.cpp	\
												$(SRC_PARSER)				\
												$(SRC_SERVER)		
######################## OBJECTS #############################
OBJ											=	$(patsubst %.cpp, $(OBJS_PATH)%.o, $(SRC))
DEB											=	$(patsubst %.cpp, $(OBJS_PATH)%.d, $(SRC))
######################## COLORS ##############################
RED											= 	\e[91m
GREEN										= 	\e[92m
YELLOW										= 	\e[93m
RC											= 	\e[0m

all: $(NAME)

$(OBJS_PATH)%.o: %.cpp $(HEADER)
	@			c++ $(FLAGS) -I $(VPATH) -c $< -o $@
	@			printf "$(GREEN).$(RC)"

$(OBJS_PATH):
	@			mkdir $@

$(NAME): $(OBJS_PATH) $(OBJ)
	@			printf "\n$(YELLOW)Creating Project$(RC)\n"
	@			c++ $(FLAGS) $(OBJ) -o $(NAME)
	@			printf "$(GREEN)$(NAME) created$(RC)\n"

-include $(DEP)

clean:
	@			rm -rf $(OBJS_PATH)
	@			@printf "$(RED)$(NAME) objects removed$(RC)\n"

fclean: clean
	@			rm -rf $(NAME)
	@			printf "$(RED)$(NAME) removed$(RC)\n"

re: fclean all

.PHONY: all clean fclean re 