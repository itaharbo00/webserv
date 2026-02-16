/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_parse_server.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:30:20 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Config::parseServerBlock(std::ifstream &file, ServerConfig &serverConfig)
{
	std::string line;

	// Lire le bloc serveur ligne par ligne
	while (std::getline(file, line))
	{
		line = trim(line);

		if (isComment(line) || isEmptyLine(line))
			continue;

		if (line == "}")
			break; // Fin du bloc serveur

		// Tokenizer la ligne (ex : "listen 8080;" -> ["listen", "8080;"])
		std::vector<std::string> tokens = split(line, ' ');
		if (tokens.empty())
			continue;

		// Extraire la directive principale
		std::string directive = tokens[0];

		// Enlever ';' si présent
		if (directive[directive.length() - 1] == ';')
			directive = directive.substr(0, directive.length() - 1);

		try
		{
			directiveServerChecker(directive, file, tokens, line, serverConfig);
		}
		catch (...)
		{
		}
	}
}

// Vérifie et traite chaque directive dans un bloc serveur
void Config::directiveServerChecker(const std::string &directive, std::ifstream &file,
									const std::vector<std::string> &tokens, std::string line,
									ServerConfig &serverConfig)
{
	if (directive == "listen")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("Listen directive missing port");
		parseListen(tokens[1], serverConfig);
	}
	else if (directive == "server_name")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("Server_name directive missing name");
		parseServerName(tokens[1], serverConfig);
	}
	else if (directive == "root")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("Root directive missing path");
		parseRoot(tokens[1], serverConfig);
	}
	else if (directive == "index")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("Index directive missing file");
		parseIndex(tokens[1], serverConfig);
	}
	else if (directive == "autoindex")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("autoindex directive missing value");
		parseServerAutoindex(tokens[1], serverConfig);
	}
	else if (directive == "client_max_body_size")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("client_max_body_size directive missing size");
		parseClientMaxBodySize(tokens[1], serverConfig);
	}
	else if (directive == "error_page")
	{
		if (tokens.size() < 3)
			throw std::runtime_error("Error_page directive missing code or path");
		parseErrorPage(line, serverConfig);
	}
	else if (directive == "location")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("Location directive missing path");

		std::string path = tokens[1]; // (ex : /images)

		if (tokens.size() > 2 && tokens[2] == "{")
		{
			// OK La ligne est de la forme "location /path {"
		}
		else
		{
			// La ligne est de la forme "location /path"
			// Lire la ligne suivante qui doit être "{"
			std::getline(file, line);
			line = trim(line);
			if (line != "{")
				throw std::runtime_error("Expected '{' after location path" + path + "'");
		}

		// Parser le bloc location
		LocationConfig location(path);
		parseLocationBlock(file, location);
		serverConfig.addLocation(location);
	}
	else
		throw std::runtime_error("Unknown directive: " + directive);
}
