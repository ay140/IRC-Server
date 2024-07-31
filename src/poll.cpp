/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:00 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/31 08:15:03 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_addToPoll(int newfd) 
{
    if (this->_online_c == this->_max_online_c) 
	{
        this->_max_online_c *= 2;
        struct pollfd* new_pfds = new struct pollfd[this->_max_online_c];
        if (new_pfds == NULL) 
        {
            std::cerr << "new[] allocation error: " << strerror(errno) << std::endl;
            throw std::runtime_error("Memory allocation failed in _addToPoll");
        }
        for (int i = this->_online_c; i < this->_max_online_c; ++i) 
        {
            new_pfds[i].fd = -1;
            new_pfds[i].events = 0;
            new_pfds[i].revents = 0;
        }
        std::memcpy(new_pfds, this->_pfds, sizeof(struct pollfd) * this->_online_c);
        delete[] this->_pfds;
        this->_pfds = new_pfds;
    }
    try
    {
    this->_pfds[this->_online_c].fd = newfd;
    this->_pfds[this->_online_c].events = POLLIN;
    this->_clients.insert(std::make_pair(newfd, new Client(newfd)));
    this->_online_c++;
    }
    catch (const std::exception &e) 
    {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        throw;
    }
}

void Server::_removeFromPoll(int index) 
{
    if (index < 0 || index >= this->_online_c) 
    {
        std::cout << "Invalid index for _removeFromPoll: " << index << std::endl;
        return;
    }
    int fd_to_remove = this->_pfds[index].fd;
    close(fd_to_remove);
    std::map<int, Client*>::iterator it = this->_clients.find(fd_to_remove);
    if (it != this->_clients.end()) 
    {
        std::string nickname = it->second->getNickName();
        if (!nickname.empty()) 
        {
            this->_clientNicknames.erase(std::remove(this->_clientNicknames.begin(), this->_clientNicknames.end(), nickname), this->_clientNicknames.end());
        }
        delete it->second;
        this->_clients.erase(it);
    } 
    else 
    {
        std::cout << "Error: FD not found in _clients map: " << fd_to_remove << std::endl;
    }
    if (index != this->_online_c - 1) 
    {
        this->_pfds[index] = this->_pfds[this->_online_c - 1];
    }
    this->_online_c--;
    
    std::cout << "Removed fd: " << fd_to_remove << " from poll and clients map. Current online count: " << this->_online_c - 1 << std::endl;
}
