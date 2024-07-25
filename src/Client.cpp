/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:09:45 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 23:05:43 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client()
    : _clientfd(0), _Auth(false), _Registered(false), _isOperator(false),
      _NickName(), _UserName(), _FullName(), _Host("gotham"), _ID(),
      _remotaddr(), _addrlen(), _modes(), _joinedChannels() {}

Client::Client(int fd)
    : _clientfd(fd), _Auth(false), _Registered(false), _isOperator(false),
      _NickName(), _UserName(), _FullName(), _Host("gotham"), _ID(),
      _remotaddr(), _addrlen(), _modes(), _joinedChannels() {}

Client::Client(const Client& x)
    : _clientfd(x._clientfd), _Auth(x._Auth), _Registered(x._Registered),
      _isOperator(x._isOperator), _NickName(x._NickName), _UserName(x._UserName),
      _FullName(x._FullName), _Host(x._Host), _ID(x._ID),
      _remotaddr(x._remotaddr), _addrlen(x._addrlen), _modes(x._modes),
      _joinedChannels(x._joinedChannels) {}

Client& Client::operator=(const Client& rhs) {
    if (this != &rhs) {
        this->_clientfd = rhs._clientfd;
        this->_Auth = rhs._Auth;
        this->_Registered = rhs._Registered;
        this->_isOperator = rhs._isOperator;
        this->_NickName = rhs._NickName;
        this->_UserName = rhs._UserName;
        this->_FullName = rhs._FullName;
        // _Host is const and cannot be assigned to
        this->_ID = rhs._ID;
        this->_remotaddr = rhs._remotaddr;
        this->_addrlen = rhs._addrlen;
        this->_modes = rhs._modes;
        this->_joinedChannels = rhs._joinedChannels;
    }
    return *this;
}

Client::~Client() {}

std::string	Client::getUserName()		const { return (this->_UserName); };
std::string	Client::getNickName()		const { return (this->_NickName); };
std::string	Client::getFullName()		const { return (this->_FullName); };
std::string Client::getHost()			const { return (this->_Host); };
std::string Client::getID()				const { return (this->_ID); }
bool		Client::getAuth()			const { return (this->_Auth); };
int			Client::getClientfd()		const { return (this->_clientfd); };
int			Client::getRegistered()		const { return (this->_Registered); };
int			Client::getisOperator()		const { return (this->_isOperator); };

int Client::getMode(char mode) const 
{
    switch (mode) 
	{
        case 'a':
            return this->_modes.away;
        case 'i':
            return this->_modes.invisible;
        case 'w':
            return this->_modes.wallops;
        case 'r':
            return this->_modes.restricted;
        case 'o':
            return this->_modes.op;
        case 'O':
            return this->_modes.localOp;
        case 's':
            return this->_modes.server;
        default:
            return 0;
    }
}

void		Client::setUserName(std::string UserName)	{ this->_UserName = UserName; };
void		Client::setNickName( std::string NickName )	{ this->_NickName = NickName; };
void		Client::setFullName( std::string FullName )	{ this->_FullName = FullName; };
void		Client::setID( std::string ID )				{ this->_ID = ID; };
void		Client::setClientfd( int Clientfd )			{ this->_clientfd = Clientfd; };
void		Client::setRegistered( int Registered )		{ this->_Registered = Registered; };
void		Client::setAuth( int Auth )					{ this->_Auth = Auth; };
void		Client::setIsOperator(int isOperator)
{
	this->_isOperator = isOperator;
	this->_modes.op = isOperator;
	this->_modes.localOp = isOperator;
};
void		Client::setMode(int value, char mode)
{
	if (mode == 'i')
		this->_modes.invisible = value;
	else if (mode == 'w')
		this->_modes.wallops = value;
	else if (mode == 'r' && value == 1)
		this->_modes.restricted = value;
	else if (mode == 'o' && value == 0)
		this->_modes.op = value;
	else if (mode == 'O' && value == 0)
		this->_modes.localOp = value;
	else if (mode == 's')
		this->_modes.server = value;
};

int		Client::isJoined(const std::string& channelName) const
{
	if (this->_joinedChannels.find(channelName) != this->_joinedChannels.end())
		return (1);
	return (0);
};

void Client::joinChannel(const std::string& channelName, Channel* channel) 
{
    if (!isJoined(channelName)) 
	{
        this->_joinedChannels.insert(std::make_pair(channelName, channel));
    }
}

std::string Client::JoinedChannels() const 
{
    std::string channels;
    for (std::map<std::string, Channel*>::const_iterator it = this->_joinedChannels.begin(); it != this->_joinedChannels.end(); ++it) 
	{
        channels.append(BLUE).append(it->first).append(RESET).append(":\n")
                .append(YELLOW).append("\tChannel Name: ").append(RESET).append(it->first).append("\n")
                .append(YELLOW).append("\tChannel Name inside class: ").append(RESET).append(it->second->getName()).append("\n")
                .append(YELLOW).append("\tChannel Creator: ").append(RESET).append(it->second->getCreator()->getFullName()).append("\n");
    }
    return channels;
}

void Client::leaveChannel(const std::string& channelName) 
{
    this->_joinedChannels.erase(channelName);
}


std::string Client::leaveAllChannels() 
{
    std::map<std::string, Channel*>::iterator it = this->_joinedChannels.begin();
    while (it != this->_joinedChannels.end()) 
	{
        std::pair<Client*, int> user = it->second->findUserRole(this->_clientfd);
        if (user.second == 0) 
		{
            it->second->removeMember(this->_clientfd);
        } 
		else if (user.second == 1) 
		{
            it->second->removeOperator(this->_clientfd);
        } 
		else 
		{
            it->second->removeVoice(this->_clientfd);
        }
        user.first->leaveChannel(it->second->getName());
        it = this->_joinedChannels.begin();
    }
    return "";
}

std::string Client::getUserInfo() const 
{
    std::string userInfo;
    userInfo.append("User Name: ").append(this->_UserName).append("\n")
            .append("Full Name: ").append(this->_FullName).append("\n")
            .append("Nick Name: ").append(this->_NickName).append("\n")
            .append("Host: ").append(this->_Host).append("\n")
            .append("Joined Channels: ").append(to_string(this->_joinedChannels.size())).append("\n")
            .append("\n");
    return userInfo;
}

std::string Client::getAllChannels() const 
{
    std::string channels;
    channels.append(YELLOW "--------------------------------------------------------\n");
    channels.append("|     Channel Name     | Online Users | Channel Creator | Channel Topic             |\n");
    channels.append("--------------------------------------------------------\n");

    for (std::map<std::string, Channel*>::const_iterator it = this->_joinedChannels.begin(); it != this->_joinedChannels.end(); ++it) 
	{
        channels.append("| ")
                .append(fillIt(it->first, 20)).append(" | ")
                .append(fillIt(to_string(it->second->getOnlineUsers()), 12)).append(" | ")
                .append(fillIt(it->second->getCreator()->getFullName(), 16)).append(" | ")
                .append(fillIt(it->second->getTopic(), 24)).append(" |\n");
        channels.append("--------------------------------------------------------\n");
    }

    if (this->_joinedChannels.empty()) 
	{
        channels.append("| You have not joined any channels                      |\n");
        channels.append("--------------------------------------------------------\n");
    }

    channels.append(RESET "\n\n");
    return channels;
}


std::string		Client::getUserPrefix() const 
{ 
	return (":" + this->_NickName + "!" + this->_UserName + "@" + this->_Host + " "); 
};

std::map<std::string, Channel *> Client::getJoinedChannels() const 
{ 
	return (this->_joinedChannels); 
};
