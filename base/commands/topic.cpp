/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:06:41 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 16:39:07 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::checkCommandTopic(std::istringstream &lineStream)
{	
	std::string channelName;
	std::string newTopic;

	lineStream >> channelName;
	lineStream >> std::ws;
	std::getline(lineStream, newTopic);
	Client client = returnClientStruct(_clientFd);
	
	
	if(!checkIfChannelExist(channelName))
	{
		std::string errMsg;
		if (channelName.empty())
			errMsg = ":ircserver 403 :" + client._nickName + " " + "*" + " :No such channel!\r\n";
		else
		{
			errMsg = ":ircserver 403 :" + client._nickName + " " + channelName + " :No such channel!\r\n";
		}
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	if (channelName.empty())
	{
		std::string errMsg = ":ircserver 461 " + channelName + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	if (newTopic == ":\r")
	{
		channelsStruct &channel = returnChannelStruct(channelName);
		channel._channelTopic.clear();
		std::string topicChange = ":" + client._nickName + "!" + client._userName + "@localhost TOPIC " + returnRealNameOfChannel(channelName) + " :" + "" + "\r\n";
		broadcastMessageToChannel(topicChange, channelName);
		return ;
	}

	if (newTopic.empty() || newTopic[0] == ' ')
	{
		std::string topic = _getChannelTopic(channelName);
		if (topic.empty())
		{
			topic = "No topic is set.";
			std::string emptyTopic = ":ircserver 331 " + _clientsMap[_clientFd]._nickName + " " + returnRealNameOfChannel(channelName) + " :" + topic + "\r\n";
			send(_clientFd, emptyTopic.c_str(), emptyTopic.size(), 0);
			return;
		}
		
		std::string currentTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + returnRealNameOfChannel(channelName) + " :" + topic + "\r\n";
		send(_clientFd, currentTopic.c_str(), currentTopic.size(), 0);

		std::string channelCpy = channelName;
		std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

		// Find the channel to get topicSetter and topicSetTime
		for (std::vector<channelsStruct>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			std::string channelCpyU = it->_channelName;
			std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
			if (channelCpyU == channelCpy)
			{
				if (!it->_topicSetter.empty())
				{
					std::ostringstream oss;
					oss << ":ircserver 333 " << _clientsMap[_clientFd]._nickName
						<< " " << returnRealNameOfChannel(channelName)
						<< " " << it->_topicSetter
						<< " " << it->_topicSetTime
						<< "\r\n";
					std::string topicWhoTime = oss.str();
					send(_clientFd, topicWhoTime.c_str(), topicWhoTime.size(), 0);
				}
				break;
			}
		}
		return;
	}
	// std::getline(lineStream, newTopic);
	commandTopic(channelName, newTopic);
}

void Ircserv::commandTopic(std::string &channelName, std::string &newTopic)
{
	Client client = returnClientStruct(_clientFd);
	// Find the channel
	std::vector<channelsStruct>::iterator It = _channels.begin();

	if (channelName.empty() || channelName[0] != '#')
	{
		std::string errMsg;
		if (channelName[0] != '#')
			errMsg = ":ircserver 403 :" + client._nickName + " " + channelName + " :No such channel!\r\n";
		else
			errMsg = ":ircserver 403 :" + client._nickName + " " + "*" + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	// Check if client is in the channel
	if (!checkIfClientInChannel(channelName, _clientFd))
	{
		std::string errMsg = ":ircserver 442 " + client._nickName + returnRealNameOfChannel(channelName) + " :User is not in the channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	// Check if +t topic lock is enabled
	if (It->_isTopicLocked)
	{	
		// If +t is set and user is not operator, deny topic change
		if (!isOperator(_clientFd, channelName))
		{
			std::string errMsg = ":ircserver 482 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " :You're not channel operator\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
	}

	newTopic.erase(0, 1);
	std::string topicChange = ":" + client._nickName + "!" + client._userName + "@localhost TOPIC " + returnRealNameOfChannel(channelName) + " :" + newTopic + "\r\n";
	broadcastMessageToChannel(topicChange, channelName);
	_changeChannelTopic(channelName, newTopic);
}