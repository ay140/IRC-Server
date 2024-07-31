# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: codespace <codespace@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/25 10:49:31 by amarzouk          #+#    #+#              #
#    Updated: 2024/07/31 05:07:02 by codespace        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./ircserv

SRCS = src
OBJ = objects
HEADER = include

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RESET = \033[0m

SRC_FILES = Client.cpp Server.cpp commands.cpp Socket.cpp \
poll.cpp Part.cpp Join.cpp Kick.cpp \
Privmsg.cpp Request.cpp File.cpp Channel.cpp Bot.cpp Invite.cpp

HEADERS_FILES = include/Client.hpp include/Server.hpp include/Channel.hpp \
include/Request.hpp include/File.hpp

MAIN = main.cpp

# Object files
OBJS = $(addprefix $(OBJ)/, $(SRC_FILES:.cpp=.o))
MAIN_OBJ = $(addprefix $(OBJ)/, $(MAIN:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	@$(CC) $(FLAGS) $(OBJS) $(MAIN_OBJ) -o $(NAME)
	@echo "$(GREEN)Executable created: $(NAME)$(RESET)"

$(OBJ)/%.o: $(SRCS)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

$(OBJ)/main.o: $(MAIN) $(HEADERS_FILES)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@echo "$(RED)Deleting object files...$(RESET)"
	@rm -rf $(OBJ)

fclean: clean
	@echo "$(RED)Deleting executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all
