/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollHandling.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:00 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 11:28:05 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void	Server::_addToPoll(int newfd)
{
	if (this->_online_c == this->_max_online_c)
	{
		this->_max_online_c *= 2;
		this->_pfds = (struct pollfd *)realloc(this->_pfds, this->_max_online_c);
	}
	this->_pfds[this->_online_c].fd = newfd;
	this->_pfds[this->_online_c].events = POLLIN;
	this->_clients.insert(std::pair<int, Client *>(newfd, new Client(newfd)));
	this->_online_c++;
};

void	Server::_removeFromPoll(int i)
{
	close(this->_pfds[i].fd);
	this->_pfds[i] = this->_pfds[this->_online_c - 1];
	this->_clients.erase(this->_pfds[i].fd);
	this->_online_c--;
};
