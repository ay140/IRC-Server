/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:10:20 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/30 11:15:31 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::_parsing(const std::string& message, int i) 
{
    Request request(_splitRequest(message));

    if (request.invalidMessage)
        return "Invalid message!\n";
    if (request.command == "PASS")
        return _setPassWord(request, i);
    else if (request.command == "NICK")
        return _setNickName(request, i);
    else if (request.command == "USER")
        return _setUserName(request, i);
    else if (request.command == "OPER")
        return _setOper(request, i);
    else if (request.command == "MODE")
        return _setMode(request, i);
    else if (request.command == "PRIVMSG")
        return _privmsg(request, i);
    else if (request.command == "NOTICE")
        return _notice(request, i);
    else if (request.command == "HELP")
        return _printHelpInfo();
    else if (request.command == "JOIN")
        return _joinChannel(request, i);
    else if (request.command == "TOPIC")
        return _topic(request, i);
    else if (request.command == "KICK")
        return _kick(request, i);
    else if (request.command == "PART")
        return _part(request, i);
    else if (request.command == "QUIT")
        return _quit(request, i);
    else if (request.command == "SENDFILE")
        return _sendFile(request, i);
    else if (request.command == "GETFILE")
        return _getFile(request, i);
    else if (request.command == "BOT")
        return _MyBot(request, i);
    else if (request.command == "CAP")
        return _handleCAP(request, i);
    else if (request.command == "WHOIS")
        return _handleWHOIS(request, i);
    else if (request.command == "PING")
        return _handlePING(request, i);
    else
        return "Invalid command\n";
}

std::string Server::_handleWHOIS(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
    {
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }
    if (request.args.size() < 1) 
    {
        return _printMessage("431", this->_clients[i]->getNickName(), ":No nickname given");
    }
    const std::string& target = request.args[0];
    for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) 
    {
        if (it->second->getNickName() == target) 
        {
            Client* client = it->second;
            return _printMessage("311", this->_clients[i]->getNickName(), target + " " + client->getUserName() + " " + client->getHost() + " * :" + client->getFullName());
        }
    }
    return _printMessage("401", this->_clients[i]->getNickName(), target + " :No such nick/channel");
}

std::string Server::_handlePING(Request request, int i) 
{
    (void)i;
    if (request.args.size() < 1) 
    {
        return "PONG :\n";
    } else 
    {
        return "PONG " + request.args[0] + "\n";
    }
}

std::string Server::_handleCAP(Request request, int i) 
{
    (void)i;
    if (request.args.size() == 0) {
        return "CAP * LS :\n"; // Empty response for CAP without arguments
    }
    if (request.args.size() > 1 && request.args[0] == "LS" && request.args[1] == "302") {
        return "CAP * LS :multi-prefix sasl account-notify extended-join\n";
    }
    return "";
}

std::string Server::_notice(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
    }

    const std::string& target = request.args[0];
    const std::string& message = request.args[1];

    // Check if the target user exists
    int userFd = _findFdByNickName(target);
    if (userFd == USERNOTFOUND) 
	{
        return _printMessage("401", this->_clients[i]->getNickName(), target + " :No such nick");
    }

    _privToUser(target, message, "NOTICE", i);
    return "";
}

int Server::_findFdByNickName(const std::string& nickName) const 
{
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) 
	{
        if (it->second->getNickName() == nickName) 
		{
            return it->second->getClientfd();
        }
    }
    return USERNOTINCHANNEL;
}

std::string Server::_topic(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.empty()) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters");
    }

    std::map<std::string, Channel*>::iterator it = this->_allChannels.find(request.args[0]);
    if (it == this->_allChannels.end()) 
	{
        // 403: ERR_NOSUCHCHANNEL - No such channel exists
        return _printMessage("403", this->_clients[i]->getNickName(), request.args[0] + " :No such channel");
    }

    Channel* channel = it->second;
    if (request.args.size() == 1) 
	{
        if (channel->getTopic().empty()) {
            // 331: RPL_NOTOPIC - No topic is set
            return _printMessage("331", this->_clients[i]->getNickName(), request.args[0] + " :No topic is set");
        } 
		else 
		{
            // 332: RPL_TOPIC - Current topic
            return _printMessage("332", this->_clients[i]->getNickName(), request.args[0] + " :" + channel->getTopic());
        }
    }

    std::pair<Client*, int> user = channel->findUserRole(i);
    if (user.second == 1) 
	{ // User is an operator
        channel->setTopic(request.args[1]);
        std::string reply = "TOPIC " + channel->getName() + " :" + request.args[1] + "\n";
        _sendToAllUsers(channel, i, reply);
    } 
	else if (user.second == -1) 
	{ // User is not in the channel
        // 442: ERR_NOTONCHANNEL - User is not on the specified channel
        return _printMessage("442", this->_clients[i]->getNickName(), request.args[0] + " :You're not on that channel");
    } 
	else 
	{ // User is not an operator
        // 482: ERR_CHANOPRIVSNEEDED - You're not a channel operator
        return _printMessage("482", this->_clients[i]->getNickName(), request.args[0] + " :You're not channel operator");
    }

    return "";
}

