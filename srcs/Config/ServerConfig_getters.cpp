/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig_getters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:33:52 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

int ServerConfig::getListen() const
{
	return p_listen;
}

std::string ServerConfig::getServerName() const
{
	return p_serverName;
}

std::string ServerConfig::getRoot() const
{
	return p_root;
}

std::string ServerConfig::getIndex() const
{
	return p_index;
}

bool ServerConfig::getAutoindex() const
{
	return p_autoindex;
}

size_t ServerConfig::getClientMaxBodySize() const
{
	return p_clientMaxBodySize;
}

std::map<int, std::string> ServerConfig::getErrorPages() const
{
	return p_errorPages;
}

std::vector<LocationConfig> ServerConfig::getLocations() const
{
	return p_locations;
}
