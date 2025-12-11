/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_cgi_execute.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:30:42 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/06 18:46:06 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include <fcntl.h> // fcntl, O_NONBLOCK

// Valider la requête CGI avant l'exécution
bool Router::validateCgiRequest(const std::string &,
								const std::string &extension, const LocationConfig *location,
								std::string &cgiPath) const
{
	// Note: L'existence du fichier est déjà vérifiée dans executeCgi()

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
bool Router::setupCgiPipes(int pipe_in[2], int pipe_out[2]) const
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
void Router::executeCgiChild(int pipe_in[2], int pipe_out[2],
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
	args[1] = const_cast<char *>(scriptPath.c_str()); // Chemin absolu du script
	args[2] = NULL;

	// Exécuter le CGI
	execve(cgiPath.c_str(), args, env);

	// Vérifier si le binaire CGI est exécutable
	struct stat st;
	if (stat(cgiPath.c_str(), &st) == 0)
	{
		if (!(st.st_mode & S_IXUSR))
			write(STDERR_FILENO, "[CGI] CGI binary is not executable\n", 36);
	}
	else
	{
		write(STDERR_FILENO, "[CGI] CGI binary not found\n", 27);
	}
	_exit(1); // Quitter le processus fils en cas d'erreur
}

// Gérer l'exécution du CGI dans le processus parent
std::string Router::executeCgiParent(int pipe_in[2], int pipe_out[2],
									 pid_t pid, const HttpRequest &request) const
{
	close(pipe_in[0]);	// Fermer lecture du pipe d'entrée
	close(pipe_out[1]); // Fermer écriture du pipe de sortie

	int flags = fcntl(pipe_out[0], F_GETFL, 0);
	if (flags != -1)
		fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK); // Rendre non-bloquant

	std::string body = request.getBody();
	if (!body.empty())
	{
		const char *buf = body.c_str();
		size_t total = body.length();
		size_t sent = 0;
		while (sent < total)
		{
			// Écrire dans le pipe d'entrée
			ssize_t written = write(pipe_in[1], buf + sent, total - sent);
			if (written > 0)
				sent += static_cast<size_t>(written);	// Mettre à jour le nombre d'octets envoyés
			else if (written == -1 && (errno == EINTR)) // Interruption par un signal
			{
				usleep(1000); // Attendre un peu avant de réessayer
				continue;	  // Retry on interrupt
			}
			else
			{
				// Erreur d'écriture
				close(pipe_in[1]);
				close(pipe_out[0]);
				throw std::runtime_error("Failed to write to CGI stdin");
			}
		}
	}

	close(pipe_in[1]); // Fermer l'écriture après avoir envoyé le corps

	// Lire la sortie du CGI avec timeout
	std::string cgiOutput;
	char buffer[4096];
	ssize_t bytesRead;
	time_t start_time = std::time(NULL);
	const int CGI_TIMEOUT = 30; // 30 secondes timeout pour CGI

	// Lire avec timeout
	while (true)
	{
		// Vérifier le timeout
		if (std::time(NULL) - start_time > CGI_TIMEOUT)
		{
			kill(pid, SIGKILL);	   // Tuer le processus CGI si timeout
			waitpid(pid, NULL, 0); // Nettoyer le processus zombie
			close(pipe_out[0]);
			throw std::runtime_error("CGI timeout: process killed after 30 seconds");
		}

		// Essayer de lire (non-bloquant serait mieux, mais on vérifie le temps)
		bytesRead = read(pipe_out[0], buffer, sizeof(buffer));
		if (bytesRead > 0)
			cgiOutput.append(buffer, bytesRead);
		else if (bytesRead == 0)
			break; // EOF
		else if (bytesRead == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// Pas de données disponibles, attendre un peu
				usleep(100000); // 100 ms
				continue;
			}
			else if (errno == EINTR)
				continue;
			else
				break; // Autre erreur, sortir
		}
	}

	close(pipe_out[0]); // Fermer la lecture après avoir tout lu

	// Attendre la fin du processus CGI (non-bloquant)
	int status;
	int wait_result = waitpid(pid, &status, WNOHANG);

	// Si le processus n'est pas encore terminé, attendre un peu plus
	if (wait_result == 0)
	{
		// Donner encore 1 seconde
		sleep(1);
		wait_result = waitpid(pid, &status, WNOHANG);
		if (wait_result == 0)
		{
			// Toujours pas terminé, tuer le processus
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
		}
	}

	// Vérifier le statut de sortie du processus CGI
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		return ""; // CGI a échoué

	return cgiOutput; // Retourner la sortie CGI
}

