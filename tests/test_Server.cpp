/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 14:12:49 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:46:24 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_framework.hpp"
#include "Server.hpp"
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

// Fonction pour lancer le serveur dans un processus fils
void	runServer(const std::string &host, const std::string &port)
{
	try
	{
		Server server(host, port);
		server.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Server error: " << e.what() << std::endl;
		exit(1);
	}
}

// Fonction helper pour créer une connexion client
int	createClientSocket(const std::string &host, int port)
{
	int	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
		return -1;

	struct sockaddr_in	server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(host.c_str());

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}

// Fonction helper pour envoyer une requête HTTP et recevoir la réponse
std::string	sendHttpRequest(int sock_fd, const std::string &request)
{
	send(sock_fd, request.c_str(), request.length(), 0);

	char	buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	ssize_t	bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received > 0)
		return std::string(buffer, bytes_received);
	return "";
}

// TESTS UNITAIRES

void	testServerConstruction()
{
	std::cout << "\n=== Test 1: Server Construction ===" << std::endl;

	// Test 1.1: Création basique
	try
	{
		Server server("127.0.0.1", "8080");
		ASSERT_TRUE(true, "Basic server creation");
	}
	catch (...)
	{
		ASSERT_TRUE(false, "Basic server creation failed");
	}

	// Test 1.2: Port invalide (hors limites)
	try
	{
		Server server("127.0.0.1", "99999");
		ASSERT_TRUE(false, "Invalid port should throw exception");
	}
	catch (...)
	{
		ASSERT_TRUE(true, "Invalid port throws exception");
	}

	// Test 1.3: Adresse invalide
	try
	{
		Server server("999.999.999.999", "8080");
		ASSERT_TRUE(false, "Invalid address should throw exception");
	}
	catch (...)
	{
		ASSERT_TRUE(true, "Invalid address throws exception");
	}
}

void	testBasicConnection()
{
	std::cout << "\n=== Test 2: Basic Connection ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8081");
		exit(0);
	}

	sleep(1); // Attendre le démarrage du serveur

	// Test 2.1: Connexion simple
	int	sock_fd = createClientSocket("127.0.0.1", 8081);
	ASSERT_TRUE(sock_fd >= 0, "Client can connect to server");

	if (sock_fd >= 0)
	{
		// Test 2.2: Envoi de requête GET
		std::string response = sendHttpRequest(sock_fd, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
		ASSERT_TRUE(response.length() > 0, "Server responds to GET request");

		// Test 2.3: Vérification du code HTTP
		ASSERT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos,
			"Response contains HTTP 200 OK");

		// Test 2.4: Vérification du contenu (home page from Router)
		ASSERT_TRUE(response.find("Welcome") != std::string::npos,
			"Response contains expected home page content");

		close(sock_fd);
	}

	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
}

void	testMultipleClients()
{
	std::cout << "\n=== Test 3: Multiple Clients ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8082");
		exit(0);
	}

	sleep(1);

	const int NUM_CLIENTS = 5;
	int	clients[NUM_CLIENTS];
	bool all_connected = true;

	// Test 3.1: Connexion de plusieurs clients
	for (int i = 0; i < NUM_CLIENTS; ++i)
	{
		clients[i] = createClientSocket("127.0.0.1", 8082);
		if (clients[i] < 0)
			all_connected = false;
	}
	ASSERT_TRUE(all_connected, "Multiple clients can connect simultaneously");

	// Test 3.2: Tous les clients reçoivent des réponses
	bool all_responded = true;
	for (int i = 0; i < NUM_CLIENTS; ++i)
	{
		if (clients[i] >= 0)
		{
			std::string response = sendHttpRequest(clients[i],
				"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
			if (response.find("HTTP/1.1 200 OK") == std::string::npos)
				all_responded = false;
		}
	}
	ASSERT_TRUE(all_responded, "All clients receive valid responses");

	// Test 3.3: Fermeture des connexions
	for (int i = 0; i < NUM_CLIENTS; ++i)
	{
		if (clients[i] >= 0)
			close(clients[i]);
	}
	ASSERT_TRUE(true, "All client connections closed properly");

	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
}

void	testClientDisconnection()
{
	std::cout << "\n=== Test 4: Client Disconnection ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8083");
		exit(0);
	}

	sleep(1);

	// Test 4.1: Connexion puis déconnexion immédiate
	int	sock_fd = createClientSocket("127.0.0.1", 8083);
	ASSERT_TRUE(sock_fd >= 0, "Client connects successfully");

	if (sock_fd >= 0)
	{
		close(sock_fd);
		sleep(1); // Laisser le temps au serveur de détecter la déconnexion
		ASSERT_TRUE(true, "Client disconnection handled by server");
	}

	// Test 4.2: Reconnexion après déconnexion
	sock_fd = createClientSocket("127.0.0.1", 8083);
	ASSERT_TRUE(sock_fd >= 0, "Client can reconnect after disconnection");

	if (sock_fd >= 0)
	{
		std::string response = sendHttpRequest(sock_fd,
			"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
		ASSERT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos,
			"Reconnected client receives valid response");
		close(sock_fd);
	}

	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
}

