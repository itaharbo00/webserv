/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_cgi_execute.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:30:42 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 18:59:37 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

// Valider la requête CGI avant l'exécution
bool	Router::validateCgiRequest(const std::string &scriptPath,
			const std::string &extension, const LocationConfig *location,
			std::string &cgiPath) const
{
	// Vérifier si le script existe et est exécutable
	if (!fileExists(scriptPath) || !isRegularFile(scriptPath))
		return false;

	// Vérifier si l'extension est configurée pour le CGI dans la location
	if (!location || !location->isCgiExtension(extension))
		return false;

	// Récupérer le chemin du binaire CGI configuré
	cgiPath = location->getCgiPass(extension);
	if (cgiPath.empty())
		return false; // Pas de chemin CGI configuré

	return true; // Validation réussie
}

// Configurer les pipes pour la communication avec le processus CGI
bool	Router::setupCgiPipes(int pipe_in[2], int pipe_out[2]) const
{
	// Créer les pipes pour la communication
	if (pipe(pipe_in) == -1)
		return false;

	// Créer le pipe de sortie
	if (pipe(pipe_out) == -1)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		return false;
	}

	return true;
}

// Exécuter le processus CGI dans le processus fils
void	Router::executeCgiChild(int pipe_in[2], int pipe_out[2],
			const std::string &cgiPath, const std::string &scriptPath,
			char **env) const
{
	// Rediriger l'entrée standard vers le pipe d'entrée
	dup2(pipe_in[0], STDIN_FILENO);
	close(pipe_in[0]); // Fermer les extrémités inutiles
	close(pipe_in[1]);

	// Rediriger la sortie standard vers le pipe de sortie
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_out[0]); // Fermer les extrémités inutiles
	close(pipe_out[1]);

	// Préparer les arguments pour execve
	char *args[3];
	args[0] = const_cast<char *>(cgiPath.c_str());
	args[1] = const_cast<char *>(scriptPath.c_str());
	args[2] = NULL;

	//Exécuter le CGI
	execve(cgiPath.c_str(), args, env);

	// Si execve échoue, afficher une erreur et quitter
	std::cerr << "execve failed" << std::strerror(errno) << std::endl;
	exit(1);
}

// Gérer l'exécution du CGI dans le processus parent
std::string	Router::executeCgiParent(int pipe_in[2], int pipe_out[2],
				pid_t pid, const HttpRequest &request) const
{
	close(pipe_in[0]); // Fermer lecture du pipe d'entrée
	close(pipe_out[1]); // Fermer écriture du pipe de sortie

	std::string	body = request.getBody();
	if (!body.empty())
	{
		// Écrire le corps de la requête dans le pipe d'entrée du CGI
		ssize_t written = write(pipe_in[1], body.c_str(), body.length());
		if (written < 0)
		{
			close(pipe_in[1]);
			close(pipe_out[0]);
			throw std::runtime_error("Failed to write to CGI stdin: "
				+ std::string(std::strerror(errno)));
		}
	}

	close(pipe_in[1]); // Fermer l'écriture après avoir envoyé le corps

	// Lire la sortie du CGI
	std::string	cgiOutput;
	char		buffer[4096];
	ssize_t		bytesRead;

	// Ajouter les données lues à la sortie CGI jusqu'à la fin
	while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, bytesRead);

	close(pipe_out[0]); // Fermer la lecture après avoir tout lu

	// Attendre la fin du processus CGI
	int	status;
	waitpid(pid, &status, 0); // Attendre la fin du processus CGI

	// Vérifier le statut de sortie du processus CGI
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		return ""; // CGI a échoué

	return cgiOutput; // Retourner la sortie CGI
}

