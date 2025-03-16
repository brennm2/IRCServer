/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 15:25:20 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/15 23:55:33 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::commandUserCheck(const int &clientFd, const Client &client)
{
	std::string errMsg;
	if (!client.hasPass)
	{
		errMsg = ":ircserver 451 * :You're not authenticated\r\n";
		send(clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (client.hasUser)
	{
		errMsg = ":ircserver 462 " + client._nickName + " You may not reregister\r\n";
		send(clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}

	return (true);
}

void Ircserv::commandUser(std::istringstream &lineStream)
{
	Client client = _clientsMap[_clientFd];

	if (!commandUserCheck(_clientFd, client))
		return ;


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
	_clientsMap[_clientFd]._userName = userName;
	_clientsMap[_clientFd]._realName = realName;
	_clientsMap[_clientFd].hasUser = true;
	
}