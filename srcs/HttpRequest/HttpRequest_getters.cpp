/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest_getters.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:57:43 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 18:32:37 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

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

// Retourne la valeur d'un header spécifique
std::string	HttpRequest::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator	it = p_headers.find(key);
	if (it != p_headers.end())
		return it->second;
	return "";
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

// Retourne si la connexion doit être fermée après la requête
bool	HttpRequest::shouldCloseConnection() const
{
	return (this->p_closeConnection);
}

// Retourne la valeur d'un cookie spécifique
std::string	HttpRequest::getCookie(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator	it = p_cookies.find(key);
	if (it != p_cookies.end())
		return (it->second);
	return ("");
}
