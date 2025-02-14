/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channelControl.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:07:24 by diogosan          #+#    #+#             */
/*   Updated: 2025/02/14 12:20:04 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


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
	// std::vector<channelsStruct>::iterator It = _channels.begin();
	if (channelName.empty()|| channelName[0] != '#' || !checkIfChannelExist(channelName))
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	Client client = returnClientStruct(_clientFd);
	if (!checkIfClientInChannel(_channels, channelName, _clientFd))
	{
		std::string errMsg = ":ircserver 442 " + channelName + " :User is not in the channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}
	
	std::string leaveMsg = ":" + client._nickName + "!" + client._userName + "@localhost PART " + channelName + "\r\n";
	broadcastMessageToChannel(leaveMsg, channelName);
	removeClientFromChannel(channelName, _clientFd);
}

void Ircserv::checkCommandTopic(std::istringstream &lineStream)
{
	std::string channelName;
	std::string newTopic;

	lineStream >> channelName;
	std::getline(lineStream >> std::ws, newTopic);
	
	if(!checkIfChannelExist(channelName))
	{
		std::cout << red << "NAO EXISTE ESSE CANAL" << "\n" << reset;
		//#TODO COLOCAR O ERRO CORRETO!
		return ;
	}

	if (channelName.empty())
	{
		std::string errMsg = ":ircserver 461 " + channelName + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	if (newTopic.empty() || newTopic[0] == ' ' )
	{
		std::string topic = _getChannelTopic(channelName);
		if (topic.empty())
		{
			topic = "No topic is set.";
			std::string emptyTopic = ":ircserver 331 " + _clientsMap[_clientFd]._nickName + " " + channelName + " :" + topic + "\r\n";
			broadcastMessageToChannel(emptyTopic, channelName);
			return;
		}
		
		std::string currentTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + channelName + " :" + topic + "\r\n";
		broadcastMessageToChannel(currentTopic, channelName);
	
		return;
	}
	// std::getline(lineStream, newTopic);
	commandTopic(channelName, newTopic);
}

void Ircserv::commandTopic(std::string &channelName, std::string &newTopic)
{
	std::vector<channelsStruct>::iterator It = _channels.begin();
	while (It != _channels.end())
	{
		if (It->_channelName == channelName)
		break;
		++It;
	}

	if (channelName.empty() || channelName[0] != '#' || It == _channels.end())
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	Client client = returnClientStruct(_clientFd);
	if (!checkIfClientInChannel(_channels, channelName, _clientFd))
	{
		std::string errMsg = ":ircserver 442 " + channelName + " :User is not in the channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	newTopic.erase(0, 1);
	std::string topicChange = ":" + client._nickName + "!" + client._userName + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
	broadcastMessageToChannel(topicChange, channelName);
	_changeChannelTopic(channelName, newTopic);
}