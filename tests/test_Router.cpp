/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Router.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:00:00 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 16:55:26 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_framework.hpp"
#include "Router.hpp"
#include "HttpRequest.hpp"

// ============================================================================
// Test Suite 1: Basic Routing
// ============================================================================

void	testRouteHomePage()
{
	std::cout << "\n=== Test Suite 1: Route Home Page ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 1.1: Status code is 200
	ASSERT_EQUAL(200, response.getStatusCode(), "Status code is 200 OK");

	// Test 1.2: Content-Type is text/html
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/html", contentType, "Content-Type is text/html");

	// Test 1.3: Body contains home page content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("Welcome") != std::string::npos, "Body contains 'Welcome'");
	ASSERT_TRUE(body.find("Home") != std::string::npos, "Body contains 'Home'");

	// Test 1.4: HTTP version matches request
	ASSERT_EQUAL("HTTP/1.1", response.getHttpVersion(), "HTTP version is HTTP/1.1");

	// Test 1.5: Connection header is keep-alive (HTTP/1.1 default)
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("keep-alive", connection, "Connection is keep-alive");
}

void	testRouteIndexHtml()
{
	std::cout << "\n=== Test Suite 2: Route /index.html redirects to / ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 2.1: Status code is 200 (same as home)
	ASSERT_EQUAL(200, response.getStatusCode(), "Status code is 200 OK");

	// Test 2.2: Body contains home page content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("Welcome") != std::string::npos, "Body contains 'Welcome'");
}

void	testRouteAboutPage()
{
	std::cout << "\n=== Test Suite 3: Route About Page ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /about HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 3.1: Status code is 200
	ASSERT_EQUAL(200, response.getStatusCode(), "Status code is 200 OK");

	// Test 3.2: Content-Type is text/html
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("text/html", response.getHeader("Content-Type"), "Content-Type is text/html");

	// Test 3.3: Body contains about page content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("About") != std::string::npos, "Body contains 'About'");
}

void	testRoute404NotFound()
{
	std::cout << "\n=== Test Suite 4: Route 404 Not Found ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /nonexistent HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 4.1: Status code is 404
	ASSERT_EQUAL(404, response.getStatusCode(), "Status code is 404 Not Found");

	// Test 4.2: Content-Type is text/html
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("text/html", response.getHeader("Content-Type"), "Content-Type is text/html");

	// Test 4.3: Body contains 404 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("404") != std::string::npos, "Body contains '404'");
	ASSERT_TRUE(body.find("Not Found") != std::string::npos, "Body contains 'Not Found'");

	// Test 4.4: Connection header is keep-alive (404 doesn't force close)
	ASSERT_EQUAL("keep-alive", connection, "Connection is keep-alive for 404");
}

void	testMethodNotAllowed()
{
	std::cout << "\n=== Test Suite 5: Method Not Allowed (405) ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 5.1: Status code is 405
	ASSERT_EQUAL(405, response.getStatusCode(), "Status code is 405 Method Not Allowed");

	// Test 5.2: Body contains 405 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("405") != std::string::npos, "Body contains '405'");
	ASSERT_TRUE(body.find("Method Not Allowed") != std::string::npos, "Body contains 'Method Not Allowed'");

	// Test 5.3: Connection header is keep-alive (405 doesn't force close)
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("keep-alive", connection, "Connection is keep-alive for 405");
}

// ============================================================================
// Test Suite 6: Error Response Creation
// ============================================================================

void	testCreateErrorResponse400()
{
	std::cout << "\n=== Test Suite 6: Create Error Response 400 ===" << std::endl;

	Router router;

	// Test 6.1: HTTP/1.1 400 forces Connection: close
	HttpResponse response = router.createErrorResponse(400, "HTTP/1.1");
	ASSERT_EQUAL(400, response.getStatusCode(), "Status code is 400");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 400 (fatal error)");

	// Test 6.2: Body contains 400 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("400") != std::string::npos, "Body contains '400'");
	ASSERT_TRUE(body.find("Bad Request") != std::string::npos, "Body contains 'Bad Request'");
}

