/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_pages_3xx.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:20:48 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// ============================================================================
// Pages de redirection (3xx)
// ============================================================================

std::string	Router::getPage_301() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>301 - Moved Permanently</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>301 - Moved Permanently</h1>\n"
		"    <p>The resource has been permanently moved to a new location.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_302() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>302 - Found</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>302 - Found</h1>\n"
		"    <p>The resource has been temporarily moved to a different location.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_303() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>303 - See Other</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>303 - See Other</h1>\n"
		"    <p>The response can be found at a different URI.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_304() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>304 - Not Modified</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>304 - Not Modified</h1>\n"
		"    <p>The resource has not been modified since the last request.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_307() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>307 - Temporary Redirect</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>307 - Temporary Redirect</h1>\n"
		"    <p>The resource is temporarily located at a different URI.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_308() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>308 - Permanent Redirect</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #fff3cd;\n"
		"        }\n"
		"        h1 { color: #856404; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>308 - Permanent Redirect</h1>\n"
		"    <p>The resource has been permanently moved to a new location.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}
