/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:14:21 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 16:29:15 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"


void Ircserv::commandQuit(std::istringstream &lineStream)
{
	std::string reason, quitMsg;
	lineStream.ignore(256, ' ');
	std::getline(lineStream, reason);
	
	if (reason[0] != ':')
	{
		Client client = returnClientStruct(_clientFd);
		quitMsg = ":" + client._nickName + "!" + client._userName + "@localhost QUIT :" + "Leaving" + "\r\n";;
	}
	else
	{
		Client client = returnClientStruct(_clientFd);
		reason.erase(0, 1);
		quitMsg = ":" + client._nickName + "!" + client._userName + "@localhost QUIT :" + reason + "\r\n";;
	}
	broadcastMessage(quitMsg, 0);
	removeClientFromEveryChannel(_clientFd);
	close(_clientFd);

	_clientsMap.erase(_clientFd);

	for (size_t i = 0; i < poll_fds.size(); i++)
	{
		if (poll_fds[i].fd == _clientFd)
		{
			poll_fds.erase(poll_fds.begin() + i);
			break;
		}
	}
	std::cout << "Client: " << _clientFd << " disconnected.\n";
}