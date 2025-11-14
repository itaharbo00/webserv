/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:34 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 22:04:22 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : p_path("/"), p_allowedMethods(), p_root(""),
	p_index(""), p_autoindex(false), p_uploadStore(""), p_cgiPass(""), p_cgiExt(""),
	p_returnCode(0), p_returnUrl("")
{
	// p_path = "/" par défaut (location root)
}

LocationConfig::LocationConfig(const std::string &path) : p_path(path),
	p_allowedMethods(), p_root(""), p_index(""), p_autoindex(false),
	p_uploadStore(""), p_cgiPass(""), p_cgiExt(""), p_returnCode(0), p_returnUrl("")
{
}

LocationConfig::~LocationConfig()
{
}

std::string	LocationConfig::getPath() const
{
	return p_path;
}

std::vector<std::string>	LocationConfig::getAllowedMethods() const
{
	return p_allowedMethods;
}

std::string	LocationConfig::getRoot() const
{
	return p_root;
}

std::string	LocationConfig::getIndex() const
{
	return p_index;
}

bool	LocationConfig::getAutoindex() const
{
	return p_autoindex;
}

std::string	LocationConfig::getUploadStore() const
{
	return p_uploadStore;
}

std::string	LocationConfig::getCgiPass() const
{
	return p_cgiPass;
}

std::string	LocationConfig::getCgiExt() const
{
	return p_cgiExt;
}

int	LocationConfig::getReturnCode() const
{
	return p_returnCode;
}

std::string	LocationConfig::getReturnUrl() const
{
	return p_returnUrl;
}

// Vérifie si une méthode est autorisée dans cette location
bool	LocationConfig::isMethodAllowed(const std::string &method) const
{
	for (std::vector<std::string>::const_iterator it = p_allowedMethods.begin();
		it != p_allowedMethods.end(); ++it)
	{
		if (*it == method)
			return true; // Méthode trouvée, elle est autorisée
	}
	return false; // Méthode non trouvée, elle n'est pas autorisée
}

bool	LocationConfig::hasRedirect() const
{
	return (p_returnCode != 0 && !p_returnUrl.empty());
}

bool	LocationConfig::hasCgi() const
{
	return (!p_cgiPass.empty() && !p_cgiExt.empty());
}

bool	LocationConfig::hasUpload() const
{
	return (!p_uploadStore.empty());
}
