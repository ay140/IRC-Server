/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzouk <amarzouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 11:29:49 by amarzouk          #+#    #+#             */
/*   Updated: 2024/07/25 11:29:54 by amarzouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string fillIt(std::string str, size_t len)
{
	if (str.length() < len)
		str.insert(str.end(), len - str.length(), ' ');
	else
	{
		str.erase(str.begin() + len - 1, str.end());
		str.append(".");
	}
	return (str);
};

const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
};
