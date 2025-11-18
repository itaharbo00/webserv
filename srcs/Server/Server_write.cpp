/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_write.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 19:30:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/17 20:32:33 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Gérer l'envoi des données en attente (gestion des envois partiels)
bool	Server::handleClientWrite(size_t index)
{
	int	client_fd = p_fds[index].fd;

	// Vérifier s'il y a une réponse en attente pour ce client
	if (p_pending_responses.find(client_fd) == p_pending_responses.end())
	{
		// Pas de réponse en attente, repasser en mode lecture
		p_fds[index].events = POLLIN;
		return true;
	}

	// Récupérer la réponse en attente
	std::string	&remaining = p_pending_responses[client_fd];

	// Tenter d'envoyer le reste
	ssize_t	sent = send(client_fd, remaining.c_str(), remaining.length(), 0);

	if (sent < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
		{
			// Socket toujours pas prêt, réessayer plus tard
			return true;
		}
		else
		{
			// Erreur fatale
			std::cerr << "[" << client_fd << "] Failed to send remaining data: "
					  << std::strerror(errno) << std::endl;
			closeClient(index);
			return false;
		}
	}
	else if (sent < static_cast<ssize_t>(remaining.length()))
	{
		// Encore un envoi partiel, garder le reste en buffer
		size_t	total_sent = p_bytes_sent[client_fd] + sent;
		std::cerr << "[" << client_fd << "] Partial send (retry): "
				  << sent << " bytes, total: " << total_sent << std::endl;
		p_bytes_sent[client_fd] = total_sent;
		remaining = remaining.substr(sent);
		return true;
	}
	else
	{
		// Envoi complet réussi !
		size_t	total_sent = p_bytes_sent[client_fd] + sent;
		std::cout << "[" << client_fd << "] Complete send after partial: "
				  << total_sent << " bytes total" << std::endl;

		// Nettoyer les buffers
		p_pending_responses.erase(client_fd);
		p_bytes_sent.erase(client_fd);

		// Vérifier si on doit fermer la connexion
		if (p_clients_request.find(client_fd) != p_clients_request.end())
		{
			if (p_clients_request[client_fd].shouldCloseConnection())
			{
				closeClient(index);
				return false;
			}
			else
			{
				// Supprimer la requête et repasser en mode lecture
				p_clients_request.erase(client_fd);
				p_fds[index].events = POLLIN;
				return true;
			}
		}
		else
		{
			// Pas de requête associée, repasser en mode lecture
			closeClient(index);
			return true;
		}
	}
}
