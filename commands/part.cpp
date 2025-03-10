/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:05:50 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/10 12:06:33 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::checkCommandPart(std::istringstream &lineStream)
{
	std::string channels;
	lineStream >> channels;

	if (channels.empty())
	{
		std::string errMsg = ":ircserver 461 " + channels + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	std::stringstream channelStream(channels);
	std::string channelName;

	while (std::getline(channelStream, channelName, ','))
	{
		if (!channelName.empty())
			commandPart(channelName);
	}
}

void Ircserv::commandPart(std::string &channelName)
{
	Client client = returnClientStruct(_clientFd);
	// std::vector<channelsStruct>::iterator It = _channels.begin();
	if (channelName.empty()|| channelName[0] != '#' || !checkIfChannelExist(channelName))
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	if (!checkIfClientInChannel(channelName, _clientFd))
	{
		std::string errMsg = ":ircserver 442 " + channelName + " :User is not in the channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}
	
	std::string leaveMsg = ":" + client._nickName + "!" + client._userName + "@localhost PART " + channelName + "\r\n";
	broadcastMessageToChannel(leaveMsg, channelName);
	removeClientFromChannel(channelName, _clientFd);
}