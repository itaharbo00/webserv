/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 17:14:11 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string &host, const std::string &port)
	: p_socket_fd(-1), p_addrinfo(NULL), p_host(host), p_port(port)
{
	try
	{
		// Validation du port
		int port_num = std::atoi(port.c_str());
		if (port_num < 1 || port_num > 65535)
			throw std::runtime_error("Invalid port number (must be 1-65535)");
		
		initSocket(); // Initialisation du socket lors de la construction du serveur
		p_router = Router("./www");
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error initializing server: " << e.what() << std::endl;
		throw; // Rejeter l'exception pour indiquer l'échec de la construction
	}
}

Server::Server(const std::string &configFile)
	: p_socket_fd(-1), p_addrinfo(NULL)
{
	try
	{
		p_config.parseConfigFile(configFile);
		p_serverConfigs = p_config.getServers();

		if (p_serverConfigs.empty())
			throw std::runtime_error("No server configurations found in config file");

		const ServerConfig	&firstServer = p_serverConfigs[0];

		std::stringstream	ss;
		ss << firstServer.getListen();
		p_port = ss.str();

		p_host = "0.0.0.0";

		initSocket(); // Initialisation du socket lors de la construction du serveur
		p_router = Router(&firstServer);

	}
	catch (const std::exception &e)
	{
		std::cerr << "Error initializing server: " << e.what() << std::endl;
		throw; // Rejeter l'exception pour indiquer l'échec de la construction
	}
}

Server::~Server()
{
	// Fermer tout les clients connectés
	for (size_t i = 1; i < p_fds.size(); ++i)
		if (p_fds[i].fd >= 0 && p_fds[i].fd != p_socket_fd)
			close(p_fds[i].fd);
	p_fds.clear();

	if (p_socket_fd >= 0)	// Verifier que le socket est valide avant de le fermer
		close(p_socket_fd);

	// Verifier que p_addrinfo n'est pas NULL avant d'appeler freeaddrinfo
	if (p_addrinfo)
		freeaddrinfo(p_addrinfo);
}

void	Server::initSocket()	// Creation, bind et listen du socket
{
	struct addrinfo	hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP socket
	hints.ai_flags = AI_PASSIVE;		// Pour bind auto à l'adresse locale (mode serveur)

	if (getaddrinfo(p_host.c_str(), p_port.c_str(), &hints, &p_addrinfo) != 0)
		throw std::runtime_error("getaddrinfo() failed");

	// Création du socket
	p_socket_fd = socket(p_addrinfo->ai_family, p_addrinfo->ai_socktype,
		p_addrinfo->ai_protocol);
	if (p_socket_fd < 0)
		throw std::runtime_error("socket() failed");

	int opt = 1;
	// Permet de réutiliser l'adresse immédiatement après la fermeture du socket
	if (setsockopt(p_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt() failed");

	// Bind du socket à l'adresse et au port spécifiés
	if (bind(p_socket_fd, p_addrinfo->ai_addr, p_addrinfo->ai_addrlen) < 0)
		throw std::runtime_error("bind() failed");

	// Écoute des connexions entrantes
	if (listen(p_socket_fd, SOMAXCONN) < 0)	// SOMAXCONN pour file d'attente maximale
		throw std::runtime_error("listen() failed");
}

void	Server::setNonBlocking(int fd)	// Mettre un descripteur en mode non-bloquant
{
	int	saved_errno;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)	// Modifie les flags du fichier descriptor
	{
		saved_errno = errno;
		close(fd);
		throw std::runtime_error("fcntl() failed to set non-blocking: " +
			std::string(std::strerror(saved_errno)));
	}

	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)	// Fermer le fd lors d'un execve
	{
		saved_errno = errno;
		close(fd);
		throw std::runtime_error("fcntl() failed to set FD_CLOEXEC: " +
			std::string(std::strerror(saved_errno)));
	}
}

void	Server::initServerPollfd()	// Initialiser le pollfd du serveur
{
	// Ajouter le socket du serveur à la liste des fds surveillés
	struct pollfd	server_pollfd;
	server_pollfd.fd = p_socket_fd;
	server_pollfd.events = POLLIN; // Surveiller les événements de lecture
	server_pollfd.revents = 0;
	p_fds.push_back(server_pollfd); // Ajouter le serveur à la liste des fds surveillés
}

