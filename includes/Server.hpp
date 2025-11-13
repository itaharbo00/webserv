/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:26 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 00:58:12 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cstdlib>
# include <unistd.h>
# include <sys/types.h>		// data types
# include <sys/socket.h>	// socket, bind, listen, setsockopt
# include <netdb.h>			// getaddrinfo, freeaddrinfo
# include <fcntl.h>			// fcntl
# include <poll.h>			// poll, struct pollfd
# include <signal.h>
# include <cerrno>
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Router.hpp"

class Server
{
public:

	Server(const std::string &host, const std::string &port);
	~Server();

	void						start();		// Méthode pour démarrer le serveur

private:

	int							p_socket_fd;	// Descripteur de fichier du socket
	struct addrinfo				*p_addrinfo;	// Informations d'adresse du serveur
	std::string					p_host;			// Adresse IP ou nom d'hôte
	std::string					p_port;			// Port d'écoute
	std::vector<struct pollfd>	p_fds;			// Descripteurs de fichiers des clients connectés

	std::map<int, HttpRequest>	p_clients_request;	// Map des clients connectés
	Router						p_router;			// Routeur pour gérer les requêtes

	void						initSocket();					// Initialisation du socket
	void						setNonBlocking(int fd);		// Mettre un descripteur en mode non-bloquant
	void						initServerPollfd();			// Initialiser le pollfd du serveur
	void						acceptNewClient();			// Accepter une nouvelle connexion client
	bool						handleClient(size_t index);	// Gérer la communication avec un client
};

#endif	// SERVER_HPP