bool Server::_validMode(Request request) 
{
    if (request.args.size() < 2) 
	{
        return false;
    }
    const std::string& modeArg = request.args[1];
    if (modeArg.length() != 2 || (modeArg[0] != '-' && modeArg[0] != '+')) 
	{
        return false;
    }
    char c = modeArg[1];
    if (c != 'a' && c != 'i' && c != 'w' && c != 'r' && c != 'o' && c != 'O' && c != 's') 
	{
        return false;
    }
    return true;
}

std::string	Server::_printUserModes(std::string ret, int i)
{
	ret.append("a: " + to_string(this->_clients[i]->getMode('a')));
	ret.append("\ni: " + to_string(this->_clients[i]->getMode('i')));
	ret.append("\nw: " + to_string(this->_clients[i]->getMode('w')));
	ret.append("\nr: " + to_string(this->_clients[i]->getMode('r')));
	ret.append("\no: " + to_string(this->_clients[i]->getMode('o')));
	ret.append("\nO: " + to_string(this->_clients[i]->getMode('O')));
	ret.append("\ns: " + to_string(this->_clients[i]->getMode('s')) + "\n");
	return ret;
}

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

std::string Server::_setOper(Request request, int i) 
{
    if (!this->_clients[i]->getRegistered()) 
	{
        // 451: ERR_NOTREGISTERED - Client must register before performing this action
        return _printMessage("451", this->_clients[i]->getNickName(), ":You have not registered");
    }

    if (request.args.size() < 2) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), "PASS :Not enough parameters, should be OPER <username> <password>");
    }

    if (request.args[0] != "ADMIN" || request.args[1] != "BATMAN") 
	{
        // 464: ERR_PASSWDMISMATCH - Username/Password incorrect
        return _printMessage("464", this->_clients[i]->getNickName(), ":Username/Password incorrect");
    }

    this->_clients[i]->setIsOperator(true);
    // 381: RPL_YOUREOPER - You are now an IRC operator
    return _printMessage("381", this->_clients[i]->getNickName(), ":You are now an IRC operator");
}

std::string	Server::_setPassWord(Request request, int i)
{
	if (request.args.size() < 1)
		return (_printMessage("461", this->_clients[i]->getNickName(), "PASS :Not enough parameters"));
	if (this->_clients[i]->getRegistered())
		return (_printMessage("462", this->_clients[i]->getNickName(), ":Unauthorized command (already registered)"));
	if (request.args[0] != this->_password)
		return (_printMessage("997", this->_clients[i]->getNickName(), ":Incorrect password"));
	else
		this->_clients[i]->setAuth(true);
	return ("");
};

