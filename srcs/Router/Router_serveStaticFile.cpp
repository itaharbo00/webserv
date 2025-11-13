/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_serveStaticFile.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:02:38 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 17:24:10 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

HttpResponse	Router::serveStaticFile(const HttpRequest &request)
{
	HttpResponse	response;

	// Vérifier que la méthode est GET
	if (request.getMethod() != "GET")
		return createErrorResponse(405, request.getHttpVersion());

	std::string	uri = request.getUri();

	// Vérifier la sécurité du chemin
	if (!isPathSecure(uri))
		return createErrorResponse(403, request.getHttpVersion());

	// Construire le chemin complet du fichier
	std::string	filePath = p_root + uri;

	// Si le chemin est un répertoire, ajouter index.html
	if (uri[uri.size() - 1] == '/')
		filePath += "index.html";

	// Vérifier si le fichier existe et est un fichier régulier
	if (!fileExists(filePath))
		return createErrorResponse(404, request.getHttpVersion());
	if (!isRegularFile(filePath))
		return createErrorResponse(403, request.getHttpVersion());
	if (isFileTooLarge(filePath))
		return createErrorResponse(413, request.getHttpVersion());

	// Lire le contenu du fichier
	std::string	fileContent;
	try
	{
		fileContent = readFile(filePath);
	}
	catch (const std::exception &e)
	{
		std::string	errorMsg = e.what();

		if (errorMsg.find("Permission denied") != std::string::npos)
			return createErrorResponse(403, request.getHttpVersion());
		else
			return createErrorResponse(500, request.getHttpVersion());
	}

	// Préparer la réponse
	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200);
	response.setHeader("Content-Type", getContentType(filePath));
	response.setBody(fileContent);

	// Gérer la connexion
	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}
