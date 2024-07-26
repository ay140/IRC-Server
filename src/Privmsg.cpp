/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:45 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 17:03:36 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_privmsg(Request request, int fd) {
    if (!this->_clients[fd]->getRegistered()) {
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) {
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters");
    }

    const std::string& target = request.args[0];
    std::string message;
    for (size_t i = 1; i < request.args.size(); ++i) {
        message += request.args[i];
        if (i != request.args.size() - 1) {
            message += " ";
        }
    }

    if (target.find(',') != std::string::npos) {
        return _printMessage("401", this->_clients[fd]->getNickName(), target + " :Too many recipients.");
    }

    if (target[0] != '&' && target[0] != '#' && target[0] != '+' && target[0] != '!') {
        return _privToUser(target, message, "PRIVMSG", fd);
    }

    return _privToChannel(target, message, fd);
}


std::string Server::_privToUser(const std::string& user, const std::string& message, const std::string& cmd, int i) {
    int userFd = _findFdByNickName(user);
    if (userFd == USERNOTFOUND) 
	{
        return _printMessage("401", this->_clients[i]->getNickName(), user + " :No such nick/channel");
    }

    std::string reply = this->_clients[i]->getUserPrefix();
    reply.append(cmd + " " + user + " :" + message + "\n");

    if (_sendall(userFd, reply) == -1) 
	{
        std::cerr << "_sendall() error: " << strerror(errno) << std::endl;
    }

    return "";
}


std::string Server::_privToChannel(const std::string& channelName, const std::string& message, int i) 
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName);
    if (it != this->_allChannels.end()) 
	{
        std::pair<Client *, int> user = it->second->findUserRole(i);
        if (user.second == -1) 
		{
            return _printMessage("404", this->_clients[i]->getNickName(), channelName + " :Cannot send to channel");
        }
        std::string msg = "PRIVMSG " + channelName + " :" + message + "\n";
        _sendToAllUsers(it->second, i, msg);
    } 
	else 
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
	return (b == -1 ? -1 : 0);
};

std::string		Server::_sendToAllUsers( Channel *channel, int senderFd, std::string message)
{
	std::map<int, Client *> allusers = channel->getAllUsers();
	std::map<int, Client *>::iterator it = allusers.begin();
	std::string reply = this->_clients[senderFd]->getUserPrefix();
	reply.append(message);
	while (it != allusers.end())
	{
		if (senderFd != it->first)
			if (_sendall(it->first, reply) == -1)
			{
				std::cout << "_sendall() error: " << strerror(errno) << std::endl;
				return ("");
			}
		it++;
	}
	return ("");
};
