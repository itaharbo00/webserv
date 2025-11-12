/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HttpRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 23:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/11 23:03:11 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_framework.hpp"
#include "HttpRequest.hpp"
#include <stdexcept>

// ============================================================================
// Test Suite 1: Basic GET Requests
// ============================================================================

void	testSimpleGET()
{
	std::cout << "\n=== Test Suite 1: Simple GET Request ===" << std::endl;

	HttpRequest req;
	std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";

	// Test 1.1: appendData et isComplete
	req.appendData(request);
	ASSERT_TRUE(req.isComplete(), "Request marked as complete");

	// Test 1.2: parse sans exception
	try
	{
		req.parse();
		ASSERT_TRUE(true, "Parse succeeds without exception");
	}
	catch (...)
	{
		ASSERT_TRUE(false, "Parse should not throw exception");
	}

	// Test 1.3: Vérification des valeurs parsées
	ASSERT_EQUAL("GET", req.getMethod(), "Method is GET");
	ASSERT_EQUAL("/", req.getUri(), "URI is /");
	ASSERT_EQUAL("HTTP/1.1", req.getHttpVersion(), "Version is HTTP/1.1");

	// Test 1.4: Header Host présent
	std::map<std::string, std::string> headers = req.getHeaders();
	ASSERT_TRUE(headers.count("Host") > 0, "Host header present");
	ASSERT_EQUAL("localhost", headers["Host"], "Host value is localhost");

	// Test 1.5: Body vide
	ASSERT_EQUAL("", req.getBody(), "Body is empty");
}

void	testGETWithPath()
{
	std::cout << "\n=== Test Suite 2: GET with Path ===" << std::endl;

	HttpRequest req;
	std::string request = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";

	req.appendData(request);
	ASSERT_TRUE(req.isComplete(), "Request complete");

	req.parse();
	ASSERT_EQUAL("GET", req.getMethod(), "Method is GET");
	ASSERT_EQUAL("/index.html", req.getUri(), "URI is /index.html");
	ASSERT_EQUAL("example.com", req.getHeaders()["Host"], "Host is example.com");
}

void	testGETWithQueryParams()
{
	std::cout << "\n=== Test Suite 3: GET with Query Parameters ===" << std::endl;

	HttpRequest req;
	std::string request = "GET /search?q=test&lang=fr HTTP/1.1\r\nHost: localhost\r\n\r\n";

	req.appendData(request);
	req.parse();

	ASSERT_EQUAL("GET", req.getMethod(), "Method is GET");
	ASSERT_EQUAL("/search?q=test&lang=fr", req.getUri(), "URI includes query params");
}

// ============================================================================
// Test Suite 4: POST Requests with Body
// ============================================================================

void	testPOSTWithContentLength()
{
	std::cout << "\n=== Test Suite 4: POST with Content-Length ===" << std::endl;

	HttpRequest req;
	std::string body = "name=John&age=30";
	std::ostringstream oss;
	oss << "POST /submit HTTP/1.1\r\n"
	    << "Host: localhost\r\n"
	    << "Content-Length: " << body.length() << "\r\n"
	    << "Content-Type: application/x-www-form-urlencoded\r\n"
	    << "\r\n"
	    << body;

	req.appendData(oss.str());
	ASSERT_TRUE(req.isComplete(), "Request complete");

	req.parse();
	ASSERT_EQUAL("POST", req.getMethod(), "Method is POST");
	ASSERT_EQUAL("/submit", req.getUri(), "URI is /submit");
	ASSERT_EQUAL(body, req.getBody(), "Body matches");

	std::map<std::string, std::string> headers = req.getHeaders();
	ASSERT_TRUE(headers.count("Content-Length") > 0, "Content-Length present");
	ASSERT_TRUE(headers.count("Content-Type") > 0, "Content-Type present");
}

void	testPOSTWithEmptyBody()
{
	std::cout << "\n=== Test Suite 5: POST with Empty Body ===" << std::endl;

	HttpRequest req;
	std::string request = "POST /submit HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "Content-Length: 0\r\n"
	                      "\r\n";

	req.appendData(request);
	req.parse();

	ASSERT_EQUAL("POST", req.getMethod(), "Method is POST");
	ASSERT_EQUAL("", req.getBody(), "Body is empty");
}

// ============================================================================
// Test Suite 6: DELETE Requests
// ============================================================================

void	testDELETE()
{
	std::cout << "\n=== Test Suite 6: DELETE Request ===" << std::endl;

	HttpRequest req;
	std::string request = "DELETE /resource/123 HTTP/1.1\r\nHost: localhost\r\n\r\n";

	req.appendData(request);
	req.parse();

	ASSERT_EQUAL("DELETE", req.getMethod(), "Method is DELETE");
	ASSERT_EQUAL("/resource/123", req.getUri(), "URI is /resource/123");
}

