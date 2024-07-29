/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:58:13 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/29 09:29:42 by codespace        ###   ########.fr       */
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
		std::vector<std::string>	args;
		std::string					command;
		bool						invalidMessage;

		Request();
		Request(const std::vector<std::string> &splittedMessage);
		Request(const Request &x);
		~Request();
};

#endif
