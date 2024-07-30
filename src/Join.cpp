/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:26:27 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 23:14:49 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_joinChannel(Request request, int fd) 
{
    if (!this->_clients[fd]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[fd]->getNickName(), ":You have not registered");
    }

    if (request.args.empty()) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[fd]->getNickName(), ":Not enough parameters, useage: JOIN <#channel>{,<#channel>} [<key>{,<key>}]");
    }

    if (request.args[0] == "0") 
	{
        return this->_clients[fd]->leaveAllChannels();
    }
    
    std::vector<std::string> parsChannels = _commaSeparator(request.args[0]);
    if (request.args.size() > 2) 
    {
        return _printMessage("999", this->_clients[fd]->getNickName(), ":too many arguments, useage: JOIN <#channel>{,<#channel>} [<key>{,<key>}]");
    }
    std::vector<std::string> parsKeys;
    if (request.args.size() == 2) 
	{
        parsKeys = _commaSeparator(request.args[1]);
    }

    std::vector<std::string>::iterator itChannels = parsChannels.begin();
    std::vector<std::string>::iterator itKeys = parsKeys.begin();
    int result = 1;

    while (itChannels != parsChannels.end() && result == 1) 
	{
        // Check if the user is already joined to the channel
        if (this->_clients[fd]->isJoined(*itChannels)) 
        {
            return _printMessage("443", this->_clients[fd]->getNickName(), *itChannels + " :You're already on that channel");
        }
        
        if (itKeys != parsKeys.end()) 
		{
            result = _createPrvChannel(*itChannels, *itKeys, fd);
            itKeys++;
        } 
		else 
		{
            result = _createChannel(*itChannels, fd);
        }

        if (result == BADCHANMASK)
        {
            return _printMessage("476", this->_clients[fd]->getNickName(), *itChannels + " :Bad Channel Mask");
        }
        else if (result == BANNEDFROMCHAN)
        {
            return _printMessage("474", this->_clients[fd]->getNickName(), *itChannels + " :Cannot join channel (+b)");
        }
        else if (result == TOOMANYCHANNELS)
        {
            return _printMessage("405", this->_clients[fd]->getNickName(), *itChannels + " :You have joined too many channels");
        }
        else if (result == BADCHANNELKEY)
        {
            return _printMessage("475", this->_clients[fd]->getNickName(), *itChannels + " :Cannot join channel (+k)");
        }
        else if (result == CHANNELISFULL)
        {
            return _printMessage("471", this->_clients[fd]->getNickName(), *itChannels + " :Cannot join channel (+l)");
        }
        else if (result == NOSUCHCHANNEL)
        {
            return _printMessage("403", this->_clients[fd]->getNickName(), *itChannels + " :No such channel");
        }
        else if (!this->_clients[fd]->isJoined(*itChannels))
        {
            return _printMessage("999", this->_clients[fd]->getNickName(), *itChannels + " :Failed to join channel");
        }

        itChannels++;
    }
        
    return "";
}

int Server::_createChannel(const std::string& channelName, int creatorFd) 
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName);
    if (it == this->_allChannels.end()) 
	{
        if (channelName[0] != '&' && channelName[0] != '#' && channelName[0] != '+' && channelName[0] != '!') 
		{
            return BADCHANMASK;
        }
        Channel* channel = new Channel(channelName, this->_clients[creatorFd]);
        this->_allChannels.insert(std::make_pair(channelName, channel));
        this->_clients[creatorFd]->joinChannel(channelName, channel);
    } 
	else 
	{
        if (it->second->getKey().empty()) 
		{
            int result = 0;
            if (this->_clients[creatorFd]->getisOperator()) 
			{
                result = it->second->addOperator(this->_clients[creatorFd]);
            } 
			else 
			{
                result = it->second->addMember(this->_clients[creatorFd]);
            }
            if (result == USERISJOINED) 
			{
                this->_clients[creatorFd]->joinChannel(it->first, it->second);
            } 
			else if (result == USERALREADYJOINED) 
			{
                return USERALREADYJOINED;
            } 
			else if (result == BANNEDFROMCHAN) 
			{
                return BANNEDFROMCHAN;
            }
            _sendall(creatorFd, this->_clients[creatorFd]->getUserPrefix() + "JOIN " + channelName + "\n");
            _sendall(creatorFd, _printMessage("332", this->_clients[creatorFd]->getNickName(), channelName + " :" + it->second->getTopic()));
            _sendall(creatorFd, _printMessage("353", this->_clients[creatorFd]->getNickName() + " = " + channelName, it->second->listAllUsers()));
            _sendall(creatorFd, _printMessage("366", this->_clients[creatorFd]->getNickName() + " " + channelName, ":End of NAMES list"));
            std::string reply = "JOIN " + channelName + "\n";
            _sendToAllUsers(it->second, creatorFd, reply);
            return USERISJOINED;
        }
    }
    return USERISJOINED;
}

int Server::_createPrvChannel(const std::string& channelName, const std::string& channelKey, int creatorFd) 
{
    std::map<std::string, Channel *>::iterator it = this->_allChannels.find(channelName);
    if (it == this->_allChannels.end()) 
	{
        if (channelName[0] != '&' && channelName[0] != '#' && channelName[0] != '+' && channelName[0] != '!') 
		{
            return BADCHANMASK;
        }
        Channel* channel = new Channel(channelName, channelKey, this->_clients[creatorFd]);
        this->_allChannels.insert(std::make_pair(channelName, channel));
        this->_clients[creatorFd]->joinChannel(channelName, channel);
    } 
	else 
	{
        if (it->second->getKey().empty())
        {
            if (channelKey.empty()) 
            {
                std::cout << "No key provided for a protected channel" << std::endl;
                return BADCHANNELKEY; // No key provided for a protected channel
            }
        }
        if (it->second->getKey() == channelKey) 
		{
            int result = 0;
            if (this->_clients[creatorFd]->getisOperator()) 
			{
                result = it->second->addOperator(this->_clients[creatorFd]);
            } 
			else 
			{
                result = it->second->addMember(this->_clients[creatorFd]);
            }
            if (result == USERISJOINED) 
			{
                this->_clients[creatorFd]->joinChannel(it->first, it->second);
            } 
			else if (result == USERALREADYJOINED) 
			{
                return USERALREADYJOINED;
            } 
			else if (result == BANNEDFROMCHAN) 
			{
                return BANNEDFROMCHAN;
            }
            _sendall(creatorFd, this->_clients[creatorFd]->getUserPrefix() + "JOIN " + channelName + "\n");
            _sendall(creatorFd, _printMessage("332", this->_clients[creatorFd]->getNickName(), channelName + " :" + it->second->getTopic()));
            _sendall(creatorFd, _printMessage("353", this->_clients[creatorFd]->getNickName() + " = " + channelName, it->second->listAllUsers()));
            _sendall(creatorFd, _printMessage("366", this->_clients[creatorFd]->getNickName() + " " + channelName, ":End of NAMES list"));
            std::string reply = "JOIN " + channelName + "\n";
            _sendToAllUsers(it->second, creatorFd, reply);
            return USERISJOINED;
        } 
		else 
		{
            return BADCHANNELKEY;
        }
    }
    return USERISJOINED;
}

std::vector<std::string> Server::_commaSeparator(const std::string& arg) 
{
    std::vector<std::string> ret;
    size_t pos = 0;
    size_t start = 0;
    while ((pos = arg.find(",", start)) != std::string::npos) {
        ret.push_back(arg.substr(start, pos - start));
        start = pos + 1;
    }
    ret.push_back(arg.substr(start));
    return ret;
}
