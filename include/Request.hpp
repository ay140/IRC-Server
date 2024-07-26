/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:58:13 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/26 11:21:04 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"

class Request
{
	public:
		std::vector<std::string>	args;
		std::string					command;
		bool						invalidMessage;

		Request();
		Request(const Request &x);
		Request & operator = (const Request &rhs);
		~Request();
};

#endif
