/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:48 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/15 15:08:41 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

Router::Router() : p_root("./www")
{
}

Router::Router(const std::string &root) : p_root(root)
{
}

Router::~Router()
{
}

HttpResponse	Router::route(const HttpRequest &request)
{
	// Récupérer la méthode et l'URI de la requête
	std::string	method = request.getMethod();
	std::string	uri = request.getUri();

	if (method == "GET")
	{
		if (uri == "/")
			return handleHomePage(request);
		else if (uri == "/about")
			return handleAboutPage(request);
		else if (uri == "/index.html")
			return handleHomePage(request); // Rediriger vers la page d'accueil

		return serveStaticFile(request);  // ← Essayer de servir un fichier
	}
	else if (method == "POST")
	{
		return handlePost(request);
	}
	else if (method == "DELETE")
	{
		return handleDelete(request);
	}
	else
		return handleMethodNotAllowed(request);
}
