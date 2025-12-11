/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:16:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/16 15:56:46 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>
# include <map>

class LocationConfig
{
public:

	LocationConfig();
	LocationConfig(const std::string &path); // Avec chemin de la location
	~LocationConfig();

	// Getters
	std::string					getPath() const;
	std::vector<std::string>	getAllowedMethods() const;
	std::string					getRoot() const;
	std::string					getIndex() const;
	bool						getAutoindex() const;
	std::string					getUploadStore() const;
	int							getReturnCode() const;
	std::string					getReturnUrl() const;
	size_t						getClientMaxBodySize() const;

	// Setters
	void						setPath(const std::string &path);
	void						setAllowedMethods(const std::string &method);
	void						setRoot(const std::string &root);
	void						setIndex(const std::string &index);
	void						setAutoindex(bool autoindex);
	void						setUploadStore(const std::string &uploadStore);
	void						setReturn(int code, const std::string &url);
	void						setClientMaxBodySize(size_t size);

	// Checkers
	bool						isMethodAllowed(const std::string &method) const;
	bool						hasRedirect() const;
	bool						hasUpload() const;

	// CGI
	void						addCgiPass(const std::string &extension,
									const std::string &cgiPath);
	bool						hasCgi() const;
	bool						isCgiExtension(const std::string &extension) const;
	std::string					getCgiPass(const std::string &extension) const;
	const std::map<std::string, std::string>& getCgiPassMap() const;

private:

	std::string							p_path;			  // Chemin de la location
	std::vector<std::string>			p_allowedMethods; // Méthodes autorisées
	std::string							p_root;			  // Racine pour cette location
	std::string							p_index;		  // Fichier index pour cette location
	bool								p_autoindex;	  // Autoindex activé ou non
	std::string							p_uploadStore;	  // Chemin pour les uploads
	std::map<std::string, std::string>	p_cgi_pass;		  // {".php" → "/usr/bin/php-cgi", ".py" → "/usr/bin/python3", ".sh" → "/bin/bash"}
	int									p_returnCode;	  // Code de redirection
	std::string							p_returnUrl;	  // URL de redirection
	size_t								p_clientMaxBodySize; // Taille max du body (0 = utiliser celle du serveur)
};

#endif	// LOCATIONCONFIG_HPP