void	testCreateErrorResponse404()
{
	std::cout << "\n=== Test Suite 7: Create Error Response 404 ===" << std::endl;

	Router router;

	// Test 7.1: HTTP/1.1 404 keeps connection alive
	HttpResponse response = router.createErrorResponse(404, "HTTP/1.1");
	ASSERT_EQUAL(404, response.getStatusCode(), "Status code is 404");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("keep-alive", connection, "Connection is keep-alive for 404");

	// Test 7.2: Body contains 404 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("404") != std::string::npos, "Body contains '404'");
}

void	testCreateErrorResponse500()
{
	std::cout << "\n=== Test Suite 8: Create Error Response 500 ===" << std::endl;

	Router router;

	// Test 8.1: HTTP/1.1 500 keeps connection alive
	HttpResponse response = router.createErrorResponse(500, "HTTP/1.1");
	ASSERT_EQUAL(500, response.getStatusCode(), "Status code is 500");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("keep-alive", connection, "Connection is keep-alive for 500");

	// Test 8.2: Body contains 500 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("500") != std::string::npos, "Body contains '500'");
	ASSERT_TRUE(body.find("Internal Server Error") != std::string::npos, "Body contains 'Internal Server Error'");
}

void	testCreateErrorResponse408()
{
	std::cout << "\n=== Test Suite 9: Create Error Response 408 ===" << std::endl;

	Router router;

	// Test 9.1: 408 forces Connection: close
	HttpResponse response = router.createErrorResponse(408, "HTTP/1.1");
	ASSERT_EQUAL(408, response.getStatusCode(), "Status code is 408");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 408 (timeout)");

	// Test 9.2: Body contains 408 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("408") != std::string::npos, "Body contains '408'");
}

void	testCreateErrorResponse413()
{
	std::cout << "\n=== Test Suite 10: Create Error Response 413 ===" << std::endl;

	Router router;

	// Test 10.1: 413 forces Connection: close
	HttpResponse response = router.createErrorResponse(413, "HTTP/1.1");
	ASSERT_EQUAL(413, response.getStatusCode(), "Status code is 413");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 413 (payload too large)");

	// Test 10.2: Body contains 413 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("413") != std::string::npos, "Body contains '413'");
}

void	testCreateErrorResponse414()
{
	std::cout << "\n=== Test Suite 11: Create Error Response 414 ===" << std::endl;

	Router router;

	// Test 11.1: 414 forces Connection: close
	HttpResponse response = router.createErrorResponse(414, "HTTP/1.1");
	ASSERT_EQUAL(414, response.getStatusCode(), "Status code is 414");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 414 (URI too long)");

	// Test 11.2: Body contains 414 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("414") != std::string::npos, "Body contains '414'");
}

void	testCreateErrorResponse431()
{
	std::cout << "\n=== Test Suite 12: Create Error Response 431 ===" << std::endl;

	Router router;

	// Test 12.1: 431 forces Connection: close
	HttpResponse response = router.createErrorResponse(431, "HTTP/1.1");
	ASSERT_EQUAL(431, response.getStatusCode(), "Status code is 431");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 431 (headers too large)");

	// Test 12.2: Body contains 431 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("431") != std::string::npos, "Body contains '431'");
}

void	testCreateErrorResponse505()
{
	std::cout << "\n=== Test Suite 13: Create Error Response 505 ===" << std::endl;

	Router router;

	// Test 13.1: 505 forces Connection: close
	HttpResponse response = router.createErrorResponse(505, "HTTP/1.1");
	ASSERT_EQUAL(505, response.getStatusCode(), "Status code is 505");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Connection is close for 505 (version not supported)");

	// Test 13.2: Body contains 505 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("505") != std::string::npos, "Body contains '505'");
}

// ============================================================================
// Test Suite 14: HTTP/1.0 Connection Behavior
// ============================================================================

void	testHttp10ConnectionClose()
{
	std::cout << "\n=== Test Suite 14: HTTP/1.0 Connection Close ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET / HTTP/1.0\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 14.1: HTTP/1.0 forces Connection: close
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "HTTP/1.0 forces Connection: close");

	// Test 14.2: HTTP version matches request
	ASSERT_EQUAL("HTTP/1.0", response.getHttpVersion(), "HTTP version is HTTP/1.0");
}

void	testHttp10ErrorResponse()
{
	std::cout << "\n=== Test Suite 15: HTTP/1.0 Error Response ===" << std::endl;

	Router router;

	// Test 15.1: HTTP/1.0 404 also closes connection
	HttpResponse response = router.createErrorResponse(404, "HTTP/1.0");
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "HTTP/1.0 forces close even for 404");
	ASSERT_EQUAL("HTTP/1.0", response.getHttpVersion(), "HTTP version is HTTP/1.0");
}

