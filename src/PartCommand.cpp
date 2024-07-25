/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PartCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:27:43 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 21:49:54 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int Server::_partChannel(const std::string& channelName, int fd, const std::string& message, int isPart) 
{
    std::map<std::string, Channel *>::iterator itCh = this->_allChannels.find(channelName);
    if (itCh == this->_allChannels.end()) 
	{
        // 403: ERR_NOSUCHCHANNEL - No such channel exists
        return NOSUCHCHANNEL;
    }

    std::pair<Client *, int> user = itCh->second->findUserRole(fd);
    if (user.second == -1) 
	{
        // 442: ERR_NOTONCHANNEL - User is not on the specified channel
        return NOTINCHANNEL;
    }

    // Remove user from channel
    if (user.second == 0) 
	{
        itCh->second->removeMember(fd);
    } 
	else if (user.second == 1) 
	{
        itCh->second->removeOperator(fd);
    } 
	else 
	{
        itCh->second->removeVoice(fd);
    }
    user.first->leaveChannel(itCh->second->getName());

    // Send PART message to all users in the channel
    if (isPart == 1) 
	{
        std::string reply = "PART " + channelName;
        if (message.empty()) 
		{
            reply.append("\n");
        } 
		else 
		{
            reply.append(" " + message + "\n");
        }
        _sendToAllUsers(itCh->second, fd, reply);
    }

    return 0;
}

std::string Server::_part(Request request, int fd) 
{
    if (!this->_clients[fd]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.empty()) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters");
    }

    std::vector<std::string> parsChannels = _commaSeparator(request.args[0]);
    for (std::vector<std::string>::iterator it = parsChannels.begin(); it != parsChannels.end(); ++it) 
	{
        int result = 0;
        if (request.args.size() == 2) 
		{
            result = _partChannel(*it, fd, request.args[1], 1);
        } 
		else 
		{
            result = _partChannel(*it, fd, "", 1);
        }

        if (result == NOSUCHCHANNEL) 
		{
            // 403: ERR_NOSUCHCHANNEL - No such channel exists
            return _printMessage("403", this->_clients[fd]->getNickName(), *it + " :No such channel");
        }

        if (result == NOTINCHANNEL) 
		{
            // 442: ERR_NOTONCHANNEL - User is not on the specified channel
            return _printMessage("442", this->_clients[fd]->getNickName(), *it + " :You're not on that channel");
        }
    }
    return "";
}
