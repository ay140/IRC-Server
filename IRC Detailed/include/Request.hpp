/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:58:13 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 13:44:55 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"

class Request
{
	private:
		Request & operator = (const Request &rhs);

	public:
		std::vector<std::string>	args; // the arugments of the command
		std::string					command; // the command is the first word in the message
		bool						invalidMessage; // flag to check if the message is invalid

		Request();
		Request(const std::vector<std::string> &splittedMessage); // New constructor
		~Request();
		Request(const Request &x);
};

#endif

