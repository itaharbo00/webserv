/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:26 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 21:30:07 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cstdlib>			// atoi, exit
# include <unistd.h>
# include <sys/types.h>		// data types
# include <sys/socket.h>	// socket, bind, listen, setsockopt
# include <netdb.h>			// getaddrinfo, freeaddrinfo
# include <fcntl.h>			// fcntl
# include <poll.h>			// poll, struct pollfd
# include <signal.h>
# include <cerrno>
# include <ctime>
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Router.hpp"
# include "Config.hpp"

// Structure pour suivre les processus CGI asynchrones
struct CgiProcess
{
	int			client_fd;		// FD du client qui a fait la requête
	pid_t		pid;			// PID du processus CGI
	int			pipe_fd;		// FD du pipe pour lire la sortie du CGI
	time_t		start_time;		// Moment du lancement du CGI
	std::string	output;			// Buffer pour accumuler la sortie du CGI
	std::string	http_version;	// Version HTTP de la requête originale
};

class Server
{
public:

	Server(const std::string &configFile);
	Server(const std::string &host, const std::string &port);
	~Server();

	void	start();		// Méthode pour démarrer le serveur

private:

	Config						p_config;		// Configuration du serveur
	std::vector<ServerConfig>	p_serverConfigs;

	std::vector<int>			p_socket_fds;	// Descripteurs de fichiers des sockets d'écoute
	std::map<int, const ServerConfig*> p_fd_to_config;	// Map socket_fd → ServerConfig
	std::vector<struct pollfd>	p_fds;			// FD des clients connectés
	std::map<int, int>			p_client_to_server_fd; // Map client_fd → server_fd d'origine

	std::map<int, HttpRequest>	p_clients_request;		 // Map des clients connectés
	std::map<int, time_t>		p_clients_last_activity; // Dernière activité des clients
	std::map<int, std::string>	p_pending_responses;		 // Buffer pour envois partiels
	std::map<int, size_t>		p_bytes_sent;			 // Compteur d'octets envoyés

	std::map<int, CgiProcess>	p_cgi_processes;		 // Map pipe_fd → CgiProcess pour les CGI en cours

	static const int			TIMEOUT_SECONDS = 60;	 // Timeout d'inactivité en secondes

	void	initSocket();				// Initialisation des sockets (un par ServerConfig)
	int		createListeningSocket(const std::string &host, const std::string &port); // Créer un socket d'écoute
	void	setNonBlocking(int fd);		// Mettre un descripteur en mode non-bloquant
	void	initServerPollfds();		// Initialiser les pollfds des serveurs
	void	acceptNewClient(int server_fd);	// Accepter une nouvelle connexion client
	bool	handleClient(size_t index);	// Gérer la communication avec un client (lecture)
	bool	handleClientWrite(size_t index);// Gérer l'envoi des données en attente
	void	checkTimeouts();			// Vérifier les timeouts des clients
	void	checkCgiProcesses();		// Vérifier l'état des processus CGI asynchrones
	void	closeClient(size_t index);	// Fermer la connexion d'un client
	const ServerConfig*	selectServerConfig(int client_fd, const HttpRequest &request) const; // Sélectionner le ServerConfig approprié
};

#endif	// SERVER_HPP
