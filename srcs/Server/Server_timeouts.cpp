/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_timeouts.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:47:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/17 20:31:38 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::closeClient(size_t index)
{
	int	client_fd = p_fds[index].fd;

	// Log de la déconnexion
	std::cout << "[" << client_fd << "] Client disconnected" << std::endl;

	// Fermer le socket
	close(client_fd);

	// Supprimer le client de toutes les structures de données
	p_clients_request.erase(client_fd);
	p_clients_last_activity.erase(client_fd);
	p_pending_responses.erase(client_fd);
	p_bytes_sent.erase(client_fd);
	p_client_to_server_fd.erase(client_fd);	// pour multi-server

	// Supprimer le client de la liste des descripteurs surveillés
	p_fds.erase(p_fds.begin() + index);
}

void	Server::checkTimeouts()
{
	time_t	current_time = std::time(NULL);

	for (size_t i = 0; i < p_fds.size(); )
	{
		int	client_fd = p_fds[i].fd;

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

		time_t	last_activity = p_clients_last_activity[client_fd];

		if (current_time - last_activity > TIMEOUT_SECONDS)
		{
			// Récupérer le ServerConfig pour ce client
			std::map<int, int>::const_iterator it = p_client_to_server_fd.find(client_fd);
			const ServerConfig* serverConfig = (it != p_client_to_server_fd.end()
									&& p_fd_to_config.find(it->second) != p_fd_to_config.end())
												? p_fd_to_config.find(it->second)->second
												: &p_serverConfigs[0];

			// Créer un Router avec le bon ServerConfig
			Router router(serverConfig);

			// Créer une réponse 408 Request Timeout
			HttpResponse	timeoutResponse = router.createErrorResponse(408, "HTTP/1.1");
			std::string		response_str = timeoutResponse.toString();

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
