/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:48:31 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 11:34:03 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage ./ircserv <port> <password>" << std::endl;
		exit(1);
	}
	Server	srv("Gotham", 10, av[1], av[2]);
	try
	{
		srv.startServer();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
};
