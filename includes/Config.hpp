/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:16:57 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 21:18:19 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include <fstream>
# include <sstream>
# include <stdexcept>
# include <iostream>
# include <cstdlib>

class Config
{
public:

	Config();
	Config(const std::string &configFile);
	~Config();

	// Parser le fichier de configuration
	void						parseConfigFile(const std::string &configFile);

	// Getters
	std::vector<ServerConfig>	getServers() const;
	const ServerConfig			*getServerByPort(int port) const;

private:

	std::vector<ServerConfig>	p_servers; // Liste des configurations de serveurs

	// Parsing helpers
	void						parseServerBlock(std::ifstream &file,
									ServerConfig &serverConfig);
	void						directiveServerChecker(const std::string &directive,
									std::ifstream &file,
									const std::vector<std::string> &tokens,
									std::string line,
									ServerConfig &serverConfig);

	void						parseLocationBlock(std::ifstream &file,
									LocationConfig &locationConfig);
	void						directiveLocationChecker(const std::string &directive,
									const std::vector<std::string> &tokens,
									std::string line,
									LocationConfig &locationConfig);

	// String utils
	std::string					trim(const std::string &str);
	std::string					readLine(std::ifstream &file);
	bool						isComment(const std::string &line);
	bool						isEmptyLine(const std::string &line);
	std::vector<std::string>	split(const std::string &str, char delimiter);

	// Directive parsers (Server level)
	void						parseListen(const std::string &value,
									ServerConfig &serverConfig);
	void						parseServerName(const std::string &value,
									ServerConfig &serverConfig);
	void						parseRoot(const std::string &value,
									ServerConfig &serverConfig);
	void						parseIndex(const std::string &value,
									ServerConfig &serverConfig);
	void						parseClientMaxBodySize(const std::string &value,
									ServerConfig &serverConfig);
	void						parseErrorPage(const std::string &value,
									ServerConfig &serverConfig);

	// Directive parsers (Location level)
	void						parseAllowedMethods(const std::string &value,
									LocationConfig &locationConfig);
	void						parseAutoindex(const std::string &value,
									LocationConfig &locationConfig);
	void						parseUploadStore(const std::string &value,
									LocationConfig &locationConfig);
	void						parseCgiPass(const std::string &value,
									LocationConfig &locationConfig);
	void						parseCgiExt(const std::string &value,
									LocationConfig &locationConfig);
	void						parseReturn(const std::string &value,
									LocationConfig &locationConfig);

	// Utils
	size_t						parseSizeValue(const std::string &sizeStr);
};

#endif	// CONFIG_HPP
