/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig_setters.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:35:04 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 15:55:32 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <iostream>

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

void	LocationConfig::setRoot(const std::string &root)
{
	p_root = root; // Met à jour la racine pour cette location
}

void	LocationConfig::setIndex(const std::string &index)
{
	p_index = index; // Met à jour le fichier index pour cette location
}

void	LocationConfig::setAutoindex(bool autoindex)
{
	p_autoindex = autoindex; // Met à jour la configuration de autoindex
}

void	LocationConfig::setUploadStore(const std::string &uploadStore)
{
	p_uploadStore = uploadStore; // Met à jour le chemin pour les uploads
}

void	LocationConfig::setReturn(int code, const std::string &url)
{
	p_returnCode = code; // Met à jour le code de redirection
	p_returnUrl = url;   // Met à jour l'URL de redirection
}

void	LocationConfig::setClientMaxBodySize(size_t size)
{
	p_clientMaxBodySize = size;
}
