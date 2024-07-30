/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:36 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 23:16:34 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(): _online_c(0),_max_online_c(0),_socketfd(0),_name(),_password(),_prefix(":"),_pfds(NULL),_clients(),_unavailableUserName(),_clientNicknames(),_files(),_allChannels(){}

Server::Server(const std::string& name, int max_online, const std::string& port, const std::string& password) 
{
    try 
    {
        _name = name;
        _max_online_c = max_online + 1;
        _password = password;
        _online_c = 0;
        _socketfd = -1;
        _pfds = new struct pollfd[max_online + 1];
        for (int i = 0; i < max_online + 1; ++i) 
        {
            _pfds[i].fd = -1;
            _pfds[i].events = 0;
            _pfds[i].revents = 0;
        }

        _getSocket(port);

        _pfds[0].fd = _socketfd;
        _pfds[0].events = POLLIN;
        _online_c++;

        signal(SIGINT, handle_signal);
    }
    catch (const std::bad_alloc& e) 
    {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        delete[] _pfds;
        throw;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        delete[] _pfds;
        throw;
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
    delete[] this->_pfds;

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

std::string	Server::_getPassword() const 
{ 
    return (this->_password); 
};

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
        std::cout << "Received SIGINT, shutting down server..." << std::endl;
        if (server_instance) 
        {
            for (int i = 1; i < server_instance->_online_c; ++i) 
            {
                server_instance->_removeFromPoll(i);
            }
            delete server_instance;
            server_instance = NULL;
        }
        exit(0);
    }
}

std::string	Server::_printMessage(std::string num, std::string nickname, std::string message)
{
	if (nickname.empty())
		nickname = "*";
	return (":" + this->_name + " " + num + " " + nickname + " " + message + "\n");
}

void Server::_newClient(void) 
{
    struct sockaddr_storage remotaddr;
    socklen_t addrlen = sizeof remotaddr;
    int newfd = accept(this->_socketfd, (struct sockaddr*)&remotaddr, &addrlen);
    
    if (newfd == -1) 
    {
        throw std::runtime_error(std::string("accept() error: ") + strerror(errno));

    }
    // Set the new socket to non-blocking mode
    if (fcntl(newfd, F_SETFL, O_NONBLOCK) == -1) 
    {
        close(newfd);
        throw std::runtime_error(std::string("fcntl() error: ") + strerror(errno));
    }
    
    try 
    {
    _addToPoll(newfd);
    } 
    catch (const std::exception& e) 
    {
        close(newfd);
        throw;
    }
    
    std::string welcome = _welcomemsg();
    if (send(newfd, welcome.c_str(), welcome.length(), 0) == -1) 
    {
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
                throw std::runtime_error(std::string("poll() error: ") + strerror(errno));
        }

        for (int i = 0; i < this->_online_c; ++i) 
        {
            if (this->_pfds[i].revents & POLLIN) 
            {
                if (this->_pfds[i].fd == this->_socketfd) 
                {
                    try 
                    {
                        _newClient(); // Handle new connection
                    } 
                    catch (const std::exception& e) 
                    {
                        std::cerr << "Error in _newClient: " << e.what() << std::endl;
                        throw;
                    }                
                } 
                else 
                {
                    _ClientRequest(i); // Handle client request
                }
            }
        }
    }
}

void Server::monitorChannels() 
{
    std::vector<std::string> emptyChannels;
    for (std::map<std::string, Channel*>::iterator it = this->_allChannels.begin(); it != this->_allChannels.end(); ++it) 
    {
        if (it->second->isEmpty()) 
        {
            emptyChannels.push_back(it->first);
        }
    }

    for (std::vector<std::string>::iterator it = emptyChannels.begin(); it != emptyChannels.end(); ++it) 
    {
        delete this->_allChannels[*it];
        this->_allChannels.erase(*it);
        std::cout << "Deleted empty channel: " << *it << std::endl;
    }
}