// ============================================================================
// Test Suite 16: Client-Requested Connection Close
// ============================================================================

void	testClientRequestsConnectionClose()
{
	std::cout << "\n=== Test Suite 16: Client Requests Connection Close ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 16.1: Client's Connection: close is honored
	std::string connection = response.getHeader("Connection");
	ASSERT_EQUAL("close", connection, "Client's Connection: close is honored");
}

// ============================================================================
// Test Suite 17: Static File Serving - Index
// ============================================================================

void	testServeStaticFile_Index()
{
	std::cout << "\n=== Test Suite 17: Static File Serving - Index ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 17.1: Status is 200 OK
	ASSERT_EQUAL(200, response.getStatusCode(), "Index returns 200 OK");

	// Test 17.2: Content-Type is text/html
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/html", contentType, "Index has text/html Content-Type");

	// Test 17.3: Body contains expected content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("<!DOCTYPE html>") != std::string::npos, "Index contains DOCTYPE");
	ASSERT_TRUE(body.find("<title>Webserv - Home</title>") != std::string::npos, "Index contains title");
	ASSERT_TRUE(body.find("Welcome to Webserv") != std::string::npos, "Index contains welcome message");
}

// ============================================================================
// Test Suite 18: Static File Serving - About HTML
// ============================================================================

void	testServeStaticFile_AboutHtml()
{
	std::cout << "\n=== Test Suite 18: Static File Serving - About HTML ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /about.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 18.1: Status is 200 OK
	ASSERT_EQUAL(200, response.getStatusCode(), "About page returns 200 OK");

	// Test 18.2: Content-Type is text/html
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/html", contentType, "About page has text/html Content-Type");

	// Test 18.3: Body contains about page content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("About Webserv") != std::string::npos, "About page contains title");
	ASSERT_TRUE(body.find("RFC 7230") != std::string::npos, "About page mentions RFC 7230");
}

// ============================================================================
// Test Suite 19: Static File Serving - CSS File
// ============================================================================

void	testServeStaticFile_CSS()
{
	std::cout << "\n=== Test Suite 19: Static File Serving - CSS File ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /css/style.css HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 19.1: Status is 200 OK
	ASSERT_EQUAL(200, response.getStatusCode(), "CSS file returns 200 OK");

	// Test 19.2: Content-Type is text/css
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/css", contentType, "CSS file has text/css Content-Type");

	// Test 19.3: Body contains CSS content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("body {") != std::string::npos, "CSS contains body selector");
	ASSERT_TRUE(body.find("background:") != std::string::npos, "CSS contains background property");
}

// ============================================================================
// Test Suite 20: Static File Serving - Test HTML
// ============================================================================

void	testServeStaticFile_TestHtml()
{
	std::cout << "\n=== Test Suite 20: Static File Serving - Test HTML ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /test.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 20.1: Status is 200 OK
	ASSERT_EQUAL(200, response.getStatusCode(), "Test page returns 200 OK");

	// Test 20.2: Content-Type is text/html
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/html", contentType, "Test page has text/html Content-Type");

	// Test 20.3: Body contains test page content
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("Test Page") != std::string::npos, "Test page contains title");
}

// ============================================================================
// Test Suite 21: Static File Serving - 404 Not Found
// ============================================================================

void	testServeStaticFile_NotFound()
{
	std::cout << "\n=== Test Suite 21: Static File Serving - 404 Not Found ===" << std::endl;

	Router router;
	HttpRequest request;
	std::string requestStr = "GET /nonexistent.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request.appendData(requestStr);
	request.parse();

	HttpResponse response = router.route(request);

	// Test 21.1: Status is 404 Not Found
	ASSERT_EQUAL(404, response.getStatusCode(), "Nonexistent file returns 404");

	// Test 21.2: Content-Type is text/html
	std::string contentType = response.getHeader("Content-Type");
	ASSERT_EQUAL("text/html", contentType, "404 response has text/html Content-Type");

	// Test 21.3: Body contains 404 error page
	std::string body = response.getBody();
	ASSERT_TRUE(body.find("404") != std::string::npos, "404 response contains error code");
	ASSERT_TRUE(body.find("Not Found") != std::string::npos, "404 response contains error message");
}

