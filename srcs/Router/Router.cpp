/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:48 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:32:59 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router()
{
}

Router::~Router()
{
}

HttpResponse	Router::route(const HttpRequest& request)
{
	// Récupérer la méthode et l'URI de la requête
	std::string	method = request.getMethod();
	std::string	uri = request.getUri();

	if (method == "GET")
	{
		if (uri == "/")
			return handleHomePage(request);
		else if (uri == "/about")
			return handleAboutPage(request);
		else if (uri == "/index.html")
			return handleHomePage(request); // Rediriger vers la page d'accueil
		else
			return handleNotFound(request);
	}
	else
		return handleMethodNotAllowed(request);
}

HttpResponse	Router::createErrorResponse(int statusCode, const std::string& httpVersion)
{
	HttpResponse	response;
	
	// Version HTTP (par défaut HTTP/1.1 si vide)
	std::string version = httpVersion;
	if (version.empty())
		version = "HTTP/1.1";
	
	response.setHttpVersion(version);
	response.setStatusCode(statusCode);
	response.setHeader("Content-Type", "text/html");

	// === Gestion du header Connection selon le type d'erreur et la version HTTP ===
	
	// Erreurs qui nécessitent de fermer la connexion (comme NGINX)
	bool shouldClose = false;
	
	if (statusCode == 400)  // Bad Request (parsing impossible)
		shouldClose = true;
	else if (statusCode == 408)  // Request Timeout
		shouldClose = true;
	else if (statusCode == 413)  // Payload Too Large
		shouldClose = true;
	else if (statusCode == 414)  // URI Too Long (URI corrompu)
		shouldClose = true;
	else if (statusCode == 431)  // Request Header Fields Too Large
		shouldClose = true;
	else if (statusCode == 505)  // HTTP Version Not Supported
		shouldClose = true;
	else if (version == "HTTP/1.0")  // HTTP/1.0 n'a pas de keep-alive par défaut
		shouldClose = true;
	
	// Définir le header Connection
	if (shouldClose)
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	std::string html;
	
	// Choisir la page HTML selon le code d'erreur
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
	
	response.setBody(html);
	return response;
}
