/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/22 18:02:15 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string &, const std::string &port)
	: p_config(), p_serverConfigs()
{
	try
	{
		// Validation du port
		int port_num = std::atoi(port.c_str());
		if (port_num < 1 || port_num > 65535)
			throw std::runtime_error("Invalid port number (must be 1-65535)");

		// Créer un ServerConfig par défaut
		ServerConfig defaultConfig;
		defaultConfig.setListen(port_num);
		defaultConfig.setServerName("localhost");
		defaultConfig.setRoot("./www");
		p_serverConfigs.push_back(defaultConfig);

		initSocket(); // Initialisation du socket lors de la construction du serveur
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error initializing server: " << e.what() << std::endl;
		throw; // Rejeter l'exception pour indiquer l'échec de la construction
	}
}

Server::Server(const std::string &configFile)
	: p_config(), p_serverConfigs()
{
	try
	{
		p_config.parseConfigFile(configFile);
		p_serverConfigs = p_config.getServers();

		if (p_serverConfigs.empty())
			throw std::runtime_error("No server configurations found in config file");

		initSocket(); // Initialisation de tous les sockets
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error initializing server: " << e.what() << std::endl;
		throw; // Rejeter l'exception pour indiquer l'échec de la construction
	}
}

Server::~Server()
{
	// Fermer tous les clients connectés
	for (size_t i = 0; i < p_fds.size(); ++i)
	{
		bool is_server_fd = false;
		for (size_t j = 0; j < p_socket_fds.size(); ++j)
		{
			if (p_fds[i].fd == p_socket_fds[j])
			{
				is_server_fd = true;
				break;
			}
		}
		if (!is_server_fd && p_fds[i].fd >= 0)
			close(p_fds[i].fd);
	}
	p_fds.clear();

	// Fermer tous les sockets d'écoute
	for (size_t i = 0; i < p_socket_fds.size(); ++i)
	{
		if (p_socket_fds[i] >= 0)
			close(p_socket_fds[i]);
	}
	p_socket_fds.clear();
}

void Server::initSocket() // Creation, bind et listen de tous les sockets
{
	// Group ServerConfigs by port to avoid binding multiple times to the same port
	std::map<int, std::vector<const ServerConfig *> > portToConfigs;

	for (size_t i = 0; i < p_serverConfigs.size(); ++i)
	{
		int port = p_serverConfigs[i].getListen();
		portToConfigs[port].push_back(&p_serverConfigs[i]);
	}

	// Create one listening socket per unique port
	for (std::map<int, std::vector<const ServerConfig *> >::iterator it = portToConfigs.begin();
		 it != portToConfigs.end(); ++it)
	{
		int port = it->first;
		std::stringstream ss;
		ss << port;
		std::string portStr = ss.str();

		int server_fd = createListeningSocket("0.0.0.0", portStr);
		p_socket_fds.push_back(server_fd);

		// Associate this socket with all ServerConfigs on this port
		// We'll use the first one by default, and select by Host header later
		p_fd_to_config[server_fd] = it->second[0];

		std::cout << "Server listening on 0.0.0.0:" << port << std::endl;
	}
}

int Server::createListeningSocket(const std::string &host, const std::string &port)
{
	struct addrinfo hints;
	struct addrinfo *addrinfo = NULL;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;		 // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP socket
	hints.ai_flags = AI_PASSIVE;	 // Pour bind auto à l'adresse locale (mode serveur)

	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo) != 0)
		throw std::runtime_error("getaddrinfo() failed for port " + port);

	// Création du socket
	int socket_fd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
	if (socket_fd < 0)
	{
		freeaddrinfo(addrinfo);
		throw std::runtime_error("socket() failed for port " + port);
	}

	int opt = 1;
	// Permet de réutiliser l'adresse immédiatement après la fermeture du socket
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(socket_fd);
		freeaddrinfo(addrinfo);
		throw std::runtime_error("setsockopt() failed for port " + port);
	}

	// Bind du socket à l'adresse et au port spécifiés
	if (bind(socket_fd, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0)
	{
		close(socket_fd);
		freeaddrinfo(addrinfo);
		throw std::runtime_error("bind() failed for port " + port);
	}

	// Écoute des connexions entrantes
	if (listen(socket_fd, SOMAXCONN) < 0) // SOMAXCONN pour file d'attente maximale
	{
		close(socket_fd);
		freeaddrinfo(addrinfo);
		throw std::runtime_error("listen() failed for port " + port);
	}

	freeaddrinfo(addrinfo);
	return socket_fd;
}

