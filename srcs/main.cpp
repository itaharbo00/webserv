/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlarbi-a <wlarbi-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 20:03:32 by itaharbo          #+#    #+#             */
/*   Updated: 2025/12/11 19:34:09 by wlarbi-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int ac, char **av)
{
	try
	{
		std::string	configFile = "webserv.conf";

		if (ac == 2)
			configFile = av[1];
		else if (ac > 2)
		{
			std::cerr << "Usage: " << av[0] << " [config_file]" << std::endl;
			return 1;
		}

		Server	server(configFile);
		server.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return EXIT_SUCCESS;
}
