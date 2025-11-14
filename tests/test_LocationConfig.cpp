/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_LocationConfig.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:54:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 16:55:08 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/LocationConfig.hpp"
#include <iostream>

int main()
{
    std::cout << "=== Test LocationConfig ===" << std::endl;
    
    // Test 1 : Constructeur par défaut
    LocationConfig loc1;
    std::cout << "Chemin par défaut: " << loc1.getPath() << std::endl;  // "/"
    
    // Test 2 : Constructeur avec chemin
    LocationConfig loc2("/uploads");
    std::cout << "Chemin uploads: " << loc2.getPath() << std::endl;  // "/uploads"
    
    // Test 3 : Ajouter des méthodes
    loc2.setAllowedMethods("GET");
    loc2.setAllowedMethods("POST");
    
    // Test 4 : Vérifier les méthodes
    std::cout << "GET autorisé ? " << (loc2.isMethodAllowed("GET") ? "OUI" : "NON") << std::endl;   // OUI
    std::cout << "POST autorisé ? " << (loc2.isMethodAllowed("POST") ? "OUI" : "NON") << std::endl; // OUI
    std::cout << "DELETE autorisé ? " << (loc2.isMethodAllowed("DELETE") ? "OUI" : "NON") << std::endl; // NON
    
    // Test 5 : Afficher toutes les méthodes
    std::vector<std::string> methods = loc2.getAllowedMethods();
    std::cout << "Méthodes autorisées: ";
    for (size_t i = 0; i < methods.size(); ++i)
        std::cout << methods[i] << " ";
    std::cout << std::endl;
    
    return 0;
}
