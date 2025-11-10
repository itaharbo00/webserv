/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:26 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/10 23:04:34 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <unistd.h>
# include <sys/types.h>		// data types
# include <sys/socket.h>	// socket, bind, listen, setsockopt
# include <netdb.h>			// getaddrinfo, freeaddrinfo
# include <fcntl.h>			// fcntl
# include <poll.h>			// poll, struct pollfd
# include <signal.h>
# include <cerrno>

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
	std::vector<struct pollfd>	p_fds;	// Descripteurs de fichiers des clients connectés

	void						p_initSocket();					// Initialisation du socket
	void						p_setNonBlocking(int fd);		// Mettre un descripteur en mode non-bloquant
	void						p_initServerPollfd();			// Initialiser le pollfd du serveur
	void						p_acceptNewClient();			// Accepter une nouvelle connexion client
	bool						p_handleClient(size_t index);	// Gérer la communication avec un client
};

#endif
