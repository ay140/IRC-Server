/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:48:31 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 13:26:52 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"

Server* server_instance = NULL;

bool isValidDigitOnlyPassword(const std::string& password) 
{
    for (std::string::size_type i = 0; i < password.length(); ++i) 
    {
        if (!isdigit(password[i])) 
        {
            return false;
        }
    }
    return true;
}

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
        
        std::string password = av[2];
        if (!isValidDigitOnlyPassword(password)) 
        {
            throw std::runtime_error("Invalid password. Password must contain digits only.");
        }
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    try
    {
        server_instance = new Server("Gotham", 256, av[1], av[2]);
        server_instance->startServer();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        delete server_instance;
        return 1;
    }

    delete server_instance;
    return 0;
}
