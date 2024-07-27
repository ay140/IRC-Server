/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:48:31 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/27 15:30:39 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>\n";
        return 1;
    }
    try 
    {
        int port = std::atoi(av[1]);
        if (port < 6660 || port > 6669) 
        {
            throw std::runtime_error("Invalid port number. Port number must be between 6660 and 6669.");
        }

        std::cout << "Using port: " << port << std::endl;
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    try
    {
        Server srv("Gotham", 100, av[1], av[2]);
        srv.startServer();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
