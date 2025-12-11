/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:34 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 15:45:51 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <iostream>

LocationConfig::LocationConfig() : p_path("/"), p_allowedMethods(), p_root(""),
	p_index(""), p_autoindex(false), p_uploadStore(""), p_cgi_pass(),
	p_returnCode(0), p_returnUrl(""), p_clientMaxBodySize(0)
{
	// p_path = "/" par défaut (location root)
}

LocationConfig::LocationConfig(const std::string &path) : p_path(path),
	p_allowedMethods(), p_root(""), p_index(""), p_autoindex(false),
	p_uploadStore(""), p_cgi_pass(), p_returnCode(0), p_returnUrl(""),
	p_clientMaxBodySize(0)
{
}

LocationConfig::~LocationConfig()
{
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

// Vérifie si une redirection est configurée
bool	LocationConfig::hasRedirect() const
{
	return (p_returnCode != 0 && !p_returnUrl.empty());
}

// Vérifie si l'upload est configuré
bool	LocationConfig::hasUpload() const
{
	return (!p_uploadStore.empty());
}

// Vérifie si des CGI sont configurés
bool	LocationConfig::hasCgi() const
{
	return !p_cgi_pass.empty();
}

// Ajoute une association extension → chemin du CGI
void	LocationConfig::addCgiPass(const std::string &extension,
			const std::string &cgiPath)
{
	p_cgi_pass[extension] = cgiPath;
}

// Vérifie si une extension est configurée pour le CGI
bool	LocationConfig::isCgiExtension(const std::string &extension) const
{
	return (p_cgi_pass.find(extension) != p_cgi_pass.end());
}

// Retourne le chemin du CGI pour une extension donnée
std::string	LocationConfig::getCgiPass(const std::string &extension) const
{
	// Chercher l'extension dans la map
	std::map<std::string, std::string>::const_iterator it = p_cgi_pass.find(extension);
	if (it != p_cgi_pass.end())
		return it->second;
	return ""; // Extension non trouvée
}

// Retourne la map complète des CGI
const std::map<std::string, std::string>&	LocationConfig::getCgiPassMap() const
{
	return p_cgi_pass;
}
