/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyBot.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:10:35 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 00:48:01 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_getBotMessage() 
{
    std::string greeting = BLUE;
    greeting.append("██████████████████████████████████████████████████████████████████████████████████\n");
    greeting.append(BLUE "█                                                                                █\n");
    greeting.append(BLUE "█\t" RED " Usage: MyBot [COMMAND_NUMBER] [MORE_OPTIONS]" BLUE "\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t How Can I Help You: (You can use The following list of commands)\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[0]" RESET " : to List all Your stats" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[1]" RESET " : to List all Your Joined Channels" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[2]" RESET " : to see How many user online" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[3]" RESET " : to List all Channels in Server" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[4]" RESET " : to List stats of specific Channel" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█\t " CYAN "[5]" RESET " : to List Infos about the Server" BLUE "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t █\n");
    greeting.append(BLUE "█                                                                                █\n");
    greeting.append(BLUE "██████████████████████████████████████████████████████████████████████████████████\n\n\n\n");
    greeting.append(RESET);
    return greeting;
}

std::string Server::_MyBot(Request req, int i) 
{
    std::string greeting = _getBotMessage();

    if (req.args.empty()) 
	{
        return greeting;
    }

    if (req.args[0] == "0") 
	{
        return this->_clients[i]->getUserInfo();
    } 
	else if (req.args[0] == "1") 
	{
        return this->_clients[i]->getAllChannels();
    } 
	else if (req.args[0] == "2") 
	{
        return "Online Users: " + to_string(this->_online_c - 1) + "\n";
    } 
	else if (req.args[0] == "3") 
	{
        return _listAllChannels();
    } 
	else if (req.args[0] == "4") 
	{
        if (req.args.size() == 2) 
		{
            return _channelInfo(req.args[1], i);
        }
		else 
		{
            return "Usage of this Command: MYBOT 4 [CHANNEL NAME]\n";
        }
    } 
	else if (req.args[0] == "5") 
	{
        return _serverInfo();
    }
    return greeting;
}


std::string Server::_serverInfo() const 
{
    std::ostringstream server;
    server << "Server Name: " << this->_name << "\n";
    server << "Online Users: " << (this->_online_c - 1) << "\n";
    server << "Max Online Users: " << this->_max_online_c << "\n";
    server << "Number of Channels in the Server: " << this->_allChannels.size() << "\n";
    return server.str();
}


std::string Server::_channelInfo(const std::string& channelName, int i) const 
{
    std::map<std::string, Channel*>::const_iterator it = this->_allChannels.find(channelName);
    if (it != this->_allChannels.end()) 
	{
        std::map<int, Client*>::const_iterator clientIt = this->_clients.find(i);
        if (clientIt != this->_clients.end() && clientIt->second->isJoined(channelName)) 
		{
            std::ostringstream info;
            info << "Channel Name: " << it->second->getName() << "\n";
            info << "Channel Creator: " << it->second->getCreator()->getFullName() << "\n";
            info << "Online Users: " << it->second->getOnlineUsers() << "\n";
            info << "Channel Topic: " << it->second->getTopic() << "\n";
            return info.str();
        } 
		else 
		{
            return "You need to join the channel first\n";
        }
    }
    return "There's no channel named " + channelName + "!\n";
}

std::string Server::_listAllChannels() const 
{
    std::ostringstream channels;
    channels << YELLOW << "┌───────────────────────────────────────────────────────────────────────────────────┐\n";
    channels << "│                             CHANNELS LIST IN THE SERVER                            │\n";
    channels << "├──────────────┬──────────────┬────────────────────┬────────────────────────────────┤\n";
    channels << "│ Channel Name │ Online Users │ Creator Of Channel │ Channel Topic                   │\n";
    channels << "├──────────────┼──────────────┼────────────────────┼────────────────────────────────┤\n";

    std::map<std::string, Channel*>::const_iterator it = this->_allChannels.begin();
    while (it != this->_allChannels.end()) 
	{
        channels << "│ " << fillIt(it->first, 12) << " │ "
                 << fillIt(to_string(it->second->getOnlineUsers()), 12) << " │ "
                 << fillIt(it->second->getCreator()->getFullName(), 18) << " │ "
                 << fillIt(it->second->getTopic(), 32) << " │\n";
        it++;
    }

    if (this->_allChannels.empty()) 
	{
        channels << "│                                                                                   │\n";
        channels << "│                                NO CHANNEL IN THE SERVER                            │\n";
        channels << "│                                                                                   │\n";
    }

    channels << "└───────────────────────────────────────────────────────────────────────────────────┘\n";
    channels << RESET << "\n\n";

    return channels.str();
}

