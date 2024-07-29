/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:00 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 11:28:04 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_addToPoll(int newfd) 
{
    if (this->_online_c == this->_max_online_c) // If we have reached the maximum number of clients
	{
        this->_max_online_c *= 2; // Double the maximum number of clients
        struct pollfd* new_pfds = (struct pollfd*)realloc(this->_pfds, sizeof(struct pollfd) * this->_max_online_c); // Reallocate memory for the new maximum number of clients
        if (new_pfds == NULL) // If realloc() fails
		{
            throw std::runtime_error("realloc() error: " + std::string(strerror(errno)));
        }
        this->_pfds = new_pfds;
    }

    try 
    {
    this->_pfds[this->_online_c].fd = newfd; // update the pollfd struct with the new file descriptor for client
    this->_pfds[this->_online_c].events = POLLIN; // Set the events to poll for to POLLIN (data to read)
    this->_clients.insert(std::make_pair(newfd, new Client(newfd))); // Add the new client to the clients map
    this->_online_c++;
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        throw;  // Rethrow the exception to be handled by the caller
    }
}


void Server::_removeFromPoll(int index) {
    if (index < 0 || index >= this->_online_c) {
        std::cout << "Invalid index for _removeFromPoll: " << index << std::endl;
        return;
    }

    int fd_to_remove = this->_pfds[index].fd;

    // Print debug information
    std::cout << "Removing fd: " << fd_to_remove << " at index: " << index << std::endl;

    // Close the socket
    close(fd_to_remove);

    // Remove from clients map and nicknames list
    std::map<int, Client*>::iterator it = this->_clients.find(fd_to_remove);
    if (it != this->_clients.end()) {
        std::string nickname = it->second->getNickName();
        if (!nickname.empty()) {
            this->_clientNicknames.erase(std::remove(this->_clientNicknames.begin(), this->_clientNicknames.end(), nickname), this->_clientNicknames.end());
        }
        delete it->second;
        this->_clients.erase(it);
    } else {
        std::cout << "Error: FD not found in _clients map: " << fd_to_remove << std::endl;
    }

    // Move the last element to the position of the element to remove
    if (index != this->_online_c - 1) {
        this->_pfds[index] = this->_pfds[this->_online_c - 1];
        std::cout << "Moved fd: " << this->_pfds[index].fd << " to index: " << index << std::endl;
    }
    this->_online_c--;

    // Print debug information
    std::cout << "Removed fd: " << fd_to_remove << " from poll and clients map. Current online count: " << this->_online_c << std::endl;
}
