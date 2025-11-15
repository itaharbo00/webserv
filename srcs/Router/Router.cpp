/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:48 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 17:52:46 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router() : p_root("./www"), p_serverConfig(NULL)
{
}

Router::Router(const std::string &root) : p_root(root), p_serverConfig(NULL)
{
}

Router::Router(const ServerConfig *serverConfig) : p_serverConfig(serverConfig)
{
	if (p_serverConfig)
		p_root = p_serverConfig->getRoot();
	else
		p_root = "./www";
}

Router::~Router()
{
}

// Méthode principale de routing
HttpResponse	Router::route(const HttpRequest &request)
{
	if (p_serverConfig)
		return routeWithConfig(request);
	else
		return routeWithoutConfig(request);
}

HttpResponse	Router::routeWithConfig(const HttpRequest &request)
{
	// Récupérer la méthode et l'URI de la requête
	std::string				method = request.getMethod();
	std::string				uri = request.getUri();

	// Trouver la location correspondante
	const LocationConfig	*location = p_serverConfig->findLocation(uri);

	// Si aucune location ne correspond, retourner 404
	if (!location)
		return handleNotFound(request);

	// Vérifier si la méthode est autorisée
	if (!location->isMethodAllowed(method))
		return handleMethodNotAllowed(request);

	// Gérer les redirections
	if (location->hasRedirect())
		return handleRedirect(request, location);

	// Router selon la méthode
	return routeByMethod(request, location);
}

// Routing principal avec configuration serveur
HttpResponse	Router::routeWithoutConfig(const HttpRequest &request)
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
				return serveStaticFile(request);  // ← Essayer de servir un fichier
		}
		else if (method == "POST")
			return handlePost(request, NULL);
		else if (method == "DELETE")
			return handleDelete(request);
		else
			return handleMethodNotAllowed(request);
}

HttpResponse	Router::handleRedirect(const HttpRequest &request,
							const LocationConfig *location)
{
	// Construire la réponse de redirection
	HttpResponse	response;
	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(location->getReturnCode());
	response.setHeader("Location", location->getReturnUrl());
	response.setHeader("Content-Type", "text/html");
	
	std::string	redirectPage = generateRedirectPage(location->getReturnCode(),
									location->getReturnUrl());
	response.setBody(redirectPage);

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse	Router::routeByMethod(const HttpRequest &request,
							const LocationConfig *location)
{
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
			return serveStaticFile(request);  // ← Essayer de servir un fichier
	}
	else if (method == "POST")
		return handlePost(request, location);
	else if (method == "DELETE")
		return handleDelete(request);
	else
		return handleMethodNotAllowed(request);
}

std::string	Router::generateRedirectPage(int statusCode,
							const std::string &locationUrl)
{
	std::string	pageBody;

	switch (statusCode)
	{
		case 301: pageBody = getPage_301(); break;
		case 302: pageBody = getPage_302(); break;
		case 303: pageBody = getPage_303(); break;
		case 304: pageBody = getPage_304(); break;
		case 307: pageBody = getPage_307(); break;
		case 308: pageBody = getPage_308(); break;
		default:  pageBody = getPage_301(); break; // Fallback
	}

	size_t	headPos = pageBody.find("</head>");
	if (headPos != std::string::npos)
	{
		std::string	metaTag = "    <meta http-equiv=\"refresh\" content=\"0; url=" +
								locationUrl + "\">\n";
		pageBody.insert(headPos, metaTag);
	}

	size_t	linkPos = pageBody.find("<a href=\"/\">Go to Home</a>");
	if (linkPos != std::string::npos)
	{
		std::string	newLink = "<a href=\"" + locationUrl + "\">Click here if not redirected</a>";
		pageBody.replace(linkPos, 31, newLink);
	}

	return pageBody;
}
