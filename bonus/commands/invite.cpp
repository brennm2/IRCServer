/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 11:55:26 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/12 15:11:55 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"
void Ircserv::placeClientInChannelInvite(const int &clientFd, const std::string &channel)
{
	channelsStruct &channelStruct = returnChannelStruct(channel);

	if (std::find(channelStruct._clientsFdInvite.begin(), \
		channelStruct._clientsFdInvite.end(), clientFd) != channelStruct._clientsFdInvite.end())
		return ;
	else
		channelStruct._clientsFdInvite.push_back(clientFd);

}

bool Ircserv::checkIfClientIsInviteded(const int& clientFd, const std::string &channel)
{
	channelsStruct channelStruct = returnChannelStruct(channel);

	std::vector<int>::const_iterator inviteFd = channelStruct._clientsFdInvite.begin();

	while(inviteFd != channelStruct._clientsFdInvite.end())
	{
		if (*inviteFd == clientFd)
			return (true);
		inviteFd++;
	}
	return (false);
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
	Client client = returnClientStruct(_clientFd);
	if (target.empty() || channel.empty())
	{
		std::string errMsg = ":ircserver 461 :" + client._nickName + " INVITE :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	if (!checkIfChannelExist(channel))
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + channel + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (!checkIfClientInChannel(channel, _clientFd))
	{
		std::string errMsg = ":ircserver 442 :" + client._nickName + " " + returnRealNameOfChannel(channel) + " :You're not on that channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (!checkIfClientInServerByNick(target))
	{
		std::string errMsg = ":ircserver 401 :" + client._nickName + " " + target + " :No such nick\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (!isOperator(_clientFd, channel))
	{
		std::string errMsg = ":ircserver 482 " + client._nickName + " " + returnRealNameOfChannel(channel) + " :You're not channel operator\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (checkIfClientInChannel(channel, returnClientFd(target)))
	{
		std::string errMsg = ":ircserver 443 :" + client._nickName + " " + \
			target + " " + returnRealNameOfChannel(channel) + " :is already on channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	return (true);
}

void Ircserv::commandInvite(const std::string &clients, const std::string &channels)
{
	Client client = returnClientStruct(_clientFd);
	if (checkCommandInvite(clients, channels))
	{
		Client clientTarget = returnClientStructToModify(returnClientFd(clients));
		brodcastInviteMessage(clientTarget, channels);
		placeClientInChannelInvite(clientTarget._fd, channels);
		return ;
	}
}