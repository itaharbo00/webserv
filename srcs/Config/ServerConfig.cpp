/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:20:32 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:41:45 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#define MAX_BODY_SIZE_DEFAULT 1048576 // 10 MB

ServerConfig::ServerConfig()
	: p_listen(8080), p_serverName("localhost"), p_root("./www"),
	  p_index(""), p_autoindex(false), p_clientMaxBodySize(MAX_BODY_SIZE_DEFAULT),
	  p_errorPages(), p_locations()
{
	p_locations.reserve(10); // Réserve de l'espace pour éviter les copies
}

ServerConfig::~ServerConfig()
{
}

const LocationConfig *ServerConfig::findLocation(const std::string &uri) const
{
	const LocationConfig *bestMatch = NULL; // Meilleur match trouvé
	size_t bestMatchLen = 0;				// Longueur du meilleur match

	// Parcourir toutes les locations
	for (size_t i = 0; i < p_locations.size(); ++i)
	{
		std::string path = p_locations[i].getPath();

		// Vérifier si le chemin de la location est un préfixe de l'URI
		if (uri.find(path) == 0)
		{
			// Si c'est le plus long match jusqu'à présent, le garder
			if (path.length() > bestMatchLen)
			{
				bestMatch = &p_locations[i];
				bestMatchLen = path.length();
			}
		}
	}

	return bestMatch; // Retourner le meilleur match ou NULL si aucun
}

std::string ServerConfig::getErrorPage(int code) const
{
	// Chercher dans la map
	std::map<int, std::string>::const_iterator it = p_errorPages.find(code);

	if (it != p_errorPages.end())
		return it->second; // Trouvé !

	return ""; // Pas de page d'erreur configurée
}
