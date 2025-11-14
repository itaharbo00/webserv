/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:34 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 16:52:12 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : p_path("/"), p_allowedMethods()
{
	// p_path = "/" par défaut (location root)
}

LocationConfig::LocationConfig(const std::string &path) : p_path(path), p_allowedMethods()
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

void	LocationConfig::setPath(const std::string &path)
{
	p_path = path; // Met à jour le chemin de la location
}

void	LocationConfig::setAllowedMethods(const std::string &method)
{
	// Ajouter la méthode seulement si elle n'est pas déjà présente
	if (!isMethodAllowed(method))
		p_allowedMethods.push_back(method);
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
