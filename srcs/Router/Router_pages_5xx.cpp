/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_pages_5xx.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:20:48 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// ============================================================================
// Pages d'erreur serveur (5xx)
// ============================================================================

std::string	Router::getPage_500() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>500 - Internal Server Error</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>500 - Internal Server Error</h1>\n"
		"    <p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_501() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>501 - Not Implemented</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>501 - Not Implemented</h1>\n"
		"    <p>The server does not support the functionality required to fulfill the request.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_502() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>502 - Bad Gateway</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>502 - Bad Gateway</h1>\n"
		"    <p>The server received an invalid response from an upstream server.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_503() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>503 - Service Unavailable</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>503 - Service Unavailable</h1>\n"
		"    <p>The server is currently unable to handle the request due to maintenance or overload.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_504() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>504 - Gateway Timeout</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>504 - Gateway Timeout</h1>\n"
		"    <p>The server did not receive a timely response from an upstream server.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_505() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>505 - HTTP Version Not Supported</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #f8d7da;\n"
		"        }\n"
		"        h1 { color: #721c24; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>505 - HTTP Version Not Supported</h1>\n"
		"    <p>The server does not support the HTTP version used in the request.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}
