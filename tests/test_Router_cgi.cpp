/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Router_cgi.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 19:30:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 19:32:07 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Router.hpp"
#include "../includes/HttpRequest.hpp"
#include "../includes/HttpResponse.hpp"
#include "../includes/LocationConfig.hpp"
#include <iostream>

int main()
{
    std::cout << "\n========== Tests CGI Router ==========" << std::endl;
    
    Router router;
    int testsPassed = 0;
    int testsFailed = 0;
    
    // Test 1: getCgiExtension - Extension basique
    {
        std::string ext = router.getCgiExtension("/cgi-bin/script.php");
        if (ext == ".php") {
            std::cout << "[OK] getCgiExtension - Extension basique" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] getCgiExtension - Attendu '.php', reçu '" << ext << "'" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 2: getCgiExtension - Pas d'extension
    {
        std::string ext = router.getCgiExtension("/cgi-bin/script");
        if (ext.empty()) {
            std::cout << "[OK] getCgiExtension - Pas d'extension" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] getCgiExtension - Attendu '', reçu '" << ext << "'" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 3: getCgiExtension - Multiple dots
    {
        std::string ext = router.getCgiExtension("/path/file.test.php");
        if (ext == ".php") {
            std::cout << "[OK] getCgiExtension - Multiple dots" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] getCgiExtension - Attendu '.php', reçu '" << ext << "'" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 4: buildBasicCgiEnv - GET avec query string
    {
        HttpRequest request;
        std::vector<std::string> env;
        
        request.setMethod("GET");
        request.setUri("/cgi-bin/hello.php?name=test&id=42");
        
        router.buildBasicCgiEnv(request, "/var/www/cgi-bin/hello.php", env);
        
        bool hasMethod = false;
        bool hasQueryString = false;
        bool hasScriptName = false;
        
        for (size_t i = 0; i < env.size(); i++) {
            if (env[i] == "REQUEST_METHOD=GET") hasMethod = true;
            if (env[i] == "QUERY_STRING=name=test&id=42") hasQueryString = true;
            if (env[i] == "SCRIPT_NAME=/cgi-bin/hello.php") hasScriptName = true;
        }
        
        if (hasMethod && hasQueryString && hasScriptName) {
            std::cout << "[OK] buildBasicCgiEnv - GET avec query string" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildBasicCgiEnv - Variables manquantes" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 5: buildServerCgiEnv - Host header
    {
        HttpRequest request;
        std::vector<std::string> env;
        
        request.addHeader("Host", "example.com:8080");
        router.buildServerCgiEnv(request, env);
        
        bool hasProtocol = false;
        bool hasServerName = false;
        bool hasServerPort = false;
        
        for (size_t i = 0; i < env.size(); i++) {
            if (env[i] == "SERVER_PROTOCOL=HTTP/1.1") hasProtocol = true;
            if (env[i] == "SERVER_NAME=example.com") hasServerName = true;
            if (env[i] == "SERVER_PORT=8080") hasServerPort = true;
        }
        
        if (hasProtocol && hasServerName && hasServerPort) {
            std::cout << "[OK] buildServerCgiEnv - Host header" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildServerCgiEnv - Variables serveur manquantes" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 6: buildContentCgiEnv - POST avec body
    {
        HttpRequest request;
        std::vector<std::string> env;
        
        request.setMethod("POST");
        request.setBody("name=Alice&email=alice@test.com");
        request.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        router.buildContentCgiEnv(request, env);
        
        bool hasLength = false;
        bool hasType = false;
        
        for (size_t i = 0; i < env.size(); i++) {
            if (env[i] == "CONTENT_LENGTH=33") hasLength = true;
            if (env[i] == "CONTENT_TYPE=application/x-www-form-urlencoded") hasType = true;
        }
        
        if (hasLength && hasType) {
            std::cout << "[OK] buildContentCgiEnv - POST avec body" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildContentCgiEnv - Variables content manquantes" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 7: buildHttpHeadersCgiEnv
    {
        HttpRequest request;
        std::vector<std::string> env;
        
        request.addHeader("User-Agent", "TestClient/1.0");
        request.addHeader("Accept", "text/html");
        
        router.buildHttpHeadersCgiEnv(request, env);
        
        bool hasUserAgent = false;
        bool hasAccept = false;
        
        for (size_t i = 0; i < env.size(); i++) {
            if (env[i] == "HTTP_USER_AGENT=TestClient/1.0") hasUserAgent = true;
            if (env[i] == "HTTP_ACCEPT=text/html") hasAccept = true;
        }
        
        if (hasUserAgent && hasAccept) {
            std::cout << "[OK] buildHttpHeadersCgiEnv - Conversion headers" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildHttpHeadersCgiEnv - Headers manquants" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 8: parseCgiOutput - Basic
    {
        std::string output = "Content-Type: text/html\r\n\r\n<html><body>Hello</body></html>";
        std::string headers, body;
        
        bool result = router.parseCgiOutput(output, headers, body);
        
        if (result && headers == "Content-Type: text/html" && body == "<html><body>Hello</body></html>") {
            std::cout << "[OK] parseCgiOutput - Basic" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] parseCgiOutput - Parsing incorrect" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 9: parseCgiOutput - Unix newlines
    {
        std::string output = "Content-Type: text/plain\n\n<html>test</html>";
        std::string headers, body;
        
        bool result = router.parseCgiOutput(output, headers, body);
        
        if (result && headers == "Content-Type: text/plain" && body == "<html>test</html>") {
            std::cout << "[OK] parseCgiOutput - Unix newlines" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] parseCgiOutput - Unix newlines incorrect" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 10: parseCgiOutput - No separator
    {
        std::string output = "Content-Type: text/html";
        std::string headers, body;
        
        bool result = router.parseCgiOutput(output, headers, body);
        
        if (!result) {
            std::cout << "[OK] parseCgiOutput - No separator (devrait échouer)" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] parseCgiOutput - Devrait échouer sans séparateur" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 11: buildCgiResponse - Basic 200
    {
        HttpRequest request;
        std::string headers = "Content-Type: text/html";
        std::string body = "<html><body>Hello</body></html>";
        
        HttpResponse response = router.buildCgiResponse(request, headers, body);
        
        if (response.getStatusCode() == 200 && response.getBody() == body) {
            std::cout << "[OK] buildCgiResponse - Basic 200" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildCgiResponse - Status ou body incorrect" << std::endl;
            testsFailed++;
        }
    }
    
    // Test 12: buildCgiResponse - Custom status 404
    {
        HttpRequest request;
        std::string headers = "Status: 404 Not Found\r\nContent-Type: text/html";
        std::string body = "<html><body>Not Found</body></html>";
        
        HttpResponse response = router.buildCgiResponse(request, headers, body);
        
        if (response.getStatusCode() == 404) {
            std::cout << "[OK] buildCgiResponse - Custom status 404" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildCgiResponse - Status 404 attendu, reçu " << response.getStatusCode() << std::endl;
            testsFailed++;
        }
    }
    
    // Test 13: buildCgiResponse - Location redirect (302)
    {
        HttpRequest request;
        std::string headers = "Location: /redirect\r\nContent-Type: text/html";
        std::string body = "";
        
        HttpResponse response = router.buildCgiResponse(request, headers, body);
        
        if (response.getStatusCode() == 302) {
            std::cout << "[OK] buildCgiResponse - Location redirect (302)" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildCgiResponse - Status 302 attendu, reçu " << response.getStatusCode() << std::endl;
            testsFailed++;
        }
    }
    
    // Test 14: buildCgiResponse - Location avec Status 301
    {
        HttpRequest request;
        std::string headers = "Status: 301 Moved Permanently\r\nLocation: /new-location";
        std::string body = "";
        
        HttpResponse response = router.buildCgiResponse(request, headers, body);
        
        if (response.getStatusCode() == 301) {
            std::cout << "[OK] buildCgiResponse - Location avec Status 301" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] buildCgiResponse - Status 301 attendu, reçu " << response.getStatusCode() << std::endl;
            testsFailed++;
        }
    }
    
    // Test 15: validateCgiRequest - Extension non supportée
    {
        HttpRequest request;
        LocationConfig location;
        std::string scriptPath = "./www/cgi-bin/hello.php";
        std::string cgiPath;
        
        request.setUri("/cgi-bin/script.php");
        // Location n'a pas de CGI configuré
        
        bool result = router.validateCgiRequest(request.getUri(), scriptPath, &location, cgiPath);
        
        if (!result) {
            std::cout << "[OK] validateCgiRequest - Extension non supportée" << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] validateCgiRequest - Devrait échouer sans CGI configuré" << std::endl;
            testsFailed++;
        }
    }
    
    std::cout << "\n========== Résumé des tests ==========" << std::endl;
    std::cout << "Tests réussis : " << testsPassed << std::endl;
    std::cout << "Tests échoués : " << testsFailed << std::endl;
    
    if (testsFailed == 0)
        std::cout << "\n✅ Tous les tests CGI passent!" << std::endl;
    else
        std::cout << "\n❌ Certains tests ont échoué" << std::endl;
    
    return (testsFailed > 0) ? 1 : 0;
}