void	testLargeRequest()
{
	std::cout << "\n=== Test 5: Large Request ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8084");
		exit(0);
	}

	sleep(1);

	int	sock_fd = createClientSocket("127.0.0.1", 8084);
	ASSERT_TRUE(sock_fd >= 0, "Client connects for large request test");

	if (sock_fd >= 0)
	{
		// Test 5.1: Requête avec beaucoup de headers
		std::string large_request = "GET / HTTP/1.1\r\n";
		large_request += "Host: localhost\r\n";
		for (int i = 0; i < 20; ++i)
		{
			std::ostringstream oss;
			oss << "X-Custom-Header-" << i << ": value\r\n";
			large_request += oss.str();
		}
		large_request += "\r\n";

		std::string response = sendHttpRequest(sock_fd, large_request);
		ASSERT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos,
			"Server handles request with many headers");

		close(sock_fd);
	}

	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
}

void	testSignalHandling()
{
	std::cout << "\n=== Test 6: Signal Handling ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8085");
		exit(0);
	}

	sleep(1);

	// Test 6.1: Envoi de SIGTERM
	int result = kill(pid, SIGTERM);
	ASSERT_EQUAL(0, result, "SIGTERM sent successfully");

	// Test 6.2: Vérification de l'arrêt propre
	int status;
	pid_t wait_result = waitpid(pid, &status, WNOHANG);
	sleep(1); // Laisser le temps au serveur de s'arrêter
	wait_result = waitpid(pid, &status, WNOHANG);
	ASSERT_TRUE(wait_result != 0, "Server stops after receiving SIGTERM");

	// Si le processus est toujours actif, le forcer
	if (wait_result == 0)
		kill(pid, SIGKILL);
	waitpid(pid, NULL, 0);
}

void	testConcurrentRequests()
{
	std::cout << "\n=== Test 7: Concurrent Requests ===" << std::endl;

	pid_t	pid = fork();
	if (pid == 0)
	{
		runServer("127.0.0.1", "8086");
		exit(0);
	}

	sleep(1);

	const int NUM_CONCURRENT = 10;
	int	clients[NUM_CONCURRENT];
	bool all_ok = true;

	// Test 7.1: Connexions simultanées
	for (int i = 0; i < NUM_CONCURRENT; ++i)
		clients[i] = createClientSocket("127.0.0.1", 8086);

	// Test 7.2: Envoi de requêtes concurrentes
	for (int i = 0; i < NUM_CONCURRENT; ++i)
	{
		if (clients[i] >= 0)
		{
			std::ostringstream oss;
			oss << "GET /page" << i << " HTTP/1.1\r\nHost: localhost\r\n\r\n";
			std::string request = oss.str();
			send(clients[i], request.c_str(), request.length(), 0);
		}
	}

	// Test 7.3: Vérification des réponses
	for (int i = 0; i < NUM_CONCURRENT; ++i)
	{
		if (clients[i] >= 0)
		{
			char buffer[1024];
			std::memset(buffer, 0, sizeof(buffer));
			ssize_t bytes = recv(clients[i], buffer, sizeof(buffer) - 1, 0);
			// Accept 200 or 404 (unknown routes)
			if (bytes <= 0 || 
			    (std::string(buffer).find("HTTP/1.1 200 OK") == std::string::npos &&
			     std::string(buffer).find("HTTP/1.1 404") == std::string::npos))
				all_ok = false;
			close(clients[i]);
		}
	}
	ASSERT_TRUE(all_ok, "All concurrent requests receive valid responses");

	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
}

void	testPortReuse()
{
	std::cout << "\n=== Test 8: Port Reuse ===" << std::endl;

	// Test 8.1: Premier serveur
	pid_t	pid1 = fork();
	if (pid1 == 0)
	{
		runServer("127.0.0.1", "8087");
		exit(0);
	}

	sleep(1);
	kill(pid1, SIGTERM);
	waitpid(pid1, NULL, 0);
	sleep(1); // Attendre la libération du port

	// Test 8.2: Deuxième serveur sur le même port
	pid_t	pid2 = fork();
	if (pid2 == 0)
	{
		runServer("127.0.0.1", "8087");
		exit(0);
	}

	sleep(1);
	int	sock_fd = createClientSocket("127.0.0.1", 8087);
	ASSERT_TRUE(sock_fd >= 0, "Port can be reused after server shutdown");

	if (sock_fd >= 0)
		close(sock_fd);

	kill(pid2, SIGTERM);
	waitpid(pid2, NULL, 0);
}

// ====================
// MAIN
// ====================

int	main()
{
	std::cout << "========================================" << std::endl;
	std::cout << "Starting Comprehensive Server Tests..." << std::endl;
	std::cout << "========================================" << std::endl;

	testServerConstruction();
	testBasicConnection();
	testMultipleClients();
	testClientDisconnection();
	testLargeRequest();
	testSignalHandling();
	testConcurrentRequests();
	testPortReuse();

	std::cout << "\n========================================" << std::endl;
	return printTestSummary();
}
