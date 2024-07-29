/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:36 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 09:50:44 by amarzouk         ###   ########.fr       */
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

        // Initialize all pollfd structs in the array
        for (int i = 0; i < max_online + 1; ++i) 
        {
            _pfds[i].fd = -1;
            _pfds[i].events = 0;
            _pfds[i].revents = 0;
        }
/* 
    The pollfd structure is used to monitor multiple file descriptors to see if they have any events that need to be handled.
    struct pollfd {
        int fd;       // The file descriptor to be monitored
        short events; // The events of interest
        short revents;// The events that actually occurred

    When you set the events field of a pollfd structure to POLLIN, you are indicating that you are interested in knowing when there is data to read on that file descriptor.
    This means that you want to be notified when there is data to read on the server socket. For a server socket, this typically means an incoming connection request.
    };
*/

        _getSocket(port); // Create the server socket, bind and listen

        _pfds[0].fd = _socketfd; // Set the first pollfd structure to the server socket
        _pfds[0].events = POLLIN;
        _online_c++; // Increment the count of online clients (server socket is online)

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
        std::cout << "Received SIGINT, shutting down server..." << std::endl;
        if (server_instance) 
        {
            delete server_instance; // This will call the destructor to clean up resources
            server_instance = NULL;
        }
        std::exit(EXIT_SUCCESS);
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

std::string	Server::_getPassword() const 
{ 
    return (this->_password); 
};


void Server::startServer(void) 
{
    while (true) 
    {
        int poll_count = poll(this->_pfds, this->_online_c, -1); // update the pollfd array
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

/*
The poll function is a system call in Unix-like operating systems that waits for events on a set of file descriptors.
 It is commonly used in network programming to manage multiple connections simultaneously, allowing a program to be notified when
  a file descriptor is ready for I/O operations (e.g., ready to read or write data).

The poll function takes three arguments:

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
fds: Pointer to an array of pollfd structures, which specify the file descriptors to be monitored and the types of events to look for.
nfds: Number of file descriptors in the fds array.
timeout: Specifies the timeout period in milliseconds. A value of -1 makes poll wait indefinitely, 0 makes it return immediately, 
and a positive value specifies the number of milliseconds to wait.

poll waits for events on the file descriptors. The events could be:

POLLIN: Data is available to read.
POLLOUT: Data can be written without blocking.
POLLERR: An error occurred.
POLLHUP: The file descriptor was hung up.

What Happens When poll Detects an Event?

1- Waiting for Events: The poll function blocks until one of the file descriptors in the fds array has an event.
2- Event Detection: When an event occurs on a file descriptor, poll sets the revents field of the corresponding pollfd structure to indicate the event.
3- Returning from poll: The poll function returns the number of file descriptors that have events. If the timeout expires, poll returns 0.


*/