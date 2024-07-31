/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 05:00:55 by codespace         #+#    #+#             */
/*   Updated: 2024/07/31 08:13:58 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_invitedToChannel(const std::string& channelName, const std::string& targetNick, int fd)
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName);
    if (it == this->_allChannels.end())
    {
        // 403: ERR_NOSUCHCHANNEL - No such channel exists
        return _printMessage("403", this->_clients[fd]->getNickName(), channelName + " :No such channel");
    }

    Channel* channel = it->second;
    std::pair<Client *, int> user = channel->findUserRole(fd);
    if (user.second != 1)
    {
        if (user.second == -1)
        {
            // 442: ERR_NOTONCHANNEL - User is not on the specified channel
            return _printMessage("442", this->_clients[fd]->getNickName(), channelName + " :You're not on that channel");
        }
        else
        {
            // 482: ERR_CHANOPRIVSNEEDED - You're not a channel operator
            return _printMessage("482", this->_clients[fd]->getNickName(), channelName + " :You're not channel operator");
        }
    }

    int targetFd = _findFdByNickName(targetNick);
    if (targetFd == USERNOTINCHANNEL)
    {
        // 401: ERR_NOSUCHNICK - No such nick/channel
        return _printMessage("401", this->_clients[fd]->getNickName(), targetNick + " :No such nick");
    }
    if (!this->_clients[targetFd]->getRegistered())
    {
        // 451: ERR_NOTREGISTERED - Target client must register before performing this action
        return _printMessage("451", this->_clients[fd]->getNickName(), targetNick + " :User is not registered");
    }

    if (fd == targetFd)
    {
        // 484: ERR_CANNOTINVITESELF - You cannot invite yourself
        return _printMessage("484", this->_clients[fd]->getNickName(), channelName + " :You cannot invite yourself");
    }
    if (channel->isMember(targetFd))
    {
        // 443: ERR_USERONCHANNEL - User is already on that channel
        return _printMessage("443", this->_clients[fd]->getNickName(), targetNick + " " + channelName + " :is already on channel");
    }
    int result = channel->addMember(this->_clients[targetFd]);
    if (result == BANNEDFROMCHAN)
    {
        return _printMessage("474", this->_clients[fd]->getNickName(), targetNick + " :Cannot join channel (+b)");
    }
    _sendall(targetFd, _printMessage("341", this->_clients[targetFd]->getNickName(), channelName + " :You have been invited to the channel"));
    _sendall(targetFd, _printMessage("332", this->_clients[targetFd]->getNickName(), channelName + " :" + channel->getTopic()));
    _sendall(targetFd, _printMessage("353", this->_clients[targetFd]->getNickName() + " = " + channelName, channel->listAllUsers()));
    _sendall(targetFd, _printMessage("366", this->_clients[targetFd]->getNickName() + " " + channelName, ":End of NAMES list"));

 _sendall(fd, _printMessage("341", this->_clients[fd]->getNickName(), targetNick + " has been invited to " + channelName));
    return "";
}

std::string Server::_invite(Request request, int fd)
{
    if (!this->_clients[fd]->getRegistered())
    {
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2)
    {
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters. Usage: INVITE <nickname> <channel>");
    }

    std::vector<std::string> channels = _commaSeparator(request.args[1]);
    std::string targetNick = request.args[0];

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        std::string result = _invitedToChannel(*it, targetNick, fd);
        if (!result.empty())
        {
            return result;
        }
    }
    return "";
}
