/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 15:40:35 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/24 17:53:11 by bde-souz         ###   ########.fr       */
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

bool Ircserv::checkIfClientCanJoinBannedChannel(const int &clientFd, const std::string &channel)
{
	channelsStruct channelTmp = returnChannelStruct(channel);

	std::vector<int> clientsBanned = channelTmp._clientsBanned;
	std::vector<int>::iterator it = std::find(clientsBanned.begin(), clientsBanned.end(), clientFd);
	
	if (it != clientsBanned.end())
		return (false);
	else
		return (true);
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

	std::vector<std::string> channelsVec = splitString(channel, ',');
	for (std::vector<std::string>::const_iterator it = channelsVec.begin(); \
			it != channelsVec.end(); it++)
	{
		std::string tempChannel = *it;
		if (tempChannel[0] != '#' && tempChannel[0] != '\0')
		{
			std::string errMsg = ":ircserver 403 :" + client._nickName + " JOIN " + tempChannel + " :No such channel\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return false;
		}
		else if (tempChannel[0] == '\0')
		{
			//Not enough parameters
			std::string errMsg = ":ircserver 461 " + client._nickName + " JOIN " + tempChannel + " :Not enough parameters\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return false;
		}
	}
	return (true);

	//#TODO ERR_BADCHANNELKEY (475)

	//#TODO ERR_CHANNELISFULL (471)
	//#TODO ERR_BADCHANMASK (476)
	//#TODO RPL_TOPIC (332)
	//#TODO RPL_TOPICWHOTIME (333)
	//#TODO RPL_NAMREPLY (353)


	//#TODO Join pode receber varios channels

}

bool Ircserv::commandJoinCheckExistingChannel(const std::string &tempChannel, const Client &client)
{
	if (!checkIfClientCanJoinPrivChannel(_clientFd, tempChannel))
	{
		std::string errMsg = ":ircserver 473 " + client._nickName + " " + tempChannel + " :Cannot join channel (+i)\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (!checkIfClientCanJoinBannedChannel(_clientFd, tempChannel))
	{
		std::string errMsg = ":ircserver 474 " + client._nickName + " " + tempChannel + " :Cannot join channel (+b)\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else
		return (true);
}


void Ircserv::commandJoin(const std::string &channel)
{
	if (!commandJoinCheck(channel))
		return ;

	Client client = returnClientStruct(_clientFd);
	std::vector<std::string> channelsVec = splitString(channel, ',');

	for (std::vector<std::string>::const_iterator it = channelsVec.begin(); \
			it != channelsVec.end(); it++)
	{
		std::string tempChannel = *it;
		//Se nao existir, cria um novo canal
		if (!checkIfChannelExist(tempChannel))
		{
			std::cout << green << "Nao existe channel, criado um novo->" << tempChannel << "\n" << reset;
			createNewChannel(tempChannel);
			addClientToChannel(tempChannel, client);
			makeUserList(tempChannel);
		}
		else
		{
			if (!commandJoinCheckExistingChannel(tempChannel, client))
				return ;
			//Verifica se ja existe o clientFd igual no canal, caso nao tenha, coloque na lista
			if (!checkIfClientInChannel(tempChannel, _clientFd))
				addClientToChannel(tempChannel, client);
			
			//Mensagem de confirmacao para dar JOIN
			std::string testeMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + _clientsMap[_clientFd]._userName + "@localhost JOIN " + tempChannel + "\r\n";
			send(_clientFd, testeMsg.c_str(), testeMsg.size(), 0);
			
			std::string msgTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + tempChannel + " :" + _getChannelTopic(tempChannel) + "\r\n";
			send(_clientFd, msgTopic.c_str(), msgTopic.size(), 0);
			
			makeUserList(tempChannel);
		}
	}
	
}