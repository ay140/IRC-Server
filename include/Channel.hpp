/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:56:23 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/31 06:38:52 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#define	USERALREADYJOINED 0
#define USERISJOINED 1
#define NOTINCHANNEL 2
#define BANNEDFROMCHAN 3
#define TOOMANYCHANNELS 4
#define BADCHANNELKEY 5
#define CHANNELISFULL 6
#define NOSUCHCHANNEL 7
#define USERISBANNED 8
#define BADCHANMASK 9
#define USERNOTINCHANNEL -1
#define USERNOTFOUND -1

#include "Server.hpp"

class Channel
{
	private:
		Channel();
		Channel(const Channel& x);
		Channel &operator=( const Channel& rhs );
		int								_onlineUsers;
		char							_prefix;
		Client*							_creator;
		std::string						_name;
		std::string						_key;
		std::string						_topic;
		std::map<int, Client *>			_members;
		std::map<int, Client *>			_operators;
		std::map<int, Client *>			_voice;
		std::vector<std::string>		_banned;
	    bool _inviteOnly;
    	bool _topicRestricted;
	public:
		Channel(const std::string& channelName, Client* creator);
		Channel(const std::string& channelName, const std::string& channelKey, Client* creator);
		~Channel();
			/*             Setters                         */
		void	setPrefix(char prefix);
		void	setOnlineUsers(int online);
		void	setName(std::string name);
		void	setKey(std::string key);
		void	setTopic(std::string topic);

 			/*             Getters                         */
		char							const &getPrefix()		const;
		int								const &getOnlineUsers()	const;
		std::string						const &getName() 		const;
		std::string						const &getKey()			const;
		std::string						const &getTopic()		const;
		std::map<int, Client *>			const &getMembers()		const;
		std::map<int, Client *>			const &getOperators()	const;
		std::map<int, Client *>			const &getVoice()		const;
		std::map<std::string, Client *>	const &getBanned()		const;
		Client*								   getCreator() 	const;
		std::map<int, Client *>				   getAllUsers() 	const;
		std::string							   listAllUsers() 	const;
		std::pair<Client *, int>			findUserRole(int i) const;

	/*             Modefiers functions                         */
		int		addMember(Client *member);
		int		addOperator(Client *member);
		int		banUser(Client *member);
		void	removeOperator(int i);
		void	removeVoice(int i);
		void	removeMember(int i);
		void	removeBanned(const std::string& nickName);

		bool isEmpty() const;
        bool isOperator(Client* member) const;
		bool isMember(int clientFd) const;

		void setInviteOnly(bool mode);
		void setTopicRestricted(bool mode);
		bool getInviteOnly() const;
		bool getTopicRestricted() const;
};

#endif