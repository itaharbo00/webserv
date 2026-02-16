/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest_body.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 19:51:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/11 23:03:11 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

// Parse le corps de la requête HTTP en utilisant le Content-Length
void	HttpRequest::contentLengthBody(std::istringstream &stream)
{
	// Valider la longueur du corps
	long	contentLength = 0;
	std::istringstream(p_headers["Content-Length"]) >> contentLength;
	if (contentLength < 0)
		throw std::runtime_error("Negative Content-Length value");

	// Lire le corps de la requête
	std::vector<char>	buffer(contentLength);
	stream.read(&buffer[0], contentLength);

	// Vérifier si on a lu le nombre attendu d'octets
	if (stream.gcount() < contentLength)
	{
		std::ostringstream oss;
		oss << "Incomplete body: expected " << contentLength 
		    << " bytes, got " << stream.gcount();
		throw std::runtime_error(oss.str());
	}

	// Affecter le corps de la requête
	p_body.assign(buffer.begin(), buffer.begin() + contentLength);
}

// Parse le corps de la requête HTTP en utilisant le transfert encodé en chunks
void	HttpRequest::chunkedBody(std::istringstream &stream)
{
	p_body.clear();
	std::string	line;

	while (std::getline(stream, line))
	{
		trimString(line);
		if (line.empty())
			continue;

		// Convertir la taille du chunk de l'hexadécimal au décimal
		size_t	chunkSize = 0;
		std::istringstream iss(line);
		iss >> std::hex >> chunkSize;

		if (chunkSize == 0) // Fin des chunks
			break;

		std::vector<char>	buffer(chunkSize);
		stream.read(&buffer[0], chunkSize);

		// Vérifier si on a lu le nombre attendu d'octets
		if (static_cast<size_t>(stream.gcount()) < chunkSize)
		{
			std::ostringstream oss;
			oss << "Incomplete chunk: expected " << chunkSize 
			    << " bytes, got " << stream.gcount();
			throw std::runtime_error(oss.str());
		}

		// Ajouter le chunk au corps de la requête
		p_body.append(buffer.begin(), buffer.begin() + chunkSize);

		// Lire la ligne vide après le chunk
		std::getline(stream, line);
	}
}
