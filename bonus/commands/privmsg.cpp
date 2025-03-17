/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 18:44:14 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/17 12:20:44 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::privMsgSintaxCheck(std::string firstWord, std::string target)
{
	if (target[0] == ':' || target[0] == '\0')
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 411 " + client._nickName + " :" + "No recipient given\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	else if (firstWord.empty())
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 412 " + client._nickName + " :" + "No text to send\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (firstWord[0] == ':' && (firstWord[1] == '\0' || firstWord[1] == '\r'))
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 412 " + client._nickName + " :" + "No text to send\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else
		return true;
}

void Ircserv::commandPrivMSG(std::istringstream &lineStream)
{
	std::string target, message, tartg;
	lineStream >> target;
	lineStream >> std::ws;
	std::getline(lineStream, message);
	std::vector<std::string> targetsVec = splitString(target, ',');

	if (!privMsgSintaxCheck(message, target))
		return;

	std::istringstream lineTmp (message);
	if (message[0] == ':')
		message.erase(0, 1);
	else
		lineTmp >> message;
	
	for(std::vector<std::string>::const_iterator it = targetsVec.begin(); \
			it != targetsVec.end(); it++)
	{
		target = *it;
		if (target[0] != '#')
		{
			if (!checkIfClientInServerByNick(*it))
			{
				std::string noNickMsg = ":ircserver 401 " + *it + " " + target + " :No such nick/channel\r\n";
				send(_clientFd, noNickMsg.c_str(), noNickMsg.size(), 0);
			}
			else
				broadcastMessagePrivate(message, *it);
		}
		else
		{
			Client client = returnClientStruct(_clientFd);
			if (!checkIfChannelExist(*it))
			{
				std::string errMsg = ":ircserver 402 " + client._nickName + " " + *it + " :No such server\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			}
			else if (!checkIfClientInChannel(*it, _clientFd))
			{
				std::string errMsg = ":ircserver 404 " + client._nickName + " " + *it + " :Cannot send to channel\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			}
			else
			{
				Client client = returnClientStruct(_clientFd);
				std::string channelMessage = ":" + client._nickName + "!" + client._userName + "@" + "localhost" + " PRIVMSG " \
					+ returnRealNameOfChannel(*it) + " :" + message + "\r\n";
				broadcastMessageToChannelExceptSender(channelMessage, *it, _clientFd);
			}
		}
	}
}