// ============================================================================
// Test Suite 22: Static File Serving - Path Traversal Security
// ============================================================================

void	testServeStaticFile_PathTraversal()
{
	std::cout << "\n=== Test Suite 22: Static File Serving - Path Traversal Security ===" << std::endl;

	Router router;

	// Test 22.1: Block ../ attack
	try
	{
		HttpRequest request1;
		std::string requestStr1 = "GET /../../../etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n";
		request1.appendData(requestStr1);
		request1.parse();
		ASSERT_TRUE(false, "Path traversal should throw exception during parsing");
	}
	catch (const std::runtime_error &e)
	{
		ASSERT_TRUE(std::string(e.what()).find("Directory traversal") != std::string::npos,
			"Path traversal with ../ is blocked during parsing");
	}

	// Test 22.2: Block embedded ../ attack
	try
	{
		HttpRequest request2;
		std::string requestStr2 = "GET /css/../../../etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n";
		request2.appendData(requestStr2);
		request2.parse();
		ASSERT_TRUE(false, "Embedded path traversal should throw exception during parsing");
	}
	catch (const std::runtime_error &e)
	{
		ASSERT_TRUE(std::string(e.what()).find("Directory traversal") != std::string::npos,
			"Embedded path traversal is blocked during parsing");
	}

	// Test 22.3: Block ~ home directory attack (if URI starts with /~, it passes parsing but fails in serveStaticFile)
	HttpRequest request3;
	std::string requestStr3 = "GET /~user/file.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request3.appendData(requestStr3);
	request3.parse();
	HttpResponse response3 = router.route(request3);
	ASSERT_EQUAL(403, response3.getStatusCode(), "Home directory access returns 403");
}

// ============================================================================
// Test Suite 23: Static File Serving - Double Slash Security
// ============================================================================

void	testServeStaticFile_DoubleSlash()
{
	std::cout << "\n=== Test Suite 23: Static File Serving - Double Slash Security ===" << std::endl;

	Router router;

	// Test 23.1: Block double slash attack
	HttpRequest request1;
	std::string requestStr1 = "GET //index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request1.appendData(requestStr1);
	request1.parse();
	HttpResponse response1 = router.route(request1);
	ASSERT_EQUAL(403, response1.getStatusCode(), "Double slash returns 403");

	// Test 23.2: Block triple slash attack
	HttpRequest request2;
	std::string requestStr2 = "GET ///etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request2.appendData(requestStr2);
	request2.parse();
	HttpResponse response2 = router.route(request2);
	ASSERT_EQUAL(403, response2.getStatusCode(), "Triple slash returns 403");

	// Test 23.3: Block embedded double slash
	HttpRequest request3;
	std::string requestStr3 = "GET /css//style.css HTTP/1.1\r\nHost: localhost\r\n\r\n";
	request3.appendData(requestStr3);
	request3.parse();
	HttpResponse response3 = router.route(request3);
	ASSERT_EQUAL(403, response3.getStatusCode(), "Embedded double slash returns 403");
}

// ============================================================================
// MAIN
// ============================================================================

int	main()
{
	std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║             ROUTER UNIT TESTS                              ║" << std::endl;
	std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

	// Basic Routing
	testRouteHomePage();
	testRouteIndexHtml();
	testRouteAboutPage();
	testRoute404NotFound();
	testMethodNotAllowed();

	// Error Response Creation - Fatal Errors (force close)
	testCreateErrorResponse400();
	testCreateErrorResponse408();
	testCreateErrorResponse413();
	testCreateErrorResponse414();
	testCreateErrorResponse431();
	testCreateErrorResponse505();

	// Error Response Creation - Non-Fatal Errors (keep-alive)
	testCreateErrorResponse404();
	testCreateErrorResponse500();

	// HTTP/1.0 Behavior
	testHttp10ConnectionClose();
	testHttp10ErrorResponse();

	// Client Control
	testClientRequestsConnectionClose();

	// Static File Serving
	testServeStaticFile_Index();
	testServeStaticFile_AboutHtml();
	testServeStaticFile_CSS();
	testServeStaticFile_TestHtml();
	testServeStaticFile_NotFound();
	testServeStaticFile_PathTraversal();
	testServeStaticFile_DoubleSlash();

	printTestSummary();
	return 0;
}