std::string Server::_setNickName(Request request, int i) 
{
    if (!this->_clients[i]->getAuth()) 
    {
        // 998: Custom error for authentication required
        return _printMessage("998", this->_clients[i]->getNickName(), ":You need to authenticate first");
    }

    if (request.args.size() < 1) 
    {
        // 431: ERR_NONICKNAMEGIVEN - No nickname given
        return _printMessage("431", this->_clients[i]->getNickName(), ":No nickname given");
    }

    if (request.args.size() > 1) 
	{
        // 431: ERR_NONICKNAMEGIVEN - No nickname given
        return _printMessage("431", this->_clients[i]->getNickName(), ":NICK should be only one Parameter");
    }
    
    // Check if the client has already set a nickname
    if (!this->_clients[i]->getNickName().empty()) 
    {
        // 999: Custom error for nickname already set
        return _printMessage("999", this->_clients[i]->getNickName(), ":Nickname can only be set once");
    }

    const std::string& nickName = request.args[0];
    for (std::string::const_iterator it = nickName.begin(); it != nickName.end(); ++it) 
    {
        if (!isalnum(*it) && *it != '-' && *it != '\r') 
        {
            // 432: ERR_ERRONEUSNICKNAME - Erroneous nickname
            return _printMessage("432", this->_clients[i]->getNickName(), nickName + " :Erroneous nickname");
        }
    }

    if (std::find(this->_clientNicknames.begin(), this->_clientNicknames.end(), nickName) != this->_clientNicknames.end()) 
    {
        // 433: ERR_NICKNAMEINUSE - Nickname is already in use
        return _printMessage("433", this->_clients[i]->getNickName(), nickName + " :Nickname is already in use");
    }

    this->_clients[i]->setNickName(nickName);
    this->_clientNicknames.push_back(nickName);

    if (!this->_clients[i]->getUserName().empty()) 
    {
        this->_clients[i]->setID(nickName + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
        this->_clients[i]->setRegistered(true);
        // 001: RPL_WELCOME - Welcome to the Internet Relay Network
        return _printMessage("001", nickName, "Welcome to the Internet Relay Network " + this->_clients[i]->getID());
    }
    return "";
}


std::string Server::_setUserName(Request request, int i) 
{
    if (!this->_clients[i]->getAuth()) 
	{
        // 998: Custom error for authentication required
        return _printMessage("998", this->_clients[i]->getNickName(), ":You need to authenticate first");
    }

    if (this->_clients[i]->getRegistered()) 
	{
        // 462: ERR_ALREADYREGISTRED - Unauthorized command (already registered)
        return _printMessage("462", this->_clients[i]->getNickName(), ":Unauthorized command (already registered)");
    }

    if (request.args.size() < 4) 
	{
        // 461: ERR_NEEDMOREPARAMS - Not enough parameters provided for the command
        return _printMessage("461", this->_clients[i]->getNickName(), "command should be USER <username> <mode> <unused> <realname>");
    }

    this->_clients[i]->setUserName(request.args[0]);
    this->_clients[i]->setFullName(request.args[3]);

    if (!this->_clients[i]->getNickName().empty()) 
	{
        this->_clients[i]->setID(this->_clients[i]->getNickName() + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
        this->_clients[i]->setRegistered(true);
        // 001: RPL_WELCOME - Welcome to the Internet Relay Network
        return _printMessage("001", this->_clients[i]->getNickName(), "Welcome to the Internet Relay Network " + this->_clients[i]->getID());
    }
    return "";
}

std::string Server::_quit(Request request, int i) 
{
    std::string ret = this->_clients[i]->getUserPrefix() + "QUIT ";
    if (request.args.size())
        ret.append(":" + request.args[0] + "\n");
    else
        ret.append("\n");
    std::map<std::string, Channel *> channels = this->_clients[i]->getJoinedChannels();
    std::map<std::string, Channel *>::iterator it = channels.begin();
    while (it != channels.end()) {
        _sendToAllUsers(it->second, i, ret);
        it++;
    }
    this->_clients[i]->leaveAllChannels();
    std::string nickname = this->_clients[i]->getNickName();
    if (!nickname.empty()) {
        this->_clientNicknames.erase(std::remove(this->_clientNicknames.begin(), this->_clientNicknames.end(), nickname), this->_clientNicknames.end());
    }
    close(this->_clients[i]->getClientfd());
    _removeFromPoll(i -3);
    return ("QUIT");
}


std::string Server::_printHelpInfo() 
{
    std::string helpInfo;

    helpInfo.append(GREEN);
    helpInfo.append("STEP 1: PASS\n");
    helpInfo.append(RESET);
    helpInfo.append("\tUse PASS command to set a password. e.g: PASS [Server Password]\n\n");

    helpInfo.append(GREEN);
    helpInfo.append("STEP 2: NICK\n");
    helpInfo.append(RESET);
    helpInfo.append("\tUse NICK command to set a nickname. e.g: NICK Batman\n\n");

    helpInfo.append(GREEN);
    helpInfo.append("STEP 3: USER\n");
    helpInfo.append(RESET);
    helpInfo.append("\tUse USER command to register your username and fullname. e.g: USER Batman * * :Bruce Wayne\n\n");

    return helpInfo;
}

