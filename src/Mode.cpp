/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 06:28:51 by codespace         #+#    #+#             */
/*   Updated: 2024/07/31 07:33:57 by codespace        ###   ########.fr       */
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
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(request.args[0]);
    if (it == this->_allChannels.end()) 
	{
        // 403: ERR_NOSUCHCHANNEL - No such channel exists
        return _printMessage("403", this->_clients[i]->getNickName(), request.args[0] + " :No such channel");
    }

    Channel* channel = it->second;
    std::pair<Client *, int> user = channel->findUserRole(i);
    if (user.second != 1) 
	{
        if (user.second == -1) 
		{
            // 442: ERR_NOTONCHANNEL - User is not on the specified channel
            return _printMessage("442", this->_clients[i]->getNickName(), request.args[0] + " :You're not on that channel");
        } 
		else 
		{
            // 482: ERR_CHANOPRIVSNEEDED - You're not a channel operator
            return _printMessage("482", this->_clients[i]->getNickName(), request.args[0] + " :You're not channel operator");
        }
    }

    if (request.args.size() < 3 && (request.args[1][1] == 'k' || request.args[1][1] == 'o')) 
	{
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters for channel mode change");
    }

    bool addMode = (request.args[1][0] == '+');
    char mode = request.args[1][1];

    int targetFd = -1; // Initialize targetFd here

    switch (mode) 
	{
        case 'i': // Invite-only mode
            channel->setInviteOnly(addMode);
            break;
        case 't': // Topic restrictions mode
            channel->setTopicRestricted(addMode);
            break;
        case 'k': // Channel key (password)
            if (addMode) 
			{
                channel->setKey(request.args[2]);
            } 
			else 
			{
                channel->setKey("");
            }
            break;
        case 'o': // Operator privileges
            targetFd = _findFdByNickName(request.args[2]);
            if (targetFd == USERNOTINCHANNEL) 
			{
                // 401: ERR_NOSUCHNICK - No such nick/channel
                return _printMessage("401", this->_clients[i]->getNickName(), request.args[2] + " :No such nick");
            }
            if (addMode) 
			{
                channel->addOperator(this->_clients[targetFd]);
            } 
			else 
			{
                channel->removeOperator(targetFd);
            }
            break;
        default:
            // 501: ERR_UMODEUNKNOWNFLAG - Unknown MODE flag
            return _printMessage("501", this->_clients[i]->getNickName(), ":Unknown MODE flag");
    }

    // Notify the channel about the mode change
    std::string reply = ":" + this->_clients[i]->getUserPrefix() + " MODE " + request.args[0] + " " + request.args[1];
    if (request.args.size() == 3) 
	{
        reply += " " + request.args[2];
    }
    reply += "\n";
    _sendToAllUsers(channel, i, reply);

    // Confirmation message for the user
    std::string confirmation = "MODE change for channel " + request.args[0] + " has been successful\n";
    _sendall(i, confirmation);

    return "";
}
