/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_server_directives.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:14:22 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Config::parseListen(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	// Enlever le ';' à la fin si présent
	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	// Convertir la valeur en entier
	int port = std::atoi(cleanValue.c_str());
	if (port <= 0 || port > 65535)
		throw std::runtime_error("Invalid port number in listen directive");

	serverConfig.setListen(port); // Mettre à jour la configuration du serveur
}

void Config::parseServerName(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	serverConfig.setServerName(cleanValue);
}

void Config::parseRoot(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	serverConfig.setRoot(cleanValue);
}

void Config::parseIndex(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	serverConfig.setIndex(cleanValue);
}

void Config::parseServerAutoindex(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	if (cleanValue == "on")
		serverConfig.setAutoindex(true);
	else if (cleanValue == "off")
		serverConfig.setAutoindex(false);
	else
		throw std::runtime_error("Invalid autoindex value: " + cleanValue);
}

void Config::parseClientMaxBodySize(const std::string &value, ServerConfig &serverConfig)
{
	std::string cleanValue = value;

	if (cleanValue[cleanValue.length() - 1] == ';')
		cleanValue = cleanValue.substr(0, cleanValue.length() - 1);

	size_t size = parseSizeValue(cleanValue);
	serverConfig.setClientMaxBodySize(size);
}

void Config::parseErrorPage(const std::string &value, ServerConfig &serverConfig)
{
	// Tokenizer la ligne
	std::vector<std::string> tokens = split(value, ' ');

	// Format attendu : error_page <code1> <code2> ... <path>;
	if (tokens.size() < 3)
		throw std::runtime_error("Invalid error_page directive");

	// Extraire le code d'erreur
	std::string path = tokens[tokens.size() - 1];
	if (path[path.length() - 1] == ';')
		path = path.substr(0, path.length() - 1);

	for (size_t i = 1; i < tokens.size() - 1; ++i)
	{
		// Extraire et valider chaque code d'erreur
		std::string codeStr = tokens[i];

		if (codeStr[codeStr.length() - 1] == ';')
			codeStr = codeStr.substr(0, codeStr.length() - 1);

		// Convertir en entier
		int code = std::atoi(codeStr.c_str());
		if (code < 100 || code > 599)
			throw std::runtime_error("Invalid HTTP error code in error_page directive");

		serverConfig.addErrorPage(code, path);
	}
}