void	Server::acceptNewClient()	// Accepter une nouvelle connexion client
{
	int	client_fd = accept(p_socket_fd, NULL, NULL);
	if (client_fd < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;	// Pas de client disponible, c'est normal

		// Erreurs critiques
		throw std::runtime_error("accept() failed");
	}

	setNonBlocking(client_fd); // Mettre le descripteur en mode non-bloquant

	// Ajouter le client à la liste des fds surveillés
	struct pollfd	client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN; // Surveiller les événements de lecture
	client_pollfd.revents = 0;
	p_fds.push_back(client_pollfd); // Ajouter le client à la liste des fds surveillés

	// Enregistrer le temps de la dernière activité du client
	p_clients_last_activity[client_fd] = std::time(NULL);
	
	// Log de la nouvelle connexion
	std::cout << "[" << client_fd << "] New client connected" << std::endl;
}

bool	Server::handleClient(size_t index)	// Gérer la communication avec un client
{
	// Obtenir le descripteur du client
	int		client_fd = p_fds[index].fd;
	char	buffer[4096]; // Buffer pour recevoir les données

	// Recevoir des données du client
	ssize_t	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';

		// Mettre à jour le temps de la dernière activité du client
		p_clients_last_activity[client_fd] = std::time(NULL);
		try
		{
			// Obtenir une référence à l'objet HttpRequest du client
			HttpRequest	&request = p_clients_request[client_fd];

			// Ajouter les données reçues à l'objet HttpRequest du client
			request.appendData(std::string(buffer, bytes_received));

			// Tenter de parser la requête
			if (p_clients_request[client_fd].isComplete())
			{
				// Parser la requête complète
				request.parse();

				// Router la requête pour obtenir une réponse
				HttpResponse	response = p_router.route(request);

				// Envoyer la réponse au client
				std::string		response_str = response.toString();
				ssize_t			sent = send(client_fd, response_str.c_str(), response_str.length(), 0);
				
				if (sent < 0)
				{
					if (errno == EWOULDBLOCK || errno == EAGAIN)
					{
						// Socket pas prêt, mettre en buffer et attendre POLLOUT
						p_pending_responses[client_fd] = response_str;
						p_bytes_sent[client_fd] = 0;
						p_fds[index].events = POLLOUT;
						return true;
					}
					else
					{
						// Erreur fatale
						std::cerr << "[" << client_fd << "] Failed to send response: " 
								  << std::strerror(errno) << std::endl;
						closeClient(index);
						return false;
					}
				}
				else if (sent < static_cast<ssize_t>(response_str.length()))
				{
					// Envoi partiel : mettre le reste en buffer et attendre POLLOUT
					std::cerr << "[" << client_fd << "] Partial send: " 
							  << sent << "/" << response_str.length() << " bytes (buffering remainder)" << std::endl;
					p_pending_responses[client_fd] = response_str.substr(sent);
					p_bytes_sent[client_fd] = sent;
					p_fds[index].events = POLLOUT;
					return true;
				}

				// Envoi complet ✅
				// Gérer la fermeture de la connexion si nécessaire
				if (request.shouldCloseConnection())
				{
					closeClient(index);
					return (false);
				}
				else	// Supprimer la requête après traitement pour la prochaine
					p_clients_request.erase(client_fd);
			}
		}
		catch (const std::exception &e)
		{
			// Essayer d'obtenir la version HTTP de la requête, sinon HTTP/1.1 par défaut
			std::string version = "HTTP/1.1";
			std::string	errorMsg = e.what();
			int			statusCode = 400; // Code d'erreur par défaut

			try
			{
				// Tenter d'obtenir la version HTTP de la requête
				version = p_clients_request[client_fd].getHttpVersion();
				if (version.empty())
					version = "HTTP/1.1";
			}
			catch (...)
			{
			}

			if (errorMsg.find("URI too long") != std::string::npos)
				statusCode = 414;
			else if (errorMsg.find("Header line too long") != std::string::npos ||
			         errorMsg.find("Request header fields too large") != std::string::npos)
				statusCode = 431;
			else if (errorMsg.find("Content-Length exceeds limit") != std::string::npos ||
			         errorMsg.find("Request entity too large") != std::string::npos ||
			         errorMsg.find("Payload too large") != std::string::npos ||
			         errorMsg.find("Request size limit exceeded") != std::string::npos)
				statusCode = 413;
			else if (errorMsg.find("Unsupported HTTP version") != std::string::npos ||
			         errorMsg.find("HTTP version not supported") != std::string::npos)
				statusCode = 505;
			else if (errorMsg.find("Directory traversal") != std::string::npos)
				statusCode = 403;
			else
				statusCode = 400;

			// Log l'erreur avec le code HTTP
			std::cerr << "[" << client_fd << "] HTTP parsing error: " 
					  << statusCode << " - " << errorMsg << std::endl;

			HttpResponse errorResponse = p_router.createErrorResponse(statusCode, version);

			std::string	error_response_str = errorResponse.toString();
			ssize_t sent = send(client_fd, error_response_str.c_str(), 
								error_response_str.length(), 0);
			
			if (sent < 0)
			{
				std::cerr << "[" << client_fd << "] Failed to send error response: " 
						  << std::strerror(errno) << std::endl;
			}

			closeClient(index);
			return false;	// Client déconnecté
		}
		return true;	// Client toujours connecté
	}
	else if (bytes_received == 0)
	{
		// Client a fermé la connexion (normal)
		std::cout << "[" << client_fd << "] Client disconnected" << std::endl;
		closeClient(index);
		return false;
	}
	else
	{
		// Erreur recv()
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return true;	// Pas de données disponibles

		// Erreur grave
		std::cerr << "[" << client_fd << "] recv() failed: " 
				  << std::strerror(errno) << std::endl;
		throw std::runtime_error("recv() failed: " + std::string(std::strerror(errno)));
	}
}

