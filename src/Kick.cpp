/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:26:48 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 14:01:02 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_kickedFromChannel(const std::string& channelName, const std::string& message, const std::vector<std::string>& users, int i) 
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName);
    if (it != this->_allChannels.end()) 
	{
        std::pair<Client *, int> user = it->second->findUserRole(i);
        if (user.second == 1) 
		{
            for (std::vector<std::string>::const_iterator userIt = users.begin(); userIt != users.end(); ++userIt) 
			{
                if (*userIt == this->_clients[i]->getNickName()) 
                {
                    return _printMessage("482", this->_clients[i]->getNickName(), channelName + " :You cannot kick yourself from the channel");
                }
                int userFd = _findFdByNickName(*userIt);
                if (userFd == USERNOTINCHANNEL) 
				{
                    return _printMessage("441", this->_clients[i]->getNickName(), *userIt + " " + channelName + " :They aren't on that channel");
                }
                std::string reply = "KICK " + channelName;
                if (message.empty()) 
				{
                    reply.append("\n");
                } 
				else 
				{
                    reply.append(" " + message + "\n");
                }
                _sendToAllUsers(it->second, i, reply);
                it->second->banUser(this->_clients[userFd]);
                _partChannel(channelName, userFd, "", 0);
            _sendall(i, "You have kicked " + *userIt + " from " + channelName + "\n");

        }
        } 
		else if (user.second == -1) 
		{
            // 442: ERR_NOTONCHANNEL - User is not on the specified channel
            return _printMessage("442", this->_clients[i]->getNickName(), channelName + " :You're not on that channel");
        } 
		else 
		{
            // 482: ERR_CHANOPRIVSNEEDED - You're not a channel operator
            return _printMessage("482", this->_clients[i]->getNickName(), channelName + " :You're not channel operator");
        }
        return "";
    }
    // 403: ERR_NOSUCHCHANNEL - No such channel exists
    return _printMessage("403", this->_clients[i]->getNickName(), channelName + " :No such channel");
}

std::string Server::_kick(Request request, int fd) 
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

    std::vector<std::string> channels = _commaSeparator(request.args[0]);
    std::vector<std::string> users = _commaSeparator(request.args[1]);

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) 
	{
        std::string result;
        if (request.args.size() == 3) 
		{
            result = _kickedFromChannel(*it, request.args[2], users, fd);
        } 
		else 
		{
            result = _kickedFromChannel(*it, "", users, fd);
        }
        if (!result.empty()) 
		{
            return result;
        }
    }
    return "";
}