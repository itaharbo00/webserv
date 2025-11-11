/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 14:02:13 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/11 17:08:52 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <map>

class HttpRequest
{
public:

	HttpRequest();
	~HttpRequest();

	std::string							getMethod() const;
	std::string							getUri() const;
	std::string							getHttpVersion() const;
	std::map<std::string, std::string>	getHeaders() const;
	std::string							getBody() const;
	bool								isComplete() const;

private:

	std::string							p_rawRequest;
	std::string							p_method;
	std::string							p_uri;
	std::string							p_http_version;
	std::map<std::string, std::string>	p_headers;
	std::string							p_body;
	bool								p_isComplete;

	void								trimString(std::string &str);
	void								appendData(const std::string &data);
	void								parseRequestLine(const std::string &line);
	void								parseHeaderLine(const std::string &line);
};

#endif
