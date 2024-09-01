# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rleslie- <rleslie-@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 15:04:07 by dapaulin          #+#    #+#              #
#    Updated: 2024/09/01 18:14:09 by rleslie-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


######################## CONFIG ##############################
NAME										= 	webServer
TAR_FILE									=	services.tar
WEB_DIR										=	../app
######################## PATHS ###############################
OBJS_PATH									= 	objs/
HEADERS										=	Http/ \
												HandlerFileSystem/ \
												Utils/  \
												Logging \
												Parser \
												Http/Server \
												Http/Response \
												Http/Request \
												Http/Route 
# Parser/
INCLUDE_FLAGS 								:= 	-g3 -O3 -MMD -MP $(addprefix -I, $(HEADERS))
VPATH										=	Http:Parser:Logging:Http/Server/:Utils/:Http/Response:HandlerFileSystem:Http/Route:Http/Request

######################## FLAGS ###############################
FLAGS										= 	-Wall -Wextra -Werror -std=c++98
######################## SOURCES #############################
SRC_RESPONSE								=	HttpResponse.cpp BuilderResponse.cpp
SRC_ROUTE									=	Route.cpp BuilderRoute.cpp
SRC_SERVER									=	Server.cpp BuilderServer.cpp
SRC_PARSER									=	Parser.cpp
SRC_HTTP									=	Http.cpp HttpRequest.cpp # CommonParameters.cpp
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

CONFIG_HOST:
#	@echo "Configure Hosts"	
#	@sudo chmod a+w /etc/hosts
#	@sudo cat /etc/hosts | grep dapaulin.42.fr || echo "127.0.0.1 dapaulin.42.fr" >> /etc/hosts
#	@sudo cat /etc/hosts | grep rleslie.42.fr || echo "127.0.0.1 rleslie.42.fr" >> /etc/hosts
	@mkdir -p $(WEB_DIR)
	@ tar -xpf $(TAR_FILE) -C $(WEB_DIR)

$(OBJS_PATH)%.o: %.cpp $(HEADER)

#	@			echo $(INCLUDE_FLAGS)
				c++ $(FLAGS) -I $(INCLUDE_FLAGS) -c $< -o $@
	@			printf "$(GREEN).$(RC)"

$(OBJS_PATH):
	@			mkdir $@

$(NAME): CONFIG_HOST $(OBJS_PATH) $(OBJ)
	@			printf "\n$(YELLOW)Creating Project '$(OBJ)'$(RC)\n" 
	@			c++ $(FLAGS) $(OBJ) $(INCLUDE_FLAGS) -o $(NAME)
	@			printf "$(GREEN)$(NAME) created$(RC)\n"

-include $(DEP)

clean:
	@			rm -rf $(OBJS_PATH)
	@			@printf "$(RED)$(NAME) objects removed$(RC)\n"

fclean: clean
	@			rm -rf $(WEB_DIR)
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

backup:
	sudo tar -cpf $(TAR_FILE) -C ../home .