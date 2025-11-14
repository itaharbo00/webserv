/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 22:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 22:04:22 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include <iostream>
#include <cassert>

void testSimpleConfig()
{
	std::cout << "\n=== Test 1: Simple Config ===" << std::endl;
	
	try
	{
		Config config("tests/config_files/simple.conf");
		
		// Vérifier qu'on a 1 serveur
		std::vector<ServerConfig> servers = config.getServers();
		assert(servers.size() == 1);
		std::cout << "✅ Found 1 server" << std::endl;
		
		// Vérifier le serveur
		const ServerConfig &server = servers[0];
		assert(server.getListen() == 8080);
		std::cout << "✅ Port: 8080" << std::endl;
		
		assert(server.getServerName() == "localhost");
		std::cout << "✅ Server name: localhost" << std::endl;
		
		assert(server.getRoot() == "./www");
		std::cout << "✅ Root: ./www" << std::endl;
		
		assert(server.getIndex() == "index.html");
		std::cout << "✅ Index: index.html" << std::endl;
		
		assert(server.getClientMaxBodySize() == 10 * 1024 * 1024);
		std::cout << "✅ Client max body size: 10MB" << std::endl;
		
		// Vérifier error pages
		std::string page404 = server.getErrorPage(404);
		assert(page404 == "/errors/404.html");
		std::cout << "✅ Error page 404: /errors/404.html" << std::endl;
		
		std::string page500 = server.getErrorPage(500);
		assert(page500 == "/errors/50x.html");
		std::cout << "✅ Error page 500: /errors/50x.html" << std::endl;
		
		// Vérifier locations
		std::vector<LocationConfig> locations = server.getLocations();
		assert(locations.size() == 2);
		std::cout << "✅ Found 2 locations" << std::endl;
		
		// Vérifier location /
		const LocationConfig *loc = server.findLocation("/");
		assert(loc != NULL);
		assert(loc->getPath() == "/");
		assert(loc->isMethodAllowed("GET"));
		assert(loc->isMethodAllowed("POST"));
		assert(loc->isMethodAllowed("DELETE"));
		std::cout << "✅ Location / configured correctly" << std::endl;
		
		// Vérifier location /uploads
		const LocationConfig *locUploads = server.findLocation("/uploads/file.jpg");
		assert(locUploads != NULL);
		assert(locUploads->getPath() == "/uploads");
		assert(locUploads->isMethodAllowed("GET"));
		assert(locUploads->isMethodAllowed("POST"));
		assert(!locUploads->isMethodAllowed("DELETE"));
		assert(locUploads->hasUpload());
		std::cout << "✅ Location /uploads configured correctly" << std::endl;
		
		std::cout << "✅ Test 1 PASSED\n" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ Test 1 FAILED: " << e.what() << std::endl;
		exit(1);
	}
}

void testMultiServer()
{
	std::cout << "\n=== Test 2: Multi-Server Config ===" << std::endl;
	
	try
	{
		Config config("tests/config_files/multi_server.conf");
		
		// Vérifier qu'on a 2 serveurs
		std::vector<ServerConfig> servers = config.getServers();
		assert(servers.size() == 2);
		std::cout << "✅ Found 2 servers" << std::endl;
		
		// Vérifier serveur 1
		const ServerConfig &server1 = servers[0];
		assert(server1.getListen() == 8080);
		assert(server1.getServerName() == "localhost");
		std::cout << "✅ Server 1: localhost:8080" << std::endl;
		
		// Vérifier serveur 2
		const ServerConfig &server2 = servers[1];
		assert(server2.getListen() == 8081);
		assert(server2.getServerName() == "example.com");
		std::cout << "✅ Server 2: example.com:8081" << std::endl;
		
		// Vérifier getServerByPort
		const ServerConfig *foundServer = config.getServerByPort(8080);
		assert(foundServer != NULL);
		assert(foundServer->getServerName() == "localhost");
		std::cout << "✅ getServerByPort(8080) works" << std::endl;
		
		std::cout << "✅ Test 2 PASSED\n" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ Test 2 FAILED: " << e.what() << std::endl;
		exit(1);
	}
}

