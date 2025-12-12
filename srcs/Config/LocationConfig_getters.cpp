/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig_getters.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 15:31:43 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <iostream>

std::string LocationConfig::getPath() const
{
	return p_path;
}

std::vector<std::string> LocationConfig::getAllowedMethods() const
{
	return p_allowedMethods;
}

std::string LocationConfig::getRoot() const
{
	return p_root;
}

std::string LocationConfig::getIndex() const
{
	return p_index;
}

bool LocationConfig::getAutoindex() const
{
	return (p_autoindex);
}

bool LocationConfig::isAutoindexSet() const
{
	return (p_autoindexSet);
}

std::string LocationConfig::getUploadStore() const
{
	return p_uploadStore;
}

int LocationConfig::getReturnCode() const
{
	return p_returnCode;
}

std::string LocationConfig::getReturnUrl() const
{
	return p_returnUrl;
}

size_t LocationConfig::getClientMaxBodySize() const
{
	return (p_clientMaxBodySize);
}
