/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:57:31 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 10:57:33 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
#define FILE_HPP

#include "Server.hpp"

class File
{
	public:
		std::string	Name;
		std::string Path;
		std::string sender;
		std::string receiver;

	private:
		File();

	public:
		File( std::string Name, std::string Path, std::string Sender, std::string Receiver );
		File( const File &x );
		~File();
		File & operator = ( const File &rhs );
};

#endif
