/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 14:02:11 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 21:30:07 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

// p_closeConnection est initialisé à true (comportement HTTP/1.0 par défaut)
// En HTTP/1.1, la connexion est "keep-alive" par défaut : validateRequestLine()
// mettra p_closeConnection à false quand la version "HTTP/1.1" est détectée.
// Ce comportement suit les recommandations de RFC7230 / RFC9112 (connexions persistantes).
HttpRequest::HttpRequest() : p_rawRequest(""), p_method(""), p_uri(""), p_http_version(""),
	  p_headers(), p_cookies(), p_body(""), p_isComplete(false),
	  p_headersCompleted(false), p_expectedBodySize(0), p_closeConnection(true)
{
}

HttpRequest::~HttpRequest()
{
}

// Parse la requête HTTP brute
void	HttpRequest::parse()
{
	// Flux pour lire la requête ligne par ligne
	std::istringstream	request_stream(p_rawRequest);
	std::string			line;
	bool				is_first_line = true;

	while (std::getline(request_stream, line))
	{
		// Vérifier la taille de chaque ligne d'en-tête
		if (line.size() > MAX_HEADER_LINE_SIZE)
			throw std::runtime_error("Header line too long");

		if (line == "\r" || line.empty() || line == "\n")
			break;	 // Fin des headers

		if (is_first_line)
		{
			parseRequestLine(line);
			is_first_line = false;
		}
		else
			parseHeaderLine(line);
	}

	// Vérifier que le header Host est présent (requis en HTTP/1.1)
	if (p_headers.find("Host") == p_headers.end())
		throw std::runtime_error("Missing required Host header");

	// Si un en-tête Cookie a été reçu (ou concaténé), le parser une seule fois ici.
	if (p_headers.find("Cookie") != p_headers.end())
		parseCookies(p_headers["Cookie"]);

	parseBody(request_stream); // Parser le corps de la requête
}

// Ajoute des données brutes à la requête
void	HttpRequest::appendData(const std::string &data)
{
	// 1. Limiter la taille totale de la requête pour éviter les abus
	if (p_rawRequest.size() + data.size() > MAX_REQUEST_SIZE)
		throw std::runtime_error("Request size limit exceeded");
	p_rawRequest += data;

	// 2. Détecter la fin des en-têtes : présence de "\r\n\r\n"
	std::size_t	headerEnd = p_rawRequest.find("\r\n\r\n");
	if (!p_headersCompleted && headerEnd != std::string::npos)
	{
		// Marquer les en-têtes comme reçus
		p_headersCompleted = true;

		// Extraire la zone des en-têtes
		std::string			headersPart = p_rawRequest.substr(0, headerEnd);
		std::istringstream	stream(headersPart);
		std::string			line;
		bool				foundLength = false;
		bool				foundEncoding = false;

		// Parcourir chaque ligne d’en‑tête
		while (std::getline(stream, line))
		{
			// enlever un éventuel '\r'
			if (!line.empty() && line[line.size()-1] == '\r')
				line.erase(line.size()-1);

			// Détecter la présence des deux en-têtes
			if (line.find("Content-Length:") == 0)
				foundLength = true;
			if (line.find("Transfer-Encoding:") == 0)
				foundEncoding = true;
		}

		// 3. Si les deux en-têtes sont présents, on marque la requête comme complète
		//    et parseBody() lèvera l’exception lors de l’appel de parse()
		if (foundLength && foundEncoding)
		{
			p_isComplete = true;
			return;
		}

		// 4. Sinon, on calcule la taille attendue du corps si Content-Length existe
		if (foundLength)
		{
			std::size_t			pos = headersPart.find("Content-Length:");
			std::size_t			endline = headersPart.find("\r\n", pos);
			std::string			lenValue = headersPart.substr(pos + 15, endline - pos - 15);
			trimString(lenValue);

			std::istringstream	iss(lenValue);
			iss >> p_expectedBodySize;

			if (p_expectedBodySize > MAX_REQUEST_SIZE)
				throw std::runtime_error("Content-Length exceeds limit");
		}
	}

	// 5. Si les en-têtes sont complets, vérifier la taille du corps reçu
	if (p_headersCompleted)
	{
		std::size_t	bodySize = p_rawRequest.size() - (headerEnd + 4);
		// Si pas de Content-Length (p_expectedBodySize==0), la requête est complète dès la fin des headers
		if (p_expectedBodySize == 0 || bodySize >= p_expectedBodySize)
			p_isComplete = true;
	}
}
