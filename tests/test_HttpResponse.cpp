/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HttpResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/12 23:09:44 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_framework.hpp"
#include "HttpResponse.hpp"

// ============================================================================
// Test Suite 1: Basic Response Construction
// ============================================================================

void	testBasicResponse()
{
	std::cout << "\n=== Test Suite 1: Basic Response ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(200);
	response.setBody("Hello, World!");

	std::string result = response.toString();
	
	ASSERT_TRUE(result.find("HTTP/1.1 200 OK") != std::string::npos, 
		"Response contains status line");
	ASSERT_TRUE(result.find("Content-Length: 13") != std::string::npos, 
		"Content-Length is automatically set");
	ASSERT_TRUE(result.find("Content-Type: text/plain") != std::string::npos, 
		"Default Content-Type is text/plain");
	ASSERT_TRUE(result.find("Hello, World!") != std::string::npos, 
		"Response contains body");
}

void	testDefaultConstructor()
{
	std::cout << "\n=== Test Suite 2: Default Constructor ===" << std::endl;

	HttpResponse response;

	ASSERT_EQUAL(response.getStatusCode(), 200, "Default status is 200");
	ASSERT_EQUAL(response.getBody(), "", "Default body is empty");
	
	std::string result = response.toString();
	ASSERT_TRUE(result.find("HTTP/1.1 200 OK") != std::string::npos, 
		"Default version is HTTP/1.1");
}

// ============================================================================
// Test Suite 3: HTTP Version Management
// ============================================================================

void	testHttp10Response()
{
	std::cout << "\n=== Test Suite 3: HTTP/1.0 Response ===" << std::endl;

	HttpResponse response;
	response.setHttpVersion("HTTP/1.0");
	response.setStatusCode(200);
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("HTTP/1.0 200 OK") != std::string::npos, 
		"Response uses HTTP/1.0 when set");
}

void	testHttp11Response()
{
	std::cout << "\n=== Test Suite 4: HTTP/1.1 Response ===" << std::endl;

	HttpResponse response;
	response.setHttpVersion("HTTP/1.1");
	response.setStatusCode(200);
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("HTTP/1.1 200 OK") != std::string::npos, 
		"Response uses HTTP/1.1 when set");
}

// ============================================================================
// Test Suite 5: Status Codes - Success (2xx)
// ============================================================================

void	testStatusCode200()
{
	std::cout << "\n=== Test Suite 5: Status Code 200 OK ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(200);

	ASSERT_EQUAL(response.getStatusCode(), 200, "Status code is 200");
	ASSERT_TRUE(response.toString().find("200 OK") != std::string::npos, 
		"Status message is OK");
}

void	testStatusCode201()
{
	std::cout << "\n=== Test Suite 6: Status Code 201 Created ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(201);
	response.setBody("Resource created");

	ASSERT_EQUAL(response.getStatusCode(), 201, "Status code is 201");
	ASSERT_TRUE(response.toString().find("201 Created") != std::string::npos, 
		"Status message is Created");
}

void	testStatusCode204()
{
	std::cout << "\n=== Test Suite 7: Status Code 204 No Content ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(204);

	ASSERT_EQUAL(response.getStatusCode(), 204, "Status code is 204");
	ASSERT_TRUE(response.toString().find("204 No Content") != std::string::npos, 
		"Status message is No Content");
}

// ============================================================================
// Test Suite 8: Status Codes - Redirection (3xx)
// ============================================================================

void	testStatusCode301()
{
	std::cout << "\n=== Test Suite 8: Status Code 301 Moved Permanently ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(301);
	response.setHeader("Location", "/new-url");

	ASSERT_EQUAL(response.getStatusCode(), 301, "Status code is 301");
	ASSERT_TRUE(response.toString().find("301 Moved Permanently") != std::string::npos, 
		"Status message is Moved Permanently");
	ASSERT_TRUE(response.toString().find("Location: /new-url") != std::string::npos, 
		"Location header is set");
}

