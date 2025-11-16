/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_cgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:20:34 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 16:26:08 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// Extrait l'extension du fichier pour déterminer le type de CGI
std::string	Router::getCgiExtension(const std::string &filePath) const
{
	size_t	dotPos = filePath.rfind('.');
	if (dotPos == std::string::npos)
		return ""; // Pas d'extension trouvée

	return filePath.substr(dotPos); // Retourne l'extension avec le '.'
}

// Libère la mémoire allouée pour l'environnement CGI
void	Router::freeCgiEnv(char **env) const
{
	if (!env)
		return;

	for (size_t i = 0; env[i] != NULL; ++i)
		delete[] env[i];

	delete[] env;
}
