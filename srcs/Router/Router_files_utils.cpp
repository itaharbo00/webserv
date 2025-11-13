/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_files_utils.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 14:57:41 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 20:57:00 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// Lit le contenu d'un fichier et le retourne sous forme de chaîne
std::string	Router::readFile(const std::string &filePath)
{
	// Ouvrir le fichier en mode lecture
	std::ifstream	file(filePath.c_str());
	if (!file.is_open())
	{
		if (fileExists(filePath))
			throw std::runtime_error("Permission denied: " + filePath);
		else
			throw std::runtime_error("File not found: " + filePath);
	}

	std::stringstream	buffer;
	buffer << file.rdbuf(); // Lire tout le contenu du fichier dans le buffer

	if (file.fail() && !file.eof())
		throw std::runtime_error("Error reading file: " + filePath);

	return buffer.str();
}

// Vérifie si le fichier existe
bool	Router::fileExists(const std::string &filePath)
{
	struct stat	buffer;

	return (stat(filePath.c_str(), &buffer) == 0);
}

// Vérifie si le chemin correspond à un fichier régulier (pas un répertoire etc.)
bool	Router::isRegularFile(const std::string &filePath)
{
	struct stat	buffer;

	if (stat(filePath.c_str(), &buffer) != 0)
		return false;

	return S_ISREG(buffer.st_mode);	// Vérifie si c'est un fichier régulier
}

// Vérifie si le chemin correspond à un répertoire
bool	Router::isDirectory(const std::string &filePath)
{
	struct stat	buffer;

	if (stat(filePath.c_str(), &buffer) != 0)
		return false;

	return S_ISDIR(buffer.st_mode);	// Vérifie si c'est un répertoire
}

// Vérifie que le chemin ne contient pas de séquences dangereuses (ex: ../)
bool	Router::isPathSecure(const std::string &uri)
{
	// Vérifie la présence de séquences dangereuses
	if (uri.find("..") != std::string::npos) // Empêche les traversées de répertoires
		return false;
	if (uri.find("~") != std::string::npos)	// Empêche l'accès aux répertoires home
		return false;
	if (uri.find("//") != std::string::npos) // Empêche les doubles slashes
		return false;

	if (uri.empty() || uri[0] != '/')
		return false;

	return true;
}

bool	Router::isFileTooLarge(const std::string &filePath)
{
	struct stat	buffer;
	
	if (stat(filePath.c_str(), &buffer) != 0)
		return false;

	const off_t	MAX_FILE_SIZE = 10 * 1024 * 1024; // 10 MB
	return (buffer.st_size > MAX_FILE_SIZE);
}

// Détermine le type de contenu (MIME type) basé sur l'extension du fichier
std::string	Router::getContentType(const std::string &filePath)
{
	// Trouver la dernière occurrence de '.'
	size_t	dot_pos = filePath.rfind('.');
	if (dot_pos == std::string::npos)
		return "application/octet-stream"; // Type par défaut si pas d'extension

	// Extraire l'extension (ex: ".html")
	std::string	extension = filePath.substr(dot_pos);

	// Map des extensions vers Content-Types
	if (extension == ".html" || extension == ".htm")
		return "text/html";
	if (extension == ".css")
		return "text/css";
	if (extension == ".js")
		return "application/javascript";
	if (extension == ".json")
		return "application/json";
	if (extension == ".xml")
		return "application/xml";
	if (extension ==".txt")
		return "text/plain";

	// Images
	if (extension == ".jpg" || extension == ".jpeg")
		return "image/jpeg";
	if (extension == ".png")
		return "image/png";
	if (extension == ".gif")
		return "image/gif";
	if (extension == ".svg")
		return "image/svg+xml";
	if (extension == ".ico")
		return "image/x-icon";

	// Vidéos
	if (extension == ".mp4")
		return "video/mp4";
	if (extension == ".webm")
		return "video/webm";

	// Fichiers binaires
	if (extension == ".pdf")
		return "application/pdf";
	if (extension == ".zip")
		return "application/zip";
	if (extension == ".mp3")
		return "audio/mpeg";

	return "application/octet-stream"; // Type par défaut
}
