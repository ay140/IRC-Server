/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messagesHandling.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:27:17 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 21:58:44 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_welcomemsg(void)
{
    std::string welcome = RED;
    welcome.append("Welcome to the IRC Server!\n");
    welcome.append(CYAN);
    welcome.append("Please login to start chatting, or type HELP for assistance.\n");
    welcome.append(RESET);
    return welcome;
}

int			Server::_sendall(int destfd, std::string message)
{
	int total = 0;
	int bytesleft = message.length();
	int b;

	while (total < (int)message.length())
	{
		b = send(destfd, message.c_str() + total, bytesleft, 0);
		if (b == -1) break;
		total += b;
		bytesleft -= b;
	}
	return (b == -1 ? -1 : 0);
};

std::string		Server::_sendToAllUsers( Channel *channel, int senderFd, std::string message)
{
	std::map<int, Client *> allusers = channel->getAllUsers();
	std::map<int, Client *>::iterator it = allusers.begin();
	std::string reply = this->_clients[senderFd]->getUserPrefix();
	reply.append(message);
	while (it != allusers.end())
	{
		if (senderFd != it->first)
			if (_sendall(it->first, reply) == -1)
			{
				std::cout << "_sendall() error: " << strerror(errno) << std::endl;
				return ("");
			}
		it++;
	}
	return ("");
};
