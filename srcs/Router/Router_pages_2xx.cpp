/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_pages_2xx.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:20:48 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// ============================================================================
// Pages de succès (2xx)
// ============================================================================

std::string	Router::getPage_200() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>200 - OK</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #d4edda;\n"
		"        }\n"
		"        h1 { color: #155724; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>200 - OK</h1>\n"
		"    <p>The request was successful.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_201() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>201 - Created</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #d4edda;\n"
		"        }\n"
		"        h1 { color: #155724; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>201 - Created</h1>\n"
		"    <p>The resource has been successfully created.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}

std::string	Router::getPage_204() const
{
	return
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"    <title>204 - No Content</title>\n"
		"    <style>\n"
		"        body { \n"
		"            font-family: Arial, sans-serif; \n"
		"            text-align: center; \n"
		"            padding: 50px; \n"
		"            background-color: #d4edda;\n"
		"        }\n"
		"        h1 { color: #155724; }\n"
		"        a { color: #007bff; text-decoration: none; }\n"
		"        a:hover { text-decoration: underline; }\n"
		"    </style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>204 - No Content</h1>\n"
		"    <p>The request was successful but there is no content to display.</p>\n"
		"    <nav>\n"
		"        <a href=\"/\">Go to Home</a>\n"
		"    </nav>\n"
		"</body>\n"
		"</html>";
}
