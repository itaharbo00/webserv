/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_partial_send.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 19:45:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 21:03:19 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Test manuel pour vérifier la gestion des envois partiels
// Ce test nécessite de réduire artificiellement le buffer du socket

int main()
{
	std::cout << "\n=== Test de gestion des envois partiels ===\n" << std::endl;

	// Créer un socket client
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		std::cerr << "❌ Failed to create socket" << std::endl;
		return 1;
	}

	// Réduire le buffer d'envoi du socket pour forcer des envois partiels
	int sendbuf_size = 1024; // 1 KB seulement
	if (setsockopt(client_fd, SOL_SOCKET, SO_SNDBUF, &sendbuf_size, sizeof(sendbuf_size)) < 0)
	{
		std::cerr << "⚠️  Warning: Could not set send buffer size" << std::endl;
	}

	// Connexion au serveur
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	std::cout << "🔌 Connecting to server..." << std::endl;
	if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "❌ Connection failed: " << std::strerror(errno) << std::endl;
		close(client_fd);
		return 1;
	}
	std::cout << "✅ Connected!" << std::endl;

	// Envoyer une requête pour un gros fichier CSS (2633 bytes)
	std::string request = "GET /css/style.css HTTP/1.1\r\nHost: localhost\r\n\r\n";
	
	std::cout << "\n📤 Sending request for large CSS file..." << std::endl;
	ssize_t sent = send(client_fd, request.c_str(), request.length(), 0);
	if (sent < 0)
	{
		std::cerr << "❌ Send failed" << std::endl;
		close(client_fd);
		return 1;
	}

	// Recevoir la réponse
	std::cout << "📥 Receiving response..." << std::endl;
	char buffer[8192];
	std::string response;
	ssize_t total_received = 0;

	while (true)
	{
		ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (received <= 0)
			break;
		
		buffer[received] = '\0';
		response += std::string(buffer, received);
		total_received += received;
		
		std::cout << "  Received " << received << " bytes (total: " << total_received << ")" << std::endl;
		
		// Vérifier si on a reçu la réponse complète
		if (response.find("\r\n\r\n") != std::string::npos)
		{
			size_t header_end = response.find("\r\n\r\n") + 4;
			
			// Vérifier Content-Length
			size_t cl_pos = response.find("Content-Length: ");
			if (cl_pos != std::string::npos)
			{
				size_t cl_start = cl_pos + 16;
				size_t cl_end = response.find("\r\n", cl_start);
				std::string cl_str = response.substr(cl_start, cl_end - cl_start);
				size_t content_length = atoi(cl_str.c_str());
				
				if (response.length() >= header_end + content_length)
					break; // Réponse complète reçue
			}
		}
	}

	std::cout << "\n✅ Total received: " << total_received << " bytes" << std::endl;

	// Vérifier que la réponse est complète
	if (response.find("200 OK") != std::string::npos)
		std::cout << "✅ Status: 200 OK" << std::endl;
	else
		std::cout << "❌ Unexpected status" << std::endl;

	if (response.find("text/css") != std::string::npos)
		std::cout << "✅ Content-Type: text/css" << std::endl;
	else
		std::cout << "❌ Wrong Content-Type" << std::endl;

	// Vérifier que le CSS contient du contenu reconnaissable
	if (response.find("background") != std::string::npos)
		std::cout << "✅ CSS content received correctly" << std::endl;
	else
		std::cout << "❌ CSS content incomplete" << std::endl;

	close(client_fd);

	std::cout << "\n=== Test terminé ===\n" << std::endl;
	
	return 0;
}
