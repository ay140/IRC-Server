/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:02 by amarzouk          #+#    #+#             */
/*   Updated: 2024/08/01 09:32:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"
#include "../include/Server.hpp"

Request::Request(): args(), command(), invalidMessage(false) {};
Request::Request(const Request& x) {*this = x;};
Request::~Request() {};

Request::Request(const std::vector<std::string> &splittedMessage)
{
    if (splittedMessage.empty())
    {
        invalidMessage = true;
        return;
    }
    command = splittedMessage[0];
    for (size_t i = 1; i < splittedMessage.size(); ++i)
    {
        args.push_back(splittedMessage[i]);
    }

    invalidMessage = false;
}

Request & Request::operator= ( const Request& rhs )
{
	if (this == &rhs)
		return (*this);
	this->args = rhs.args;
	this->command = rhs.command;
	this->invalidMessage = rhs.invalidMessage;
	return (*this);
};

void Server::_ClientRequest(int index) 
{
    static std::map<int, std::string> commandBuffers;  // Stores incomplete commands for each client

    char buf[6000];
    int sender_fd = this->_pfds[index].fd;
    int nbytes = recv(sender_fd, buf, sizeof(buf) - 1, 0); // -1 to leave space for null-terminator

    if (nbytes < 0) 
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) 
        {
            // No data available right now, try again later
            return;
        } 
        else 
        {
            std::cout << "recv() error: " << strerror(errno) << std::endl;
            close(sender_fd);
            _removeFromPoll(index);
            commandBuffers.erase(sender_fd);
            return;
        }
    } 
    else if (nbytes == 0) 
    {
        std::cout << "[" << currentDateTime() << "]: socket " << sender_fd << " hung up" << std::endl;
        close(sender_fd);
        _removeFromPoll(index);
        commandBuffers.erase(sender_fd);
        return;
    } 

    buf[nbytes] = '\0';
    commandBuffers[sender_fd] += buf; // Append received data to the buffer

    size_t pos;
    while ((pos = commandBuffers[sender_fd].find('\n')) != std::string::npos) 
    {
        std::string message = commandBuffers[sender_fd].substr(0, pos);
        commandBuffers[sender_fd].erase(0, pos + 1); // Remove processed command from the buffer

        // Remove trailing '\r' if present
        if (!message.empty() && message[message.size() - 1] == '\r') 
        {
            message.erase(message.size() - 1);
        }

        std::string ret = _parsing(message, sender_fd);

        if (message != "QUIT" && !ret.empty() && send(sender_fd, ret.c_str(), ret.length(), 0) == -1) 
        {
            std::cout << "send() error: " << strerror(errno) << std::endl;
        }
    }
}

Request Server::_splitRequest(const std::string& req) 
{
    Request request;
    request.invalidMessage = false;  // Ensure this is initialized
    size_t i = 0;
    size_t j = 0;

    // Check for leading spaces or empty input
    if (req.empty() || req[i] == ' ') 
    {
        request.invalidMessage = true;
        return request;
    }

    j = i;
    while (i < req.size()) 
    {
        if (req[i] == ' ') 
        {
            if (i + 1 < req.size() && req[i + 1] == ' ') 
            {
                request.invalidMessage = true;
                return request;
            }
            request.args.push_back(req.substr(j, i - j));
            while (i < req.size() && req[i] == ' ')
                i++;
            j = i;
        }
        else if (req[i] == ':') 
        {
            if (i > 0 && req[i - 1] != ' ') 
            {
                request.invalidMessage = true;
                return request;
            }
            request.args.push_back(req.substr(i + 1));
            request.command = request.args[0];
            request.args.erase(request.args.begin());
            return request;
        }
        else 
        {
            i++;
        }
    }

    if (i > 0 && j < req.size())
        request.args.push_back(req.substr(j, i - j));

    if (!request.args.empty())
    {
        request.command = request.args[0];
        request.args.erase(request.args.begin());
    }
    else
    {
        request.invalidMessage = true;
    }

    return request;
}
