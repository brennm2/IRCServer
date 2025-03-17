/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 13:26:55 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/17 12:19:12 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::checkCommandKick(const std::string &channel, \
	const std::vector<std::string> &clientsVec)
{
	Client client = returnClientStruct(_clientFd);
	if (channel.empty())
	{
		std::string errMsg = ":ircserver 461 :" + client._nickName + " KICK :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if(channel[0] != '#' || !checkIfChannelExist(channel))
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + channel + " :No such channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (clientsVec.size() < 1)
	{
		std::string errMsg = ":ircserver 461 :" + client._nickName + " KICK :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (!checkIfClientInChannel(channel, _clientFd))
	{
		std::string errMsg = ":ircserver 442 :" + client._nickName + " " + returnRealNameOfChannel(channel) + " :You're not on that channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if(!isOperator(_clientFd, channel))
	{
		std::string errMsg = ":ircserver 482 " + client._nickName + " " + returnRealNameOfChannel(channel) + " :You're not channel operator\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if(true)
	{
		for(std::vector<std::string>::const_iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
		{
			if(!checkIfClientInChannelByNick(channel, *it))
			{
				std::string errMsg = ":ircserver 441 :" + client._nickName + " " \
					 + returnRealNameOfChannel(channel) + " " + *it + " :They aren't on that channel\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				return (false);
			}
		}
	}
	return (true);
}

void Ircserv::commandKick(std::istringstream &lineStream)
{
	Client client = returnClientStruct(_clientFd);
	std::string channel, clients, reason;

	lineStream >> channel;
	lineStream >> clients;
	lineStream >> std::ws;
	std::getline(lineStream, reason);
	std::vector<std::string> clientsVec = splitString(clients, ',');
	if (!checkCommandKick(channel, clientsVec))
		return ;

	if (!reason.empty())
	{
		std::istringstream lineTmp (reason);
		if (reason[0] == ':')
			reason.erase(0, 1);
		else
			lineTmp >> reason;
	}
	for (std::vector<std::string>::const_iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
	{
		std::string target = *it;
		if (target[0] == ':')
			target.erase(0, 1);

		std::string kickMsg;
		target = returnRealNameOfClient(target);
		if (reason.empty())
		{
			reason = target;
			kickMsg = ":" + client._nickName + "!" + client._userName + "@localhost KICK " \
				+ channel + " " + target + " :" + reason + "\r\n";
		}
		else
		{
			kickMsg = ":" + client._nickName + "!" + client._userName + "@localhost KICK " \
				+ channel + " " + target + " :" + reason + "\r\n";
		}
		broadcastMessageToChannel(kickMsg, channel);
		removeClientFromChannel(channel, returnClientFd(target));
	}
}

std::string Ircserv::returnRealNameOfClient(const std::string &nickName)
{
	std::map<int, Client>::const_iterator clientIt = _clientsMap.begin();

		std::string nickCpy = nickName;
		std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);

	while (clientIt != _clientsMap.end())
	{
		std::string nickCpyU = clientIt->second._nickName;
		std::transform(nickCpyU.begin(), nickCpyU.end(), nickCpyU.begin(), ::tolower);

		if (nickCpyU == nickCpy)
		{
			return clientIt->second._nickName;
		}
		clientIt++;
	}
	return "No channel found";
}