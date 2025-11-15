/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:45 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 18:38:02 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "ServerConfig.hpp"
# include <fstream>
# include <sys/stat.h>	// stat

class Router
{
public:
	Router();
	Router(const std::string &root); // Avec racine des fichiers
	Router(const ServerConfig *serverConfig); // Avec config serveur
	~Router();

	// Méthode principale de routing
	HttpResponse		route(const HttpRequest &request);
	
	// Générer une réponse d'erreur sans requête valide
	HttpResponse		createErrorResponse(int statusCode,
							const std::string &httpVersion);

private:

	std::string			p_root; // Racine des fichiers à servir
	const ServerConfig	*p_serverConfig; // Configuration du serveur

	// Handlers pour les différentes routes
	HttpResponse		handleHomePage(const HttpRequest &request);
	HttpResponse		handleAboutPage(const HttpRequest &request);
	HttpResponse		handleNotFound(const HttpRequest &request);
	HttpResponse		handleMethodNotAllowed(const HttpRequest &request);
	HttpResponse		handlePost(const HttpRequest &request,
							const LocationConfig *location);
	HttpResponse		handleDelete(const HttpRequest &request);
	HttpResponse		serveStaticFile(const HttpRequest &request);

	HttpResponse		routeWithConfig(const HttpRequest &request);
	HttpResponse		routeWithoutConfig(const HttpRequest &request);
	HttpResponse		routeByMethod(const HttpRequest &request,
							const LocationConfig *location);
	HttpResponse		handleRedirect(const HttpRequest &request,
							const LocationConfig *location);
	std::string			generateRedirectPage(int statusCode,
							const std::string &locationUrl);

	// Fonctions utilitaires pour la gestion des fichiers
	std::string			readFile(const std::string &filePath);
	bool				fileExists(const std::string &filePath);
	bool				isRegularFile(const std::string &filePath);
	bool				isDirectory(const std::string &filePath);
	bool				isPathSecure(const std::string &uri);
	bool				isFileTooLarge(const std::string &filePath);
	std::string			getContentType(const std::string &filePath);
	std::string			generateDirectoryListing(const std::string &dirPath,
							const std::string &uri);

	// Pages de contenu (home, about)
	bool				getRightPages(int statusCode, std::string &page);
	std::string			getPage_home() const;
	std::string			getPage_about() const;

	// Pages HTML - Succès (2xx)
	std::string			getPage_200() const;
	std::string			getPage_201() const;
	std::string			getPage_204() const;

	// Pages HTML - Redirections (3xx)
	std::string			getPage_301() const;
	std::string			getPage_302() const;
	std::string			getPage_303() const;
	std::string			getPage_304() const;
	std::string			getPage_307() const;
	std::string			getPage_308() const;

	// Pages HTML - Erreurs client (4xx)
	std::string			getPage_400() const;
	std::string			getPage_401() const;
	std::string			getPage_403() const;
	std::string			getPage_404() const;
	std::string			getPage_405() const;
	std::string			getPage_408() const;
	std::string			getPage_411() const;
	std::string			getPage_413() const;
	std::string			getPage_414() const;
	std::string			getPage_415() const;
	std::string			getPage_431() const;

	// Pages HTML - Erreurs serveur (5xx)
	std::string			getPage_500() const;
	std::string			getPage_501() const;
	std::string			getPage_502() const;
	std::string			getPage_503() const;
	std::string			getPage_504() const;
	std::string			getPage_505() const;
};

#endif	// ROUTER_HPP
