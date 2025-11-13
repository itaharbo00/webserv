/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router_handlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 00:56:10 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 16:14:52 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

HttpResponse	Router::handleHomePage(const HttpRequest &request)
{
	HttpResponse	response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200);
	response.setHeader("Content-Type", "text/html");
	response.setBody(getPage_home());

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse	Router::handleAboutPage(const HttpRequest &request)
{
	HttpResponse	response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(200);
	response.setHeader("Content-Type", "text/html");
	response.setBody(getPage_about());

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse	Router::handleNotFound(const HttpRequest &request)
{
	HttpResponse	response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(404);
	response.setHeader("Content-Type", "text/html");
	response.setBody(getPage_404());

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}

HttpResponse	Router::handleMethodNotAllowed(const HttpRequest &request)
{
	HttpResponse	response;

	response.setHttpVersion(request.getHttpVersion());
	response.setStatusCode(405);
	response.setHeader("Content-Type", "text/html");
	response.setBody(getPage_405());

	if (request.shouldCloseConnection())
		response.setHeader("Connection", "close");
	else
		response.setHeader("Connection", "keep-alive");

	return response;
}
