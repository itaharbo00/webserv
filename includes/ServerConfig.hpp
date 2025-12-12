/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:16:55 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include "LocationConfig.hpp"

class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();

	// Getters (recupérer les valeurs)
	int getListen() const;
	std::string getServerName() const;
	std::string getRoot() const;
	std::string getIndex() const;
	bool getAutoindex() const;
	size_t getClientMaxBodySize() const;
	std::map<int, std::string> getErrorPages() const;
	std::vector<LocationConfig> getLocations() const;

	// Setters (modifier les valeurs)
	void setListen(int port);
	void setServerName(const std::string &name);
	void setRoot(const std::string &root);
	void setIndex(const std::string &index);
	void setAutoindex(bool autoindex);
	void setClientMaxBodySize(size_t size);
	void addErrorPage(int code, const std::string &path);
	void addLocation(const LocationConfig &location);

	// Finders (chercher des valeurs)
	const LocationConfig *findLocation(const std::string &uri) const;
	std::string getErrorPage(int code) const;

private:
	int p_listen;							 // Port d'écoute
	std::string p_serverName;				 // Nom du serveur
	std::string p_root;						 // Racine du serveur
	std::string p_index;					 // Page d'index
	bool p_autoindex;						 // Autoindex activé par défaut
	size_t p_clientMaxBodySize;				 // Taille max du corps de la requête
	std::map<int, std::string> p_errorPages; // Pages d'erreur personnalisées
	std::vector<LocationConfig> p_locations; // Configurations des locations
};

#endif // SERVERCONFIG_HPP
