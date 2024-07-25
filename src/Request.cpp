/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:02 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 11:29:09 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"


Request::Request(): args(), command(), invalidMessage(false) {};
Request::Request( const Request& x ) { *this = x; };
Request::~Request() {};

Request & Request::operator= ( const Request& rhs )
{
	if (this == &rhs)
		return (*this);
	this->args = rhs.args;
	this->command = rhs.command;
	this->invalidMessage = rhs.invalidMessage;
	return (*this);
};

