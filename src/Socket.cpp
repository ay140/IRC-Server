/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:26:12 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 09:13:06 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_getSocket(const std::string& port) 
{
    int yes = 1;
    int status;
    struct addrinfo hint, *serverinfo, *tmp;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;

    // Added check for getprotobyname
    struct protoent *proto = getprotobyname("TCP");
    if (proto == NULL) 
    {
        throw std::runtime_error("getprotobyname() error: " + std::string(strerror(errno)));
    }
    hint.ai_protocol = proto->p_proto;

    status = getaddrinfo("0.0.0.0", port.c_str(), &hint, &serverinfo);
    if (status != 0) 
	{
        std::cerr << "getaddrinfo() error: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
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
