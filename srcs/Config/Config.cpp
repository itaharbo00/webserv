/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:24 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 20:19:53 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() : p_servers()
{
}

Config::Config(const std::string &configFile) : p_servers()
{
	parseConfigFile(configFile);
}

Config::~Config()
{
}

std::vector<ServerConfig>	Config::getServers() const
{
	return p_servers;
}

// Retourne la configuration du serveur écoutant sur le port donné
const ServerConfig	*Config::getServerByPort(int port) const
{
	for (size_t i = 0; i < p_servers.size(); ++i)
	{
		// Vérifier si le serveur écoute sur le port demandé
		if (p_servers[i].getListen() == port)
			return &p_servers[i]; // Return un pointeur vers config du serveur
	}

	return NULL; // Aucun serveur trouvé pour ce port
}

void	Config::parseConfigFile(const std::string &configFile)
{
	// Ouvrir le fichier de configuration
	std::ifstream	file(configFile.c_str());
	if (!file.is_open())
		throw std::runtime_error("Could not open config file: " + configFile);

	std::string		line;

	// Lire le fichier ligne par ligne
	while (std::getline(file, line))
	{
		line = trim(line); // Enlever les espaces inutiles
		
		if (isComment(line) || isEmptyLine(line))
			continue;

		if (line == "server" || line == "server {")
		{
			// Si "server" seul, lire le '{'
			if (line == "server")
			{
				std::getline(file, line);
				line = trim(line);
				if (line != "{")
					throw std::runtime_error("Expected '{' after 'server'");
			}

			// Parser le bloc serveur
			ServerConfig	server;
			parseServerBlock(file, server);
			p_servers.push_back(server);
		}
	}

	file.close(); // Fermer le fichier après la lecture

	// Vérifier qu'au moins un serveur a été configuré
	if (p_servers.empty())
		throw std::runtime_error("No server configuration found in file");
}

size_t	Config::parseSizeValue(const std::string &value)
{
	std::string	numStr;
	char		suffix = 0;

	// Extraire la partie numérique et le suffixe
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (std::isdigit(value[i]))
			numStr += value[i];
		else
		{
			suffix = value[i];
			break;
		}
	}

	// Convertir la partie numérique en size_t
	size_t	num = std::atol(numStr.c_str());

	// Appliquer le multiplicateur en fonction du suffixe
	if (suffix == 'K' || suffix == 'k')
		num *= 1024;
	else if (suffix == 'M' || suffix == 'm')
		num *= 1024 * 1024;
	else if (suffix == 'G' || suffix == 'g')
		num *= 1024 * 1024 * 1024;

	return num; // Retourner la taille en octets
}
