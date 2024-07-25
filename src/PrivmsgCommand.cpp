/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivmsgCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:28:45 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 21:58:44 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_privmsg(Request request, int fd) 
{
    if (!this->_clients[fd]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters");
    }

    const std::string& target = request.args[0];
    const std::string& message = request.args[1];

    if (request.args.size() == 2) 
	{
        if (target.find(',') != std::string::npos) 
		{
            // 401: ERR_NOSUCHNICK - Target nickname or channel does not exist / Too many recipients
            return _printMessage("401", this->_clients[fd]->getNickName(), target + " :Too many recipients.");
        }

        if (target[0] != '&' && target[0] != '#' && target[0] != '+' && target[0] != '!') 
		{
            // Sending a private message to a user
            return _privToUser(target, message, "PRIVMSG", fd);
        }

        // Sending a private message to a channel
        return _privToChannel(target, message, fd);
    }

    return "";
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

