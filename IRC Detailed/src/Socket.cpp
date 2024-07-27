/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:26:12 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/27 21:26:11 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_getSocket(const std::string& port) 
{
    int yes = 1; // flag used for setting socket options.
    int status; //  store the return value of getaddrinfo.
    struct addrinfo hint, *serverinfo, *tmp; // hint is a struct addrinfo used to set the criteria for selecting the socket address.  serverinfo and tmp are pointers to struct addrinfo used for storing the results from getaddrinfo.

    memset(&hint, 0, sizeof(hint)); // Initialize the hint struct to zero.
    hint.ai_family = AF_INET; // specifies that the address family should be IPv4.
    hint.ai_socktype = SOCK_STREAM; // specifies that the socket type should be a stream socket.
    hint.ai_protocol = getprotobyname("TCP")->p_proto; // specifies that the protocol should be TCP.

    status = getaddrinfo("0.0.0.0", port.c_str(), &hint, &serverinfo);
    /*
    getaddrinfo is a system library function used to resolve network addresses. 
    It converts human-readable text strings representing hostnames or IP addresses 
    and service names or port numbers into a linked list of struct addrinfo structures 
    that can be used by network functions such as socket, bind, and connect.
    Parameters:
    "0.0.0.0": This is the hostname or IP address to resolve. "0.0.0.0" is a special IP address
     that tells the operating system to bind to all available network interfaces. 
    This is commonly used in server applications to accept connections on any network interface.

    port.c_str(): This is the service name or port number to resolve. >> convert the port number to a C-style string.

    &hint: This is a pointer to a struct addrinfo that specifies the criteria for selecting the socket address structures 
    to return in the list pointed to by serverinfo. 
    hint was set up to specify IPv4 (AF_INET), TCP (SOCK_STREAM), and the TCP protocol (getprotobyname("TCP")->p_proto).

    &serverinfo: This is a pointer to a pointer to struct addrinfo. getaddrinfo will allocate memory and store a pointer to the result list in this variable.
    
    if getaddrinfo returns 0, the function was successful. If it returns a non-zero value, an error occurred.
    */
    if (status != 0) // check if getaddrinfo was successful.
	{
        throw std::runtime_error(std::string("getaddrinfo() error: ") + gai_strerror(status));
    }
    
    for (tmp = serverinfo; tmp != NULL; tmp = tmp->ai_next) 
	{
        this->_socketfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (this->_socketfd < 0) 
		{
            continue;
        }

        if (setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
		{
            std::cerr << "setsockopt() error: " << strerror(errno) << std::endl;
            close(this->_socketfd);
            continue;
        }

        if (bind(this->_socketfd, tmp->ai_addr, tmp->ai_addrlen) < 0) 
		{
            std::cerr << "bind() error: " << strerror(errno) << std::endl;
            close(this->_socketfd);
            continue;
        }
        break;
    }

    freeaddrinfo(serverinfo);

    if (tmp == NULL) 
	{
        throw std::runtime_error(std::string("bind() error: ") + strerror(errno));

    }

    if (listen(this->_socketfd, this->_max_online_c) == -1) 
	{
        throw std::runtime_error(std::string("listen() error: ") + strerror(errno));

    }
}

std::string fillIt(const std::string& str, size_t len) 
{
    std::string result = str;
    if (result.length() < len) 
	{
        result.insert(result.end(), len - result.length(), ' ');
    } 
	else 
	{
        result.erase(result.begin() + len - 1, result.end());
        result.append(".");
    }
    return result;
}


const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
};
