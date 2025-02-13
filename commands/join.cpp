/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 15:40:35 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/13 17:26:54 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::createNewChannel(const std::string& channelName)
{
	channelsStruct newChannel;
	newChannel._channelName = channelName;
	newChannel._channelTopic = "My cool server yay!";

	std::string joinMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + _clientsMap[_clientFd]._userName + "@localhost JOIN " + channelName + "\r\n";
	send(_clientFd, joinMsg.c_str(), joinMsg.size(), 0);

	std::string msgTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + channelName + " :My cool server yay!\r\n";
	send(_clientFd, msgTopic.c_str(), msgTopic.size(), 0);

	_channels.push_back(newChannel);

}

void Ircserv::addClientToChannel(const std::string& channelName, const Client& client)
{
	std::vector<channelsStruct>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->_channelName == channelName)
		{
			it->_clients.push_back(client);
			return;
		}
		++it;
	}
	throw std::runtime_error("Channel not found: " + channelName);
}

bool Ircserv::commandJoinCheck(const std::string &channel)
{
	Client client = returnClientStruct(_clientFd);
	if (channel[0] != '#' && channel[0] != '\0')
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " JOIN " + channel + " :No such channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	else if (channel[0] == '\0')
	{
		//Not enough parameters
		std::string errMsg = ":ircserver 461 " + client._nickName + " JOIN " + channel + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	return (true);

	//#TODO ERR_BADCHANNELKEY (475)
	//#TODO ERR_BANNEDFROMCHAN (474)
	//#TODO ERR_CHANNELISFULL (471)
	//#TODO ERR_INVITEONLYCHAN (473)
	//#TODO ERR_BADCHANMASK (476)
	//#TODO RPL_TOPIC (332)
	//#TODO RPL_TOPICWHOTIME (333)
	//#TODO RPL_NAMREPLY (353)


	//#TODO Join pode receber varios channels

}