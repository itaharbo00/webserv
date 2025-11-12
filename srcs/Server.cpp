/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:29 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/12 23:09:44 by itaharbo         ###   ########.fr       */
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
		
		p_initSocket(); // Initialisation du socket lors de la construction du serveur
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

	if (p_addrinfo)	// Verifier que p_addrinfo n'est pas NULL avant d'appeler freeaddrinfo
		freeaddrinfo(p_addrinfo);
}

void	Server::p_initSocket()	// Creation, bind et listen du socket
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

void	Server::p_setNonBlocking(int fd)	// Mettre un descripteur en mode non-bloquant
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

void	Server::p_initServerPollfd()	// Initialiser le pollfd du serveur
{
	struct pollfd	server_pollfd;
	server_pollfd.fd = p_socket_fd;
	server_pollfd.events = POLLIN; // Surveiller les événements de lecture
	server_pollfd.revents = 0;
	p_fds.push_back(server_pollfd); // Ajouter le serveur à la liste des fds surveillés
}

void	Server::p_acceptNewClient()	// Accepter une nouvelle connexion client
{
	int	client_fd = accept(p_socket_fd, NULL, NULL);
	if (client_fd < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;	// Pas de client disponible, c'est normal

		// Erreurs critiques
		throw std::runtime_error("accept() failed");
	}

	p_setNonBlocking(client_fd); // Mettre le descripteur en mode non-bloquant

	struct pollfd	client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN; // Surveiller les événements de lecture
	client_pollfd.revents = 0;
	p_fds.push_back(client_pollfd); // Ajouter le client à la liste des fds surveillés
}

bool	Server::p_handleClient(size_t index)	// Gérer la communication avec un client
{
	int		client_fd = p_fds[index].fd;
	char	buffer[4096]; // Buffer pour recevoir les données

	// Recevoir des données du client
	ssize_t	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';

		try
		{
			// Ajouter les données reçues à l'objet HttpRequest du client
			p_clients_request[client_fd].appendData(std::string(buffer, bytes_received));
			// Tenter de parser la requête
			if (p_clients_request[client_fd].isComplete())
			{
				HttpRequest	&request = p_clients_request[client_fd];
				request.parse();

				HttpResponse	response;

				response.setHttpVersion(request.getHttpVersion());
				response.setStatusCode(200);
				response.setHeader("Content-Type", "text/plain");
				response.setBody("Hello, World!");
				if (request.shouldCloseConnection())
				{
					response.setHeader("Connection", "close");
				}
				else
					response.setHeader("Connection", "keep-alive");

				std::string	response_str = response.toString();
				send(client_fd, response_str.c_str(), response_str.length(), 0);

				if (request.shouldCloseConnection())
				{
					close(client_fd);
					p_fds.erase(p_fds.begin() + index);
					p_clients_request.erase(client_fd);
					return (false);
				}
				else	// Supprimer la requête après traitement pour la prochaine
					p_clients_request.erase(client_fd);
			}
		}
		catch (const std::exception &e)
		{
			HttpResponse	errorResponse;

			// Essayer d'obtenir la version HTTP de la requête, sinon HTTP/1.1 par défaut
			std::string version = "HTTP/1.1";
			try
			{
				version = p_clients_request[client_fd].getHttpVersion();
				if (version.empty())
					version = "HTTP/1.1";
			}
			catch (...)
			{
			}
			errorResponse.setHttpVersion(version);
			errorResponse.setStatusCode(400); // Bad Request
			errorResponse.setHeader("Content-Type", "text/plain");
			errorResponse.setBody("Bad Request");
			errorResponse.setHeader("Connection", "close");
			std::string	error_response_str = errorResponse.toString();
			send(client_fd, error_response_str.c_str(),
				error_response_str.length(), 0);

			p_clients_request.erase(client_fd); // Supprimer la requête erronée
			close(client_fd);
			p_fds.erase(p_fds.begin() + index);
			return false;	// Client déconnecté
		}
		return true;	// Client toujours connecté
	}
	else if (bytes_received == 0)
	{
		// Client a fermé la connexion (normal)
		close(client_fd);
		p_fds.erase(p_fds.begin() + index);
		p_clients_request.erase(client_fd);
		return false;
	}
	else
	{
		// Erreur recv()
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return true;	// Pas de données disponibles

		// Erreur grave
		throw std::runtime_error("recv() failed");
	}
}

static bool	g_isRunning = false;	// Variable globale pour contrôler l'exécution du serveur

static void	handleSignal(int signum)	// Gérer les signaux (ex: SIGINT)
{
	(void)signum;
	g_isRunning = false; // Variable globale pour arrêter la boucle du serveur
}

void	Server::start()	// Méthode pour démarrer le serveur
{
	signal(SIGINT, handleSignal);	// Gérer l'interruption clavier (Ctrl+C)
	signal(SIGTERM, handleSignal);	// Gérer le signal de terminaison

	p_setNonBlocking(p_socket_fd); // Mettre le socket du serveur en mode non-bloquant
	p_initServerPollfd();          // Initialiser le pollfd du serveur

	g_isRunning = true;
	
	// Boucle principale pour accepter les connexions entrantes
	while (g_isRunning)
	{
		try
		{
			int	poll_count = poll(&p_fds[0], p_fds.size(), -1);	// Attente indéfinie
			if (poll_count < 0)
			{
				if (errno == EINTR)	// Vérifier si poll a été interrompu par un signal
					continue;		// Recommencer la boucle si interrompu
				throw std::runtime_error("poll() failed");
			}

			// Parcourir les descripteurs pour vérifier lesquels ont des événements
			for (size_t i = 0; i < p_fds.size(); ++i)
			{
				if (p_fds[i].revents & POLLIN)	// Vérifier les événements de lecture
				{
					if (p_fds[i].fd == p_socket_fd)
					{
						try
						{
							p_acceptNewClient();	// Nouvelle connexion entrante
						}
						catch (const std::exception&)
						{
						}
					}
					else
					{
						try
						{
							if (!p_handleClient(i)) // Données dispo à lire d'un client existant
								--i;	// Ajuster l'index si un client a été supprimé
						}
						catch (const std::exception&)
						{
							if (i > 0)
							{
								close(p_fds[i].fd);
								p_fds.erase(p_fds.begin() + i);
								--i;
							}
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
}
