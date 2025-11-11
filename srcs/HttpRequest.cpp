/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 14:02:11 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/11 17:31:17 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: p_rawRequest(""), p_method(""), p_uri(""), p_http_version(""),
	  p_headers(), p_body(""), p_isComplete(false)
{
}

HttpRequest::~HttpRequest()
{
}

// Retourne la méthode HTTP de la requête
std::string	HttpRequest::getMethod() const
{
	return p_method;
}

// Retourne l'URI de la requête
std::string	HttpRequest::getUri() const
{
	return p_uri;
}

// Retourne la version HTTP de la requête
std::string	HttpRequest::getHttpVersion() const
{
	return p_http_version;
}

// Retourne les headers de la requête HTTP
std::map<std::string, std::string>	HttpRequest::getHeaders() const
{
	return p_headers;
}

// Retourne le corps de la requête HTTP
std::string	HttpRequest::getBody() const
{
	return p_body;
}

// Retourne si la requête est complète
bool	HttpRequest::isComplete() const
{
	return p_isComplete;
}

// Trim les espaces en début et fin de chaîne
void	HttpRequest::trimString(std::string &str)
{
	size_t	start = str.find_first_not_of(" \t\r\n");
	size_t	end = str.find_last_not_of(" \t\r\n");

	if (start == std::string::npos || end == std::string::npos)
		str = "";
	else
		str = str.substr(start, end - start + 1);
}

// Ajoute des données brutes à la requête
void	HttpRequest::appendData(const std::string &data)
{
	p_rawRequest += data;
	if (p_rawRequest.find("\r\n\r\n") != std::string::npos) // Fin des headers
		p_isComplete = true;
}

// Parse la ligne de requête (Request Line)
void	HttpRequest::parseRequestLine(const std::string &line)
{
	std::istringstream	iss(line);

	if (!(iss >> p_method >> p_uri >> p_http_version))
		throw std::runtime_error("Malformed request line: " + line);
	trimString(p_method);
	trimString(p_uri);
	trimString(p_http_version);

	// Enlever \r final
	if (!p_http_version.empty() && 
	    p_http_version[p_http_version.length() - 1] == '\r')
	{
		p_http_version.erase(p_http_version.length() - 1);
	}

	// Valider les composants de la requête
	if (p_method.empty())
		throw std::runtime_error("Empty HTTP method");
	if (p_uri.empty())
		throw std::runtime_error("Empty URI");
	if (p_http_version.empty())
		throw std::runtime_error("Empty HTTP version");

	// Vérifier la version HTTP
	if (p_http_version != "HTTP/1.1" && p_http_version != "HTTP/1.0")
		throw std::runtime_error("Unsupported HTTP version: " + p_http_version);

	// Vérifier que la méthode est valide
	if (p_method != "GET" && p_method != "POST" && p_method != "DELETE")
		throw std::runtime_error("Unsupported HTTP method: " + p_method);

	// Vérifier que l'URI commence par '/' ou est vide
	if (p_uri[0] != '/')
		throw std::runtime_error("Invalid URI: " + p_uri);

	// Vérifier directory traversal
	if (p_uri.find("..") != std::string::npos)
		throw std::runtime_error("Directory traversal attempt in URI: " + p_uri);

	// Vérifier longueur de l'URI
	if (p_uri.length() > 2048)
		throw std::runtime_error("URI too long");

	// Vérifier caractères invalides
	for (size_t i = 0; i < p_uri.length(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(p_uri[i]);
		// Bloquer caractères de contrôle
		if (c < 32 || c == 127)  // Caractères ASCII non-imprimables
			throw std::runtime_error("Invalid character in URI");
	}
}
