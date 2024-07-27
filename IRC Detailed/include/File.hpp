/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayman_marzouk <ayman_marzouk@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:57:31 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/27 15:40:48 by ayman_marzo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
#define FILE_HPP

#include "Server.hpp"

class File
{
	private:
		File();
		File& operator=(const File &rhs);
	public:
		std::string	Name;
		std::string Path;
		std::string sender;
		std::string receiver;

		File( std::string Name, std::string Path, std::string Sender, std::string Receiver );
		File(const File &x);
		~File();
};

#endif
