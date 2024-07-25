/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:56:52 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 22:38:56 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include <sstream>


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
		int									_clientfd;
		bool								_Auth;
		bool								_Registered;
		bool								_isOperator;
		std::string							_NickName;
		std::string							_UserName;
		std::string							_FullName;
		const std::string					_Host;
		std::string							_ID;
		struct sockaddr_storage				_remotaddr;
		socklen_t							_addrlen;
		struct Modes						_modes;
		std::map<std::string, Channel *>	_joinedChannels;

	// private:

	public:
		Client();
		Client( int fd );
		Client(const Client & x );
		~Client();
		Client & operator= (const Client & rhs );

	public: /*             Getters                         */
		std::string							getUserName()		const;
		std::string							getNickName()		const;
		std::string							getFullName()		const;
		std::string							getID()				const;
		std::string							getHost()			const;
		int									getClientfd()		const;
		bool								getAuth()			const;
		int									getRegistered()		const;
		int									getisOperator()		const;
		int									getMode(char mode)	const;
		std::string							getUserPrefix()		const;
		std::string							getUserInfo()		const;
		std::string							getAllChannels()	const;
		std::map<std::string, Channel *>	getJoinedChannels()	const;


	public: /*             Setters                         */
		void			setUserName(std::string UserName);
		void			setNickName(std::string NickName);
		void			setFullName(std::string FullName);
		// void			setHost(std::string Host);
		void			setID(std::string ID);
		void			setClientfd(int clientfd);
		void			setAuth(int Auth);
		void			setRegistered(int Registered);
		void			setIsOperator(int isOperator);
		void			setMode(int value, char mode);
		void			joinChannel(const std::string& channelName, Channel *channel );
		void			leaveChannel(const std::string& channelName);
		std::string		leaveAllChannels();

	public:
		int			isJoined(const std::string& channelName) const;

	public:
		std::string		JoinedChannels() const;
};


#endif