void Server::setNonBlocking(int fd) // Mettre un descripteur en mode non-bloquant
{
	int saved_errno;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) // Modifie les flags du fichier descriptor
	{
		saved_errno = errno;
		close(fd);
		throw std::runtime_error("fcntl() failed to set non-blocking: " +
								 std::string(std::strerror(saved_errno)));
	}

	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) // Fermer le fd lors d'un execve
	{
		saved_errno = errno;
		close(fd);
		throw std::runtime_error("fcntl() failed to set FD_CLOEXEC: " +
								 std::string(std::strerror(saved_errno)));
	}
}

void Server::initServerPollfds() // Initialiser les pollfds des serveurs
{
	// Ajouter tous les sockets d'écoute à la liste des fds surveillés
	for (size_t i = 0; i < p_socket_fds.size(); ++i)
	{
		struct pollfd server_pollfd;
		server_pollfd.fd = p_socket_fds[i];
		server_pollfd.events = POLLIN; // Surveiller les événements de lecture
		server_pollfd.revents = 0;
		p_fds.push_back(server_pollfd); // Ajouter le serveur à la liste des fds surveillés
	}
}

void Server::acceptNewClient(int server_fd) // Accepter une nouvelle connexion client
{
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd < 0)
	{
		int saved_errno = errno;
		if (saved_errno == EWOULDBLOCK || saved_errno == EAGAIN)
			return; // Pas de client disponible, c'est normal

		// Erreurs critiques
		throw std::runtime_error("accept() failed");
	}

	setNonBlocking(client_fd); // Mettre le descripteur en mode non-bloquant

	// Ajouter le client à la liste des fds surveillés
	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN; // Surveiller les événements de lecture
	client_pollfd.revents = 0;
	p_fds.push_back(client_pollfd); // Ajouter le client à la liste des fds surveillés

	// Enregistrer le temps de la dernière activité du client
	p_clients_last_activity[client_fd] = std::time(NULL);

	// Enregistrer le serveur d'origine pour ce client
	p_client_to_server_fd[client_fd] = server_fd;

	// Log de la nouvelle connexion
	std::cout << "[" << client_fd << "] New client connected" << std::endl;
}

