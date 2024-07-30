/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:45 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 11:07:01 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_privmsg(Request request, int fd) 
{
    if (!this->_clients[fd]->getRegistered()) // check if the client is registered
    {
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) // check if the request has enough parameters
    {
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters");
    }

    const std::string& target = request.args[0]; // get the target which is first parameter after command
    std::string message; // get the message which is the rest of the parameters
    for (size_t i = 1; i < request.args.size(); ++i) 
    {
        message += request.args[i]; // concatenate the message and add a space between each word
        if (i != request.args.size() - 1) 
        {
            message += " ";
        }
    }
    if (target.find(',') != std::string::npos) // check if the target has multiple recipients by searching for a comma in the target string which should contain only one user
    {
        /*
        std::string::npos is a useful constant for checking if a search operation on a string was successful or not. In your code, 
        it helps to ensure that certain characters are not present in strings where they are not allowed, thereby validating inputs 
        and maintaining the integrity of operations.
        */
        return _printMessage("401", this->_clients[fd]->getNickName(), target + " :Too many recipients.");
    }

    if (target[0] != '&' && target[0] != '#' && target[0] != '+' && target[0] != '!') // check if the target is a channel or a user, if starts with a # & + ! means channel
    {
        return _privToUser(target, message, "PRIVMSG", fd);
    }

    return _privToChannel(target, message, fd);
}


std::string Server::_privToUser(const std::string& user, const std::string& message, const std::string& cmd, int i) 
{
    int userFd = _findFdByNickName(user); // find the user by nickname and return his fd
    if (userFd == USERNOTFOUND) // if -1 then the user is not found
	{
        return _printMessage("401", this->_clients[i]->getNickName(), user + " :No such nick/channel");
    }

    std::string reply = this->_clients[i]->getUserPrefix();
    reply.append(cmd + " " + user + " :" + message + "\n");

    if (_sendall(userFd, reply) == -1) // send the message to the user, if == -1 means failed
	{
        std::cerr << "_sendall() error: " << strerror(errno) << std::endl;
    }

    return "";
}


std::string Server::_privToChannel(const std::string& channelName, const std::string& message, int i) 
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName); // find the channel by name
    if (it != this->_allChannels.end())  // if the channel is found
	{
        std::pair<Client *, int> user = it->second->findUserRole(i);
        if (user.second == -1) // if the user is not in the channel
		{
            return _printMessage("404", this->_clients[i]->getNickName(), channelName + " :Cannot send to channel");
        }
        std::string msg = "PRIVMSG " + channelName + " :" + message + "\n"; // create the message
        _sendToAllUsers(it->second, i, msg); // send the message to all users in the channel
    } 
	else // if the channel is not found
	{
        return _printMessage("401", this->_clients[i]->getNickName(), channelName + " :No such nick/channel");
    }
    return "";
}

int			Server::_sendall(int destfd, std::string message)
{
	int total = 0;
	int bytesleft = message.length();
	int b;

	while (total < (int)message.length())
	{
		b = send(destfd, message.c_str() + total, bytesleft, 0);
		if (b == -1) break;
		total += b;
		bytesleft -= b;
	}
	return (b == -1 ? -1 : 0); // return -1 if send failed, 0 if send was successful
};

std::string		Server::_sendToAllUsers( Channel *channel, int senderFd, std::string message)
{
	std::map<int, Client *> allusers = channel->getAllUsers(); // get all users in the channel
	std::map<int, Client *>::iterator it = allusers.begin(); // create an iterator to iterate over all users
	std::string reply = this->_clients[senderFd]->getUserPrefix(); 
	reply.append(message); // create the message to send
	while (it != allusers.end()) // iterate over all users
	{
		if (senderFd != it->first) // if the sender is not the current user
			if (_sendall(it->first, reply) == -1) // send the message to the user
			{
				std::cout << "_sendall() error: " << strerror(errno) << std::endl;
				return ("");
			}
		it++; // move to the next user
	}
	return ("");
};
