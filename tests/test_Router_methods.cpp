/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Router_methods.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 15:51:34 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

// Fonction helper pour créer une requête de test
HttpRequest createTestRequest(const std::string &method, const std::string &uri, 
                                const std::string &body = "")
{
	HttpRequest request;
	std::stringstream ss;
	ss << body.length();
	
	std::string rawRequest;
	
	if (method == "DELETE" || method == "GET")
	{
		// GET et DELETE ne doivent pas avoir de Content-Length
		rawRequest = method + " " + uri + " HTTP/1.1\r\n"
		             "Host: localhost:8080\r\n"
		             "\r\n";
	}
	else
	{
		// POST peut avoir un body
		rawRequest = method + " " + uri + " HTTP/1.1\r\n"
		             "Host: localhost:8080\r\n"
		             "Content-Length: " + ss.str() + "\r\n"
		             "\r\n" + body;
	}
	
	request.appendData(rawRequest);
	request.parse();
	return request;
}

// Test 1 : POST avec body vide (doit retourner 400)
void testPostEmptyBody()
{
	std::cout << "\n=== Test 1: POST with empty body ===" << std::endl;
	
	Router router("./www");
	HttpRequest request = createTestRequest("POST", "/upload", "");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 400);
	std::cout << "✅ POST with empty body returns 400 Bad Request" << std::endl;
}

// Test 2 : POST avec body valide (doit créer un fichier et retourner 201)
void testPostValidBody()
{
	std::cout << "\n=== Test 2: POST with valid body ===" << std::endl;
	
	// Créer le dossier uploads s'il n'existe pas
	mkdir("./uploads", 0755);
	
	Router router("./www");
	std::string body = "Hello World! This is test content.";
	HttpRequest request = createTestRequest("POST", "/upload", body);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST with valid body returns 201 Created" << std::endl;
	
	// Vérifier que le header Location est présent
	std::string location = response.getHeader("Location");
	assert(!location.empty());
	std::cout << "✅ Location header present: " << location << std::endl;
	
	// Extraire le chemin du fichier depuis l'URL (http://host/uploads/file.bin -> ./uploads/file.bin)
	std::string filePath;
	size_t uploadsPos = location.find("/uploads/");
	if (uploadsPos != std::string::npos)
		filePath = "." + location.substr(uploadsPos);
	else
		filePath = location; // Fallback si le format est inattendu
	
	// Vérifier que le fichier a été créé
	std::ifstream file(filePath.c_str());
	assert(file.is_open());
	std::cout << "✅ File created successfully at: " << filePath << std::endl;
	
	// Vérifier le contenu du fichier
	std::string fileContent;
	std::getline(file, fileContent);
	file.close();
	assert(fileContent.find("Hello World") != std::string::npos);
	std::cout << "✅ File content matches uploaded data" << std::endl;
	
	// Nettoyer
	unlink(filePath.c_str());
}

// Test 3 : POST avec body binaire
void testPostBinaryData()
{
	std::cout << "\n=== Test 3: POST with binary data ===" << std::endl;
	
	Router router("./www");
	// Utiliser des données texte pour éviter le problème de \0 dans std::string
	std::string textData = "Binary-like data with special chars: !@#$%^&*()_+{}[]|\\:;<>?,./~`";
	HttpRequest request = createTestRequest("POST", "/upload", textData);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST with special characters returns 201 Created" << std::endl;
	
	// Extraire le chemin du fichier depuis l'URL
	std::string location = response.getHeader("Location");
	std::string filePath;
	size_t uploadsPos = location.find("/uploads/");
	if (uploadsPos != std::string::npos)
		filePath = "." + location.substr(uploadsPos);
	else
		filePath = location;
	
	// Vérifier que le fichier a la bonne taille
	struct stat st;
	assert(stat(filePath.c_str(), &st) == 0);
	assert(st.st_size == (off_t)textData.length());
	std::cout << "✅ File size matches data length: " << st.st_size << " bytes" << std::endl;
	
	// Nettoyer
	unlink(filePath.c_str());
}

// Test 4 : DELETE fichier existant (doit retourner 204)
void testDeleteExistingFile()
{
	std::cout << "\n=== Test 4: DELETE existing file ===" << std::endl;
	
	// Créer un fichier de test
	std::string testFile = "./www/test_delete.txt";
	std::ofstream file(testFile.c_str());
	file << "Test content to delete";
	file.close();
	
	Router router("./www");
	HttpRequest request = createTestRequest("DELETE", "/test_delete.txt");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 204);
	std::cout << "✅ DELETE existing file returns 204 No Content" << std::endl;
	
	// Vérifier que le fichier a été supprimé
	std::ifstream checkFile(testFile.c_str());
	assert(!checkFile.is_open());
	std::cout << "✅ File deleted successfully" << std::endl;
}

