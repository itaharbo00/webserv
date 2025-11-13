# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/31 20:03:22 by itaharbo          #+#    #+#              #
#    Updated: 2025/11/12 23:57:55 by itaharbo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -I includes

SRCDIR		= srcs
OBJDIR		= objs

SRCS		= $(SRCDIR)/main.cpp \
			  $(SRCDIR)/Server/Server.cpp

OBJS		= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Compiling..."
	@echo "Linking $(NAME)..."
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Compiled successfully!"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJDIR)
	@echo "Clean completed!"

fclean: clean
	@echo "Cleaning..."
	@rm -f $(NAME)
	@echo "Full clean completed!"

re: fclean all

.PHONY: all clean fclean re
