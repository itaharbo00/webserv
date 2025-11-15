/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Router_locationconfig.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 19:30:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 18:06:33 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

// Helper pour créer une requête HTTP
HttpRequest createTestRequest(const std::string &method, const std::string &uri, 
                                const std::string &body = "")
{
	HttpRequest request;
	std::stringstream ss;
	ss << body.length();
	
	std::string rawRequest;
	
	if (method == "DELETE" || method == "GET")
	{
		rawRequest = method + " " + uri + " HTTP/1.1\r\n"
		             "Host: localhost:8080\r\n"
		             "\r\n";
	}
	else
	{
		rawRequest = method + " " + uri + " HTTP/1.1\r\n"
		             "Host: localhost:8080\r\n"
		             "Content-Length: " + ss.str() + "\r\n"
		             "\r\n" + body;
	}
	
	request.appendData(rawRequest);
	request.parse();
	return request;
}

// Helper pour créer une config de test
ServerConfig* createTestServerConfig()
{
	ServerConfig *serverConfig = new ServerConfig();
	
	serverConfig->setListen(8080);
	serverConfig->setServerName("localhost");
	serverConfig->setRoot("./www");
	serverConfig->setIndex("index.html");
	
	// Location 1 : / (toutes méthodes autorisées)
	LocationConfig location1;
	location1.setPath("/");
	location1.setAllowedMethods("GET");
	location1.setAllowedMethods("POST");
	location1.setAllowedMethods("DELETE");
	serverConfig->addLocation(location1);
	
	// Location 2 : /upload (seulement POST, upload_store custom)
	LocationConfig location2;
	location2.setPath("/upload");
	location2.setAllowedMethods("POST");
	location2.setUploadStore("./custom_uploads");
	serverConfig->addLocation(location2);
	
	// Location 3 : /readonly (seulement GET)
	LocationConfig location3;
	location3.setPath("/readonly");
	location3.setAllowedMethods("GET");
	serverConfig->addLocation(location3);
	
	// Location 4 : /redirect (redirect 301 vers /)
	LocationConfig location4;
	location4.setPath("/redirect");
	location4.setAllowedMethods("GET");
	location4.setReturn(301, "/");
	serverConfig->addLocation(location4);
	
	// Location 5 : /api (seulement GET et POST)
	LocationConfig location5;
	location5.setPath("/api");
	location5.setAllowedMethods("GET");
	location5.setAllowedMethods("POST");
	serverConfig->addLocation(location5);
	
	return serverConfig;
}

// =============================================================================
// TESTS
// =============================================================================

// Test 1 : GET sur / autorisé
void testGetOnRoot()
{
	std::cout << "\n=== Test 1: GET on / (allowed) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("GET", "/");
	HttpResponse response = router.route(request);
	
	// GET est autorisé sur /, doit retourner 200 ou 404 (selon si fichier existe)
	assert(response.getStatusCode() == 200 || response.getStatusCode() == 404);
	std::cout << "✅ GET on / returns " << response.getStatusCode() << std::endl;
	
	delete serverConfig;
}

// Test 2 : POST sur /upload avec upload_store custom
void testPostOnUploadCustomStore()
{
	std::cout << "\n=== Test 2: POST on /upload (custom upload_store) ===" << std::endl;
	
	// Créer le dossier custom_uploads
	mkdir("./custom_uploads", 0755);
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	std::string body = "Test upload data";
	HttpRequest request = createTestRequest("POST", "/upload", body);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST on /upload returns 201 Created" << std::endl;
	
	// Vérifier que le fichier a été créé dans custom_uploads/
	std::string location = response.getHeader("Location");
	assert(location.find("/custom_uploads/") != std::string::npos);
	std::cout << "✅ File created in custom_uploads: " << location << std::endl;
	
	delete serverConfig;
}

// Test 3 : POST sur /readonly interdit (405)
void testPostOnReadonlyForbidden()
{
	std::cout << "\n=== Test 3: POST on /readonly (forbidden) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	std::string body = "Trying to post";
	HttpRequest request = createTestRequest("POST", "/readonly", body);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 405);
	std::cout << "✅ POST on /readonly returns 405 Method Not Allowed" << std::endl;
	
	delete serverConfig;
}

// Test 4 : DELETE sur /readonly interdit (405)
void testDeleteOnReadonlyForbidden()
{
	std::cout << "\n=== Test 4: DELETE on /readonly (forbidden) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("DELETE", "/readonly/test.txt");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 405);
	std::cout << "✅ DELETE on /readonly returns 405 Method Not Allowed" << std::endl;
	
	delete serverConfig;
}

