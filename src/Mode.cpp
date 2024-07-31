/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 06:28:51 by codespace         #+#    #+#             */
/*   Updated: 2024/07/31 08:02:50 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_setMode(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
	{
        std::string ret;
        if (request.args.size() == 1 && request.args[0] == this->_clients[i]->getNickName()) 
		{
            ret = _printUserModes(ret, i);
        }
        ret.append("461 ERR_NEEDMOREPARAMS :Not enough parameters useage: MODE <nickname> <mode>\n");
        return ret;
    }

    // Check if it's a channel or user mode change
    if (request.args[0][0] == '#' || request.args[0][0] == '&' || request.args[0][0] == '!' || request.args[0][0] == '+') 
	{
        // Channel mode change
        return _setChannelMode(request, i);
    } 
	else 
	{
        // User mode change
        if (request.args[0] != this->_clients[i]->getNickName()) 
		{
            // 502: ERR_USERSDONTMATCH - Cannot change mode for other users
            return _printMessage("502", this->_clients[i]->getNickName(), ":Cannot change mode for other users");
        }

        if (!_validMode(request)) 
		{
            // 501: ERR_UMODEUNKNOWNFLAG - Unknown MODE flag
            return _printMessage("501", this->_clients[i]->getNickName(), ":Unknown MODE flag");
        }

        bool addMode = (request.args[1][0] == '+');
        this->_clients[i]->setMode(addMode, request.args[1][1]);

        // 221: RPL_UMODEIS - Mode change
        return _printMessage("221", this->_clients[i]->getNickName(), request.args[1]);
    }
}

std::string Server::_setChannelMode(Request request, int i) 
{
    if (request.args.size() < 2) 
	{
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
    }

    std::string channelName = request.args[0];
    std::string mode = request.args[1];

    std::map<std::string, Channel*>::iterator it = this->_allChannels.find(channelName);
    if (it == this->_allChannels.end()) 
	{
        return _printMessage("403", this->_clients[i]->getNickName(), channelName + " :No such channel");
    }

    Channel* channel = it->second;
    std::pair<Client*, int> user = channel->findUserRole(i);
    if (user.second != 1) 
	{
        return _printMessage("482", this->_clients[i]->getNickName(), channelName + " :You're not a channel operator");
    }

    bool addMode = (mode[0] == '+');
    std::string reply = ":" + this->_clients[i]->getUserPrefix() + " MODE " + channelName + " " + mode;
    switch (mode[1]) 
	{
        case 'o':
        {
            if (request.args.size() < 3) 
			{
                return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
            }
            std::string targetNick = request.args[2];
            int targetFd = _findFdByNickName(targetNick);
            if (targetFd == USERNOTINCHANNEL) 
			{
                return _printMessage("401", this->_clients[i]->getNickName(), targetNick + " :No such nick");
            }

            if (addMode) 
			{
                channel->addOperator(this->_clients[targetFd]);
            } 
			else 
			{
                channel->removeOperator(targetFd);
            }
            reply += " " + targetNick;
            break;
        }
        case 'i':
        {
            channel->setInviteOnly(addMode);
            break;
        }
        case 't':
        {
            channel->setTopicRestricted(addMode);
            break;
        }
        case 'k':
        {
            if (addMode) 
			{
                if (request.args.size() < 3) 
				{
                    return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters for key");
                }
                channel->setKey(request.args[2]);
                reply += " " + request.args[2];
            } 
			else 
			{
                channel->setKey("");
            }
            break;
        }
        case 'l':
        {
            if (addMode) 
			{
                if (request.args.size() < 3) 
				{
                    return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters for limit");
                }
                std::istringstream iss(request.args[2]);
                int limit;
                iss >> limit;
                if (iss.fail()) 
				{
                    return _printMessage("461", this->_clients[i]->getNickName(), ":Invalid limit parameter");
                }
                channel->setUserLimit(limit);
                reply += " " + request.args[2];
            } 
			else 
			{
                channel->removeUserLimit();
            }
            break;
        }
        default:
            return _printMessage("501", this->_clients[i]->getNickName(), ":Unknown MODE flag");
    }

    reply += "\n";
    _sendToAllUsers(channel, i, reply);

    return _printMessage("324", this->_clients[i]->getNickName(), channelName + " " + mode);
}
