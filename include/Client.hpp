/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:56:52 by amarzouk          #+#    #+#             */
/*   Updated: 2024/08/01 09:41:12 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"


# define GREEN "\e[1;32m"
# define RESET "\e[0m"
# define RED "\e[1;91m"
# define CYAN "\e[1;36m"
# define YELLOW "\e[1;33m"
# define PURPLE "\e[1;35m"
# define BLUE "\e[1;34m"
# define GRAY "\033[1;2m"
# define ED "\033[0m"

struct Modes
{
	bool	away;
	bool	invisible;
	bool	wallops;
	bool	restricted;
	bool	op;
	bool	localOp;
	bool	server;
};

std::string fillIt(const std::string& str, size_t len);
const std::string	currentDateTime();

class Client
{
	private:
		Client();
		Client(const Client & x);
		Client& operator=(const Client & rhs);
		int									_clientfd;
		bool								_Auth;
		bool								_Registered;
		bool								_isOperator;
		std::string							_NickName;
		std::string							_UserName;
		std::string							_FullName;
		std::string							_ID;
		const std::string					_Host;
		struct sockaddr_storage				_remotaddr;
		socklen_t							_addrlen;
		struct Modes						_modes;
		std::map<std::string, Channel *>	_joinedChannels;

	public:
		Client(int fd);
		~Client();

		/*             Getters                         */
		int									getClientfd()		const;
		int									getRegistered()		const;
		int									getisOperator()		const;
		int									getMode(char mode)	const;
		bool								getAuth()			const;
		std::string							getUserName()		const;
		std::string							getNickName()		const;
		std::string							getFullName()		const;
		std::string							getID()				const;
		std::string							getHost()			const;
		std::string							getUserPrefix()		const;
		std::string							getUserInfo()		const;
		std::string							getAllChannels()	const;
		std::map<std::string, Channel *>	getJoinedChannels()	const;


		/*             Setters                         */
		void			setUserName(std::string UserName);
		void			setNickName(std::string NickName);
		void			setFullName(std::string FullName);
		void			setID(std::string ID);
		void			setClientfd(int clientfd);
		void			setAuth(int Auth);
		void			setRegistered(int Registered);
		void			setIsOperator(int isOperator);
		void			setMode(int value, char mode);
		void			joinChannel(const std::string& channelName, Channel *channel);
		void			leaveChannel(const std::string& channelName);
		std::string		leaveAllChannels();


		int				isJoined(const std::string& channelName) const;
		std::string		JoinedChannels() const;
};

#endif
