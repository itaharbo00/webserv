/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 21:35:44 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:46:24 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <sstream>

class HttpResponse
{
public:

	HttpResponse();
	~HttpResponse();

	// Setters
	void								setStatusCode(int code);
	void								setHttpVersion(const std::string &version);
	void								setHeader(const std::string &key, const std::string &value);
	void								setBody(const std::string &body);

	// Getters
	std::string							getHttpVersion() const;
	int									getStatusCode() const;
	std::string							getBody() const;
	std::string							getHeader(const std::string &key) const;

	// Génère la réponse HTTP complète sous forme de chaîne
	std::string							toString() const;

private:

	std::string							p_http_version;		// Version HTTP
	int									p_statusCode;		// Code de statut HTTP
	std::string							p_statusMessage;	// Message de statut HTTP
	std::map<std::string, std::string>	p_headers;
	std::string							p_body;

	std::string							getStatusMessage(int code) const;
};

#endif	// HTTPRESPONSE_HPP