// ============================================================================
// Test Suite 7: Invalid Requests
// ============================================================================

void	testInvalidMethod()
{
	std::cout << "\n=== Test Suite 7: Invalid Method ===" << std::endl;

	HttpRequest req;
	std::string request = "INVALID / HTTP/1.1\r\nHost: localhost\r\n\r\n";

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for invalid method");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for invalid method");
	}
}

void	testMalformedRequest()
{
	std::cout << "\n=== Test Suite 8: Malformed Request ===" << std::endl;

	HttpRequest req;
	std::string request = "GET\r\nHost: localhost\r\n\r\n"; // Missing URI and version

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for malformed request");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for malformed request");
	}
}

void	testMissingHost()
{
	std::cout << "\n=== Test Suite 9: Missing Host Header ===" << std::endl;

	HttpRequest req;
	std::string request = "GET / HTTP/1.1\r\n\r\n"; // Missing Host

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for missing Host");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for missing Host");
	}
}

void	testURITooLong()
{
	std::cout << "\n=== Test Suite 10: URI Too Long ===" << std::endl;

	HttpRequest req;
	std::string longURI(2100, 'a'); // Plus de 2048 caractères
	std::string request = "GET /" + longURI + " HTTP/1.1\r\nHost: localhost\r\n\r\n";

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for URI too long");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for URI too long");
	}
}

void	testDirectoryTraversal()
{
	std::cout << "\n=== Test Suite 11: Directory Traversal ===" << std::endl;

	HttpRequest req;
	std::string request = "GET /../etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n";

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for directory traversal");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for directory traversal");
	}
}

// ============================================================================
// Test Suite 12: Large Request Protection
// ============================================================================

void	testRequestSizeLimit()
{
	std::cout << "\n=== Test Suite 12: Request Size Limit ===" << std::endl;

	HttpRequest req;
	std::string largeData(1024 * 1024 + 1, 'x'); // Plus de 1MB

	try
	{
		req.appendData(largeData);
		ASSERT_TRUE(false, "Should throw exception for request too large");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for request too large");
	}
}

void	testAcceptableSize()
{
	std::cout << "\n=== Test Suite 13: Acceptable Request Size ===" << std::endl;

	HttpRequest req;
	std::string largeBody(500 * 1024, 'x'); // 500KB (acceptable)
	std::ostringstream oss;
	oss << "POST /upload HTTP/1.1\r\n"
	    << "Host: localhost\r\n"
	    << "Content-Length: " << largeBody.length() << "\r\n"
	    << "\r\n"
	    << largeBody;

	try
	{
		req.appendData(oss.str());
		ASSERT_TRUE(req.isComplete(), "Large but acceptable request is complete");
	}
	catch (...)
	{
		ASSERT_TRUE(false, "Should not throw for acceptable size");
	}
}

// ============================================================================
// Test Suite 14: Chunked Transfer-Encoding
// ============================================================================

void	testChunkedEncoding()
{
	std::cout << "\n=== Test Suite 14: Chunked Transfer-Encoding ===" << std::endl;

	HttpRequest req;
	std::string request = "POST /upload HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "Transfer-Encoding: chunked\r\n"
	                      "\r\n"
	                      "5\r\n"
	                      "Hello\r\n"
	                      "6\r\n"
	                      " World\r\n"
	                      "0\r\n"
	                      "\r\n";

	req.appendData(request);
	req.parse();

	ASSERT_EQUAL("POST", req.getMethod(), "Method is POST");
	ASSERT_EQUAL("Hello World", req.getBody(), "Chunked body decoded correctly");
}

void	testAmbiguousTransferEncoding()
{
	std::cout << "\n=== Test Suite 15: Ambiguous Transfer-Encoding + Content-Length ===" << std::endl;

	HttpRequest req;
	std::string request = "POST /upload HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "Transfer-Encoding: chunked\r\n"
	                      "Content-Length: 10\r\n"
	                      "\r\n";

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for ambiguous headers");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for ambiguous headers (RFC 9112)");
	}
}

// ============================================================================
// Test Suite 16: Multiple Headers
// ============================================================================

