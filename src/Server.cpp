/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:36 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 00:23:26 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"


Server::Server() : _name(), _password(), _socketfd(0), _clients(), _pfds(NULL), _online_c(0), _max_online_c(0), _prefix(":"), _allChannels(), _unavailableUserName(), _clientNicknames() {};

Server::Server(const std::string& name, int max_online, const std::string& port, const std::string& password) {
    _name = name;
    _password = password;
    _max_online_c = max_online + 1;
    _online_c = 0;
    _socketfd = -1;
    _pfds = new struct pollfd[max_online + 1];

    _getSocket(port);
    _pfds[0].fd = _socketfd;
    _pfds[0].events = POLLIN;
    _online_c++;

    // Set up signal handler for graceful shutdown
    signal(SIGINT, handle_signal);
}

Server::~Server() 
{
    delete[] this->_pfds; // Always non-null after initialization

    // Delete all clients
    for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        delete it->second;
    }
    this->_clients.clear();

    // Delete all channels
    for (std::map<std::string, Channel *>::iterator itC = this->_allChannels.begin(); itC != this->_allChannels.end(); ++itC) {
        delete itC->second;
    }
    this->_allChannels.clear();

    // Close the socket
    if (_socketfd != -1) 
	{
        close(_socketfd);
    }
}

void Server::handle_signal(int signal) 
{
    if (signal == SIGINT) 
    {
        std::cout << "  Received SIGINT, shutting down server..." << std::endl;
        exit(0);
    }
}

std::string	Server::_printMessage(std::string num, std::string nickname, std::string message)
{
	if (nickname.empty())
		nickname = "*";
	return (":" + this->_name + " " + num + " " + nickname + " " + message + "\n");
}

void Server::_newClient(void) {
    struct sockaddr_storage remotaddr;
    socklen_t addrlen = sizeof remotaddr;
    int newfd = accept(this->_socketfd, (struct sockaddr*)&remotaddr, &addrlen);
    
    if (newfd == -1) {
        std::cout << "accept() error: " << strerror(errno) << std::endl;
        return;
    }
    
    // Set the new socket to non-blocking mode
    if (fcntl(newfd, F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "fcntl() error: " << strerror(errno) << std::endl;
        close(newfd);
        return;
    }
    
    _addToPoll(newfd);
    this->_clients[newfd] = new Client(newfd); // Ensure correct client initialization

    std::string welcome = _welcomemsg();
    if (send(newfd, welcome.c_str(), welcome.length(), 0) == -1) {
        std::cout << "send() error: " << strerror(errno) << std::endl;
    }
    
    std::cout << "[" << currentDateTime() << "]: new connection from "
              << inet_ntoa(((struct sockaddr_in*)&remotaddr)->sin_addr)
              << " on socket " << newfd << std::endl;
}


void* Server::get_in_addr(struct sockaddr* sa) 
{
    if (sa->sa_family == AF_INET) 
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Server::startServer(void) {
    while (true) {
        int poll_count = poll(this->_pfds, this->_online_c, -1);
        if (poll_count == -1) {
            std::cout << "poll() error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < this->_online_c; ++i) {
            if (this->_pfds[i].revents & POLLIN) {
                if (this->_pfds[i].fd == this->_socketfd) {
                    _newClient(); // Handle new connection
                } else {
                    _ClientRequest(i); // Handle client request
                }
            }
        }
    }
}



std::string	Server::_getPassword() const 
{ 
    return (this->_password); 
};