void Router::parseCgiOutput(const std::string &cgiOutput,
							std::string &headers, std::string &body) const
{
	size_t headerEnd = cgiOutput.find("\r\n\r\n");

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

HttpResponse Router::buildCgiResponse(const std::string &cgiHeaders,
									  const std::string &cgiBody, const HttpRequest &request) const
{
	HttpResponse response;
	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200); // Par défaut, 200 OK
	response.setBody(cgiBody);

	// Parser les headers CGI
	std::istringstream headerStream(cgiHeaders);
	std::string line;

	while (std::getline(headerStream, line))
	{
		if (line.empty() || line == "\r")
			continue;
		// Enlever le retour chariot éventuel
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			// Enlever les espaces avant et après la valeur
			size_t start = value.find_first_not_of(" \t");
			if (start != std::string::npos)
				value = value.substr(start);

			// Gérer le header Status pour définir le code de statut
			if (key == "Status")
			{
				std::istringstream statusStream(value);
				int statusCode;
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

HttpResponse Router::executeCgi(const HttpRequest &request,
								const LocationConfig *location, const std::string &scriptPath)
{
	// Vérifier d'abord si le fichier existe → 404
	if (!fileExists(scriptPath) || !isRegularFile(scriptPath))
		return createErrorResponse(404, request.getHttpVersion());

	// Valider la configuration CGI (extension, cgi_pass) → 500
	std::string extension = getCgiExtension(scriptPath);
	std::string cgiPath;
	if (!validateCgiRequest(scriptPath, extension, location, cgiPath))
		return createErrorResponse(500, request.getHttpVersion());

	// Configurer les pipes pour la communication CGI
	int pipe_in[2];
	int pipe_out[2];
	if (!setupCgiPipes(pipe_in, pipe_out))
		return createErrorResponse(500, request.getHttpVersion());

	// Construire l'environnement CGI
	char **env = buildCgiEnv(request, location, scriptPath);

	// Forker le processus pour exécuter le CGI
	pid_t pid = fork();
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
		std::string cgiOutput;
		try
		{
			cgiOutput = executeCgiParent(pipe_in, pipe_out, pid, request);
		}
		catch (const std::exception &e)
		{
			// Timeout ou autre erreur CGI
			freeCgiEnv(env);
			std::cerr << "CGI error: " << e.what() << std::endl;
			return createErrorResponse(500, request.getHttpVersion());
		}

		// Libérer la mémoire de l'environnement
		freeCgiEnv(env);

		// Vérifier si le CGI a échoué
		if (cgiOutput.empty())
			return createErrorResponse(500, request.getHttpVersion());

		// Parser la sortie CGI
		std::string cgiHeaders;
		std::string cgiBody;
		parseCgiOutput(cgiOutput, cgiHeaders, cgiBody);

		// Construire et retourner la réponse HTTP
		return buildCgiResponse(cgiHeaders, cgiBody, request);
	}

	return createErrorResponse(500, request.getHttpVersion());
}

// Lancer un CGI en mode asynchrone (non-bloquant)
// Retourne le FD du pipe de lecture ou -1 en cas d'erreur
int Router::startCgiAsync(const HttpRequest &request,
						  const LocationConfig *location, const std::string &scriptPath,
						  pid_t &pid) const
{
	// 1. Valider la requête CGI
	std::string extension = getCgiExtension(scriptPath);
	std::string cgiPath;
	if (!validateCgiRequest(scriptPath, extension, location, cgiPath))
		return -1;

	// 2. Construire l'environnement CGI
	char **env = buildCgiEnv(request, location, scriptPath);
	if (!env)
		return -1;

	// 3. Créer les pipes
	int pipe_in[2];
	int pipe_out[2];
	if (!setupCgiPipes(pipe_in, pipe_out))
	{
		freeCgiEnv(env);
		return -1;
	}

	// 4. Fork pour créer le processus CGI
	pid = fork();
	if (pid == -1)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		freeCgiEnv(env);
		return -1;
	}

	// 5. Processus fils : exécute le CGI
	if (pid == 0)
	{
		executeCgiChild(pipe_in, pipe_out, cgiPath, scriptPath, env);
		// executeCgiChild ne retourne jamais (execve ou _exit)
	}

	// 6. Processus parent : préparer la communication
	close(pipe_in[0]);	// Fermer lecture du pipe d'entrée
	close(pipe_out[1]); // Fermer écriture du pipe de sortie

	// 7. Écrire le body de la requête dans le pipe d'entrée (si nécessaire)
	std::string body = request.getBody();
	if (!body.empty())
	{
		const char *buf = body.c_str();
		size_t total = body.length();
		size_t sent = 0;
		while (sent < total)
		{
			ssize_t written = write(pipe_in[1], buf + sent, total - sent);
			if (written > 0)
				sent += static_cast<size_t>(written);
			else if (written == -1 && errno == EINTR)
				continue;
			else
			{
				// Erreur d'écriture - nettoyer
				close(pipe_in[1]);
				close(pipe_out[0]);
				freeCgiEnv(env);
				kill(pid, SIGKILL);
				waitpid(pid, NULL, 0);
				return -1;
			}
		}
	}
	close(pipe_in[1]); // Fermer écriture après envoi du body

	// 8. Rendre le pipe de sortie non-bloquant
	int flags = fcntl(pipe_out[0], F_GETFL, 0);
	if (flags != -1)
		fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK);

	freeCgiEnv(env);
	return pipe_out[0]; // Retourner le FD du pipe de lecture
}

// Construire une réponse HTTP à partir de la sortie CGI complète
HttpResponse Router::buildCgiResponseAsync(const std::string &cgiOutput,
										   const HttpRequest &) const
{
	std::string cgiHeaders;
	std::string cgiBody;
	parseCgiOutput(cgiOutput, cgiHeaders, cgiBody);
	
	// Créer une requête par défaut avec HTTP/1.1
	HttpRequest defaultRequest;
	return buildCgiResponse(cgiHeaders, cgiBody, defaultRequest);
}
