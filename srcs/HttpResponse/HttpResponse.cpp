/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 21:38:40 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:46:24 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : p_http_version("HTTP/1.1"), p_statusCode(200),
	p_statusMessage("OK"), p_headers(), p_body("")
{
	// Par défaut, la connexion est keep-alive en HTTP/1.1
}

HttpResponse::~HttpResponse()
{
}

void	HttpResponse::setStatusCode(int code)
{
	p_statusCode = code;
	// Met à jour le message de statut en fonction du code
	p_statusMessage = getStatusMessage(code);
}

void	HttpResponse::setHttpVersion(const std::string &version)
{
	p_http_version = version;
}

void	HttpResponse::setHeader(const std::string &key, const std::string &value)
{
	p_headers[key] = value;
}

void	HttpResponse::setBody(const std::string &body)
{
	p_body = body;

	// Met à jour le header Content-Length
	std::ostringstream	oss;
	oss << static_cast<unsigned long>(p_body.size());
	setHeader("Content-Length", oss.str());

	// Si body est défini mais pas le Content-Type, on met par défaut à text/plain
	if (p_headers.find("Content-Type") == p_headers.end())
		setHeader("Content-Type", "text/plain");
}

std::string	HttpResponse::getHttpVersion() const
{
	return p_http_version;
}

int	HttpResponse::getStatusCode() const
{
	return p_statusCode;
}

std::string	HttpResponse::getBody() const
{
	return p_body;
}

std::string	HttpResponse::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = p_headers.find(key);
	if (it != p_headers.end())
		return it->second;
	return "";
}

std::string	HttpResponse::toString() const
{
	std::ostringstream	response_stream;

	// Ligne de statut
	response_stream << p_http_version << " " << p_statusCode << " " << p_statusMessage
		<< "\r\n";

	// Copie des headers pour pouvoir ajouter Content-Length si nécessaire
	std::map<std::string, std::string> headers_copy = p_headers;
	
	// Si Content-Length n'est pas déjà défini, on l'ajoute
	if (headers_copy.find("Content-Length") == headers_copy.end())
	{
		std::ostringstream oss;
		oss << static_cast<unsigned long>(p_body.size());
		headers_copy["Content-Length"] = oss.str();
	}
	
	// Si Content-Type n'est pas défini et qu'il y a un body, on met text/plain par défaut
	if (headers_copy.find("Content-Type") == headers_copy.end() && !p_body.empty())
		headers_copy["Content-Type"] = "text/plain";

	// Headers
	for (std::map<std::string, std::string>::const_iterator it = headers_copy.begin();
		 it != headers_copy.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}

	// Ligne vide entre les headers et le body
	response_stream << "\r\n" << p_body;

	return response_stream.str();
}

std::string	HttpResponse::getStatusMessage(int code) const
{
	switch (code)
	{
		// Code de succès (2xx)
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";

		// Redirections (3xx)
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

		// Erreurs client (4xx)
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 431: return "Request Header Fields Too Large";

		// Erreurs serveur (5xx)
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";

		default: return "Unknown Status";
	}
}
