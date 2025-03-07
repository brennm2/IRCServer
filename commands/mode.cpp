/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 10:25:52 by diodos-s          #+#    #+#             */
/*   Updated: 2025/03/05 18:52:38 by bde-souz         ###   ########.fr       */
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
	Client client = returnClientStruct(_clientFd);
	
	std::string target, modes, parameters;
	lineStream >> target >> modes;
	lineStream >> std::ws;
	std::getline(lineStream, parameters);

	if (target.empty())
	{
		std::string errMsg = ":ircserver 461 " + client._nickName + " " + modes + "MODE :Not enough parameters\r\n";
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
			std::string errMsg = ":ircserver 403 :" + client._nickName + " " + target + " :No such channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeChannel(target, modes, parameters);
	}
}

void Ircserv::commandModeChannel(std::string &channelName, std::string &modes, std::string &parameters)
{
	Client client = returnClientStruct(_clientFd);
	
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channelName)
		break;
		++channelIt;
	}

	if (channelName.empty() || channelName[0] != '#' || channelIt == _channels.end())
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	// Check if client is in the channel
	if (!checkIfClientInChannel(channelName, _clientFd))
	{
		std::string errMsg = ":ircserv 442 " + client._nickName + " " + channelName + " :You're not in that channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	// Check if client is operator for mode restricted commands
	if (!isOperator(_clientFd, channelName))
	{
		std::string errMsg = ":ircserver 482 " + client._nickName + " " + channelName + " :You're not channel operator\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	// Apply mode changes
	if (applyChannelModes(channelName, modes, parameters))
	{
		std::cout << parameters << std::endl;
		std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + "@localhost MODE " + channelName + " " + modes + " " + parameters + "\r\n";
		broadcastMessageToChannel(modeChangeMsg, channelName);
	}
}

bool Ircserv::applyChannelModes(std::string &channelName, std::string &modes, std::string &parameters)
{
	Client client = returnClientStruct(_clientFd);
	// Check if channel exists
	if (!checkIfChannelExist(channelName))
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}

	// Find channel in _channels vector
	channelsStruct *channel = NULL;
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]._channelName == channelName)
		{
			channel = &_channels[i];
			break;
		}
	}
	if (!channel)
		return false;

	bool adding = (modes[0] == '+'); // Determine if we're adding or removing a mode
	std::istringstream paramStream(parameters);
	std::string param;

	int opFd = -1;

	for (size_t i = 0; i < modes.size(); i++)
	{
		char mode = modes[i];
		if (mode == '+' || mode == '-')
			continue;

		switch(mode)
		{
			case 'i': // Invite only mode
				channel->_isPrivate = adding; // Reset invite list
				break;

			case 't': //Topic lock mode (only ops can change topic)
				channel->_isTopicLocked = adding;
				break;

			case 'k':
				if (adding)
				{
					paramStream >> param;
					if (param.empty())
					{
						std::string errMsg = ":ircserv 461 " + client._nickName + " " + channelName + " k :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
                        return false;
					}
					channel->_channelPassword = param; // Store the password
					channel->_hasPassword = true;
				}
				else
				{
					channel->_channelPassword.erase(); // Remove the password
					channel->_hasPassword = false;
				}
				break;

			case 'o': // Give or remove operator status
				paramStream >> param;
				if (!param.empty())
				{
					opFd = returnClientFd(param);
					if (opFd == -1 || !checkIfClientInChannel(channelName, opFd))
					{
						std::string errMsg = ":ircserver 441 " + client._nickName + " " + param + " " + channelName + " :They aren't on that channel\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						return false;
					}

					// Find the client in the channel
					for (size_t j = 0; j < channel->_clients.size(); j++)
					{
						if (channel->_clients[j]._fd == opFd)
						{
							channel->_clients[j]._isOperator = adding;
							break;
						}
					}
				}
				break;

			case 'l': // Set user limit
				if (adding)
				{
					paramStream >> param;
					if (!param.empty() || std::atoi(param.c_str()) > 0)
					{
						if (channel->_maxUsers > 0)
						{
							channel->_maxUsers = std::atoi(param.c_str());
							std::string modeMsg = ":ircserver 324 " + client._nickName + " " + channelName + " +l " + param + "\r\n";
							broadcastMessageToChannel(modeMsg, channelName);
						}
					}
					else
					{
						std::string errMsg = ":ircserver 461 " + client._nickName + " " + channelName + " l :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						return false;
					}
				}
				else
				{
					channel->_maxUsers = -1;
					std::string modeMsg = ":ircserver 324 " + client._nickName + " " + channelName + " -l\r\n";
					broadcastMessageToChannel(modeMsg, channelName);
				}
				break;

			default:
				std::string errMsg = ":ircserver 472 " + client._nickName + std::string(1, mode) + " :is unknown mode char to me\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				break;
		}
	}
	return true;
}