static bool	g_isRunning = false; // Variable globale pour contrôler l'exécution du serveur

static void	handleSignal(int signum)	// Gérer les signaux (ex: SIGINT)
{
	(void)signum;
	g_isRunning = false; // Variable globale pour arrêter la boucle du serveur
}

void	Server::start()	// Méthode pour démarrer le serveur
{
	signal(SIGINT, handleSignal);	// Gérer l'interruption clavier (Ctrl+C)
	signal(SIGTERM, handleSignal);	// Gérer le signal de terminaison

	setNonBlocking(p_socket_fd); // Mettre le socket du serveur en mode non-bloquant
	initServerPollfd();          // Initialiser le pollfd du serveur

	// Log du démarrage du serveur
	std::cout << "Server listening on " << p_host << ":" << p_port << std::endl;
	std::cout << "Waiting for connections..." << std::endl;

	g_isRunning = true;
	
	// Boucle principale pour accepter les connexions entrantes
	while (g_isRunning)
	{
		try
		{
			int	poll_count = poll(&p_fds[0], p_fds.size(), 1000);	// Attente indéfinie
			if (poll_count < 0)
			{
				if (errno == EINTR)	// Vérifier si poll a été interrompu par un signal
					continue;		// Recommencer la boucle si interrompu
				throw std::runtime_error("poll() failed");
			}

			checkTimeouts(); // Vérifier les timeouts des clients

			// Parcourir les descripteurs pour vérifier lesquels ont des événements
			for (size_t i = 0; i < p_fds.size(); ++i)
			{
				if (p_fds[i].revents & POLLIN)	// Vérifier les événements de lecture
				{
					if (p_fds[i].fd == p_socket_fd)
					{
						try
						{
							acceptNewClient();	// Nouvelle connexion entrante
						}
						catch (const std::exception&)
						{
						}
					}
					else
					{
						try
						{
							if (!handleClient(i)) // Données dispo à lire d'un client existant
								--i;	// Ajuster l'index si un client a été supprimé
						}
						catch (const std::exception&)
						{
							if (i > 0)
							{
								closeClient(i);  // Utiliser closeClient() pour tout nettoyer
								--i;
							}
						}
					}
				}
				else if (p_fds[i].revents & POLLOUT)	// Socket prêt pour écriture
				{
					try
					{
						if (!handleClientWrite(i))	// Envoyer les données en attente
							--i;	// Ajuster l'index si un client a été supprimé
					}
					catch (const std::exception&)
					{
						if (i > 0)
						{
							closeClient(i);
							--i;
						}
					}
				}
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "Critical server error: " << e.what() << std::endl;
			break;  // Sortir de la boucle proprement
		}
	}
	
	// Log de l'arrêt du serveur
	std::cout << "\nServer shutting down gracefully..." << std::endl;
}
