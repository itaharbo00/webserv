/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_timeouts.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:47:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/12 22:39:28 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::closeClient(size_t index)
{
	int client_fd = p_fds[index].fd;

	// Log de la déconnexion
	std::cout << "[" << client_fd << "] Client disconnected" << std::endl;

	// Fermer le socket
	close(client_fd);

	// Supprimer le client de toutes les structures de données
	p_clients_request.erase(client_fd);
	p_clients_last_activity.erase(client_fd);
	p_pending_responses.erase(client_fd);
	p_bytes_sent.erase(client_fd);
	p_client_to_server_fd.erase(client_fd); // pour multi-server

	// Supprimer le client de la liste des descripteurs surveillés
	p_fds.erase(p_fds.begin() + index);
}

void Server::checkTimeouts()
{
	time_t current_time = std::time(NULL);

	for (size_t i = 0; i < p_fds.size();)
	{
		int client_fd = p_fds[i].fd;

		// Vérifier si c'est un socket d'écoute (pas un client)
		bool is_server_fd = false;
		for (size_t j = 0; j < p_socket_fds.size(); ++j)
		{
			if (client_fd == p_socket_fds[j])
			{
				is_server_fd = true;
				break;
			}
		}

		if (is_server_fd)
		{
			++i; // Ignorer les sockets d'écoute
			continue;
		}

		// Verifier qu on a une activité enregistrée pour ce client
		std::map<int, time_t>::const_iterator it_activity = p_clients_last_activity.find(client_fd);
		if (it_activity == p_clients_last_activity.end())
		{
			++i; // Pas d'activité enregistrée, passer au suivant
			continue;
		}

		time_t last_activity = p_clients_last_activity[client_fd];

		if (current_time - last_activity > TIMEOUT_SECONDS)
		{
			// Récupérer le ServerConfig pour ce client
			std::map<int, int>::const_iterator it = p_client_to_server_fd.find(client_fd);
			const ServerConfig *serverConfig = (it != p_client_to_server_fd.end() && p_fd_to_config.find(it->second) != p_fd_to_config.end())
												   ? p_fd_to_config.find(it->second)->second
												   : &p_serverConfigs[0];

			// Créer un Router avec le bon ServerConfig
			Router router(serverConfig);

			// Créer une réponse 408 Request Timeout
			HttpResponse timeoutResponse = router.createErrorResponse(408, "HTTP/1.1");
			std::string response_str = timeoutResponse.toString();

			p_pending_responses[client_fd] = response_str;
			p_bytes_sent[client_fd] = 0;

			p_clients_request.erase(client_fd);
			p_fds[i].events = POLLOUT; // Changer l'événement à POLLOUT pour envoyer la réponse
			++i;
		}
		else
			++i; // Incrémenter seulement si le client n'a pas été supprimé
	}
}

