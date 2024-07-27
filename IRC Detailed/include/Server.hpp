/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:58:52 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/27 20:40:57 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP
#define __SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <utility>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <sstream>
#include <fcntl.h>

class Client;
class Channel;
class Request;
class File;

class Server
{
	public:
		Server(const std::string& name, int max_online, const std::string& port, const std::string& password);
		~Server();

		void startServer(void);
	
	private:
		int									_online_c; // count of online clients
		int									_max_online_c; // max online clients
		int									_socketfd; // socket file descriptor
		std::string							_name; // server name
		std::string							_password; // server password
		std::string							_prefix; // server prefix
		struct pollfd						*_pfds; // pointer to an array of pollfd structures. The pollfd structure is used by the poll system call to monitor multiple file descriptors to see if I/O is possible on any of them.
		std::map<int, Client *>				_clients; // associates each client file descriptor, used to keep track of all connected clients and their respective information.
		std::vector<std::string>			_unavailableUserName; // list of unavailable usernames (already taken)
		std::vector<std::string>			_clientNicknames; // list of all client nicknames ensures that each nickname is unique and manages nickname assignments.
		std::map<std::string, File>			_files; // associates each file name with a File object, used to keep track of all files being transferred.
		std::map<std::string, Channel *>	_allChannels; // associates each channel name with a Channel object, used to keep track of all channels and their respective information.

		Server();
		Server(const Server& x);
		Server & operator=( const Server & rhs );
		
							/* setters */
		std::string						_setUserName(Request request, int i);
		std::string						_setNickName(Request request, int i);
		std::string						_setPassWord(Request request, int i);
		std::string						_setOper(Request request, int i);
		std::string						_setMode(Request request, int i);
		
							/* getters */
		std::string						_getPassword() const;

							/* privmsg */
		std::string						_privmsg(Request request, int i);
		std::string						_privToUser(const std::string& user, const std::string& message, const std::string& cmd, int i);
		std::string						_privToChannel(const std::string& channelName, const std::string& message, int i);
		
		
							/* Channel */
		std::string						_joinChannel(Request request, int i);
		int								_createPrvChannel(const std::string& channelName, const std::string& channelKey, int creatorFd);
		int								_createChannel(const std::string& channelName, int creatorFd);
		std::vector<std::string>		_commaSeparator(const std::string& arg);
		std::string						_part(Request request, int i);
		int								_partChannel(const std::string& channelName, int fd, const std::string& message, int isPart);
		
							/* File Transfer */
		std::string						_sendFile( Request request, int i );
		std::string						_getFile( Request request, int i );
		
							/* Bot */
		std::string						_MyBot( Request request, int i );
		std::string						_getBotMessage();
		std::string						_listAllChannels() const;
		std::string						_channelInfo(const std::string& channelName, int i) const;
		std::string						_serverInfo() const;
		
							/* kick */
		std::string						_kick(Request request, int i);
		std::string						_kickedFromChannel(const std::string& channelName, const std::string& message, const std::vector<std::string>& users, int i);

							/* server */
		void							_newClient(void);
		static void 					handle_signal(int signal);
		std::string						_welcomemsg(void);
		std::string						_printHelpInfo();
		std::string						_printMessage(std::string num, std::string nickname, std::string message);

							/* socket */
		void							_getSocket(const std::string& port);
	
							/* poll */
		void							_addToPoll(int newfd);
		void							_removeFromPoll(int i);
		
							/* request */
		void							_ClientRequest(int i);
		Request							_splitRequest(const std::string& req);
		
							/* send */
		int								_sendall(int destfd, std::string message);
		std::string						_sendToAllUsers( Channel *channel, int senderFd, std::string message);


							/* commands */
		std::string						_parsing(const std::string& message, int i);
		int								_findFdByNickName(const std::string& nickName) const;
		std::string						_notice(Request request, int i);
		std::string						_topic(Request request, int i);
		std::string						_printUserModes(std::string ret, int i);
		bool							_validMode(Request request);
		std::string						_quit(Request request, int i);
};

template <typename T>
std::string to_string(T value) 
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#include "Channel.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "File.hpp"

#endif