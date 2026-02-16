/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:45 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/11 20:48:19 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ServerConfig.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h> // stat
#include <sys/wait.h> // waitpid
#include <signal.h>	  // kill, SIGKILL
#include <ctime>	  // time()

// Forward declarations des modules
class RouterGET;
class RouterPOST;
class RouterCGI;

class Router
{
public:
	Router();
	Router(const std::string &root);		  // Avec racine des fichiers
	Router(const ServerConfig *serverConfig); // Avec config serveur
	~Router();

	// Méthode principale de routing
	HttpResponse route(const HttpRequest &request);

	// Générer une réponse d'erreur sans requête valide
	HttpResponse createErrorResponse(int statusCode, const std::string &httpVersion);

	// CGI asynchrone
	bool isCgiRequest(const HttpRequest &request) const;
	int startCgiAsync(const HttpRequest &request,
					  const LocationConfig *location, const std::string &scriptPath,
					  pid_t &pid) const;
	HttpResponse buildCgiResponseAsync(const std::string &cgiOutput,
									   const HttpRequest &request) const;

private:
	std::string p_root;					// Racine des fichiers à servir
	const ServerConfig *p_serverConfig; // Configuration du serveur

	// Modules spécialisés
	RouterGET *p_getModule;
	RouterPOST *p_postModule;
	RouterCGI *p_cgiModule;

	// Handlers pour les différentes routes
	HttpResponse handleHomePage(const HttpRequest &request);
	HttpResponse handleAboutPage(const HttpRequest &request);
	HttpResponse handleNotFound(const HttpRequest &request);
	HttpResponse handleMethodNotAllowed(const HttpRequest &request);
	HttpResponse handlePost(const HttpRequest &request,
							const LocationConfig *location);
	HttpResponse handleDelete(const HttpRequest &request);
	HttpResponse serveStaticFile(const HttpRequest &request);

	HttpResponse routeWithConfig(const HttpRequest &request);
	HttpResponse routeWithoutConfig(const HttpRequest &request);
	HttpResponse routeByMethod(const HttpRequest &request,
							   const LocationConfig *location);
	HttpResponse handleRedirect(const HttpRequest &request,
								const LocationConfig *location);
	std::string generateRedirectPage(int statusCode, const std::string &locationUrl);

	// CGI env
	void buildBasicCgiEnv(std::vector<std::string> &env,
						  const HttpRequest &request,
						  const std::string &scriptPath) const;
	void buildServerCgiEnv(std::vector<std::string> &env,
						   const HttpRequest &request) const;
	void buildContentCgiEnv(std::vector<std::string> &env,
							const HttpRequest &request) const;
	void buildHttpHeadersCgiEnv(std::vector<std::string> &env,
								const HttpRequest &request) const;
	char **convertEnvToArray(const std::vector<std::string> &env) const;
	char **buildCgiEnv(const HttpRequest &request,
					   const LocationConfig *location, const std::string &scriptPath) const; // CGI execution
	bool validateCgiRequest(const std::string &scriptPath,
							const std::string &extension, const LocationConfig *location,
							std::string &cgiPath) const;
	bool setupCgiPipes(int pipe_in[2], int pipe_out[2]) const;
	void executeCgiChild(int pipe_in[2], int pipe_out[2],
						 const std::string &cgiPath, const std::string &scriptPath,
						 char **env) const;
	std::string executeCgiParent(int pipe_in[2], int pipe_out[2],
								 pid_t pid, const HttpRequest &request) const;
	void parseCgiOutput(const std::string &cgiOutput,
						std::string &headers, std::string &body) const;
	HttpResponse buildCgiResponse(const std::string &cgiHeaders,
								  const std::string &cgiBody, const HttpRequest &request) const;
	HttpResponse executeCgi(const HttpRequest &request,
							const LocationConfig *location, const std::string &scriptPath);

	// Utils pour le CGI
	std::string getCgiExtension(const std::string &filePath) const;
	void freeCgiEnv(char **env) const;

	// Fonctions utilitaires pour la gestion des fichiers
	std::string readFile(const std::string &filePath);
	bool fileExists(const std::string &filePath) const;
	bool isRegularFile(const std::string &filePath) const;
	bool isDirectory(const std::string &filePath);
	bool isPathSecure(const std::string &uri);
	bool isFileTooLarge(const std::string &filePath);
	std::string getContentType(const std::string &filePath);
	std::string generateDirectoryListing(const std::string &dirPath,
										 const std::string &uri);

	// Pages de contenu (home, about)
	bool getRightPages(int statusCode, std::string &page);
	std::string getPage_home() const;
	std::string getPage_about() const;

	// Pages HTML - Succès (2xx)
	std::string getPage_200() const;
	std::string getPage_201() const;
	std::string getPage_204() const;

	// Pages HTML - Redirections (3xx)
	std::string getPage_301() const;
	std::string getPage_302() const;
	std::string getPage_303() const;
	std::string getPage_304() const;
	std::string getPage_307() const;
	std::string getPage_308() const;

	// Pages HTML - Erreurs client (4xx)
	std::string getPage_400() const;
	std::string getPage_401() const;
	std::string getPage_403() const;
	std::string getPage_404() const;
	std::string getPage_405() const;
	std::string getPage_408() const;
	std::string getPage_411() const;
	std::string getPage_413() const;
	std::string getPage_414() const;
	std::string getPage_415() const;
	std::string getPage_431() const;

	// Pages HTML - Erreurs serveur (5xx)
	std::string getPage_500() const;
	std::string getPage_501() const;
	std::string getPage_502() const;
	std::string getPage_503() const;
	std::string getPage_504() const;
	std::string getPage_505() const;
};

#endif // ROUTER_HPP
