/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:16:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/14 21:34:14 by itaharbo         ###   ########.fr       */
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
	void						setRoot(const std::string &root);
	void						setIndex(const std::string &index);
	void						setAutoindex(bool autoindex);
	void						setUploadStore(const std::string &uploadStore);
	void						setCgiPass(const std::string &cgiPath);
	void						setCgiExt(const std::string &cgiExt);
	void						setReturn(int code, const std::string &url);

	// Checkers
	bool						isMethodAllowed(const std::string &method) const;
	bool						hasRedirect() const;
	bool						hasCgi() const;
	bool						hasUpload() const;

private:

	std::string					p_path;				// Chemin de la location
	std::vector<std::string>	p_allowedMethods;	// Méthodes autorisées
	std::string					p_root;				// Racine pour cette location
	std::string					p_index;			// Fichier index pour cette location
	bool						p_autoindex;		// Autoindex activé ou non
	std::string					p_uploadStore;		// Chemin pour les uploads
	std::string					p_cgiPass;			// Chemin du binaire CGI
	std::string					p_cgiExt;			// Extension des fichiers CGI
	int							p_returnCode;		// Code de redirection
	std::string					p_returnUrl;		// URL de redirection
};

#endif	// LOCATIONCONFIG_HPP
