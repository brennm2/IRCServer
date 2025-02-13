/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:14:21 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/13 18:24:57 by bde-souz         ###   ########.fr       */
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
		quitMsg = ":" + client._nickName + "!" + client._userName + "@localhost QUIT :" + "Quit: Leaving" + "\r\n";;
	}
	else
	{
		Client client = returnClientStruct(_clientFd);
		reason.erase(0, 1);
		quitMsg = ":" + client._nickName + "!" + client._userName + "@localhost QUIT :" + "Quit: " + reason + "\r\n";;
	}
	broadcastMessage(quitMsg, 0);
	removeClientFromEveryChannel(_clientFd);
	close(_clientFd);
}