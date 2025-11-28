/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_methods.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:04:18 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/22 17:59:08 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include <sstream>
#include <cerrno>

// Génère un nom de fichier unique basé sur le timestamp actuel
static std::string generateUniqueFilename(const std::string &uploadDir)
{
	std::stringstream ss;
	std::time_t now = std::time(NULL);

	ss << uploadDir << "/upload_" << now << ".bin";
	return ss.str();
}

HttpResponse Router::handlePost(const HttpRequest &request, const LocationConfig *location)
{
	// 1. Validation Content-Length OU Transfer-Encoding
	std::map<std::string, std::string> headers = request.getHeaders();
	std::map<std::string, std::string>::const_iterator itLength = headers.find("Content-Length");
	std::map<std::string, std::string>::const_iterator itTransfer = headers.find("Transfer-Encoding");

	// Exiger Content-Length OU Transfer-Encoding pour POST
	if (itLength == headers.end() && itTransfer == headers.end())
		return createErrorResponse(411, request.getHttpVersion()); // 411 Length Required

	// 2. Récupérer le body (déjà dechunké si Transfer-Encoding: chunked)
	std::string body = request.getBody();

	if (body.empty())
		return createErrorResponse(400, request.getHttpVersion());

	// 3. Déterminer le dossier d'upload
	std::string uploadDir = "./uploads";

	// Si location fournie et upload activé, utiliser son chemin
	if (location && location->hasUpload())
		uploadDir = location->getUploadStore();

	// 4. Créer le dossier s'il n'existe pas
	struct stat st;
	if (stat(uploadDir.c_str(), &st) != 0)
	{
		if (mkdir(uploadDir.c_str(), 0755) != 0)
			return createErrorResponse(500, request.getHttpVersion());
	}

	// 5. Générer un nom de fichier unique
	std::string uniqueFilename = generateUniqueFilename(uploadDir);

	// 6. Écrire le fichier
	std::ofstream outFile(uniqueFilename.c_str(), std::ios::binary);
	if (!outFile.is_open())
		return createErrorResponse(500, request.getHttpVersion());

	outFile.write(body.c_str(), body.size());
	if (outFile.fail())
	{
		outFile.close();
		return createErrorResponse(500, request.getHttpVersion());
	}
	outFile.close();

	// 7. Construire l'URL absolue pour le header Location (comme NGINX)
	std::map<std::string, std::string>::const_iterator hostIt = headers.find("Host");

	std::string host;
	if (hostIt != headers.end())
		host = hostIt->second;
	else
		host = "localhost:8080";

	// Enlever "./" du début et construire l'URL complète
	std::string relativePath = uniqueFilename.substr(2); // Enlève "./"
	std::string locationUrl = "http://" + host + "/" + relativePath;

	// 8. Créer la réponse 201 Created
	HttpResponse response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(201);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Location", locationUrl); // URL absolue comme NGINX
	response.setBody(getPage_201());

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse Router::handleDelete(const HttpRequest &request)
{
	// 1. Récupérer l'URI et construire le chemin
	std::string uri = request.getUri();
	std::string filePath = p_root + uri;

	// 2. Vérifier la sécurité du path (pas de ../)
	if (!isPathSecure(uri))
		return createErrorResponse(403, request.getHttpVersion());

	// 3. Vérifier que le fichier existe
	if (!fileExists(filePath))
		return createErrorResponse(404, request.getHttpVersion());

	// 4. Vérifier que c'est un fichier régulier (pas un dossier)
	if (!isRegularFile(filePath))
		return createErrorResponse(403, request.getHttpVersion());

	// 5. Supprimer le fichier (comme NGINX)
	if (unlink(filePath.c_str()) != 0)
	{
		// Sauvegarder errno IMMÉDIATEMENT après unlink
		int saved_errno = errno;

		// Vérifier le type d'erreur (comme NGINX)
		if (saved_errno == EACCES || saved_errno == EPERM)
			return createErrorResponse(403, request.getHttpVersion()); // Permission denied
		else
			return createErrorResponse(500, request.getHttpVersion()); // Other errors
	}

	// 6. Retourner 204 No Content (comme NGINX)
	HttpResponse response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(204);
	response.setHeader("Content-Type", "text/html");
	response.setBody(""); // Pas de body pour 204

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}