bool Server::handleClient(size_t index) // Gérer la communication avec un client
{
	// Obtenir le descripteur du client
	int client_fd = p_fds[index].fd;
	char buffer[4096]; // Buffer pour recevoir les données

	// Recevoir des données du client
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';

		// Mettre à jour le temps de la dernière activité du client
		p_clients_last_activity[client_fd] = std::time(NULL);
		try
		{
			// Obtenir une référence à l'objet HttpRequest du client
			HttpRequest &request = p_clients_request[client_fd];

			// Ajouter les données reçues à l'objet HttpRequest du client
			request.appendData(std::string(buffer, bytes_received));

			// Tenter de parser la requête
			if (p_clients_request[client_fd].isComplete())
			{
				// Parser la requête complète
				request.parse();

				// Sélectionner le ServerConfig approprié en fonction du port et du Host header
				const ServerConfig *serverConfig = selectServerConfig(client_fd, request);

				// Créer un Router avec le bon ServerConfig
				Router router(serverConfig);

				// Vérifier si c'est une requête CGI
				if (router.isCgiRequest(request))
				{
					// Lancer le CGI en mode asynchrone
					std::string uri = request.getUri();
					const LocationConfig *location = serverConfig->findLocation(uri);
					
					std::string root = location->getRoot();
					if (root.empty())
						root = serverConfig->getRoot();

					size_t queryPos = uri.find('?');
					std::string uriPath = (queryPos != std::string::npos) ? uri.substr(0, queryPos) : uri;

					std::string scriptPath;
					if (uriPath[0] == '/' && root[root.size() - 1] == '/')
						scriptPath = root + uriPath.substr(1);
					else if (uriPath[0] != '/' && root[root.size() - 1] != '/')
						scriptPath = root + "/" + uriPath;
					else
						scriptPath = root + uriPath;

					pid_t pid;
					int pipe_fd = router.startCgiAsync(request, location, scriptPath, pid);

					if (pipe_fd >= 0)
					{
						// CGI lancé avec succès en mode asynchrone
						CgiProcess cgi;
						cgi.client_fd = client_fd;
						cgi.pid = pid;
						cgi.pipe_fd = pipe_fd;
						cgi.start_time = std::time(NULL);
						cgi.http_version = request.getHttpVersion();

						// Ajouter le pipe_fd à la map des processus CGI
						p_cgi_processes[pipe_fd] = cgi;

						// Ajouter le pipe_fd à poll pour surveiller
						struct pollfd pfd;
						pfd.fd = pipe_fd;
						pfd.events = POLLIN;
						pfd.revents = 0;
						p_fds.push_back(pfd);

						// Nettoyer la requête du client (elle est maintenant dans CgiProcess)
						p_clients_request.erase(client_fd);

						return true; // Requête traitée, attendre le CGI
					}
					else
					{
						// Erreur de lancement du CGI - envoyer une erreur 500
						HttpResponse errorResponse = router.createErrorResponse(500, request.getHttpVersion());
						std::string response_str = errorResponse.toString();
						p_pending_responses[client_fd] = response_str;
						p_bytes_sent[client_fd] = 0;
						p_fds[index].events = POLLOUT;
						return true;
					}
				}

				// Requête non-CGI : router normalement (synchrone)
				HttpResponse response = router.route(request);

				// Envoyer la réponse au client
				std::string response_str = response.toString();
				ssize_t sent = send(client_fd, response_str.c_str(), response_str.length(), 0);

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
							  << sent << "/" << response_str.length()
							  << " bytes (buffering remainder)" << std::endl;
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
				else // Supprimer la requête après traitement pour la prochaine
					p_clients_request.erase(client_fd);
			}
		}
		catch (const std::exception &e)
		{
			// Essayer d'obtenir la version HTTP de la requête, sinon HTTP/1.1 par défaut
			std::string version = "HTTP/1.1";
			std::string errorMsg = e.what();
			int statusCode = 400; // Code d'erreur par défaut

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

			// Récupérer le ServerConfig pour créer un Router temporaire
			std::map<int, int>::const_iterator it = p_client_to_server_fd.find(client_fd);
			const ServerConfig *serverConfig = (it != p_client_to_server_fd.end() &&
												p_fd_to_config.find(it->second) != p_fd_to_config.end())
												   ? p_fd_to_config.find(it->second)->second
												   : &p_serverConfigs[0];
			Router router(serverConfig);

			HttpResponse errorResponse = router.createErrorResponse(statusCode, version);

			std::string error_response_str = errorResponse.toString();
			ssize_t sent = send(client_fd, error_response_str.c_str(),
								error_response_str.length(), 0);

			if (sent < 0)
			{
				int saved_errno = errno;
				std::cerr << "[" << client_fd << "] Failed to send error response: "
						  << std::strerror(saved_errno) << std::endl;
			}

			closeClient(index);
			return false; // Client déconnecté
		}
		return true; // Client toujours connecté
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
		// Erreur recv() - Sauvegarder errno IMMÉDIATEMENT
		int saved_errno = errno;

		if (saved_errno == EWOULDBLOCK || saved_errno == EAGAIN)
			return true; // Pas de données disponibles

		// Erreur grave - utiliser saved_errno uniquement
		std::cerr << "[" << client_fd << "] recv() failed: "
				  << std::strerror(saved_errno) << std::endl;
		throw std::runtime_error("recv() failed");
	}
}

static bool g_isRunning = false; // Variable globale pour contrôler l'exécution du serveur

static void handleSignal(int signum) // Gérer les signaux (ex: SIGINT)
{
	(void)signum;
	g_isRunning = false; // Variable globale pour arrêter la boucle du serveur
}

