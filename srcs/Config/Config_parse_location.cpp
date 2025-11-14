/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_parse_location.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:30:26 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 22:04:22 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void	Config::parseLocationBlock(std::ifstream &file, LocationConfig &locationConfig)
{
	std::string	line;

	// Lire le bloc location ligne par ligne
	while (std::getline(file, line))
	{
		line = trim(line);

		if (isComment(line) || isEmptyLine(line))
			continue;

		if (line == "}")
			break; // Fin du bloc location

		// Tokenizer la ligne (ex : "autoindex on;" -> ["autoindex", "on;"])
		std::vector<std::string>	tokens = split(line, ' ');
		if (tokens.empty())
			continue;

		// Extraire la directive principale
		std::string	directive = tokens[0];

		// Enlever ';' si présent
		if (directive[directive.length() - 1] == ';')
			directive = directive.substr(0, directive.length() - 1);

		try
		{
			directiveLocationChecker(directive, tokens, line, locationConfig);
		}
		catch (...)
		{
		}
	}
}

void	Config::directiveLocationChecker(const std::string &directive,
			const std::vector<std::string> &tokens, std::string line,
			LocationConfig &locationConfig)
{
	if (directive == "allow_methods")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("allow_methods directive missing methods");
		parseAllowedMethods(line, locationConfig);
	}
	else if (directive == "autoindex")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("autoindex directive missing value");
		parseAutoindex(tokens[1], locationConfig);
	}
	else if (directive == "upload_store")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("upload_store directive missing path");
		parseUploadStore(tokens[1], locationConfig);
	}
	else if (directive == "cgi_pass")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("cgi_pass directive missing path");
		parseCgiPass(tokens[1], locationConfig);
	}
	else if (directive == "cgi_ext")
	{
		if (tokens.size() < 2)
			throw std::runtime_error("cgi_ext directive missing extension");
		parseCgiExt(tokens[1], locationConfig);
	}
	else if (directive == "return")
	{
		if (tokens.size() < 3)
			throw std::runtime_error("return directive missing code or URL");
		parseReturn(line, locationConfig);
	}
	else
		throw std::runtime_error("Unknown directive in location block: " + directive);
}
