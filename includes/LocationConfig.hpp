/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:16:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 16:37:32 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>

class LocationConfig
{
public:

	LocationConfig();
	LocationConfig(const std::string &path); // Avec chemin de la location
	~LocationConfig();

	// Getters
	std::string					getPath() const;
	std::vector<std::string>	getAllowedMethods() const;

	// Setters
	void						setPath(const std::string &path);
	void						setAllowedMethods(const std::string &method);

	// Checkers
	bool						isMethodAllowed(const std::string &method) const;

private:

	std::string					p_path;				// Chemin de la location
	std::vector<std::string>	p_allowedMethods;	// Méthodes autorisées
};

#endif	// LOCATIONCONFIG_HPP
