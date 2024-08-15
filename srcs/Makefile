# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 15:04:07 by dapaulin          #+#    #+#              #
#    Updated: 2024/08/10 21:45:48 by dapaulin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


######################## CONFIG ##############################
NAME										= 	webServer
######################## PATHS ###############################
OBJS_PATH									= 	objs/
HEADERS										=	Server/ \
												HandlerFileSystem/ \
												Utils/  \
												Parser/ \
												Server/Response \
												Server/Route
INCLUDE_FLAGS 								:= 	-g3 -O3 -MMD -MP $(addprefix -I, $(HEADERS))
VPATH										=	Server/:Utils/:Parser/:Server/Response:HandlerFileSystem:Server/Route

######################## FLAGS ###############################
FLAGS										= 	-Wall -Wextra -std=c++98
######################## SOURCES #############################
SRC_RESPONSE								=	HttpResponse.cpp BuilderResponse.cpp
SRC_ROUTE									=	Route.cpp BuilderRoute.cpp
SRC_SERVER									=	Server.cpp
# SRC_PARSER									=	Parser.cpp Handler.cpp
SRC_HTTP									=	Http.cpp HttpRequest.cpp CommonParameters.cpp
SRC_UTILS									=	Utils.cpp Handler.cpp
SRC											= 	$(SRC_UTILS)				\
												$(SRC_RESPONSE)				\
												$(SRC_ROUTE)				\
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
	@			c++ $(FLAGS) $(OBJ) $(INCLUDE_FLAGS) -o $(NAME)
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
	@			cd googletest && cmake -S . -B build
	@			cd googletest && cmake --build build

test_build:
	@			cmake -S . -B $(TEST_BUILD)
	@			cmake --build $(TEST_BUILD)

test_run:
	@			cd $(TEST_BUILD) && ctest --rerun-failed --output-on-failure

test_rund:
	@			cd $(TEST_BUILD) && ctest --rerun-failed --output-on-failure --verbose --interactive-debug-mode


testd: t_clean $(GOOGLE_TEST) test_build test_rund

test: $(GOOGLE_TEST) test_build test_run

t_clean:
	@			rm -rf $(TEST_BUILD)

t_fclean: t_clean
	@			rm -rf $(GOOGLE_TEST)