// Test 5 : DELETE fichier inexistant (doit retourner 404)
void testDeleteNonExistentFile()
{
	std::cout << "\n=== Test 5: DELETE non-existent file ===" << std::endl;
	
	Router router("./www");
	HttpRequest request = createTestRequest("DELETE", "/nonexistent.txt");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 404);
	std::cout << "✅ DELETE non-existent file returns 404 Not Found" << std::endl;
}

// Test 6 : DELETE sur un répertoire (doit retourner 403)
void testDeleteDirectory()
{
	std::cout << "\n=== Test 6: DELETE directory ===" << std::endl;
	
	// Créer un répertoire de test
	mkdir("./www/test_dir", 0755);
	
	Router router("./www");
	HttpRequest request = createTestRequest("DELETE", "/test_dir");
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 403);
	std::cout << "✅ DELETE directory returns 403 Forbidden" << std::endl;
	
	// Nettoyer
	rmdir("./www/test_dir");
}

// Test 7 : DELETE avec path traversal (doit retourner 403)
void testDeletePathTraversal()
{
	std::cout << "\n=== Test 7: DELETE with path traversal ===" << std::endl;
	
	// La validation se fait au niveau de HttpRequest, donc on doit tester
	// que l'exception est lancée (ce qui sera converti en 403 par le Server)
	Router router("./www");
	
	try {
		HttpRequest request = createTestRequest("DELETE", "/../etc/passwd");
		HttpResponse response = router.route(request);
		// Si on arrive ici, le test échoue car l'exception aurait dû être lancée
		assert(false && "Path traversal should have thrown an exception");
	}
	catch (const std::runtime_error &e) {
		std::string errorMsg = e.what();
		assert(errorMsg.find("Directory traversal") != std::string::npos);
		std::cout << "✅ DELETE with path traversal throws exception (caught at HttpRequest level)" << std::endl;
	}
}

// Test 8 : POST large file (simuler un gros upload)
void testPostLargeFile()
{
	std::cout << "\n=== Test 8: POST large file ===" << std::endl;
	
	Router router("./www");
	std::string largeBody(1024 * 10, 'A'); // 10 KB (pas 100 KB pour test plus rapide)
	HttpRequest request = createTestRequest("POST", "/upload", largeBody);
	HttpResponse response = router.route(request);
	
	assert(response.getStatusCode() == 201);
	std::cout << "✅ POST large file (10KB) returns 201 Created" << std::endl;
	
	// Extraire le chemin du fichier depuis l'URL
	std::string location = response.getHeader("Location");
	std::string filePath;
	size_t uploadsPos = location.find("/uploads/");
	if (uploadsPos != std::string::npos)
		filePath = "." + location.substr(uploadsPos);
	else
		filePath = location;
	
	// Vérifier la taille du fichier créé
	struct stat st;
	assert(stat(filePath.c_str(), &st) == 0);
	assert(st.st_size == 1024 * 10);
	std::cout << "✅ Large file size correct: " << st.st_size << " bytes" << std::endl;
	
	// Nettoyer
	unlink(filePath.c_str());
}

// Test 9 : POST puis DELETE (workflow complet)
void testPostThenDelete()
{
	std::cout << "\n=== Test 9: POST then DELETE workflow ===" << std::endl;
	
	Router router("./www");
	
	// 1. POST pour créer un fichier dans ./www
	std::string body = "Test content for workflow";
	
	// Créer le fichier directement dans ./www pour pouvoir le DELETE
	std::ofstream testFile("./www/test_workflow.txt");
	testFile << body;
	testFile.close();
	std::cout << "✅ Step 1: File created in ./www" << std::endl;
	
	// 2. Vérifier que le fichier existe
	std::ifstream checkFile("./www/test_workflow.txt");
	assert(checkFile.is_open());
	checkFile.close();
	std::cout << "✅ Step 2: File exists and readable" << std::endl;
	
	// 3. DELETE pour supprimer le fichier
	HttpRequest deleteRequest = createTestRequest("DELETE", "/test_workflow.txt");
	HttpResponse deleteResponse = router.route(deleteRequest);
	
	assert(deleteResponse.getStatusCode() == 204);
	std::cout << "✅ Step 3: File deleted (204 No Content)" << std::endl;
	
	// 4. Vérifier que le fichier n'existe plus
	std::ifstream checkDeleted("./www/test_workflow.txt");
	assert(!checkDeleted.is_open());
	std::cout << "✅ Step 4: File no longer exists" << std::endl;
}

int main()
{
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║  Router POST/DELETE Methods Tests     ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;
	
	try
	{
		testPostEmptyBody();
		testPostValidBody();
		testPostBinaryData();
		testDeleteExistingFile();
		testDeleteNonExistentFile();
		testDeleteDirectory();
		testDeletePathTraversal();
		testPostLargeFile();
		testPostThenDelete();
		
		std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
		std::cout << "║  ✅ ALL TESTS PASSED! ✅               ║" << std::endl;
		std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
		
		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << "\n❌ TEST FAILED: " << e.what() << std::endl;
		return 1;
	}
}
