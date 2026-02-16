/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:30:38 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 19:53:24 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// Enlève les espaces et tabs au début et à la fin de la chaîne
std::string	Config::trim(const std::string &str)
{
	size_t	start = str.find_first_not_of(" \t\r\n");
	size_t	end = str.find_last_not_of(" \t\r\n");

	if (start == std::string::npos)
		return "";

	return str.substr(start, end - start + 1);
}

// Lit une ligne du fichier et la retourne
std::string	Config::readLine(std::ifstream &file)
{
	std::string	line;

	if (std::getline(file, line))
		return line;
	else
		return "";
}

// Vérifie si la ligne est un commentaire (commence par '#')
bool	Config::isComment(const std::string &line)
{
	std::string	trimmed = trim(line);
	return (!trimmed.empty() && trimmed[0] == '#');
}

// Vérifie si la ligne est vide (après trim)
bool	Config::isEmptyLine(const std::string &line)
{
	std::string	trimmed = trim(line);
	return trimmed.empty();
}

// Divise une chaîne en tokens basés sur le délimiteur donné
std::vector<std::string>	Config::split(const std::string &str,
									char delimiter)
{
	std::vector<std::string>	tokens;
	std::stringstream			ss(str); // Utiliser stringstream pour le split
	std::string					item;

	// Extraire chaque token
	while (std::getline(ss, item, delimiter))
	{
		item = trim(item);
		// Ajouter le token s'il n'est pas vide
		if (!item.empty())
			tokens.push_back(item);
	}

	return tokens;
}
