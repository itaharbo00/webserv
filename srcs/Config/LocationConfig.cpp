/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:34 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 21:37:21 by itaharbo         ###   ########.fr       */
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
