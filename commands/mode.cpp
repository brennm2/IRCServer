/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diodos-s <diodos-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 10:25:52 by diodos-s          #+#    #+#             */
/*   Updated: 2025/02/26 17:16:06 by diodos-s         ###   ########.fr       */
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

	// else // It's a user
	// {
	// 	if (_clientsMap.find(_clientFd) == _clientsMap.end())
	// 	{
	// 		std::string errMsg = ":ircserver 401 " + target + " :No such nick\r\n";
	// 		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
	// 		return;
	// 	}
	// 	commandModeUser(target, modes, parameters);
	// }
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
	
	// Check if client is operator for mode restricted commands
	if (!isOperator(_clientFd, channelName))
	{
		std::string errMsg = ":ircserver 482 " + client._nickName + " " + channelName + " :You're not channel operator\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + "@localhost MODE " + channelName + " " + modes + " " + parameters + "\r\n";
	broadcastMessageToChannel(modeChangeMsg, channelName);

	// Apply mode changes
	applyChannelModes(channelName, modes, parameters);
}

// void Ircserv::commandModeUser(std::string &targetNick, std::string &modes, std::string &parameters)
// {
// 	Client &client = _clientsMap[_clientFd];

// 	// Ensure user is modifying his own modes
// 	if (client._nickName != targetNick)
// 	{
// 		std::string errMsg = ":ircserv 502 " + targetNick + " :Cannot change mode for other users\r\n";
// 		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
// 		return;
// 	}

// 	std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + "@localhost MODE " + targetNick + " " + modes + " " + parameters + "\r\n";
// 	send(_clientFd, modeChangeMsg.c_str(), modeChangeMsg.size(), 0);

// 	// Apply user modes
// 	// applyUserModes(client, modes);
// }

void Ircserv::applyChannelModes(std::string &channelName, std::string &modes, std::string &parameters)
{
	// Check if channel exists
	if (!checkIfChannelExist(channelName))
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
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
		return;

	bool adding = (modes[0] == '+'); // Determine if we're adding or removing a mode
	std::istringstream paramStream(parameters);
	std::string param;

	int opFd = -1;
	int limit = -1;

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
						std::string errMsg = ":ircserv 461 " + channelName + " k :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
                        return;
					}
					_channelTopics[channelName] = param; // Store the password
				}
				else
				{
					_channelTopics.erase(channelName); // Remove the password
				}
				break;

			case 'o': // Give or remove operator status
				paramStream >> param;
				if (!param.empty())
				{
					opFd = returnClientFd(param);
					if (opFd == -1 || !checkIfClientInChannel(channelName, opFd))
					{
						std::string errMsg = ":ircserver 441 " + param + " " + channelName + " :They aren't on that channel\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						return;
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
					if (!param.empty())
					{
						limit = std::atoi(param.c_str());
						if (limit > 0)
						{
							// Implement logic to set max users (store in a channel attribute if needed)
						}
					}
					else
					{
						std::string errMsg = ":ircserver 461 " + channelName + " l :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						return;
					}
				}
				else
				{
					// Remove user limit
				}
				break;

			default:
				std::string errMsg = ":ircserver 472 " + std::string(1, mode) + " :is unknown mode char\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				break;
		}
	}
}

