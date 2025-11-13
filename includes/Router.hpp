/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:56:45 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/13 01:20:48 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class Router
{
public:
	Router();
	~Router();

	// Méthode principale de routing
	HttpResponse		route(const HttpRequest& request);
	
	// Générer une réponse d'erreur sans requête valide
	HttpResponse		createErrorResponse(int statusCode, const std::string& httpVersion);

private:
	// Handlers pour les différentes routes
	HttpResponse		handleHomePage(const HttpRequest& request);
	HttpResponse		handleAboutPage(const HttpRequest& request);
	HttpResponse		handleNotFound(const HttpRequest& request);
	HttpResponse		handleMethodNotAllowed(const HttpRequest& request);

	// Pages HTML - Succès (2xx)
	std::string			getPage_200() const;
	std::string			getPage_201() const;
	std::string			getPage_204() const;

	// Pages HTML - Redirections (3xx)
	std::string			getPage_301() const;
	std::string			getPage_302() const;
	std::string			getPage_303() const;
	std::string			getPage_304() const;
	std::string			getPage_307() const;
	std::string			getPage_308() const;

	// Pages HTML - Erreurs client (4xx)
	std::string			getPage_400() const;
	std::string			getPage_401() const;
	std::string			getPage_403() const;
	std::string			getPage_404() const;
	std::string			getPage_405() const;
	std::string			getPage_408() const;
	std::string			getPage_413() const;
	std::string			getPage_414() const;
	std::string			getPage_415() const;
	std::string			getPage_431() const;

	// Pages HTML - Erreurs serveur (5xx)
	std::string			getPage_500() const;
	std::string			getPage_501() const;
	std::string			getPage_502() const;
	std::string			getPage_503() const;
	std::string			getPage_504() const;
	std::string			getPage_505() const;

	// Pages de contenu (home, about)
	std::string			getPage_home() const;
	std::string			getPage_about() const;
};

#endif	// ROUTER_HPP
