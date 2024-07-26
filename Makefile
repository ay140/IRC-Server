# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/25 10:49:31 by amarzouk          #+#    #+#              #
#    Updated: 2024/07/26 12:10:13 by ayman_marzo      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./ircserv

# Directories
SRCS_FOLDER = src
OBJECTSDIR = objects
HEADERS_FOLDER = include

# Compiler and flags
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

# Colors
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RESET = \033[0m

# Source and header files
SRC_FILES = Client.cpp Server.cpp commands.cpp getSocket.cpp \
pollHandling.cpp utils.cpp PartCommand.cpp Join.cpp Kick.cpp \
Privmsg.cpp Request.cpp File.cpp requestHandling.cpp \
Channel.cpp Bot.cpp

HEADERS_FILES = include/Client.hpp include/Server.hpp include/Channel.hpp \
include/Request.hpp include/File.hpp

MAIN = main.cpp

# Object files
OBJS = $(addprefix $(OBJECTSDIR)/, $(SRC_FILES:.cpp=.o))
MAIN_OBJ = $(addprefix $(OBJECTSDIR)/, $(MAIN:.cpp=.o))

# Build rules
all: $(NAME)
	

$(NAME): $(OBJS) $(MAIN_OBJ)
	@$(CC) $(FLAGS) $(OBJS) $(MAIN_OBJ) -o $(NAME)
	@echo "$(GREEN)Executable created: $(NAME)$(RESET)"

$(OBJECTSDIR)/%.o: $(SRCS_FOLDER)/%.cpp $(HEADERS_FILES)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

$(OBJECTSDIR)/main.o: $(MAIN) $(HEADERS_FILES)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@echo "$(RED)Deleting object files...$(RESET)"
	@rm -rf $(OBJECTSDIR)

fclean: clean
	@echo "$(RED)Deleting executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all