void	testStatusCode302()
{
	std::cout << "\n=== Test Suite 9: Status Code 302 Found ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(302);
	response.setHeader("Location", "/temporary-url");

	ASSERT_EQUAL(response.getStatusCode(), 302, "Status code is 302");
	ASSERT_TRUE(response.toString().find("302 Found") != std::string::npos, 
		"Status message is Found");
}

void	testStatusCode304()
{
	std::cout << "\n=== Test Suite 10: Status Code 304 Not Modified ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(304);

	ASSERT_EQUAL(response.getStatusCode(), 304, "Status code is 304");
	ASSERT_TRUE(response.toString().find("304 Not Modified") != std::string::npos, 
		"Status message is Not Modified");
}

// ============================================================================
// Test Suite 11: Status Codes - Client Error (4xx)
// ============================================================================

void	testStatusCode400()
{
	std::cout << "\n=== Test Suite 11: Status Code 400 Bad Request ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(400);
	response.setBody("Bad Request");

	ASSERT_EQUAL(response.getStatusCode(), 400, "Status code is 400");
	ASSERT_TRUE(response.toString().find("400 Bad Request") != std::string::npos, 
		"Status message is Bad Request");
}

void	testStatusCode403()
{
	std::cout << "\n=== Test Suite 12: Status Code 403 Forbidden ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(403);
	response.setBody("Forbidden");

	ASSERT_EQUAL(response.getStatusCode(), 403, "Status code is 403");
	ASSERT_TRUE(response.toString().find("403 Forbidden") != std::string::npos, 
		"Status message is Forbidden");
}

void	testStatusCode404()
{
	std::cout << "\n=== Test Suite 13: Status Code 404 Not Found ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(404);
	response.setBody("Not Found");

	ASSERT_EQUAL(response.getStatusCode(), 404, "Status code is 404");
	ASSERT_TRUE(response.toString().find("404 Not Found") != std::string::npos, 
		"Status message is Not Found");
}

void	testStatusCode405()
{
	std::cout << "\n=== Test Suite 14: Status Code 405 Method Not Allowed ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(405);
	response.setHeader("Allow", "GET, POST");
	response.setBody("Method Not Allowed");

	ASSERT_EQUAL(response.getStatusCode(), 405, "Status code is 405");
	ASSERT_TRUE(response.toString().find("405 Method Not Allowed") != std::string::npos, 
		"Status message is Method Not Allowed");
	ASSERT_TRUE(response.toString().find("Allow: GET, POST") != std::string::npos, 
		"Allow header is set");
}

void	testStatusCode413()
{
	std::cout << "\n=== Test Suite 15: Status Code 413 Payload Too Large ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(413);
	response.setBody("Payload Too Large");

	ASSERT_EQUAL(response.getStatusCode(), 413, "Status code is 413");
	ASSERT_TRUE(response.toString().find("413 Payload Too Large") != std::string::npos, 
		"Status message is Payload Too Large");
}

void	testStatusCode414()
{
	std::cout << "\n=== Test Suite 16: Status Code 414 URI Too Long ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(414);
	response.setBody("URI Too Long");

	ASSERT_EQUAL(response.getStatusCode(), 414, "Status code is 414");
	ASSERT_TRUE(response.toString().find("414 URI Too Long") != std::string::npos, 
		"Status message is URI Too Long");
}

// ============================================================================
// Test Suite 17: Status Codes - Server Error (5xx)
// ============================================================================

void	testStatusCode500()
{
	std::cout << "\n=== Test Suite 17: Status Code 500 Internal Server Error ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(500);
	response.setBody("Internal Server Error");

	ASSERT_EQUAL(response.getStatusCode(), 500, "Status code is 500");
	ASSERT_TRUE(response.toString().find("500 Internal Server Error") != std::string::npos, 
		"Status message is Internal Server Error");
}

