/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_pages.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:36:40 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:23:14 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

std::string	Router::getPage_home() const
{
	std::string content = 
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>Webserv - Home</title>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>Welcome to Webserv!</h1>\n"
		"    <p>This is the home page.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Home</a> | \n"
		"        <a href=\"/about\">About</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>\n";

	return content;
}

std::string	Router::getPage_about() const
{
	std::string content = 
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>Webserv - About</title>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>About Webserv</h1>\n"
		"    <p>Webserv is a simple HTTP server implemented in C++.</p>\n"
		"    <p>It supports:</p>\n"
		"    <ul>\n"
		"        <li>HTTP/1.0 and HTTP/1.1</li>\n"
		"        <li>Persistent connections (keep-alive)</li>\n"
		"        <li>Cookie parsing</li>\n"
		"    </ul>\n"
		"    <nav>\n"
		"        <a href=\"/\">Home</a> | \n"
		"        <a href=\"/about\">About</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>\n";

	return content;
}
