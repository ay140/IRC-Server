/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:26:12 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 09:25:22 by amarzouk         ###   ########.fr       */
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
    
    struct protoent *proto = getprotobyname("TCP"); // get the TCP protocol.
    if (proto == NULL) // check if getprotobyname was successful.
    {
        throw std::runtime_error("getprotobyname() error: " + std::string(strerror(errno)));
    }
    hint.ai_protocol = proto->p_proto;

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
    
    for (tmp = serverinfo; tmp != NULL; tmp = tmp->ai_next) // iterate over the linked list of struct addrinfo.
	{
        this->_socketfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol); // create a socket using the address family 
        if (this->_socketfd < 0) // if the socket creation failed, continue to the next address.
		{
            continue;
        }

        if (setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)  
		{
        /*
        If the socket creation is successful, it sets the socket option SO_REUSEADDR (The name of the option to set)
        This allows the socket to bind to an address that is already in use (helpful during development).

        The SO_REUSEADDR option allows a socket to forcibly bind to a port in use by a socket in TIME_WAIT state. This can be useful in server applications that need to be restarted frequently.

        When SO_REUSEADDR is set, the following behaviors are enabled:
        Address Reuse: The socket can bind to an address that is in TIME_WAIT state. Normally, if a socket is closed, the operating system puts the socket into TIME_WAIT state for a short period of time (typically a few minutes). During this time, other sockets cannot bind to the same address and port. Setting SO_REUSEADDR allows a new socket to bind to the same address and port even if they are in TIME_WAIT state.
        Multiple Bindings: Multiple sockets can bind to the same address and port, as long as they all set the SO_REUSEADDR option. This is particularly useful for multicast applications.
        
        Usage: By setting SO_REUSEADDR, you ensure that your server can quickly restart and bind to the same port without waiting for the operating system to release the previous socket.
        */
            std::cerr << "setsockopt() error: " << strerror(errno) << std::endl;
            close(this->_socketfd);
            continue;
        }
        if (bind(this->_socketfd, tmp->ai_addr, tmp->ai_addrlen) < 0) 
		{
            /*
            The socket function is used to create a new socket. This socket can be used for communication over a network.
            It specifies the protocol family (e.g., AF_INET for IPv4), the socket type (e.g., SOCK_STREAM for TCP), and the protocol (e.g., IPPROTO_TCP for TCP).
            Creating a socket is akin to getting a handle or an endpoint that you can use to establish a connection, send, and receive data.
            bind Function:

            The bind function is used to associate the socket with a specific network address and port number. This is particularly important for server applications that need to listen for incoming connections on a specific port.
            When a socket is created, it is not yet bound to any address. The bind function assigns the address specified in the sockaddr structure to the socket. This tells the operating system that the application wants to receive data sent to that address.
            Without binding, a server socket would not know which port it should listen to for incoming connections.

            Why bind is Important?
            Server Listening: For a server, the bind function is crucial because it tells the operating system which address and port the server will listen to for incoming connections.
            Address Assignment: Without bind, the socket remains unassigned to any specific address, and the server would not receive any incoming connection requests
            
            socket Function:
            The socket function creates a socket (a file descriptor) and prepares it for network communication.
            This involves specifying the address family (e.g., IPv4), the socket type (e.g., TCP), and the protocol (e.g., TCP).
            At this stage, the socket is ready to be used for communication, but it is not yet associated with any specific network address.
            
            bind Function:
            The bind function associates the socket with a specific network address (IP address and port number).
            This tells the operating system that the application wants to use this socket to receive data sent to that address.
            Without bind, the socket remains unlinked to any specific address, and the server would not be able to receive any incoming connection requests.
            */
            std::cerr << "bind() error: " << strerror(errno) << std::endl;
            close(this->_socketfd);
            continue;
        }
        break;
    }
    /*
    When getaddrinfo is called, it fills the serverinfo linked list with one or more addrinfo structures. 
    Each structure represents a possible network configuration. Here is a simplified example of what serverinfo might contain:

    serverinfo (addrinfo*) ---> [addrinfo] ---> [addrinfo] ---> [addrinfo] ---> NULL
                           ai_next       ai_next       ai_next
    First addrinfo Structure
    {
    ai_flags = 0;
    ai_family = AF_INET;
    ai_socktype = SOCK_STREAM;
    ai_protocol = IPPROTO_TCP;
    ai_addrlen = sizeof(struct sockaddr_in);
    ai_addr = { 
        .sin_family = AF_INET, 
        .sin_port = htons(6667), 
        .sin_addr = inet_addr("127.0.0.1") 
    };
    ai_canonname = NULL;
    ai_next = <pointer to next addrinfo structure>;
    }
    Second addrinfo Structure
    {
    ai_flags = 0;
    ai_family = AF_INET;
    ai_socktype = SOCK_STREAM;
    ai_protocol = IPPROTO_TCP;
    ai_addrlen = sizeof(struct sockaddr_in);
    ai_addr = { 
        .sin_family = AF_INET, 
        .sin_port = htons(6667), 
        .sin_addr = inet_addr("192.168.1.10") 
    };
    ai_canonname = NULL;
    ai_next = <pointer to next addrinfo structure>;
    }
    Why We Loop?
    Multiple Potential Configurations:
    The getaddrinfo function can return multiple addrinfo structures, each representing a potential network configuration (e.g., different IP addresses, protocols, socket types) that the system can use.
    These configurations can include IPv4 and IPv6 addresses, different network interfaces, etc.
    
    Finding a Working Configuration:
    Not all configurations returned by getaddrinfo might be usable or available. For instance, some addresses might be reserved or already in use, or certain protocols might not be supported by the system.
    The loop allows the code to try each configuration until it finds one that works. This makes the server more robust and adaptable to different network environments.
    
    How Many Sockets Are We Trying to Create?
    Single Successful Socket:
    The loop aims to create and bind only one successful socket. The reason for looping through multiple configurations is to find a suitable one that works.
    The loop continues only until it finds a valid configuration. Once a socket is successfully created and bound, the loop breaks.

    */
    freeaddrinfo(serverinfo); // free the memory allocated by getaddrinfo.

    if (tmp == NULL) 
	{
        /*
        If the loop finishes and tmp is NULL, it means that the function failed to 
        bind the socket to any address in the serverinfo list.
        */
        throw std::runtime_error(std::string("bind() error: ") + strerror(errno));
    }

    if (listen(this->_socketfd, this->_max_online_c) == -1) 
	{
        /*
        The listen function is used to mark the socket as a passive socket, which means it will be used to accept incoming connection requests. 
        this part sets up the socket to listen for incoming connections and specifies the maximum number of connections that can be queued for acceptance.
        */
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