void	testStatusCode501()
{
	std::cout << "\n=== Test Suite 18: Status Code 501 Not Implemented ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(501);
	response.setBody("Not Implemented");

	ASSERT_EQUAL(response.getStatusCode(), 501, "Status code is 501");
	ASSERT_TRUE(response.toString().find("501 Not Implemented") != std::string::npos, 
		"Status message is Not Implemented");
}

void	testStatusCode503()
{
	std::cout << "\n=== Test Suite 19: Status Code 503 Service Unavailable ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(503);
	response.setBody("Service Unavailable");

	ASSERT_EQUAL(response.getStatusCode(), 503, "Status code is 503");
	ASSERT_TRUE(response.toString().find("503 Service Unavailable") != std::string::npos, 
		"Status message is Service Unavailable");
}

void	testStatusCode505()
{
	std::cout << "\n=== Test Suite 20: Status Code 505 HTTP Version Not Supported ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(505);
	response.setBody("HTTP Version Not Supported");

	ASSERT_EQUAL(response.getStatusCode(), 505, "Status code is 505");
	ASSERT_TRUE(response.toString().find("505 HTTP Version Not Supported") != std::string::npos, 
		"Status message is HTTP Version Not Supported");
}

// ============================================================================
// Test Suite 21: Headers Management
// ============================================================================

void	testCustomHeaders()
{
	std::cout << "\n=== Test Suite 21: Custom Headers ===" << std::endl;

	HttpResponse response;
	response.setHeader("X-Custom-Header", "custom-value");
	response.setHeader("X-Another-Header", "another-value");
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("X-Custom-Header: custom-value") != std::string::npos, 
		"Custom header is present");
	ASSERT_TRUE(result.find("X-Another-Header: another-value") != std::string::npos, 
		"Another custom header is present");
}

void	testContentTypeOverride()
{
	std::cout << "\n=== Test Suite 22: Content-Type Override ===" << std::endl;

	HttpResponse response;
	response.setHeader("Content-Type", "application/json");
	response.setBody("{\"status\":\"ok\"}");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Content-Type: application/json") != std::string::npos, 
		"Content-Type is overridden");
	ASSERT_TRUE(result.find("text/plain") == std::string::npos, 
		"Default Content-Type is not present");
}

void	testMultipleHeaders()
{
	std::cout << "\n=== Test Suite 23: Multiple Headers ===" << std::endl;

	HttpResponse response;
	response.setHeader("Server", "WebServ/1.0");
	response.setHeader("Cache-Control", "no-cache");
	response.setHeader("X-Frame-Options", "DENY");
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Server: WebServ/1.0") != std::string::npos, 
		"Server header is present");
	ASSERT_TRUE(result.find("Cache-Control: no-cache") != std::string::npos, 
		"Cache-Control header is present");
	ASSERT_TRUE(result.find("X-Frame-Options: DENY") != std::string::npos, 
		"X-Frame-Options header is present");
}

// ============================================================================
// Test Suite 24: Connection Header Management
// ============================================================================

void	testConnectionClose()
{
	std::cout << "\n=== Test Suite 24: Connection: close ===" << std::endl;

	HttpResponse response;
	response.setHeader("Connection", "close");
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Connection: close") != std::string::npos, 
		"Connection: close header is set");
}

void	testConnectionKeepAlive()
{
	std::cout << "\n=== Test Suite 25: Connection: keep-alive ===" << std::endl;

	HttpResponse response;
	response.setHeader("Connection", "keep-alive");
	response.setBody("Test");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Connection: keep-alive") != std::string::npos, 
		"Connection: keep-alive header is set");
}

// ============================================================================
// Test Suite 26: Body Management
// ============================================================================

void	testEmptyBody()
{
	std::cout << "\n=== Test Suite 26: Empty Body ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(204);

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Content-Length: 0") != std::string::npos, 
		"Content-Length is 0 for empty body");
}