void	testMultipleHeaders()
{
	std::cout << "\n=== Test Suite 16: Multiple Headers ===" << std::endl;

	HttpRequest req;
	std::string request = "GET / HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "User-Agent: TestClient/1.0\r\n"
	                      "Accept: text/html\r\n"
	                      "Accept-Language: en-US\r\n"
	                      "Connection: keep-alive\r\n"
	                      "\r\n";

	req.appendData(request);
	req.parse();

	std::map<std::string, std::string> headers = req.getHeaders();
	ASSERT_EQUAL("localhost", headers["Host"], "Host header");
	ASSERT_EQUAL("TestClient/1.0", headers["User-Agent"], "User-Agent header");
	ASSERT_EQUAL("text/html", headers["Accept"], "Accept header");
	ASSERT_EQUAL("en-US", headers["Accept-Language"], "Accept-Language header");
	ASSERT_EQUAL("keep-alive", headers["Connection"], "Connection header");
}

void	testDuplicateContentLength()
{
	std::cout << "\n=== Test Suite 17: Duplicate Content-Length ===" << std::endl;

	HttpRequest req;
	std::string request = "POST / HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "Content-Length: 10\r\n"
	                      "Content-Length: 20\r\n"
	                      "\r\n";

	req.appendData(request);

	try
	{
		req.parse();
		ASSERT_TRUE(false, "Should throw exception for duplicate Content-Length");
	}
	catch (const std::exception &e)
	{
		ASSERT_TRUE(true, "Throws exception for duplicate Content-Length");
	}
}

// ============================================================================
// Test Suite 18: Incremental Data
// ============================================================================

void	testIncrementalAppend()
{
	std::cout << "\n=== Test Suite 18: Incremental Data Append ===" << std::endl;

	HttpRequest req;

	// Test 18.1: Données incomplètes
	req.appendData("GET / HTTP/1.1\r\n");
	ASSERT_TRUE(!req.isComplete(), "Request not complete yet");

	// Test 18.2: Ajout de plus de données
	req.appendData("Host: localhost\r\n");
	ASSERT_TRUE(!req.isComplete(), "Still not complete");

	// Test 18.3: Complétion
	req.appendData("\r\n");
	ASSERT_TRUE(req.isComplete(), "Request complete after full data");

	// Test 18.4: Parse réussi
	req.parse();
	ASSERT_EQUAL("GET", req.getMethod(), "Method parsed correctly");
	ASSERT_EQUAL("localhost", req.getHeaders()["Host"], "Host parsed correctly");
}

// ============================================================================
// Test Suite 19: Edge Cases
// ============================================================================

void	testEmptyHeaderValue()
{
	std::cout << "\n=== Test Suite 19: Empty Header Value ===" << std::endl;

	HttpRequest req;
	std::string request = "GET / HTTP/1.1\r\n"
	                      "Host: localhost\r\n"
	                      "X-Empty:\r\n"
	                      "\r\n";

	req.appendData(request);
	req.parse();

	std::map<std::string, std::string> headers = req.getHeaders();
	ASSERT_TRUE(headers.count("X-Empty") > 0, "Empty header present");
	ASSERT_EQUAL("", headers["X-Empty"], "Empty header value is empty string");
}

void	testWhitespaceInHeaders()
{
	std::cout << "\n=== Test Suite 20: Whitespace in Headers ===" << std::endl;

	HttpRequest req;
	std::string request = "GET / HTTP/1.1\r\n"
	                      "Host:   localhost   \r\n"
	                      "User-Agent:   TestClient   \r\n"
	                      "\r\n";

	req.appendData(request);
	req.parse();

	std::map<std::string, std::string> headers = req.getHeaders();
	ASSERT_EQUAL("localhost", headers["Host"], "Host trimmed");
	ASSERT_EQUAL("TestClient", headers["User-Agent"], "User-Agent trimmed");
}

// ============================================================================
// MAIN
// ============================================================================

int	main()
{
	std::cout << "========================================" << std::endl;
	std::cout << "Starting HttpRequest Unit Tests..." << std::endl;
	std::cout << "========================================" << std::endl;

	// Basic GET Tests
	testSimpleGET();
	testGETWithPath();
	testGETWithQueryParams();

	// POST Tests
	testPOSTWithContentLength();
	testPOSTWithEmptyBody();

	// DELETE Tests
	testDELETE();

	// Invalid Request Tests
	testInvalidMethod();
	testMalformedRequest();
	testMissingHost();
	testURITooLong();
	testDirectoryTraversal();

	// Size Limit Tests
	testRequestSizeLimit();
	testAcceptableSize();

	// Chunked Encoding Tests
	testChunkedEncoding();
	testAmbiguousTransferEncoding();

	// Header Tests
	testMultipleHeaders();
	testDuplicateContentLength();

	// Incremental Data Tests
	testIncrementalAppend();

	// Edge Cases
	testEmptyHeaderValue();
	testWhitespaceInHeaders();

	std::cout << "\n========================================" << std::endl;
	return printTestSummary();
}
