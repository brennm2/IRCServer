/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 15:25:20 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/13 11:57:52 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandUser(std::istringstream &lineStream)
{
	Client client = _clientsMap[_clientFd];
	if (client.hasUser)
	{
		std::string noNickMsg = ":ircserver 462 " + client._nickName + " You may not reregister\r\n";
		send(_clientFd, noNickMsg.c_str(), noNickMsg.size(), 0);
		return ;
	}

	// Separa as strings
	std::string userName, realName, tempStr, tempStr2;
	lineStream >> userName;
	lineStream >> tempStr;
	lineStream >> tempStr2;
	lineStream.ignore(256, ' ');
	std::getline(lineStream, realName);

	if (tempStr != "0" || tempStr2[0] != '*' \
		|| realName[0] == '\0' || realName[0] != ':' )
	{
		std::string noNickMsg = ":ircserver 461 " + client._nickName + " USER :Not enough parameters\r\n";
		send(_clientFd, noNickMsg.c_str(), noNickMsg.size(), 0);
		return ;
	}

	realName = realName.erase(0,1);
	// Guarda os nicks
	_clientsMap[_clientFd]._userName = userName;
	_clientsMap[_clientFd]._realName = realName;
	_clientsMap[_clientFd].hasUser = true;
	
}