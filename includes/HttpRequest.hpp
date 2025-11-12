/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 14:02:13 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/12 00:57:01 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <map>
# include <vector>
# include <stdexcept>
# include <cctype>

# define MAX_HEADER_LINE_SIZE 8192  // 8 KB
# define MAX_REQUEST_SIZE 1048576 // 1 MB

class HttpRequest
{
public:

	HttpRequest();
	~HttpRequest();

	// Getters
	std::string							getMethod() const;
	std::string							getUri() const;
	std::string							getHttpVersion() const;
	std::map<std::string, std::string>	getHeaders() const;
	std::string							getBody() const;
	bool								isComplete() const;

	void								appendData(const std::string &data);
	void								parse();

private:

	std::string							p_rawRequest;  // Requête brute reçue
	std::string							p_method;
	std::string							p_uri;
	std::string							p_http_version;
	std::map<std::string, std::string>	p_headers;
	std::string							p_body;
	bool								p_isComplete; // Indique si la requête est complète
	bool								p_headersCompleted; // Indique si les headers ont été complètement reçus
	size_t								p_expectedBodySize; // Taille attendue du corps (si connue)

	// Fonctions de parsing
	void								parseRequestLine(const std::string &line);
	void								parseHeaderLine(const std::string &line);
	void								parseBody(std::istringstream &stream);
	void								contentLengthBody(std::istringstream &stream);
	void								chunkedBody(std::istringstream &stream);
	
	// Fonctions utilitaires
	void								trimString(std::string &str);

	// Fonctions de validation
	void								validateRequestLine();
	void								handleDuplicateHeaders(const std::string &key,
											const std::string &value);
	void								validateCriticalHeader(const std::string &key,
											const std::string &value);
};

#endif
