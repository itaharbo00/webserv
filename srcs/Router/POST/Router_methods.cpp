/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_methods.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:04:18 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/06 18:24:41 by wlarbi-a         ###   ########.fr       */
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

// Extrait le boundary du Content-Type header
static std::string extractBoundary(const std::string &contentType)
{
	size_t pos = contentType.find("boundary=");
	if (pos == std::string::npos)
		return "";

	std::string boundary = contentType.substr(pos + 9); // après "boundary="

	// Enlever les guillemets si présents
	if (!boundary.empty() && boundary[0] == '"')
		boundary = boundary.substr(1);
	if (!boundary.empty() && boundary[boundary.length() - 1] == '"')
		boundary = boundary.substr(0, boundary.length() - 1);

	return boundary;
}

// Parse le contenu multipart et retourne le contenu du fichier et son nom
static bool parseMultipartBody(const std::string &body, const std::string &boundary,
							   std::string &fileContent, std::string &filename)
{
	// Construire le boundary complet avec les tirets
	std::string fullBoundary = "--" + boundary;
	std::string endBoundary = fullBoundary + "--";

	// Trouver le début du premier part
	size_t partStart = body.find(fullBoundary);
	if (partStart == std::string::npos)
		return false;

	partStart += fullBoundary.length();

	// Trouver la fin des headers (ligne vide)
	size_t headersEnd = body.find("\r\n\r\n", partStart);
	if (headersEnd == std::string::npos)
		headersEnd = body.find("\n\n", partStart);
	if (headersEnd == std::string::npos)
		return false;

	// Extraire la section des headers
	std::string headers = body.substr(partStart, headersEnd - partStart);

	// Chercher le filename dans Content-Disposition
	size_t filenamePos = headers.find("filename=");
	if (filenamePos != std::string::npos)
	{
		size_t start = headers.find("\"", filenamePos);
		if (start != std::string::npos)
		{
			start++;
			size_t end = headers.find("\"", start);
			if (end != std::string::npos)
				filename = headers.substr(start, end - start);
		}
	}

	// Le contenu commence après "\r\n\r\n" ou "\n\n"
	size_t contentStart = headersEnd + 4; // après "\r\n\r\n"
	if (body[headersEnd + 2] != '\r')
		contentStart = headersEnd + 2; // après "\n\n"

	// Trouver la fin du contenu (boundary suivant)
	size_t contentEnd = body.find(fullBoundary, contentStart);
	if (contentEnd == std::string::npos)
		return false;

	// Enlever les \r\n avant le boundary
	while (contentEnd > contentStart &&
		   (body[contentEnd - 1] == '\n' || body[contentEnd - 1] == '\r'))
		contentEnd--;

	// Extraire le contenu
	fileContent = body.substr(contentStart, contentEnd - contentStart);

	return true;
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

	// 2.5 Parser le multipart si c'est le cas
	std::string fileContent = body;
	std::string originalFilename;

	std::map<std::string, std::string>::const_iterator itContentType = headers.find("Content-Type");
	if (itContentType != headers.end() &&
		itContentType->second.find("multipart/form-data") != std::string::npos)
	{
		// Extraire le boundary
		std::string boundary = extractBoundary(itContentType->second);
		if (boundary.empty())
			return createErrorResponse(400, request.getHttpVersion());

		// Parser le contenu multipart
		if (!parseMultipartBody(body, boundary, fileContent, originalFilename))
			return createErrorResponse(400, request.getHttpVersion());
	}

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

	// 5. Générer un nom de fichier
	std::string finalFilename;
	if (!originalFilename.empty())
	{
		// Utiliser le nom original du fichier uploadé
		finalFilename = uploadDir + "/" + originalFilename;
	}
	else
	{
		// Générer un nom unique si pas de multipart
		finalFilename = generateUniqueFilename(uploadDir);
	}

	// 6. Écrire le fichier
	std::ofstream outFile(finalFilename.c_str(), std::ios::binary);
	if (!outFile.is_open())
		return createErrorResponse(500, request.getHttpVersion());

	outFile.write(fileContent.c_str(), fileContent.size());
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
	std::string relativePath = finalFilename.substr(2); // Enlève "./"
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
	std::string filePath;

	// Vérifier si c'est un fichier dans le répertoire uploads
	// Si l'URI commence par /uploads/, utiliser directement ce chemin
	if (uri.find("/uploads/") == 0)
		filePath = "." + uri; // ./uploads/...
	else
		filePath = p_root + uri; // ./www/...

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
