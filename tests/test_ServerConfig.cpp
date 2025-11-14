/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ServerConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 18:43:03 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 18:43:11 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerConfig.hpp"
#include <iostream>

int main()
{
    std::cout << "=== Test ServerConfig ===" << std::endl;
    
    // Test 1 : Constructeur par défaut
    ServerConfig server;
    std::cout << "Port par défaut: " << server.getListen() << std::endl;  // 8080
    std::cout << "Server name: " << server.getServerName() << std::endl;  // localhost
    std::cout << "Root: " << server.getRoot() << std::endl;               // ./www
    
    // Test 2 : Setters
    server.setListen(3000);
    server.setServerName("example.com");
    server.setRoot("./public");
    
    std::cout << "\nAprès setters:" << std::endl;
    std::cout << "Port: " << server.getListen() << std::endl;             // 3000
    std::cout << "Server name: " << server.getServerName() << std::endl;  // example.com
    
    // Test 3 : Error pages
    server.addErrorPage(404, "/errors/404.html");
    server.addErrorPage(500, "/errors/50x.html");
    server.addErrorPage(502, "/errors/50x.html");
    
    std::cout << "\nError pages:" << std::endl;
    std::cout << "404: " << server.getErrorPage(404) << std::endl;  // /errors/404.html
    std::cout << "500: " << server.getErrorPage(500) << std::endl;  // /errors/50x.html
    std::cout << "403: " << server.getErrorPage(403) << std::endl;  // (vide)
    
    // Test 4 : Locations
    LocationConfig loc1("/");
    loc1.setAllowedMethods("GET");
    loc1.setAllowedMethods("POST");
    
    LocationConfig loc2("/api");
    loc2.setAllowedMethods("GET");
    
    LocationConfig loc3("/api/v2");
    loc3.setAllowedMethods("GET");
    loc3.setAllowedMethods("DELETE");
    
    server.addLocation(loc1);
    server.addLocation(loc2);
    server.addLocation(loc3);
    
    // Test 5 : findLocation (longest prefix match)
    const LocationConfig* found1 = server.findLocation("/api/v2/users");
    const LocationConfig* found2 = server.findLocation("/api/test");
    const LocationConfig* found3 = server.findLocation("/home");
    
    std::cout << "\nLongest prefix match:" << std::endl;
    if (found1)
        std::cout << "/api/v2/users → " << found1->getPath() << std::endl;  // /api/v2
    if (found2)
        std::cout << "/api/test → " << found2->getPath() << std::endl;      // /api
    if (found3)
        std::cout << "/home → " << found3->getPath() << std::endl;          // /
    
    // Test 6 : Vérifier les méthodes de la location trouvée
    if (found1 && found1->isMethodAllowed("DELETE"))
        std::cout << "\nDELETE autorisé sur /api/v2 ✅" << std::endl;
    
    return 0;
}
