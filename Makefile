# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/31 20:03:22 by itaharbo          #+#    #+#              #
#    Updated: 2025/11/26 17:49:23 by wlarbi-a         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -I includes

SRCDIR		= srcs
OBJDIR		= objs

SRCS		= $(SRCDIR)/main.cpp \
			  $(SRCDIR)/Server/Server.cpp \
			  $(SRCDIR)/Server/Server_timeouts.cpp \
			  $(SRCDIR)/Server/Server_write.cpp \
			  $(SRCDIR)/HttpRequest/HttpRequest.cpp \
			  $(SRCDIR)/HttpRequest/HttpRequest_parsing.cpp \
			  $(SRCDIR)/HttpRequest/HttpRequest_body.cpp \
			  $(SRCDIR)/HttpRequest/HttpRequest_checkers.cpp \
			  $(SRCDIR)/HttpRequest/HttpRequest_getters.cpp \
			  $(SRCDIR)/HttpResponse/HttpResponse.cpp \
			  $(SRCDIR)/Router/Router.cpp \
			  $(SRCDIR)/Router/Router_handlers.cpp \
			  $(SRCDIR)/Router/GET/RouterGET.cpp \
			  $(SRCDIR)/Router/GET/Router_serveStaticFile.cpp \
			  $(SRCDIR)/Router/GET/Router_autoindex.cpp \
			  $(SRCDIR)/Router/GET/Router_files_utils.cpp \
			  $(SRCDIR)/Router/POST/RouterPOST.cpp \
			  $(SRCDIR)/Router/POST/Router_methods.cpp \
			  $(SRCDIR)/Router/CGI/RouterCGI.cpp \
			  $(SRCDIR)/Router/CGI/Router_cgi.cpp \
			  $(SRCDIR)/Router/CGI/Router_cgi_env.cpp \
			  $(SRCDIR)/Router/CGI/Router_cgi_execute.cpp \
			  $(SRCDIR)/Router/Router_error_responses.cpp \
			  $(SRCDIR)/Router/Router_pages.cpp \
			  $(SRCDIR)/Router/Router_pages_2xx.cpp \
			  $(SRCDIR)/Router/Router_pages_3xx.cpp \
			  $(SRCDIR)/Router/Router_pages_4xx.cpp \
			  $(SRCDIR)/Router/Router_pages_5xx.cpp \
			  $(SRCDIR)/Config/Config.cpp \
			  $(SRCDIR)/Config/Config_parse_server.cpp \
			  $(SRCDIR)/Config/Config_parse_location.cpp \
			  $(SRCDIR)/Config/Config_server_directives.cpp \
			  $(SRCDIR)/Config/Config_location_directives.cpp \
			  $(SRCDIR)/Config/Config_utils.cpp \
			  $(SRCDIR)/Config/ServerConfig.cpp \
			  $(SRCDIR)/Config/ServerConfig_getters.cpp \
			  $(SRCDIR)/Config/ServerConfig_setters.cpp \
			  $(SRCDIR)/Config/LocationConfig.cpp \
			  $(SRCDIR)/Config/LocationConfig_setters.cpp \
			  $(SRCDIR)/Config/LocationConfig_getters.cpp

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
