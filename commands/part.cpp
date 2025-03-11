/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:05:50 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/11 13:21:05 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::checkCommandPart(const std::string &channels, std::string reason)
{
	Client client = returnClientStruct(_clientFd);
	if (channels.empty())
	{
		std::string errMsg = ":ircserver 461 " + client._nickName + " PART :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}
	commandPart(channels, reason);
}

void Ircserv::commandPart(const std::string &channelName, std::string reason)
{
	Client client = returnClientStruct(_clientFd);
	std::vector<std::string> channelVec = splitString(channelName, ',');

	for (std::vector<std::string>::const_iterator channelIt = channelVec.begin(); \
			channelIt != channelVec.end(); channelIt++)
	{
		std::string tempChannel = *channelIt;
		if (tempChannel.empty()|| tempChannel[0] != '#' || !checkIfChannelExist(tempChannel))
		{
			std::string errMsg = ":ircserver 403 :" + client._nickName + " " + tempChannel + " :No such channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return ;
		}
		else if (!checkIfClientInChannel(tempChannel, _clientFd))
		{
			std::string errMsg = ":ircserver 442 " + tempChannel + " :User is not in the channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return ;
		}
		
		std::string leaveMsg;
		if (reason.empty())
			leaveMsg = ":" + client._nickName + "!" + client._userName + \
				"@localhost PART " + tempChannel + " :Leaving\r\n";
		else
		{
			if (reason[0] == ':')
				reason.erase(0, 1);
			leaveMsg = ":" + client._nickName + "!" + client._userName + "@localhost PART " \
				+ tempChannel + " :" + reason +"\r\n";
		}

		broadcastMessageToChannel(leaveMsg, tempChannel);
		removeClientFromChannel(tempChannel, _clientFd);
	}
}