/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_pages_4xx.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 15:51:34 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// ============================================================================
// Pages d'erreur client (4xx)
// ============================================================================

std::string	Router::getPage_400() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>400 - Bad Request</title>\n"
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
		"    <h1>400 - Bad Request</h1>\n"
		"    <p>The request could not be understood or was missing required parameters.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_401() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>401 - Unauthorized</title>\n"
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
		"    <h1>401 - Unauthorized</h1>\n"
		"    <p>Authentication is required to access this resource.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_403() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>403 - Forbidden</title>\n"
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
		"    <h1>403 - Forbidden</h1>\n"
		"    <p>You do not have permission to access this resource.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_404() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>404 - Not Found</title>\n"
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
		"    <h1>404 - Not Found</h1>\n"
		"    <p>The page you are looking for does not exist.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_405() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>405 - Method Not Allowed</title>\n"
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
		"    <h1>405 - Method Not Allowed</h1>\n"
		"    <p>The HTTP method used is not allowed for this resource.</p>\n"
		"    <p>Allowed methods: <strong>GET</strong></p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_408() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>408 - Request Timeout</title>\n"
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
		"    <h1>408 - Request Timeout</h1>\n"
	"    <p>The server timed out waiting for the request.</p>\n"
	"    <nav>\n"
	"        <a href=\"/\">Go to Home</a>\n"
	"    </nav>\n"
	"</body>\n"
	"</html>";
}

std::string	Router::getPage_411() const
{
	return
		"<!DOCTYPE html>\\n"
		"<html>\\n"
		"<head>\\n"
		"    <title>411 - Length Required</title>\\n"
		"    <style>\\n"
		"        body { \\n"
		"            font-family: Arial, sans-serif; \\n"
		"            text-align: center; \\n"
		"            padding: 50px; \\n"
		"            background-color: #f8d7da;\\n"
		"        }\\n"
		"        h1 { color: #721c24; }\\n"
		"        a { color: #007bff; text-decoration: none; }\\n"
		"        a:hover { text-decoration: underline; }\\n"
		"    </style>\\n"
		"</head>\\n"
		"<body>\\n"
		"    <h1>411 - Length Required</h1>\\n"
		"    <p>The request did not specify the length of its content, which is required.</p>\\n"
		"    <nav>\\n"
		"        <a href=\\\"/\\\">Go to Home</a>\\n"
		"    </nav>\\n"
		"</body>\\n"
		"</html>";
}

std::string	Router::getPage_413() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>413 - Payload Too Large</title>\n"
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
		"    <h1>413 - Payload Too Large</h1>\n"
		"    <p>The request payload is too large for the server to process.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_414() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>414 - URI Too Long</title>\n"
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
		"    <h1>414 - URI Too Long</h1>\n"
		"    <p>The URI requested is too long for the server to process.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_415() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>415 - Unsupported Media Type</title>\n"
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
		"    <h1>415 - Unsupported Media Type</h1>\n"
		"    <p>The media type of the request is not supported by the server.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_431() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>431 - Request Header Fields Too Large</title>\n"
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
		"    <h1>431 - Request Header Fields Too Large</h1>\n"
		"    <p>The request header fields are too large for the server to process.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}
