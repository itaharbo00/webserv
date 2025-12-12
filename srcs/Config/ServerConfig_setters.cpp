/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig_setters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:34:52 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

void ServerConfig::setListen(int port)
{
	p_listen = port;
}

void ServerConfig::setServerName(const std::string &name)
{
	p_serverName = name;
}

void ServerConfig::setRoot(const std::string &root)
{
	p_root = root;
}

void ServerConfig::setIndex(const std::string &index)
{
	p_index = index;
}

void ServerConfig::setAutoindex(bool autoindex)
{
	p_autoindex = autoindex;
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
	p_clientMaxBodySize = size;
}

void ServerConfig::addErrorPage(int code, const std::string &path)
{
	p_errorPages[code] = path; // Ajoute ou met à jour la page d'erreur
}

void ServerConfig::addLocation(const LocationConfig &location)
{
	p_locations.push_back(location); // Ajoute une nouvelle location
}
