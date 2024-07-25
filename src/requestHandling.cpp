/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHandling.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:16 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 12:12:59 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_ClientRequest(int i)
{
    char buf[6000];
    int sender_fd = this->_pfds[i].fd;
    int nbytes = recv(sender_fd, buf, sizeof(buf), 0);

    if (nbytes <= 0)
    {
        if (nbytes == 0)
            std::cout << "[" << currentDateTime() << "]: socket " << sender_fd << " hung up" << std::endl;
        else
            std::cout << "recv() error: " << strerror(errno) << std::endl;

        close(sender_fd);
        _removeFromPoll(i);
    }
    else
    {
        std::string message(buf, strlen(buf) - 1);
        
        // Check if the last character is '\r' and remove it
        if (!message.empty() && message[message.size() - 1] == '\r')
            message.erase(message.size() - 1);

        std::string ret = _parsing(message, this->_pfds[i].fd);
        if (send(sender_fd, ret.c_str(), ret.length(), 0) == -1)
            std::cout << "send() error: " << strerror(errno) << std::endl;
    }
    memset(&buf, 0, sizeof(buf));
}


Request	Server::_splitRequest(std::string req)
{
	Request	request;
	size_t	i = 0;
	size_t	j = 0;

	if (req[i] == ' ' || !req[i]) {
		request.invalidMessage = true;
		return (request);
	}
	j = i;
	while (req[i])
	{
		if (req[i] == ' ')
		{
			if (req[i + 1] == ' ') {
				request.invalidMessage = true;
				return (request);
			}
			request.args.push_back(req.substr(j, i - j));
			while (req[i] == ' ')
				i++;
			j = i;
		}
		if (req[i] == ':')
		{
			if (req[i - 1] != ' ') {
				request.invalidMessage = true;
				return (request);
			}
			request.args.push_back(req.substr(i + 1, req.length() - i));
			request.command = request.args[0];
			request.args.erase(request.args.begin());
			return (request);
		}
		i++;
	}

	if (i && req[j])
		request.args.push_back(req.substr(j, i - j));
	request.command = request.args[0];
	request.args.erase(request.args.begin());
	return (request);
};

