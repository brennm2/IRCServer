/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channelControl.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogosan <diogosan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:07:24 by diogosan          #+#    #+#             */
/*   Updated: 2025/02/05 18:21:07 by diogosan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"
#include <string>


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

void Ircserv::updateUserList(std::string channel)
{
	std::map<std::string, std::vector<Client>>::const_iterator channelIt = _channels.find(channel);
	if (channelIt != _channels.end())
	{
		const std::vector<Client>& clients = channelIt->second;

		std::string nameList = ":ircserver 353 " + _clientsMap[_clientFd]._nickName + " @ " + channel + " :";
		for (std::vector<Client>::const_iterator itVector = clients.begin(); itVector != clients.end(); ++itVector)
		{
			nameList+= itVector->_nickName + " ";
		}
		nameList += "\r\n";
		broadcastMessageToChannel(nameList, channel);
		// send(_clientFd, nameList.c_str(), nameList.size(), 0);

		std::string endOfNames = ":ircserver 366 " + _clientsMap[_clientFd]._nickName + " " + channel + " :End of /NAMES list.\r\n";
		send(_clientFd, endOfNames.c_str(), endOfNames.size(), 0);

	}
	else
		throw std::runtime_error("No server was found!");
}




void Ircserv::commandPart(std::string &channelName)
{
	std::map<std::string, std::vector<Client>>::const_iterator It = _channels.find(channelName);
	if (channelName.empty()|| channelName[0] != '#' || It == _channels.end())
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}

	std::vector<Client>::const_iterator client = LookClientInChannel(channelName);
	if (client == std::vector<Ircserv::Client>::const_iterator())
	{
		std::string errMsg = ":ircserver 442 " + channelName + " :User is not in the channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return ;
	}
	
	std::string leaveMsg = ":" + client->_nickName + "!" + client->_userName + "@localhost PART " + channelName + "\r\n";
	_channels[channelName].erase(client);
	send(_clientFd, leaveMsg.c_str(), leaveMsg.size(), 0);
	broadcastMessageToChannel(leaveMsg, channelName);

	if (_channels[channelName].empty())
		_channels.erase(channelName);

	std::cout << "LEAVE CHANNEL" << channelName <<"\n";
}




