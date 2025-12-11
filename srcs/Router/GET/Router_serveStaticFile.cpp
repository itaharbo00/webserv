/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_serveStaticFile.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:02:38 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/11 18:53:51 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

HttpResponse Router::serveStaticFile(const HttpRequest &request)
{
	HttpResponse response;

	// Vérifier que la méthode est GET ou HEAD
	if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
		return createErrorResponse(405, request.getHttpVersion());

	std::string uri = request.getUri();

	// Vérifier la sécurité du chemin
	if (!isPathSecure(uri))
		return createErrorResponse(403, request.getHttpVersion());

	// Séparer l'URI de la query string pour obtenir le chemin du fichier
	std::string uriPath = uri;
	size_t queryPos = uri.find('?');
	if (queryPos != std::string::npos)
		uriPath = uri.substr(0, queryPos);

	// Construire le chemin complet du fichier
	std::string filePath = p_root + uriPath;

	// Vérifier les CGI
	if (p_serverConfig)
	{
		const LocationConfig *location = p_serverConfig->findLocation(uriPath);
		if (location && location->hasCgi())
		{
			// Extraire l'extension du fichier
			std::string extension = getCgiExtension(filePath);
			// Vérifier si l'extension est gérée par le CGI
			if (location->isCgiExtension(extension))
				return executeCgi(request, location, filePath); // Exécuter le CGI
		}
	}

	// Vérifier si le chemin est un répertoire
	if (isDirectory(filePath))
	{
		// Si l'URI ne se termine pas par '/', rediriger
		if (uri[uri.size() - 1] != '/')
		{
			HttpResponse redirectResponse;
			redirectResponse.setHttpVersion(request.getHttpVersion());
			redirectResponse.setStatusCode(301);
			redirectResponse.setHeader("Location", uri + "/");
			redirectResponse.setHeader("Connection", request.shouldCloseConnection() ? "close" : "keep-alive");
			return redirectResponse;
		}

		// Essayer d'abord de servir le fichier index configuré
		std::string indexFile = ""; // Par défaut

		// Récupérer l'index depuis la configuration
		if (p_serverConfig)
		{
			const LocationConfig *location = p_serverConfig->findLocation(uri);
			if (location && !location->getIndex().empty())
				indexFile = location->getIndex();
			else if (!p_serverConfig->getIndex().empty())
				indexFile = p_serverConfig->getIndex();
		}

		std::string indexPath = filePath + "/" + indexFile;
		if (!indexFile.empty() && fileExists(indexPath) && isRegularFile(indexPath))
		{
			filePath = indexPath;
		}
		else
		{
			// Pas de fichier index : vérifier si autoindex est activé
			// Si p_serverConfig existe, chercher la location et vérifier autoindex
			if (p_serverConfig)
			{
				const LocationConfig *location = p_serverConfig->findLocation(uri);
				if (location && location->getAutoindex())
				{
					// Générer le listing du répertoire
					try
					{
						std::string listing = generateDirectoryListing(filePath, uri);
						response.setHttpVersion(request.getHttpVersion());
						response.setStatusCode(200);
						response.setHeader("Content-Type", "text/html");
						// HEAD ne renvoie pas de body
						if (request.getMethod() != "HEAD")
							response.setBody(listing);
						response.setHeader("Connection", request.shouldCloseConnection() ? "close" : "keep-alive");
						return response;
					}
					catch (const std::exception &e)
					{
						return createErrorResponse(500, request.getHttpVersion());
					}
				}
			}
			// Autoindex désactivé ou pas de config : 403 Forbidden
			return createErrorResponse(403, request.getHttpVersion());
		}
	}

	// Vérifier si le fichier existe et est un fichier régulier
	if (!fileExists(filePath))
		return createErrorResponse(404, request.getHttpVersion());
	if (!isRegularFile(filePath))
		return createErrorResponse(403, request.getHttpVersion());
	if (isFileTooLarge(filePath))
		return createErrorResponse(413, request.getHttpVersion());

	// Lire le contenu du fichier
	std::string fileContent;
	try
	{
		fileContent = readFile(filePath);
	}
	catch (const std::exception &e)
	{
		std::string errorMsg = e.what();

		if (errorMsg.find("Permission denied") != std::string::npos)
			return createErrorResponse(403, request.getHttpVersion());
		else
			return createErrorResponse(500, request.getHttpVersion());
	}

	// Préparer la réponse
	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200);
	response.setHeader("Content-Type", getContentType(filePath));
	// HEAD ne renvoie pas de body
	if (request.getMethod() != "HEAD")
		response.setBody(fileContent);

	// Gérer la connexion
	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}
