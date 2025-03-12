/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 15:40:35 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/12 18:49:33 by bde-souz         ###   ########.fr       */
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

bool Ircserv::checkIfClientCanJoinPrivChannel(const int &clientFd, const std::string &channel)
{
	channelsStruct channelTmp = returnChannelStruct(channel);
	if (channelTmp._isPrivate)
	{
		std::vector<int> clientsFdInvite = channelTmp._clientsFdInvite;
		std::vector<int>::iterator it = std::find(clientsFdInvite.begin(), clientsFdInvite.end(), clientFd);
		
		if (it != clientsFdInvite.end())
			return (true);
		else
			return (false);
	}
	else
		return (true);
}

bool Ircserv::checkIfChannelHasPassword(const std::string &channel)
{
	channelsStruct channelTemp = returnChannelStruct(channel);
	if (!channelTemp._hasPassword)
		return (false);
	else
		return (true);
}

bool Ircserv::checkIfChannelHasCorrectPassword(const std::string &channel, const std::string &password)
{
	channelsStruct channelTemp = returnChannelStruct(channel);
	if (channelTemp._channelPassword == password)
		return (true);
	else
		return (false);
}

bool Ircserv::commandJoinCheck(const std::string &channel)
{
	Client client = returnClientStruct(_clientFd);
	if (channel[0] == '\0')
	{
		//Not enough parameters
		std::string errMsg = ":ircserver 461 " + client._nickName + " JOIN " + channel + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	if (channel[0] != '#' && channel[0] != '\0')
	{
			std::string errMsg = ":ircserver 476 : " + channel + " :Bad Channel Mask\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return false;
	}
	else if (channel[0] == '\0')
	{
		std::string errMsg = ":ircserver 461 " + client._nickName + " JOIN " + channel + " :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	return (true);
}

bool Ircserv::commandJoinCheckExistingChannel(const std::string &tempChannel, \
	const Client &client, \
	std::vector<std::string>::iterator &keyVecIt,
	const std::vector<std::string>::const_iterator &endVec, \
	bool emptyKeyFlag)
{
	if (!checkIfClientCanJoinPrivChannel(_clientFd, tempChannel))
	{
		std::string errMsg = ":ircserver 473 " + client._nickName + " " + tempChannel + " :Cannot join channel (+i)\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (checkIfChannelHasPassword(tempChannel))
	{
		if(checkIfClientIsInviteded(_clientFd, tempChannel))
			return (true);
		else if (keyVecIt != endVec && \
			!emptyKeyFlag && \
			checkIfChannelHasCorrectPassword(tempChannel, *keyVecIt))
		{
			keyVecIt++;
			return(true);
		}
		else
		{
			if(!emptyKeyFlag && keyVecIt != endVec)
				keyVecIt++;
			std::string errMsg = ":ircserver 475 " + client._nickName + " " + tempChannel + " :Cannot join channel (+k)\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return (false);
		}
	}
	else
		return (true);
}


void Ircserv::commandJoin(const std::string &channel, const std::string &key)
{
	Client &client = returnClientStructToModify(_clientFd);
	std::vector<std::string> channelsVec = splitString(channel, ',');
	std::vector<std::string> keyVec;
	std::vector<std::string>::iterator keyVecIt;
	bool emptyKeyFlag = false;
	keyVec = splitString(key, ',');

	if (channelsVec.size() < 1)
	{
		std::string errMsg = ":ircserver 461 :" + client._nickName + " JOIN :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	if (!key.empty())
		keyVecIt = keyVec.begin();
	else
		emptyKeyFlag = true;

	for (std::vector<std::string>::const_iterator it = channelsVec.begin(); \
			it != channelsVec.end(); it++)
	{
		std::string tempChannel = *it;
		if (!commandJoinCheck(tempChannel))
			return ;
		if (!checkIfChannelExist(tempChannel))
		{
			std::cout << green << "There is no channel, a new one has been created-> " << tempChannel << "\n" << reset;
			createNewChannel(tempChannel);
			addClientToChannel(tempChannel, client);
			changeClientToOperator(_clientFd, tempChannel);
			makeUserList(tempChannel);
		}
		else
		{
			channelsStruct &channelsStruct = returnChannelStruct(tempChannel);

			if (channelsStruct._maxUsers > 0 && (int)channelsStruct._clients.size() >= channelsStruct._maxUsers)
			{
				std::string errMsg = ":ircserver 471 " + _clientsMap[_clientFd]._nickName + " " + tempChannel + " :Cannot join channel (+l)\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				continue;
			}
			
			if (!commandJoinCheckExistingChannel(tempChannel, client, keyVecIt, keyVec.end() ,emptyKeyFlag))
				continue;
			//Verifica se ja existe o clientFd igual no canal, caso nao tenha, coloque na lista
			if (!checkIfClientInChannel(tempChannel, _clientFd))
				addClientToChannel(tempChannel, client);
			
			//Mensagem de confirmacao para dar JOIN
			std::string testeMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + _clientsMap[_clientFd]._userName + "@localhost JOIN " + tempChannel + "\r\n";
			send(_clientFd, testeMsg.c_str(), testeMsg.size(), 0);
			
			std::string topic = _getChannelTopic(tempChannel);
			if (topic.empty())
			{
				topic = "No topic is set.";
				std::string emptyTopic = ":ircserver 331 " + _clientsMap[_clientFd]._nickName + " " + 
					tempChannel + " :" + topic + "\r\n";
				send(_clientFd, emptyTopic.c_str(), emptyTopic.size(), 0);
			}
			else
			{
				std::string msgTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + tempChannel + " :" + _getChannelTopic(tempChannel) + "\r\n";
				send(_clientFd, msgTopic.c_str(), msgTopic.size(), 0);
			}

			
			makeUserList(tempChannel);
		}
	}
}

