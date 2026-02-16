/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_cgi_env.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:30:45 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/11 20:48:19 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

void Router::buildBasicCgiEnv(std::vector<std::string> &env,
							  const HttpRequest &request,
							  const std::string &scriptPath) const
{
	// REQUEST_METHOD (GET, POST, DELETE)
	env.push_back("REQUEST_METHOD=" + request.getMethod());

	// Extraire l'URI et séparer la query string
	std::string uri = request.getUri();
	std::string uriPath = uri;
	size_t queryPos = uri.find('?');
	if (queryPos != std::string::npos)
		uriPath = uri.substr(0, queryPos);

	// SCRIPT_NAME (chemin du script dans l'URL, sans PATH_INFO ni query)
	// Ex: /cgi-bin/script.php/extra → SCRIPT_NAME=/cgi-bin/script.php
	std::string scriptName = uriPath;

	// Trouver où se termine le nom du script
	// Chercher l'extension du script dans l'URI
	size_t extPos = uriPath.find(".php");
	if (extPos == std::string::npos)
		extPos = uriPath.find(".py");
	if (extPos == std::string::npos)
		extPos = uriPath.find(".sh");
	if (extPos == std::string::npos)
		extPos = uriPath.find(".pl");

	// PATH_INFO: tout ce qui vient après le nom du script
	std::string pathInfo;
	if (extPos != std::string::npos)
	{
		// Trouver la fin de l'extension (3 ou 4 caractères)
		size_t scriptEnd = uriPath.find('/', extPos);
		if (scriptEnd != std::string::npos)
		{
			// Il y a du PATH_INFO
			scriptName = uriPath.substr(0, scriptEnd);
			pathInfo = uriPath.substr(scriptEnd);
			env.push_back("PATH_INFO=" + pathInfo);
		}
		else
		{
			// Pas de PATH_INFO
			scriptName = uriPath;
			env.push_back("PATH_INFO=");
		}
	}
	else
	{
		env.push_back("PATH_INFO=");
	}

	env.push_back("SCRIPT_NAME=" + scriptName);

	// SCRIPT_FILENAME (chemin absolu du script sur le système de fichiers)
	env.push_back("SCRIPT_FILENAME=" + scriptPath);

	// QUERY_STRING (partie après '?' dans l'URL)
	if (queryPos != std::string::npos)
		env.push_back("QUERY_STRING=" + uri.substr(queryPos + 1));
	else
		env.push_back("QUERY_STRING=");
}

// Construire les variables d'environnement CGI liées au serveur
void Router::buildServerCgiEnv(std::vector<std::string> &env,
							   const HttpRequest &request) const
{
	// SERVER_PROTOCOL
	env.push_back("SERVER_PROTOCOL=" + request.getHttpVersion());

	// SERVER_NAME et SERVER_PORT (extrait du header Host)
	std::string hostHeader = request.getHeader("Host");
	if (!hostHeader.empty())
	{
		// Séparer le nom d'hôte et le port si présent
		size_t colonPos = hostHeader.find(':');
		if (colonPos != std::string::npos)
		{
			env.push_back("SERVER_NAME=" + hostHeader.substr(0, colonPos));
			env.push_back("SERVER_PORT=" + hostHeader.substr(colonPos + 1));
		}
		else // Pas de port spécifié
		{
			env.push_back("SERVER_NAME=" + hostHeader);
			env.push_back("SERVER_PORT=80"); // Port par défaut
		}
	}
	else // Header Host absent
	{
		env.push_back("SERVER_NAME=localhost");
		env.push_back("SERVER_PORT=8080"); // Port par défaut
	}

	// REDIRECT_STATUS (requis par PHP-CGI pour des raisons de sécurité)
	env.push_back("REDIRECT_STATUS=200");
}

// Construire les variables d'environnement CGI liées au contenu de la requête
void Router::buildContentCgiEnv(std::vector<std::string> &env,
								const HttpRequest &request) const
{
	// CONTENT_LENGTH (pour POST)
	std::string contentLength = request.getHeader("Content-Length");
	if (!contentLength.empty())
		env.push_back("CONTENT_LENGTH=" + contentLength);
	else
		env.push_back("CONTENT_LENGTH=0");

	// CONTENT_TYPE (pour POST)
	std::string contentType = request.getHeader("Content-Type");
	if (!contentType.empty())
		env.push_back("CONTENT_TYPE=" + contentType);
}

// Construire les variables d'environnement CGI pour les headers HTTP
void Router::buildHttpHeadersCgiEnv(std::vector<std::string> &env, const HttpRequest &request) const
{
	std::map<std::string, std::string> headers = request.getHeaders();

	// Parcourir tous les headers et les ajouter à l'environnement CGI
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		 it != headers.end(); ++it)
	{
		std::string key = it->first;
		std::string value = it->second;

		// Convertir le nom du header en majuscules et remplacer '-' par '_'
		for (size_t i = 0; i < key.length(); ++i)
		{
			if (key[i] == '-') // Remplacer '-' par '_'
				key[i] = '_';
			else
				key[i] = std::toupper(key[i]);
		}
		if (key != "CONTENT_TYPE" && key != "CONTENT_LENGTH" && key != "HOST")
			env.push_back("HTTP_" + key + "=" + value); // Préfixer avec HTTP_
	}
}

// Convertir le vecteur d'environnement en tableau char**
char **Router::convertEnvToArray(const std::vector<std::string> &env) const
{
	// Allouer le tableau de pointeurs
	char **envArray = new char *[env.size() + 1]; // +1 pour le NULL final

	// Allouer et copier chaque chaîne dans le tableau
	for (size_t i = 0; i < env.size(); ++i)
	{
		envArray[i] = new char[env[i].length() + 1];
		std::strcpy(envArray[i], env[i].c_str());
	}
	envArray[env.size()] = NULL; // Terminer par NULL

	return envArray;
}

// Construire l'environnement complet pour le CGI
char **Router::buildCgiEnv(const HttpRequest &request,
						   const LocationConfig *location, const std::string &scriptPath) const
{
	(void)location; // Pour l'instant inutilisé, mais peut être utile plus tard
	std::vector<std::string> env;

	// 1. Variables de base (REQUEST_METHOD, SCRIPT_NAME, QUERY_STRING, etc.)
	buildBasicCgiEnv(env, request, scriptPath);
	// 2. Variables serveur (SERVER_PROTOCOL, SERVER_NAME, SERVER_PORT)
	buildServerCgiEnv(env, request);
	// 3. Variables de contenu (CONTENT_LENGTH, CONTENT_TYPE)
	buildContentCgiEnv(env, request);
	// 4. Headers HTTP (HTTP_*)
	buildHttpHeadersCgiEnv(env, request);

	return convertEnvToArray(env); // Convertir en tableau char**
}
