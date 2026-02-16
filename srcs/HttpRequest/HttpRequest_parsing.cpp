/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest_parsing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 20:34:27 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/06 18:38:14 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

// Parse la ligne de requête (Request Line)
void HttpRequest::parseRequestLine(const std::string &line)
{
	if (line.empty() || line == "\r" || line == "\n")
		throw std::runtime_error("Empty request line");

	std::istringstream iss(line);

	// Extraire méthode, URI et version HTTP
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

	validateRequestLine(); // Valider les composants de la requête
}

// Parse une ligne d'en-tête (Header Line)
void HttpRequest::parseHeaderLine(const std::string &line)
{
	size_t colon_pos = line.find(':'); // Trouver le séparateur clé:valeur
	if (colon_pos == std::string::npos)
		throw std::runtime_error("Malformed header line: " + line);

	// Séparer clé et valeur
	std::string key = line.substr(0, colon_pos);
	std::string value = line.substr(colon_pos + 1);

	trimString(key);
	trimString(value);

	// Enlever \r final
	if (!value.empty() && value[value.length() - 1] == '\r')
		value.erase(value.length() - 1);

	if (key.empty())
		throw std::runtime_error("Empty header key in line: " + line);

	for (size_t i = 0; i < key.length(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(key[i]);
		// Bloquer caractères de contrôle dans la clé
		if (c < 33 || c == 127) // Caractères ASCII non-imprimables et espace
			throw std::runtime_error("Invalid character in header key: " + key);
	}

	validateCriticalHeader(key, value); // Valider les headers critiques
	handleDuplicateHeaders(key, value); // Gérer les headers en double si nécessaire
}

void HttpRequest::parseBody(std::istringstream &stream)
{
	// Valider les combinaisons de headers
	if ((p_method == "GET" || p_method == "DELETE" || p_method == "HEAD") && p_headers.count("Content-Length"))
		throw std::runtime_error("Content-Length not allowed for GET/DELETE/HEAD requests");

	// Gérer le cas où les deux headers sont présents
	// RFC 9112 Section 6.3
	if (p_headers.count("Transfer-Encoding") && p_headers.count("Content-Length"))
		throw std::runtime_error("Both Transfer-Encoding and Content-Length headers present");

	if (!p_headers.count("Content-Length") && !p_headers.count("Transfer-Encoding"))
		return; // Pas de corps à parser

	// Parser le corps avec Transfer-Encoding
	if (p_headers.count("Transfer-Encoding"))
	{
		if (p_headers["Transfer-Encoding"] == "chunked")
			chunkedBody(stream);
		else if (p_headers["Transfer-Encoding"] == "identity")
			return; // Pas de corps à parser
		else
			throw std::runtime_error("Unsupported Transfer-Encoding: " +
									 p_headers["Transfer-Encoding"]);

		return;
	}

	// Parser le corps avec Content-Length
	if (p_headers.count("Content-Length"))
		contentLengthBody(stream);
}

// Parse l'en-tête Cookie selon le format : name1=value1; name2=value2
// Format attendu : Les cookies sont séparés par "; "
// Les noms et valeurs sont automatiquement trimmés (espaces supprimés)
// Limitations actuelles :
//   - Pas de validation stricte des caractères (permissif)
//   - Les cookies malformés (sans '=') sont ignorés silencieusement
void HttpRequest::parseCookies(const std::string &cookieHeader)
{
	std::stringstream ss(cookieHeader);
	std::string pair;

	// RFC-like validation: cookie-name should be a token (no CTLs or separators).
	// We implement a conservative validation: allow alphanum and a small set of symbols.
	const char *allowed_extra = "-_.!#$%&'*+^`|~";

	while (std::getline(ss, pair, ';'))
	{
		size_t eq_pos = pair.find('=');
		if (eq_pos == std::string::npos)
		{
			// Cookie malformé (pas de '=') -> ignorer
			continue;
		}

		std::string key = pair.substr(0, eq_pos);
		std::string value = pair.substr(eq_pos + 1);
		trimString(key);
		trimString(value);

		if (key.empty())
			continue;

		// Valider les caractères du nom de cookie
		bool valid = true;
		for (size_t i = 0; i < key.size(); ++i)
		{
			unsigned char c = static_cast<unsigned char>(key[i]);
			if (std::isalnum(c))
				continue;
			if (std::strchr(allowed_extra, c) != NULL)
				continue;
			valid = false;
			break;
		}

		if (!valid)
			continue; // Ignorer les noms non-conformes

		p_cookies[key] = value;
	}
}