// Test 5 : GET sur /redirect retourne 301
void testGetOnRedirect()
{
	std::cout << "\n=== Test 5: GET on /redirect (301 redirect) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("GET", "/redirect");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 301);
	std::cout << "✅ GET on /redirect returns 301 Moved Permanently" << std::endl;
	
	// Vérifier le header Location
	std::string location = response.getHeader("Location");
	assert(location == "/");
	std::cout << "✅ Location header: " << location << std::endl;
	
	// Vérifier que le body contient une page HTML avec meta refresh
	std::string body = response.getBody();
	assert(body.find("meta http-equiv=\"refresh\"") != std::string::npos);
	assert(body.find("url=/") != std::string::npos);
	std::cout << "✅ Response body contains meta refresh tag" << std::endl;
	
	delete serverConfig;
}

// Test 6 : DELETE sur /api interdit (405)
void testDeleteOnApiForbidden()
{
	std::cout << "\n=== Test 6: DELETE on /api (forbidden) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("DELETE", "/api/resource");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 405);
	std::cout << "✅ DELETE on /api returns 405 Method Not Allowed" << std::endl;
	
	delete serverConfig;
}

// Test 7 : POST sur /api autorisé
void testPostOnApiAllowed()
{
	std::cout << "\n=== Test 7: POST on /api (allowed) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	std::string body = "API data";
	HttpRequest request = createTestRequest("POST", "/api", body);
	HttpResponse response = router.route(request);
	
	// POST est autorisé sur /api, mais pas d'upload_store configuré
	// Devrait utiliser ./uploads par défaut
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST on /api returns 201 Created" << std::endl;
	
	// Vérifier que le fichier va dans ./uploads (défaut)
	std::string location = response.getHeader("Location");
	assert(location.find("/uploads/") != std::string::npos);
	std::cout << "✅ File created in default uploads: " << location << std::endl;
	
	delete serverConfig;
}

// Test 8 : GET sur /api autorisé
void testGetOnApiAllowed()
{
	std::cout << "\n=== Test 8: GET on /api (allowed) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("GET", "/api");
	HttpResponse response = router.route(request);
	
	// GET est autorisé, devrait retourner 200 ou 404
	assert(response.getStatusCode() == 200 || response.getStatusCode() == 404);
	std::cout << "✅ GET on /api returns " << response.getStatusCode() << std::endl;
	
	delete serverConfig;
}

// Test 9 : POST sur / utilise upload_store par défaut
void testPostOnRootDefaultUpload()
{
	std::cout << "\n=== Test 9: POST on / (default upload_store) ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	std::string body = "Root upload data";
	HttpRequest request = createTestRequest("POST", "/", body);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST on / returns 201 Created" << std::endl;
	
	// Vérifier que le fichier va dans ./uploads (défaut)
	std::string location = response.getHeader("Location");
	assert(location.find("/uploads/") != std::string::npos);
	std::cout << "✅ File created in default uploads: " << location << std::endl;
	
	delete serverConfig;
}

// Test 10 : Location inexistante retourne 404
void testNonExistentLocation()
{
	std::cout << "\n=== Test 10: GET on non-existent location ===" << std::endl;
	
	ServerConfig *serverConfig = createTestServerConfig();
	Router router(serverConfig);
	
	HttpRequest request = createTestRequest("GET", "/doesnotexist");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 404);
	std::cout << "✅ GET on /doesnotexist returns 404 Not Found" << std::endl;
	
	delete serverConfig;
}

// =============================================================================
// MAIN
// =============================================================================

int main()
{
	std::cout << "╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║  Router + LocationConfig Tests        ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;

	try
	{
		testGetOnRoot();
		testPostOnUploadCustomStore();
		testPostOnReadonlyForbidden();
		testDeleteOnReadonlyForbidden();
		testGetOnRedirect();
		testDeleteOnApiForbidden();
		testPostOnApiAllowed();
		testGetOnApiAllowed();
		testPostOnRootDefaultUpload();
		testNonExistentLocation();
		
		std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
		std::cout << "║  ✅ ALL TESTS PASSED! ✅               ║" << std::endl;
		std::cout << "╚════════════════════════════════════════╝" << std::endl;
		
		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << "\n❌ TEST FAILED: " << e.what() << std::endl;
		return 1;
	}
}
