/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHandling.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:16 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 12:06:53 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::_ClientRequest(int index) 
{
    char buf[6000];
    int sender_fd = this->_pfds[index].fd;
    int nbytes = recv(sender_fd, buf, sizeof(buf) - 1, 0); // -1 to leave space for null-terminator

    if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available right now, try again later
            return;
        } else {
            std::cout << "recv() error: " << strerror(errno) << std::endl;
            close(sender_fd);
            _removeFromPoll(index);
        }
    } else if (nbytes == 0) {
        std::cout << "[" << currentDateTime() << "]: socket " << sender_fd << " hung up" << std::endl;
        close(sender_fd);
        _removeFromPoll(index);
    } else {
        buf[nbytes] = '\0'; // Null-terminate the received data
        std::string message(buf);

        // Remove trailing '\r' or '\n' if present
        while (!message.empty() && (message.back() == '\r' || message.back() == '\n')) {
            message.pop_back();
        }
        
        std::string ret = _parsing(message, this->_pfds[index].fd);
        if (!ret.empty() && send(sender_fd, ret.c_str(), ret.length(), 0) == -1) {
            std::cout << "send() error: " << strerror(errno) << std::endl;
        }
    }
    memset(buf, 0, sizeof(buf)); // Clear the buffer
}



Request	Server::_splitRequest(const std::string& req)
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

