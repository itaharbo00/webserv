/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_timeouts.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:47:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 21:30:26 by itaharbo         ###   ########.fr       */
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
	p_pending_responses.erase(client_fd);	// ← Nouveau
	p_bytes_sent.erase(client_fd);			// ← Nouveau

	// Supprimer le client de la liste des descripteurs surveillés
	p_fds.erase(p_fds.begin() + index);
}

void	Server::checkTimeouts()
{
	time_t	current_time = std::time(NULL);

	for (size_t i = 1; i < p_fds.size(); ) // Commencer à 1 pour ignorer le serveur
	{
		int	client_fd = p_fds[i].fd;
		time_t	last_activity = p_clients_last_activity[client_fd];

		if (current_time - last_activity > TIMEOUT_SECONDS)
		{
			// Créer une réponse 408 Request Timeout
			HttpResponse	timeoutResponse = p_router.createErrorResponse(408, "HTTP/1.1");
			std::string		response_str = timeoutResponse.toString();

			// Envoyer la réponse au client
			if (send(client_fd, response_str.c_str(), response_str.length(), 0) < 0)
				std::cerr << "Error sending timeout response to client [" << client_fd << "]" << std::endl;

			// Fermer la connexion du client
			closeClient(i);
		}
		else
			++i; // Incrémenter seulement si le client n'a pas été supprimé
	}
}
