/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_location_directives.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:14:19 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 15:54:34 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void	Config::parseAllowedMethods(const std::string &value, LocationConfig &locationConfig)
{
	std::vector<std::string>	methods = split(value, ' ');

	// Commencer à partir de l'index 1 pour ignorer "allow_methods"
	for (size_t i = 1; i < methods.size(); ++i)
	{
		std::string	method = methods[i];

		// Enlever le ';' à la fin si présent
		if (method[method.length() - 1] == ';')
			method = method.substr(0, method.length() - 1);

		// Vérifier si la méthode est valide
		if (method == "GET" || method == "POST" || method == "DELETE")
			locationConfig.setAllowedMethods(method);
		else
			throw std::runtime_error("Invalid method in allow_methods directive: " + method);
	}
}

void	Config::parseAutoindex(const std::string &value, LocationConfig &locationConfig)
{
	std::string	cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	// Mettre à jour la configuration de autoindex
	if (cleanValue == "on")
		locationConfig.setAutoindex(true);
	else if (cleanValue == "off")
		locationConfig.setAutoindex(false);
	else
		throw std::runtime_error("Invalid value for autoindex directive (must be 'on' or 'off')");
}

void	Config::parseUploadStore(const std::string &value, LocationConfig &locationConfig)
{
	std::string	cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	locationConfig.setUploadStore(cleanValue);
}

void	Config::parseCgiPass(const std::string &value, LocationConfig &locationConfig)
{
	std::vector<std::string>	tokens = split(value, ' ');

	if (tokens.size() < 3)
		throw std::runtime_error("cgi_pass directive missing extension or path");

	// Extraire l'extension et le chemin du CGI
	std::string	extension = tokens[1];
	std::string	cgiPath = tokens[2];

	if (cgiPath[cgiPath.length() - 1] == ';')
		cgiPath = cgiPath.substr(0, cgiPath.length() - 1);
	
	if (extension.empty() || extension[0] != '.')
		throw std::runtime_error("CGI extension must start with a dot");
		
	locationConfig.addCgiPass(extension, cgiPath);
}

void	Config::parseReturn(const std::string &value, LocationConfig &locationConfig)
{
	std::vector<std::string>	tokens = split(value, ' ');

	if (tokens.size() < 3)
		throw std::runtime_error("return directive missing code or URL");

	// Extraire le code de redirection
	int			code = std::atoi(tokens[1].c_str());
	std::string	url = tokens[2];

	if (url[url.length() - 1] == ';')
		url = url.substr(0, url.length() - 1);

	if (code < 300 || code > 399)
		throw std::runtime_error("Invalid HTTP redirect code in return directive");

	locationConfig.setReturn(code, url);
}