void Server::start() // Méthode pour démarrer le serveur
{
	signal(SIGINT, handleSignal);  // Gérer l'interruption clavier (Ctrl+C)
	signal(SIGTERM, handleSignal); // Gérer le signal de terminaison

	// Mettre tous les sockets d'écoute en mode non-bloquant
	for (size_t i = 0; i < p_socket_fds.size(); ++i)
		setNonBlocking(p_socket_fds[i]);

	initServerPollfds(); // Initialiser les pollfds des serveurs

	std::cout << "Waiting for connections..." << std::endl;

	g_isRunning = true;

	// Boucle principale pour accepter les connexions entrantes
	while (g_isRunning)
	{
		try
		{
			int poll_count = poll(&p_fds[0], p_fds.size(), 1000); // Attente indéfinie
			if (poll_count < 0)
			{
				if (errno == EINTR) // Vérifier si poll a été interrompu par un signal
					continue;		// Recommencer la boucle si interrompu
				throw std::runtime_error("poll() failed");
			}

			checkTimeouts(); // Vérifier les timeouts des clients
			checkCgiProcesses(); // Vérifier l'état des processus CGI asynchrones

			// Parcourir les descripteurs pour vérifier lesquels ont des événements
			for (size_t i = 0; i < p_fds.size(); ++i)
			{
				// Sauvegarder le fd actuel pour vérifier s'il est toujours valide
				int current_fd = p_fds[i].fd;
				bool client_closed = false;

				if (p_fds[i].revents & POLLIN) // Vérifier les événements de lecture
				{
					// Vérifier si c'est un socket d'écoute
					bool is_server_fd = false;
					for (size_t j = 0; j < p_socket_fds.size(); ++j)
					{
						if (p_fds[i].fd == p_socket_fds[j])
						{
							is_server_fd = true;
							try
							{
								acceptNewClient(p_socket_fds[j]); // Nouvelle connexion entrante
							}
							catch (const std::exception &)
							{
							}
							break;
						}
					}

					if (!is_server_fd)
					{
						try
						{
							if (!handleClient(i)) // Données dispo à lire d'un client existant
							{
								client_closed = true;
								--i; // Ajuster l'index si un client a été supprimé
							}
						}
						catch (const std::exception &)
						{
							closeClient(i); // Utiliser closeClient() pour tout nettoyer
							client_closed = true;
							--i;
						}
					}
				}

				// Ne traiter POLLOUT que si le client n'a pas été fermé ET l'index est toujours valide
				if (!client_closed && i < p_fds.size() &&
					p_fds[i].fd == current_fd && (p_fds[i].revents & POLLOUT)) // Socket prêt pour écriture
				{
					try
					{
						if (!handleClientWrite(i)) // Envoyer les données en attente
							--i;				   // Ajuster l'index si un client a été supprimé
					}
					catch (const std::exception &)
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
			break; // Sortir de la boucle proprement
		}
	}

	// Log de l'arrêt du serveur
	std::cout << "\nServer shutting down gracefully..." << std::endl;
}

const ServerConfig *Server::selectServerConfig(int client_fd, const HttpRequest &request) const
{
	// Trouver le server_fd d'origine pour ce client
	std::map<int, int>::const_iterator it = p_client_to_server_fd.find(client_fd);
	if (it == p_client_to_server_fd.end())
	{
		// Fallback: retourner le premier ServerConfig si on ne trouve pas le mapping
		return &p_serverConfigs[0];
	}

	int server_fd = it->second;

	// Récupérer le ServerConfig par défaut pour ce port
	std::map<int, const ServerConfig *>::const_iterator configIt = p_fd_to_config.find(server_fd);
	if (configIt == p_fd_to_config.end())
	{
		// Fallback: retourner le premier ServerConfig
		return &p_serverConfigs[0];
	}

	const ServerConfig *defaultConfig = configIt->second;
	int port = defaultConfig->getListen();

	// Essayer de matcher le Host header avec les server_names sur le même port
	std::string hostHeader = request.getHeader("Host");
	if (!hostHeader.empty())
	{
		// Extraire le hostname (enlever le port si présent)
		size_t colonPos = hostHeader.find(':');
		std::string hostname = (colonPos != std::string::npos)
								   ? hostHeader.substr(0, colonPos)
								   : hostHeader;

		// Chercher un ServerConfig qui match le hostname sur ce port
		for (size_t i = 0; i < p_serverConfigs.size(); ++i)
		{
			if (p_serverConfigs[i].getListen() == port &&
				p_serverConfigs[i].getServerName() == hostname)
			{
				return &p_serverConfigs[i];
			}
		}
	}

	// Pas de match spécifique : retourner le ServerConfig par défaut pour ce port
	return defaultConfig;
}
