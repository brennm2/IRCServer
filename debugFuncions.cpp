/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugFuncions.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 17:14:04 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/30 17:45:28 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void Ircserv::debugShowChannelInfo()
{
	for (std::map<std::string, std::vector<int> >::const_iterator channel = _channels.begin(); channel != _channels.end(); ++channel)
	{
		std::cout << "Channel: " << channel->first << std::endl;
		std::cout << "User FD: ";
		for (std::vector<int>::const_iterator userFd = channel->second.begin(); userFd != channel->second.end(); ++userFd)
		{
			std::cout << *userFd << " ";
		}
		std::cout << std::endl;
	}
}

void Ircserv::debugShowLastClient(void)
{
	std::map<int, Client>::const_iterator it = _clientsMap.end();
	it--;
	std::cout << "fd: " << it->first << "\n";
	std::cout << "Nick Name: " << it->second._nickName << "\n";
	std::cout << "User Name: " << it->second._userName << "\n";
	std::cout << "Real Name: " << it->second._realName << "\n";
}
