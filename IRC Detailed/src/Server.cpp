/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:36 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/27 21:29:06 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(): _online_c(0),_max_online_c(0),_socketfd(0),_name(),_password(),_prefix(":"),_pfds(NULL),_clients(),_unavailableUserName(),_clientNicknames(),_files(),_allChannels(){}

Server::Server(const std::string& name, int max_online, const std::string& port, const std::string& password) 
{
    try 
    {
        _name = name;
        _max_online_c = max_online + 1; // +1 for the server socket
        _password = password;
        _online_c = 0;
        _socketfd = -1; // -1 indicates that the socket is not yet created
        _pfds = new struct pollfd[max_online + 1];  // Allocate memory for pollfd array

        _getSocket(port); // Create the server socket

        _pfds[0].fd = _socketfd;
        _pfds[0].events = POLLIN;
        _online_c++;

        signal(SIGINT, handle_signal); // Set up signal handler for shutdown
    }
    catch (const std::bad_alloc& e) 
    {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        delete[] _pfds; // Free allocated memory
        throw; // Re-throw the exception
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        delete[] _pfds; // Free allocated memory
        throw; // Re-throw the exception
    }
}

Server::Server(const Server & x)
    : _online_c(x._online_c),
      _max_online_c(x._max_online_c),
      _socketfd(x._socketfd),
      _name(x._name),
      _password(x._password),
      _prefix(x._prefix),
      _pfds(NULL),
      _clients(x._clients),
      _unavailableUserName(x._unavailableUserName),
      _clientNicknames(x._clientNicknames),
      _files(x._files),
      _allChannels(x._allChannels)
{
    if (x._pfds) 
    {
        _pfds = new struct pollfd[_max_online_c];
        std::copy(x._pfds, x._pfds + _max_online_c, _pfds);
    }
}

Server & Server::operator=(const Server & rhs)
{
    if (this != &rhs) 
    {
        _online_c = rhs._online_c;
        _max_online_c = rhs._max_online_c;
        _socketfd = rhs._socketfd;
        _name = rhs._name;
        _password = rhs._password;
        _prefix = rhs._prefix;

        // Deep copy of _pfds
        if (_pfds) 
        {
            delete[] _pfds;
        }
        if (rhs._pfds) 
        {
            _pfds = new struct pollfd[_max_online_c];
            std::copy(rhs._pfds, rhs._pfds + _max_online_c, _pfds);
        } 
        else 
        {
            _pfds = NULL;
        }
        _clients = rhs._clients;
        _unavailableUserName = rhs._unavailableUserName;
        _clientNicknames = rhs._clientNicknames;
        _files = rhs._files;
        _allChannels = rhs._allChannels;
    }
    return *this;
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

std::string Server::_welcomemsg(void)
{
    std::string welcome = RED;
    welcome.append("Welcome to the IRC Server!\n");
    welcome.append(CYAN);
    welcome.append("Please login to start chatting, or type HELP for assistance.\n");
    welcome.append(RESET);
    return welcome;
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

void Server::startServer(void) 
{
    while (true) 
    {
        int poll_count = poll(this->_pfds, this->_online_c, -1);
        if (poll_count == -1) 
        {
            std::cout << "poll() error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < this->_online_c; ++i) 
        {
            if (this->_pfds[i].revents & POLLIN) 
            {
                if (this->_pfds[i].fd == this->_socketfd) 
                {
                    _newClient(); // Handle new connection
                } 
                else 
                {
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