void testCompleteConfig()
{
	std::cout << "\n=== Test 3: Complete Config ===" << std::endl;
	
	try
	{
		Config config("tests/config_files/complete.conf");
		
		std::vector<ServerConfig> servers = config.getServers();
		const ServerConfig &server = servers[0];
		
		// Test location avec toutes les directives
		const LocationConfig *locCgi = server.findLocation("/cgi-bin/script.php");
		assert(locCgi != NULL);
		assert(locCgi->hasCgi());
		assert(locCgi->getCgiPass() == "/usr/bin/php-cgi");
		assert(locCgi->getCgiExt() == ".php");
		std::cout << "✅ Location /cgi-bin with CGI configured" << std::endl;
		
		// Test redirect
		const LocationConfig *locRedirect = server.findLocation("/redirect");
		assert(locRedirect != NULL);
		assert(locRedirect->hasRedirect());
		assert(locRedirect->getReturnCode() == 301);
		assert(locRedirect->getReturnUrl() == "/");
		std::cout << "✅ Location /redirect with redirect 301 configured" << std::endl;
		
		// Test autoindex
		const LocationConfig *locApi = server.findLocation("/api/users");
		assert(locApi != NULL);
		assert(locApi->getAutoindex() == true);
		std::cout << "✅ Location /api with autoindex on" << std::endl;
		
		std::cout << "✅ Test 3 PASSED\n" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ Test 3 FAILED: " << e.what() << std::endl;
		exit(1);
	}
}

void testLongestPrefixMatch()
{
	std::cout << "\n=== Test 4: Longest Prefix Match ===" << std::endl;
	
	try
	{
		Config config("tests/config_files/complete.conf");
		std::vector<ServerConfig> servers = config.getServers();
		const ServerConfig &server = servers[0];
		
		// Test 1: /api/users doit matcher /api (pas /)
		const LocationConfig *loc1 = server.findLocation("/api/users");
		assert(loc1 != NULL);
		assert(loc1->getPath() == "/api");
		std::cout << "✅ /api/users matches /api (not /)" << std::endl;
		
		// Test 2: /uploads/file.jpg doit matcher /uploads (pas /)
		const LocationConfig *loc2 = server.findLocation("/uploads/file.jpg");
		assert(loc2 != NULL);
		assert(loc2->getPath() == "/uploads");
		std::cout << "✅ /uploads/file.jpg matches /uploads" << std::endl;
		
		// Test 3: /unknown doit matcher / (fallback)
		const LocationConfig *loc3 = server.findLocation("/unknown");
		assert(loc3 != NULL);
		assert(loc3->getPath() == "/");
		std::cout << "✅ /unknown matches / (fallback)" << std::endl;
		
		std::cout << "✅ Test 4 PASSED\n" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ Test 4 FAILED: " << e.what() << std::endl;
		exit(1);
	}
}

void testParseSizeValue()
{
	std::cout << "\n=== Test 5: Parse Size Values ===" << std::endl;
	
	try
	{
		Config config("tests/config_files/simple.conf");
		std::vector<ServerConfig> servers = config.getServers();
		const ServerConfig &server = servers[0];
		
		// Vérifier 10M
		assert(server.getClientMaxBodySize() == 10 * 1024 * 1024);
		std::cout << "✅ 10M = " << server.getClientMaxBodySize() << " bytes" << std::endl;
		
		// Note: client_max_body_size dans location n'est pas encore supporté
		// mais le parsing doit fonctionner
		
		std::cout << "✅ Test 5 PASSED\n" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ Test 5 FAILED: " << e.what() << std::endl;
		exit(1);
	}
}

void testErrorHandling()
{
	std::cout << "\n=== Test 6: Error Handling ===" << std::endl;
	
	// Test fichier inexistant
	try
	{
		Config config("tests/config_files/nonexistent.conf");
		std::cerr << "❌ Should have thrown exception for nonexistent file" << std::endl;
		exit(1);
	}
	catch (const std::exception &e)
	{
		std::cout << "✅ Correctly throws exception for nonexistent file" << std::endl;
	}
	
	std::cout << "✅ Test 6 PASSED\n" << std::endl;
}

int main()
{
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║     Config Parser Test Suite          ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;
	
	testSimpleConfig();
	testMultiServer();
	testCompleteConfig();
	testLongestPrefixMatch();
	testParseSizeValue();
	testErrorHandling();
	
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║   ✅ ALL CONFIG TESTS PASSED! ✅       ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
	
	return 0;
}
