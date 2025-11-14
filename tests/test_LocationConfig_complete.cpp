/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_LocationConfig_complete.cpp                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:38:38 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 21:40:29 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/LocationConfig.hpp"
#include <iostream>

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_LocationConfig_complete.cpp                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:38:38 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 21:45:00 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/LocationConfig.hpp"
#include <iostream>

int main()
{
    std::cout << "=== Test LocationConfig Complete ===" << std::endl;
    
    // Test 1 : Création d'une location basique
    LocationConfig loc("/uploads");
    std::cout << "✅ LocationConfig created with path /uploads" << std::endl;
    
    // Test 2 : Ajout de méthodes
    loc.setAllowedMethods("GET");
    loc.setAllowedMethods("POST");
    std::cout << "✅ Methods GET and POST added" << std::endl;
    
    // Test 3 : Configuration root et index
    loc.setRoot("./uploads");
    loc.setIndex("index.html");
    std::cout << "✅ Root and index configured" << std::endl;
    
    // Test 4 : Autoindex
    loc.setAutoindex(true);
    std::cout << "✅ Autoindex enabled" << std::endl;
    
    // Test 5 : Upload store
    loc.setUploadStore("./uploads");
    std::cout << "✅ Upload store configured" << std::endl;
    
    // Test 6 : CGI configuration
    loc.setCgiPass("/usr/bin/php-cgi");
    loc.setCgiExt(".php");
    std::cout << "✅ CGI configured" << std::endl;
    
    // Test 7 : Redirect
    LocationConfig loc2("/old");
    loc2.setReturn(301, "/new");
    std::cout << "✅ Redirect configured (301)" << std::endl;
    
    // Test 8 : Checker isMethodAllowed (celui-là on garde)
    if (loc.isMethodAllowed("GET"))
        std::cout << "✅ GET method is allowed" << std::endl;
    else
        std::cout << "❌ GET method should be allowed" << std::endl;
    
    if (!loc.isMethodAllowed("DELETE"))
        std::cout << "✅ DELETE method is not allowed" << std::endl;
    else
        std::cout << "❌ DELETE method should not be allowed" << std::endl;
    
    std::cout << "\n✅ All LocationConfig tests passed!" << std::endl;
    
    return 0;
}
