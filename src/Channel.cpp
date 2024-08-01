/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:09:13 by amarzouk          #+#    #+#             */
/*   Updated: 2024/08/01 09:04:33 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel()
    : _onlineUsers(0), _prefix(), _creator(NULL), _name(), _key(), _topic(),
      _members(), _operators(), _voice(), _banned(), _inviteOnly(false), _topicRestricted(false), _userLimit(-1)  {}


Channel::Channel(const std::string& channelName, Client* creator)
    : _onlineUsers(1), _prefix(), _creator(creator), _name(channelName), _key(), _topic(),
      _members(), _operators(), _voice(), _banned(), _inviteOnly(false), _topicRestricted(false) , _userLimit(-1)
{
    this->_operators.insert(std::make_pair(creator->getClientfd(), creator));
}

Channel::Channel(const std::string& channelName, const std::string& channelKey, Client* creator)
    : _onlineUsers(1), _prefix(), _creator(creator), _name(channelName), _key(channelKey), _topic(),
      _members(), _operators(), _voice(), _banned(), _inviteOnly(false), _topicRestricted(false) , _userLimit(-1)
      {
    this->_operators.insert(std::make_pair(creator->getClientfd(), creator));
}

Channel::Channel(const Channel& x)
    : _onlineUsers(x._onlineUsers), _prefix(x._prefix), _creator(x._creator),
      _name(x._name), _key(x._key), _topic(x._topic),
      _members(x._members), _operators(x._operators), _voice(x._voice), _banned(x._banned),
      _inviteOnly(x._inviteOnly), _topicRestricted(x._topicRestricted), _userLimit(x._userLimit) {}

Channel& Channel::operator=(const Channel& rhs) {
    if (this != &rhs) {
        this->_prefix = rhs._prefix;
        this->_creator = rhs._creator;
        this->_onlineUsers = rhs._onlineUsers;
        this->_name = rhs._name;
        this->_key = rhs._key;
        this->_topic = rhs._topic;
        this->_members = rhs._members;
        this->_operators = rhs._operators;
        this->_voice = rhs._voice;
        this->_banned = rhs._banned;
        this->_inviteOnly = rhs._inviteOnly;
        this->_topicRestricted = rhs._topicRestricted;
        this->_userLimit = rhs._userLimit;
    }
    return *this;
}

Channel::~Channel() {}

char							const &Channel::getPrefix()			const { return this->_prefix; };
int								const &Channel::getOnlineUsers()	const { return this->_onlineUsers; };
std::string						const &Channel::getName() 			const { return this->_name; };
std::string						const &Channel::getKey()			const { return this->_key; };
std::string						const &Channel::getTopic()			const { return this->_topic; };
std::map<int, Client *>			const &Channel::getMembers()		const { return this->_members; };
std::map<int, Client *>			const &Channel::getOperators()		const { return this->_operators; };
std::map<int, Client *>			const &Channel::getVoice()			const { return this->_voice; };

Client*		Channel::getCreator() const { return (this->_creator); };

void	Channel::setPrefix(char prefix)			{ this->_prefix = prefix; };
void	Channel::setOnlineUsers(int online)		{ this->_onlineUsers = online; };
void	Channel::setName(std::string name)		{ this->_name = name; };
void	Channel::setKey(std::string key)		{ this->_key = key; };
void	Channel::setTopic(std::string topic)	{ this->_topic = topic; };

int Channel::addMember(Client* member) 
{
    if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end()) 
	{
        return BANNEDFROMCHAN;
    }
    if (this->_members.find(member->getClientfd()) == this->_members.end()) 
	{
        this->_members.insert(std::make_pair(member->getClientfd(), member));
        this->_onlineUsers++;
        return USERISJOINED;
    }
    return (-1);
}

int Channel::addOperator(Client* member) 
{
    if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end()) 
	{
        return BANNEDFROMCHAN;
    }
    
    if (this->_operators.find(member->getClientfd()) == this->_operators.end()) 
	{
        this->_operators.insert(std::make_pair(member->getClientfd(), member));
        this->_onlineUsers++;
        return USERISJOINED;
    }
    
    return (-1);
}

int Channel::banUser(Client* member) 
{
    if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end()) 
	{
        return BANNEDFROMCHAN;
    }
    this->_banned.push_back(member->getNickName());
    return USERISBANNED;
}

void Channel::removeOperator(int i) 
{
    this->_operators.erase(i);
    if (this->_onlineUsers > 0) 
	{
        this->_onlineUsers--;
    }
}

void Channel::removeVoice(int i) 
{
    this->_voice.erase(i);
    if (this->_onlineUsers > 0) 
	{
        this->_onlineUsers--;
    }
}

void Channel::removeBanned(const std::string& nickName) 
{
    std::vector<std::string>::iterator it = std::find(this->_banned.begin(), this->_banned.end(), nickName);
    if (it != this->_banned.end()) 
	{
        this->_banned.erase(it);
    }
}

void Channel::removeMember(int i) 
{
    this->_members.erase(i);
    if (this->_onlineUsers > 0) 
	{
        this->_onlineUsers--;
    }
}

std::map<int, Client*> Channel::getAllUsers() const 
{
    std::map<int, Client*> allUsers(this->_members.begin(), this->_members.end());
    allUsers.insert(this->_operators.begin(), this->_operators.end());
    allUsers.insert(this->_voice.begin(), this->_voice.end());
    return allUsers;
}


std::pair<Client*, int> Channel::findUserRole(int i) const 
{
    std::map<int, Client*>::const_iterator it = this->_operators.find(i);
    if (it != this->_operators.end()) 
	{
        return std::make_pair(it->second, 1);
    }
    it = this->_members.find(i);
    if (it != this->_members.end()) 
	{
        return std::make_pair(it->second, 0);
    }
    it = this->_voice.find(i);
    if (it != this->_voice.end()) 
	{
        return std::make_pair(it->second, 2);
    }
    return std::make_pair(static_cast<Client*>(NULL), -1);
}


std::string Channel::listAllUsers() const 
{
    std::string allUsers(":");
    std::map<int, Client*>::const_iterator it;

    for (it = this->_operators.begin(); it != this->_operators.end(); ++it) 
    {
        if (it->second) 
        {
            allUsers.append("@").append(it->second->getNickName()).append(" ");
        }
    }
    for (it = this->_members.begin(); it != this->_members.end(); ++it) 
    {
        if (it->second) 
        {
            allUsers.append(it->second->getNickName()).append(" ");
        }
    }
    for (it = this->_voice.begin(); it != this->_voice.end(); ++it) 
    {
        if (it->second) 
        {
            allUsers.append("+").append(it->second->getNickName()).append(" ");
        }
    }
    return allUsers;
}



bool Channel::isEmpty() const 
{
    return this->_members.empty();
}

bool Channel::isOperator(Client* member) const 
{
    return this->_operators.find(member->getClientfd()) != this->_operators.end();
}

bool Channel::isMember(int clientFd) const 
{
        return _members.find(clientFd) != _members.end() || _operators.find(clientFd) != _operators.end() || _voice.find(clientFd) != _voice.end();
    }

    void Channel::setInviteOnly(bool mode) 
    {
        _inviteOnly = mode;
    }

    bool Channel::getInviteOnly() const 
    {
        return _inviteOnly;
    }

    void Channel::setTopicRestricted(bool mode) 
    {
        _topicRestricted = mode;
    }

    bool Channel::getTopicRestricted() const 
    {
        return _topicRestricted;
    }


    void Channel::setUserLimit(int limit) 
    {
        _userLimit = limit;
    }

    int Channel::getUserLimit() const 
    {
        return _userLimit;
    }

    void Channel::removeUserLimit() 
    {
        _userLimit = -1;
    }