void	testLargeBody()
{
	std::cout << "\n=== Test Suite 27: Large Body ===" << std::endl;

	HttpResponse response;
	std::string largeBody(10000, 'X');
	response.setBody(largeBody);

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Content-Length: 10000") != std::string::npos, 
		"Content-Length is correctly set for large body");
	ASSERT_EQUAL(response.getBody().length(), static_cast<size_t>(10000), 
		"Body length is correct");
}

void	testBodyWithSpecialCharacters()
{
	std::cout << "\n=== Test Suite 28: Body with Special Characters ===" << std::endl;

	HttpResponse response;
	response.setBody("Hello\r\nWorld\n\tTest");

	std::string result = response.toString();
	ASSERT_TRUE(result.find("Hello\r\nWorld\n\tTest") != std::string::npos, 
		"Body with special characters is preserved");
}

// ============================================================================
// Test Suite 29: Response Format Validation
// ============================================================================

void	testResponseFormat()
{
	std::cout << "\n=== Test Suite 29: Response Format Validation ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(200);
	response.setHeader("Content-Type", "text/html");
	response.setBody("<html><body>Hello</body></html>");

	std::string result = response.toString();
	
	// Vérifier le format général: Status Line, Headers, Blank Line, Body
	ASSERT_TRUE(result.find("HTTP/1.1 200 OK\r\n") != std::string::npos, 
		"Status line ends with CRLF");
	ASSERT_TRUE(result.find("\r\n\r\n") != std::string::npos, 
		"Headers and body separated by blank line");
	ASSERT_TRUE(result.find("<html><body>Hello</body></html>") != std::string::npos, 
		"Body is present at the end");
}

void	testResponseHeaderOrder()
{
	std::cout << "\n=== Test Suite 30: Response Headers Present ===" << std::endl;

	HttpResponse response;
	response.setStatusCode(200);
	response.setHeader("Server", "WebServ/1.0");
	response.setHeader("Content-Type", "text/plain");
	response.setBody("Test");

	std::string result = response.toString();
	
	// Vérifier que tous les headers sont présents (ordre peut varier avec map)
	ASSERT_TRUE(result.find("Server: WebServ/1.0") != std::string::npos, 
		"Server header is present");
	ASSERT_TRUE(result.find("Content-Type: text/plain") != std::string::npos, 
		"Content-Type header is present");
	ASSERT_TRUE(result.find("Content-Length:") != std::string::npos, 
		"Content-Length header is present");
}

// ============================================================================
// MAIN
// ============================================================================

int	main()
{
	std::cout << "========================================" << std::endl;
	std::cout << "Starting HttpResponse Unit Tests..." << std::endl;
	std::cout << "========================================" << std::endl;

	// Basic Construction
	testBasicResponse();
	testDefaultConstructor();

	// HTTP Version
	testHttp10Response();
	testHttp11Response();

	// Status Codes - Success (2xx)
	testStatusCode200();
	testStatusCode201();
	testStatusCode204();

	// Status Codes - Redirection (3xx)
	testStatusCode301();
	testStatusCode302();
	testStatusCode304();

	// Status Codes - Client Error (4xx)
	testStatusCode400();
	testStatusCode403();
	testStatusCode404();
	testStatusCode405();
	testStatusCode413();
	testStatusCode414();

	// Status Codes - Server Error (5xx)
	testStatusCode500();
	testStatusCode501();
	testStatusCode503();
	testStatusCode505();

	// Headers Management
	testCustomHeaders();
	testContentTypeOverride();
	testMultipleHeaders();

	// Connection Management
	testConnectionClose();
	testConnectionKeepAlive();

	// Body Management
	testEmptyBody();
	testLargeBody();
	testBodyWithSpecialCharacters();

	// Response Format
	testResponseFormat();
	testResponseHeaderOrder();

	std::cout << "\n========================================" << std::endl;
	return printTestSummary();
}
