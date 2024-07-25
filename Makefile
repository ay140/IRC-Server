# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/25 10:49:31 by amarzouk          #+#    #+#              #
#    Updated: 2024/07/25 11:31:23 by amarzouk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./ircserv

# Project's directories
SRCS_FOLDER = src
OBJECTSDIR = objects
HEADERS_FOLDER = include

# Name of variables
FLAGS = -Wall -Wextra -Werror
CPP_STANDARD = -std=c++98
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RESET = \033[0m

# Exercise files variable
SRC_FILES = Client.cpp Server.cpp commands.cpp getSocket.cpp messagesHandling.cpp pollHandling.cpp \
utils.cpp PartCommand.cpp JoinCommand.cpp KickCommand.cpp PrivmsgCommand.cpp FileTransfer.cpp Request.cpp \
File.cpp requestHandling.cpp Channel.cpp DeezNuts.cpp


HEADERS_FILES = include/Client.hpp \
include/Server.hpp \
include/Channel.hpp \
include/Request.hpp \
include/File.hpp \

MAIN = main.cpp
# Define objects for all sources
OBJS := $(addprefix $(OBJECTSDIR)/, $(SRC_FILES:.cpp=.o))
MAIN_OBJ = $(addprefix $(OBJECTSDIR)/, $(MAIN:.cpp=.o))

# Name the compiler
CC = c++

# OS specific part
RM = rm -rf
RMDIR = rm -rf
MKDIR = mkdir -p
MAKE = make -C
ECHO = /bin/echo
ERRIGNORE = 2>/dev/null

all: $(NAME)
	@echo "$(BLUE)██████████████████████ Compiling is DONE ███████████████████████$(RESET)"


head:
	@echo "$(BLUE)█████████████████████ Making ft_irc Server █████████████████████$(RESET)"

# Phonebook making rules
$(NAME): head $(OBJS) $(MAIN_OBJ) $(HEADERS_FILES)
	@$(CC) $(CPP_STANDARD) $(OBJECTSDIR)/main.o $(OBJS) -o $@ -g

$(OBJECTSDIR)/%.o: $(SRCS_FOLDER)/%.cpp $(HEADERS_FILES)
	@$(MKDIR) $(dir $@)
	@printf "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

$(OBJECTSDIR)/%.o: main.cpp $(HEADERS_FILES)
	@printf "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

# Remove all objects, dependencies and executable files generated during the build
clean:
	@echo "$(RED)deleting$(RESET): " $(OBJECTSDIR)
	@$(RMDIR) $(OBJECTSDIR)


fclean: clean
	@echo "$(RED)deleting$(RESET): " $(NAME)
	@$(RM) $(NAME) $(ERRIGNORE)

re: fclean all
