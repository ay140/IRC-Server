/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:02 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 13:36:20 by amarzouk         ###   ########.fr       */
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
    command = splittedMessage[0]; // Copy the command
    // Copy the arguments
    for (size_t i = 1; i < splittedMessage.size(); ++i)
    {
        args.push_back(splittedMessage[i]); // Copy the arguments to the vector
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

void Server::_ClientRequest(int index) // handles incoming data from a client connected to the server. It processes the data received from the client and sends an appropriate response back to the client if necessary.
{
    char buf[6000]; // 6000 bytes buffer to store the incoming data from the client.
    int sender_fd = this->_pfds[index].fd;
    int nbytes = recv(sender_fd, buf, sizeof(buf) - 1, 0); // -1 to leave space for null-terminator
    /*
    The recv function is used to receive data from a socket. Here’s the general form of the function:
    int recv(int sockfd, void *buf, int len, unsigned int flags);
    sockfd: The socket file descriptor.
    buf: The buffer where the received data will be stored.
    len: The maximum number of bytes to receive.
    flags: Set to 0 for normal operation.

    return value: 
    Positive Value: Indicates the number of bytes received.
    Zero: Indicates the client has closed the connection.
    Negative Value: Indicates an error occurred.
    */
    if (nbytes < 0) 
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)  // No data available right now, try again later
        {
            return;
        }
        else // For other errors, an error message is printed, the client socket is closed, and the client is removed from the poll array.
        {
            std::cout << "recv() error: " << strerror(errno) << std::endl;
            close(sender_fd);
            _removeFromPoll(index);
        }
    } 
    else if (nbytes == 0) // returns 0 (nbytes == 0), it indicates the client has closed the connection.
    {
        std::cout << "[" << currentDateTime() << "]: socket " << sender_fd << " hung up" << std::endl;
        close(sender_fd);
        _removeFromPoll(index);
    } 
    else 
    {
        buf[nbytes] = '\0'; // Null-terminate the received data
        std::string message(buf); // Convert the received data to a string

        // Remove trailing '\r' or '\n' if present
        while (!message.empty() && (message[message.size() - 1] == '\r' || message[message.size() - 1] == '\n')) 
        {
            message.erase(message.size() - 1);
        }
        
        std::string ret = _parsing(message, this->_pfds[index].fd); // Parse the message and generate a response
        
        if (message != "QUIT" && !ret.empty() && send(sender_fd, ret.c_str(), ret.length(), 0) == -1) 
        {
            std::cout << "send() error: " << strerror(errno) << std::endl;
        }
    }
    memset(buf, 0, sizeof(buf)); // Clear the buffer
}

Request Server::_splitRequest(const std::string& req) 
{
    /*
    split a raw request string into its components: the command and its arguments. It handles spaces and 
    colons to correctly parse the command and arguments and checks for invalid messages.
    */
    Request request;
    size_t i = 0;
    size_t j = 0;

    if (req[i] == ' ' || !req[i]) // If the first character is a space or the message is empty, the message is invalid.
    {
        request.invalidMessage = true;
        return request;
    }
    j = i;
    while (req[i]) 
    {
        if (req[i] == ' ') 
        {
            if (req[i + 1] == ' ')  // If there are two consecutive spaces, the message is invalid.
            {
                request.invalidMessage = true;
                return request;
            }
            request.args.push_back(req.substr(j, i - j));
            while (req[i] == ' ')
                i++;
            j = i;
        }
        if (req[i] == ':') 
        {
            if (req[i - 1] != ' ') 
            {
                request.invalidMessage = true;
                return request;
            }
            request.args.push_back(req.substr(i + 1));
            request.command = request.args[0];
            request.args.erase(request.args.begin());
            return request;
        }
        i++;
    }

    if (i && req[j])
        request.args.push_back(req.substr(j, i - j));
    request.command = request.args[0];
    request.args.erase(request.args.begin());
    return request;
}