void	Router::parseCgiOutput(const std::string &cgiOutput,
			std::string &headers, std::string &body) const
{
	size_t	headerEnd = cgiOutput.find("\r\n\r\n");

	if (headerEnd != std::string::npos)
	{
		headers = cgiOutput.substr(0, headerEnd);
		body = cgiOutput.substr(headerEnd + 4); // +4 pour sauter "\r\n\r\n"
	}
	else
	{
		// Seulement des sauts de ligne "\n\n" (certains CGI utilisent ça)
		headerEnd = cgiOutput.find("\n\n");
		if (headerEnd != std::string::npos)
		{
			headers = cgiOutput.substr(0, headerEnd);
			body = cgiOutput.substr(headerEnd + 2); // +2 pour sauter "\n\n"
		}
		else
		{
			// Pas de séparation claire, tout est considéré comme corps
			headers = "";
			body = cgiOutput;
		}
	}
}

HttpResponse	Router::buildCgiResponse(const std::string &cgiHeaders,
					const std::string &cgiBody, const HttpRequest &request) const
{
	HttpResponse	response;
	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200); // Par défaut, 200 OK
	response.setBody(cgiBody);

	// Parser les headers CGI
	std::istringstream	headerStream(cgiHeaders);
	std::string			line;

	while (std::getline(headerStream, line))
	{
		if (line.empty() || line == "\r")
			continue;
		// Enlever le retour chariot éventuel
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t	colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string	key = line.substr(0, colonPos);
			std::string	value = line.substr(colonPos + 1);

			// Enlever les espaces avant et après la valeur
			size_t	start = value.find_first_not_of(" \t");
			if (start != std::string::npos)
				value = value.substr(start);

			// Gérer le header Status pour définir le code de statut
			if (key == "Status")
			{
				std::istringstream	statusStream(value);
				int					statusCode;
				statusStream >> statusCode;
				response.setStatusCode(statusCode);
			}
			else if (key == "Location")
			{
				response.setHeader(key, value);
				// Pour les redirections, définir le code 302 si pas déjà défini
				if (response.getStatusCode() == 200)
					response.setStatusCode(302);
			}
			else
			{
				// Ajouter les autres headers normalement
				response.setHeader(key, value);
			}
		}
	}

	// Gérer la connexion
	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse	Router::executeCgi(const HttpRequest &request,
			const LocationConfig *location, const std::string &scriptPath)
{
	// Valider la requête CGI
	std::string	extension = getCgiExtension(scriptPath);
	std::string	cgiPath;
	if (!validateCgiRequest(scriptPath, extension, location, cgiPath))
		return createErrorResponse(500, request.getHttpVersion());

	// Configurer les pipes pour la communication CGI
	int	pipe_in[2];
	int	pipe_out[2];
	if (!setupCgiPipes(pipe_in, pipe_out))
		return createErrorResponse(500, request.getHttpVersion());

	// Construire l'environnement CGI
	char	**env = buildCgiEnv(request, location, scriptPath);

	// Forker le processus pour exécuter le CGI
	pid_t	pid = fork();
	if (pid < 0)
	{
		// Échec du fork
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		return createErrorResponse(500, request.getHttpVersion());
	}
	else if (pid == 0)
	{
		// Processus fils
		executeCgiChild(pipe_in, pipe_out, cgiPath, scriptPath, env);
		// Ne retourne jamais (exit ou execve)
	}
	else
	{
		// Envoyer le body de la requête au CGI et lire la sortie
		std::string	cgiOutput = executeCgiParent(pipe_in, pipe_out, pid, request);

		// Libérer la mémoire de l'environnement
		freeCgiEnv(env);

		// Vérifier si le CGI a échoué
		if (cgiOutput.empty())
			return createErrorResponse(500, request.getHttpVersion());

		// Parser la sortie CGI
		std::string	cgiHeaders;
		std::string	cgiBody;
		parseCgiOutput(cgiOutput, cgiHeaders, cgiBody);

		// Construire et retourner la réponse HTTP
		return buildCgiResponse(cgiHeaders, cgiBody, request);
	}

	return createErrorResponse(500, request.getHttpVersion());
}
