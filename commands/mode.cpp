/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 15:42:18 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/25 17:46:43 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::changeClientToOperator(int clientFd, std::string channel)
{
	channelsStruct &tempChannel = returnChannelStruct(channel);
	std::vector<Client> &clientsVec = tempChannel._clients;

	for (std::vector<Client>::iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
	{
		if(it->_fd == clientFd)
		{
			it->_isOperator = true;
			return ;
		}
	}
}

bool Ircserv::isOperator(const int clientFd, const std::string &channel)
{
	channelsStruct tempChannel = returnChannelStruct(channel);
	std::vector<Client> clientsVec = tempChannel._clients;

	for (std::vector<Client>::iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
	{
		if (it->_fd == clientFd)
		{
			if (!it->_isOperator)
				return (false);
			else
				return (true);
		}
	}
	return (false);
}


void Ircserv::checkCommandMode(std::istringstream &lineStream)
{
	std::string target, modes, parameters;
	lineStream >> target >> modes;
	std::getline(lineStream, parameters);

	if (target.empty())
	{
		std::string errMsg = ":ircserver 461 MODE :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	std::vector<channelsStruct>::iterator It = _channels.begin();
	while (It != _channels.end())
	{
		if (It->_channelName == target)
		break;
		++It;
	}

	if (target[0] == '#') // It's a channel
	{
		if (It == _channels.end())
		{
			std::string errMsg = ":ircserver 403 " + target + " :No such channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeChannel(target, modes, parameters);
	}

	else // It's a user
	{
		if (_clientsMap.find(_clientFd) == _clientsMap.end())
		{
			std::string errMsg = ":ircserver 401 " + target + " :No such nick\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeUser(target, modes, parameters);
	}
}

void Ircserv::commandModeChannel(std::string &channelName, std::string &modes, std::string &parameters)
{
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channelName)
		break;
		++channelIt;
	}

	if (channelName.empty() || channelName[0] != '#' || channelIt == _channels.end())
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	// Check if client is in the channel
	if (!checkIfClientInChannel(channelName, _clientFd))
	{
		std::string errMsg = ":ircserv 442 " + channelName + " :You're not in that channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	for (std::vector<Client>::const_iterator clients = channelIt->_clients.begin(); clients != channelIt->_clients.end(); ++clients)
	{
	// Check if the client is an operator (for mode-restricted commands)
	if (!clients->_isOperator)
    {
        std::string errMsg = ":ircserver 482 " + channelName + " :You're not channel operator\r\n";
        send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
        return;
    }
		std::string modeChangeMsg = ":" + clients->_nickName + "!" + clients->_userName + "@localhost MODE " + channelName + " " + modes + " " + parameters + "\r\n";
		broadcastMessageToChannel(modeChangeMsg, channelName);
	}

	// Apply mode changes
	// applyChannelModes(channelName, modes, parameters);
}

void Ircserv::commandModeUser(std::string &targetNick, std::string &modes, std::string &parameters)
{
	Client &client = _clientsMap[_clientFd];

	// Ensure user is modifying his own modes
	if (client._nickName != targetNick)
	{
		std::string errMsg = ":ircserv 502 " + targetNick + " :Cannot change mode for other users\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + "@localhost MODE " + targetNick + " " + modes + " " + parameters + "\r\n";
	send(_clientFd, modeChangeMsg.c_str(), modeChangeMsg.size(), 0);

	// Apply user modes
	// applyUserModes(client, modes);
}

