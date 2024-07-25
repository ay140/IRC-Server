/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:58:13 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 10:58:14 by amarzouk         ###   ########.fr       */
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

	public:
		Request();
		~Request();
		Request( const Request &x );
		Request & operator = ( const Request &rhs );
};

#endif
