/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest_checkers.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:56:43 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 21:30:07 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

// Fonction pour décoder une URL (convertir %XX en caractère)
std::string	HttpRequest::urlDecode(const std::string &str)
{
	std::string result;
	result.reserve(str.length());
	
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			// Convertir les deux caractères hexadécimaux suivants
			std::string hex = str.substr(i + 1, 2);
			char *endptr;
			long value = strtol(hex.c_str(), &endptr, 16);
			
			// Vérifier que la conversion a réussi
			if (endptr == hex.c_str() + 2)
			{
				result += static_cast<char>(value);
				i += 2; // Sauter les deux caractères hex
			}
			else
			{
				// Séquence d'échappement invalide, garder tel quel
				result += str[i];
			}
		}
		else if (str[i] == '+')
		{
			// '+' est parfois utilisé pour représenter un espace
			result += ' ';
		}
		else
		{
			result += str[i];
		}
	}
	
	return result;
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

void	HttpRequest::validateRequestLine()
{
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

	if (p_http_version == "HTTP/1.1")
		p_closeConnection = false;

	// Vérifier que la méthode est valide
	if (p_method != "GET" && p_method != "POST" && p_method != "DELETE")
		throw std::runtime_error("Unsupported HTTP method: " + p_method);

	// Vérifier que l'URI commence par '/' ou est vide
	if (p_uri[0] != '/')
		throw std::runtime_error("Invalid URI: " + p_uri);

	// Décoder l'URL pour détecter les path traversal encodés (%2e%2e, etc.)
	std::string decodedUri = urlDecode(p_uri);
	
	// Vérifier directory traversal (sur l'URI décodée)
	if (decodedUri.find("..") != std::string::npos)
		throw std::runtime_error("Directory traversal attempt in URI: " + p_uri);

	// Vérifier longueur de l'URI
	if (p_uri.length() > 2048)
		throw std::runtime_error("URI too long");

	// Vérifier caractères invalides
	for (size_t i = 0; i < p_uri.length(); ++i)
	{
		unsigned char	c = static_cast<unsigned char>(p_uri[i]);
		// Bloquer caractères de contrôle
		if (c < 32 || c == 127)  // Caractères ASCII non-imprimables
			throw std::runtime_error("Invalid character in URI");
	}
}

// Validate un header critique spécifique
void	HttpRequest::validateCriticalHeader(const std::string &key, const std::string &value)
{
	if (key == "Content-Length")
	{
		if (value.empty())
			throw std::runtime_error("Empty Content-Length value");
		// Valider que Content-Length est un entier positif
		for (size_t i = 0; i < value.length(); ++i)
		{
			if (!std::isdigit(value[i]))
				throw std::runtime_error("Invalid Content-Length value: " + value);
		}

		// Vérifier que la valeur n'est pas déraisonnablement grande
		std::istringstream	iss(value);
		long				content_length;
		iss >> content_length;
		if (content_length < 0 || content_length > MAX_REQUEST_SIZE) // Limite arbitraire de 10MB
			throw std::runtime_error("Unreasonable Content-Length value: " + value);
	}
	else if (key == "Host" && value.empty())
			throw std::runtime_error("Empty Host header");
	else if (key == "Transfer-Encoding" && value != "chunked" && value != "identity")
			throw std::runtime_error("Unsupported Transfer-Encoding: " + value);
}

// Gérer les headers en double
void	HttpRequest::handleDuplicateHeaders(const std::string &key, const std::string &value)
{
	// Gestion spéciale de Connection : met à jour p_closeConnection
	if (key == "Connection")
	{
		if (value == "close")
			p_closeConnection = true;
		else if (value == "keep-alive")
			p_closeConnection = false;
		// Note: "upgrade" pourrait être supporté ici pour WebSocket
		p_headers[key] = value;
		return;
	}

	// Gestion spéciale de Cookie : concaténation avec "; " et parsing immédiat
	if (key == "Cookie")
	{
		if (p_headers.find(key) != p_headers.end())
			p_headers[key] += "; " + value;
		else
			p_headers[key] = value;
		// Ne pas parser ici — parsera une seule fois après réception complète des headers
		// (évite le reparsing répétitif lorsque plusieurs en-têtes Cookie arrivent).
		return;
	}

	// Pour les headers critiques, rejeter les duplicatas
	if (key == "Content-Length" || key == "Host" || key == "Transfer-Encoding"
		|| key == "Content-Type")
	{
		if (p_headers.find(key) != p_headers.end())
			throw std::runtime_error("Duplicate critical header: " + key);
		p_headers[key] = value;
	}
	// Pour certains headers, concaténer les valeurs avec ", "
	// Note: Cookie est géré au-dessus, donc retiré de cette liste
	else if (key == "Accept" || key == "Cache-Control"
		|| key == "Accept-Encoding" || key == "Accept-Language")
	{
		std::map<std::string, std::string>::iterator	it = p_headers.find(key);
		if (it != p_headers.end())
			it->second += ", " + value;
		else
			p_headers[key] = value;
	}
	else
	{
		// Pour les autres headers, écraser la valeur précédente (comportement par défaut)
		p_headers[key] = value;
	}
}
