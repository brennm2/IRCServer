/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 11:55:26 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/27 18:05:37 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"
void Ircserv::placeClientInChannelInvite(const int &clientFd, const std::string &channel)
{
	channelsStruct &channelStruct = returnChannelStruct(channel);

	channelStruct._clientsFdInvite.push_back(clientFd);
}

void Ircserv::brodcastInviteMessage(const Client &clientTarget, const std::string &channels)
{
	std::string positiveMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + \
			_clientsMap[_clientFd]._userName + "@localhost INVITE " + \
				clientTarget._nickName + " " + channels + "\r\n";
	send(_clientFd, positiveMsg.c_str(), positiveMsg.size(), 0);
	send(clientTarget._fd, positiveMsg.c_str(), positiveMsg.size(), 0);
}


bool Ircserv::checkCommandInvite(const std::string &target, const std::string &channel)
{
	if (!checkIfChannelExist(channel))
	{
		std::string errMsg = ":ircserver 403 " + channel + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	// else if (!checkIfClientInChannel(channel, _clientFd))
	// {
		
	// }
}

void Ircserv::commandInvite(const std::string &clients, const std::string &channels)
{
	Client client = returnClientStruct(_clientFd);
	Client clientTarget = returnClientStructToModify(returnClientFd(clients));
	//std::vector<std::string> clientsVec = splitString(clients, ',');

	// for
	//  Parameters: <nickname> <channel>

	if (checkIfClientInServer(clientTarget._fd))
	{
		brodcastInviteMessage(clientTarget, channels);
		placeClientInChannelInvite(clientTarget._fd, channels);
		return ;
	}
}