/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 13:26:55 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/24 18:30:18 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void printAsciiValues(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        char c = *it;
        std::cout << "Character: " << c << " ASCII: " << static_cast<int>(c) << std::endl;
    }
}

void printVector(const std::vector<std::string>& vec)
{
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        std::cout << "vector->" << *it << std::endl;
    }
}


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
		std::string errMsg = ":ircserver 442 :" + client._nickName + " " + channel + " :You're not on that channel\r\n";
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
					 + channel + " " + *it + " :They aren't on that channel\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				return (false);
			}
		}
	}
	
	return (true);

	//#TODO ERR_CHANOPRIVSNEEDED (482)
	
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

	for (std::vector<std::string>::const_iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
	{
		std::string target = *it;
		if (target[0] == ':')
			target.erase(0, 1);

		std::string kickMsg;
		if (reason.empty())
		{
			kickMsg = ":" + client._nickName + "!" + client._userName + "@localhost KICK " \
				+ channel + " " + target + "\r\n";
		}
		else
		{
			if (reason[0] == ':')
				reason.erase(0,1);
			kickMsg = ":" + client._nickName + "!" + client._userName + "@localhost KICK " \
				+ channel + " " + target + " :" + reason + "\r\n";
		}
		broadcastMessageToChannel(kickMsg, channel);
		removeClientFromChannel(channel, returnClientFd(target));
	}
}