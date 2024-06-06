# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 15:04:07 by dapaulin          #+#    #+#              #
#    Updated: 2024/06/04 21:36:45 by dapaulin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


######################## CONFIG ##############################
NAME										= 	webServer
######################## PATHS ###############################
OBJS_PATH									= 	objs/
HEADERS										=	Server/ \
												Utils/  \
												Parser/ 
INCLUDE_FLAGS 								:= 	-g3 -O3 -MMD -MP $(addprefix -I, $(HEADERS))
VPATH										=	Server/:Utils/:Parser/

######################## FLAGS ###############################
FLAGS										= 	-Wall -Wextra -std=c++98
######################## SOURCES #############################
SRC_SERVER									=	Route.cpp Server.cpp
SRC_PARSER									=	Parser.cpp
SRC_HTTP									=	Http.cpp HttpRequest.cpp HttpResponse.cpp CommonParameters.cpp RouteResponse.cpp
SRC_UTILS									=	Utils.cpp
SRC											= 	$(SRC_UTILS)				\
												$(SRC_SERVER)				\
												$(SRC_HTTP)					\
												$(SRC_PARSER)				\
												main.cpp				
######################## OBJECTS #############################
OBJ											=	$(patsubst %.cpp, $(OBJS_PATH)%.o, $(SRC))
DEB											=	$(patsubst %.cpp, $(OBJS_PATH)%.d, $(SRC))
######################## COLORS ##############################
RED											= 	\e[91m
GREEN										= 	\e[92m
YELLOW										= 	\e[93m
RC											= 	\e[0m

######################## COLORS ##############################
GOOGLE_TEST									=	googletest
TEST_BUILD									=	Tests/build

all: $(NAME)

$(OBJS_PATH)%.o: %.cpp $(HEADER)

#	@			echo $(INCLUDE_FLAGS)
				c++ $(FLAGS) -I $(INCLUDE_FLAGS) -c $< -o $@
	@			printf "$(GREEN).$(RC)"

$(OBJS_PATH):
	@			mkdir $@

$(NAME): $(OBJS_PATH) $(OBJ)
	@			printf "\n$(YELLOW)Creating Project '$(OBJ)'$(RC)\n" 
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

$(GOOGLE_TEST):
	@			git clone https://github.com/google/googletest.git

test_build:
	@			cmake -S . -B $(TEST_BUILD)
	@			cmake --build $(TEST_BUILD)

test_run:
	@			cd $(TEST_BUILD) && ctest --rerun-failed --output-on-failure

test: $(GOOGLE_TEST) test_build test_run

t_clean:
	@			rm -rf $(TEST_BUILD)

t_fclean: t_clean
	@			rm -rf $(GOOGLE_TEST)
