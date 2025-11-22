/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_error_responses.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lybey <lybey@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 15:27:04 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/21 19:57:03 by lybey            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include <sstream>

static bool	shouldCloseForError(int statusCode, const std::string &httpVersion)
{
	// Erreurs qui nécessitent de fermer la connexion (comme NGINX)
	if (statusCode == 400)	// Bad Request (parsing impossible)
		return true;
	else if (statusCode == 408)	// Request Timeout
		return true;
	else if (statusCode == 413)	// Payload Too Large
		return true;
	else if (statusCode == 414)	// URI Too Long (URI corrompu)
		return true;
	else if (statusCode == 431)	// Request Header Fields Too Large
		return true;
	else if (statusCode == 505)	// HTTP Version Not Supported
		return true;
	else if (httpVersion == "HTTP/1.0")	// HTTP/1.0 n'a pas de keep-alive par défaut
		return true;

	return false;
}

bool	Router::getRightPages(int statusCode, std::string &html)
{
	// 1) If ServerConfig defines a custom error_page for this code, use it.
	if (p_serverConfig)
	{
		std::string cfgPath = p_serverConfig->getErrorPage(statusCode);
		if (!cfgPath.empty())
		{
			std::string fullPath;
			// If path starts with '/', treat it as relative to server root
			if (cfgPath.size() > 0 && cfgPath[0] == '/')
				fullPath = p_root + cfgPath;
			else
				fullPath = cfgPath; // could be absolute or relative filesystem path

			if (fileExists(fullPath))
			{
				html = readFile(fullPath);
				return true;
			}
		}
	}

	// 2) Next, try conventional external error pages under <root>/errors/<code>.html
	// std::ostringstream ss;
	// ss << statusCode;
	// std::string codeStr = ss.str();
	// std::string errorPath = p_root + "/errors/" + codeStr + ".html";
	// if (fileExists(errorPath))
	// {
	// 	html = readFile(errorPath);
	// 	return true;
	// }

	// 3) Fallback to built-in page generators
	switch (statusCode)
	{
		// Succès (2xx)
		case 200: html = getPage_200(); break;
		case 201: html = getPage_201(); break;
		case 204: html = getPage_204(); break;

		// Redirections (3xx)
		case 301: html = getPage_301(); break;
		case 302: html = getPage_302(); break;
		case 303: html = getPage_303(); break;
		case 304: html = getPage_304(); break;
		case 307: html = getPage_307(); break;
		case 308: html = getPage_308(); break;

		// Erreurs client (4xx)
		case 400: html = getPage_400(); break;
		case 401: html = getPage_401(); break;
		case 403: html = getPage_403(); break;
		case 404: html = getPage_404(); break;
		case 405: html = getPage_405(); break;
		case 408: html = getPage_408(); break;
		case 411: html = getPage_411(); break;
		case 413: html = getPage_413(); break;
		case 414: html = getPage_414(); break;
		case 415: html = getPage_415(); break;
		case 431: html = getPage_431(); break;

		// Erreurs serveur (5xx)
		case 500: html = getPage_500(); break;
		case 501: html = getPage_501(); break;
		case 502: html = getPage_502(); break;
		case 503: html = getPage_503(); break;
		case 504: html = getPage_504(); break;
		case 505: html = getPage_505(); break;

		// Par défaut : erreur serveur
		default:
			html = getPage_500();
			break;
	}

	return true;
}

HttpResponse	Router::createErrorResponse(int statusCode,
					const std::string &httpVersion)
{
	HttpResponse	response;
	
	// Version HTTP (par défaut HTTP/1.1 si vide)
	std::string version = httpVersion;
	if (version.empty())
		version = "HTTP/1.1";
	
	response.setHttpVersion(version);
	response.setStatusCode(statusCode);
	response.setHeader("Content-Type", "text/html");

	// Définir le header Connection
	if (shouldCloseForError(statusCode, version))
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	std::string html;
	getRightPages(statusCode, html); // Récupérer la page HTML correspondante
	response.setBody(html);

	return response;
}