// Vérifier l'état des processus CGI asynchrones
void Server::checkCgiProcesses()
{
	std::vector<int> completed_fds;
	time_t current_time = std::time(NULL);
	const int CGI_TIMEOUT = 30; // 30 secondes

	// Parcourir tous les processus CGI en cours
	for (std::map<int, CgiProcess>::iterator it = p_cgi_processes.begin();
		 it != p_cgi_processes.end(); ++it)
	{
		int pipe_fd = it->first;
		CgiProcess &cgi = it->second;

		// Vérifier le timeout
		if (current_time - cgi.start_time > CGI_TIMEOUT)
		{
			// Timeout - tuer le processus
			kill(cgi.pid, SIGKILL);
			waitpid(cgi.pid, NULL, 0);
			close(pipe_fd);

			// Envoyer une erreur 504 Gateway Timeout
			std::map<int, int>::const_iterator srv_it = p_client_to_server_fd.find(cgi.client_fd);
			const ServerConfig *serverConfig = (srv_it != p_client_to_server_fd.end() && p_fd_to_config.find(srv_it->second) != p_fd_to_config.end())
												   ? p_fd_to_config.find(srv_it->second)->second
												   : &p_serverConfigs[0];

			Router router(serverConfig);
			HttpResponse errorResponse = router.createErrorResponse(504, cgi.http_version);
			std::string response_str = errorResponse.toString();

			p_pending_responses[cgi.client_fd] = response_str;
			p_bytes_sent[cgi.client_fd] = 0;

			// Marquer le client pour l'envoi
			for (size_t i = 0; i < p_fds.size(); ++i)
			{
				if (p_fds[i].fd == cgi.client_fd)
				{
					p_fds[i].events = POLLOUT;
					break;
				}
			}

			completed_fds.push_back(pipe_fd);
			continue;
		}

		// Essayer de lire toutes les données disponibles (non-bloquant)
		char buffer[4096];
		bool got_eof = false;

		while (true)
		{
			ssize_t bytesRead = read(pipe_fd, buffer, sizeof(buffer));

			if (bytesRead > 0)
			{
				// Données reçues
				cgi.output.append(buffer, bytesRead);
			}
			else if (bytesRead == 0)
			{
				// EOF
				got_eof = true;
				break;
			}
			else if (bytesRead == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					// Pas de données disponibles pour l'instant
					break;
				}
				else
				{
					// Erreur de lecture réelle
					close(pipe_fd);
					kill(cgi.pid, SIGKILL);
					waitpid(cgi.pid, NULL, 0);

					// Envoyer une erreur 500
					std::map<int, int>::const_iterator srv_it = p_client_to_server_fd.find(cgi.client_fd);
					const ServerConfig *serverConfig = (srv_it != p_client_to_server_fd.end() && p_fd_to_config.find(srv_it->second) != p_fd_to_config.end())
														   ? p_fd_to_config.find(srv_it->second)->second
														   : &p_serverConfigs[0];

					Router router(serverConfig);
					HttpResponse errorResponse = router.createErrorResponse(500, cgi.http_version);
					std::string response_str = errorResponse.toString();

					p_pending_responses[cgi.client_fd] = response_str;
					p_bytes_sent[cgi.client_fd] = 0;

					// Marquer le client pour l'envoi
					for (size_t i = 0; i < p_fds.size(); ++i)
					{
						if (p_fds[i].fd == cgi.client_fd)
						{
							p_fds[i].events = POLLOUT;
							break;
						}
					}

					completed_fds.push_back(pipe_fd);
					goto next_cgi;
				}
			}
		}

		if (got_eof)
		{
			// EOF - le CGI a terminé
			close(pipe_fd);

			// Attendre le processus CGI
			int status;
			waitpid(cgi.pid, &status, 0);

			// Construire la réponse HTTP
			std::map<int, int>::const_iterator srv_it = p_client_to_server_fd.find(cgi.client_fd);
			const ServerConfig *serverConfig = (srv_it != p_client_to_server_fd.end() && p_fd_to_config.find(srv_it->second) != p_fd_to_config.end())
												   ? p_fd_to_config.find(srv_it->second)->second
												   : &p_serverConfigs[0];

			Router router(serverConfig);
			HttpResponse response;

			if (WIFEXITED(status) && WEXITSTATUS(status) == 0 && !cgi.output.empty())
			{
				// Parser la sortie CGI manuellement pour construire la réponse
				std::string cgiHeaders;
				std::string cgiBody;
				size_t headerEnd = cgi.output.find("\r\n\r\n");
				if (headerEnd == std::string::npos)
					headerEnd = cgi.output.find("\n\n");

				if (headerEnd != std::string::npos)
				{
					cgiHeaders = cgi.output.substr(0, headerEnd);
					if (cgi.output[headerEnd] == '\r')
						cgiBody = cgi.output.substr(headerEnd + 4);
					else
						cgiBody = cgi.output.substr(headerEnd + 2);
				}
				else
					cgiBody = cgi.output;

				// Construire la réponse avec la bonne version HTTP
				response.setHttpVersion(cgi.http_version.empty() ? "HTTP/1.1" : cgi.http_version);
				response.setStatusCode(200);
				response.setBody(cgiBody);

				// Parser les headers CGI
				std::istringstream headerStream(cgiHeaders);
				std::string line;
				while (std::getline(headerStream, line))
				{
					if (line.empty() || line == "\r")
						continue;
					if (!line.empty() && line[line.length() - 1] == '\r')
						line.erase(line.length() - 1);

					size_t colonPos = line.find(':');
					if (colonPos != std::string::npos)
					{
						std::string key = line.substr(0, colonPos);
						std::string value = line.substr(colonPos + 1);
						size_t start = value.find_first_not_of(" \t");
						if (start != std::string::npos)
							value = value.substr(start);

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
							if (response.getStatusCode() == 200)
								response.setStatusCode(302);
						}
						else
							response.setHeader(key, value);
					}
				}
				response.setHeader("Connection", "close");
			}
			else
				response = router.createErrorResponse(500, cgi.http_version);

			std::string response_str = response.toString();
			p_pending_responses[cgi.client_fd] = response_str;
			p_bytes_sent[cgi.client_fd] = 0;

			// Marquer le client pour l'envoi
			for (size_t i = 0; i < p_fds.size(); ++i)
			{
				if (p_fds[i].fd == cgi.client_fd)
				{
					p_fds[i].events = POLLOUT;
					break;
				}
			}

			completed_fds.push_back(pipe_fd);
		}

	next_cgi:
		continue;
	}

	// Supprimer les CGI complétés
	for (size_t i = 0; i < completed_fds.size(); ++i)
		p_cgi_processes.erase(completed_fds[i]);
}
