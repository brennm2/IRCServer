/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 10:25:52 by diodos-s          #+#    #+#             */
/*   Updated: 2025/03/13 17:02:17 by bde-souz         ###   ########.fr       */
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
	
	if (target[0] == '#') // It's a channel
	{
		if (!checkIfChannelExist(target))
		{
			std::string errMsg = ":ircserver 403 :" + client._nickName + " " + target + " :No such channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeChannel(target, modes, parameters);
	}
	else
	{
		std::string errMsg = ":ircserver 403 :" + client._nickName + " " + target + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
}

void Ircserv::commandModeChannel(std::string &channelName, std::string &modes, std::string &parameters)
{
	Client client = returnClientStruct(_clientFd);
	
	std::string channelCpy = channelName;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);

		if (channelCpyU == channelCpy)
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
		std::string errMsg = ":ircserv 442 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " :You're not in that channel\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	if (modes.empty())
	{
		std::string modeString = "+";

		if (channelIt->_isPrivate)
			modeString += "i";
		if (channelIt->_isTopicLocked)
			modeString += "t";
		if (channelIt->_hasPassword)
			modeString += "k";
		if (channelIt->_hasLimit)
			modeString += "l";

		std::string modeResponse = ":ircserver 324 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " " + modeString;

		if (modeString.length() > 1)
		{
			if (channelIt->_hasLimit)
			{
				modeResponse += " " + to_string(channelIt->_maxUsers);
			}
		}
		modeResponse += "\r\n";
		send(_clientFd, modeResponse.c_str(), modeResponse.size(), 0);
		return;
	}
	
	// Check if client is operator for mode restricted commands
	if (!isOperator(_clientFd, channelName))
	{
		std::string errMsg = ":ircserver 482 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " :You're not channel operator\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	// Apply mode changes
	applyChannelModes(channelName, modes, parameters);

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

	channelsStruct &channel = returnChannelStruct(channelName);

	std::istringstream paramStream(parameters);
	std::string param;
	static bool lastSignal = false;
	bool hasSignal = false;
	bool adding = true;
	std::cout << "MODE:" << modes << "\n";
	for (size_t i = 0; i < modes.size(); i++)
	{
		char mode = modes[i];
		if (mode == '+' || mode == '-')
			hasSignal = true;

		if (mode == '+')
			lastSignal = true;
		else if (mode == '-')
			lastSignal = false;

		if (!hasSignal)
			adding = true;
		else
			adding = lastSignal;

		if (mode == '+' || mode == '-')
			continue;
		switch(mode)
		{
			case 'i': // Invite only mode
			{
				if (adding)
				{
					if (!channel._isPrivate)
					{
						channel._isPrivate = true; // Reset invite list
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
												"@localhost MODE " + returnRealNameOfChannel(channelName) + " +" + mode + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				else
				{
					if (channel._isPrivate)
					{
						channel._isPrivate = false;
						channel._clientsFdInvite.clear();
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
											"@localhost MODE " + returnRealNameOfChannel(channelName) + " -" + mode + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				continue;
			}

			case 't': //Topic lock mode (only ops can change topic)
			{
				if (adding)
				{
					if (!channel._isTopicLocked)
					{
						channel._isTopicLocked = true;
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
											"@localhost MODE " + returnRealNameOfChannel(channelName) + " +" + mode + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				else
				{
					if (channel._isTopicLocked)
					{
						channel._isTopicLocked = false;
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
											"@localhost MODE " + returnRealNameOfChannel(channelName) + " -" + mode + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				continue;
				
			}

			case 'k':
				if (adding)
				{
					if (!(paramStream >> param))
						param.clear();
					if (param.empty())
					{
						std::string errMsg = ":ircserver 461 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " k :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						continue;
					}
					
					channel._channelPassword = param; // Store the password
					channel._hasPassword = true;
					std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
							"@localhost MODE " + returnRealNameOfChannel(channelName) + " +" + mode + " " + param + "\r\n";
					broadcastMessageToChannel(modeChangeMsg, channelName);
				}
				else
				{
					if (channel._hasPassword)
					{
						channel._channelPassword.erase(); // Remove the password
						channel._hasPassword = false;
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
								"@localhost MODE " + returnRealNameOfChannel(channelName) + " -" + mode + " *\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				continue;

			case 'o': // Give or remove operator status
				if (!(paramStream >> param)) 
						param.clear();

				std::cout << "PARAM:" << param << "\n";

				if (adding)
				{
					if (param.empty())
					{
						std::string errMsg = ":ircserver 461 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " o :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						continue ;
					}
					else if (!checkIfClientInServerByNick(param))
					{
						std::string errMsg = ":ircserver 441 " + client._nickName + " " + param + " " + returnRealNameOfChannel(channelName) + " :They aren't on that channel\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						continue;
					}
					// Find the client in the channel
					else
					{
						for (size_t j = 0; j < channel._clients.size(); j++)
						{
							std::string nickCpy = channel._clients[j]._nickName;
							std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);

							std::string tempParam = param;
							std::transform(tempParam.begin(), tempParam.end(), tempParam.begin(), ::tolower);

							if (nickCpy == tempParam)
							{
								if (!channel._clients[j]._isOperator)
								{
									channel._clients[j]._isOperator = true;
									std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
										"@localhost MODE " + returnRealNameOfChannel(channelName) + " +" + mode + " " + returnRealNameOfClient(param) + "\r\n";
									broadcastMessageToChannel(modeChangeMsg, channelName);
									continue;
								}
							}
						}
					}
				}
				else
				{
					if (param.empty())
					{
						std::string errMsg = ":ircserver 461 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " o :Not enough parameters\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						continue;
					}
					if (!checkIfClientInServerByNick(param))
					{
						std::string errMsg = ":ircserver 441 " + client._nickName + " " + param + " " + returnRealNameOfChannel(channelName) + " :They aren't on that channel\r\n";
						send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
						continue;
					}
					for (size_t j = 0; j < channel._clients.size(); j++)
					{
						std::string nickCpy = channel._clients[j]._nickName;
						std::string tempParam = param;

						std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);
						std::transform(tempParam.begin(), tempParam.end(), tempParam.begin(), ::tolower);

						if (nickCpy == tempParam)
						{
							if (channel._clients[j]._isOperator)
							{
								std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
										"@localhost MODE " + returnRealNameOfChannel(channelName) + " -" + mode + " " + returnRealNameOfClient(param) + "\r\n";
								broadcastMessageToChannel(modeChangeMsg, channelName);
								channel._clients[j]._isOperator = false;
								continue;
							}
						}
					}
				}
				continue;

			case 'l': // Set user limit
				if (adding)
				{
					if (!(paramStream >> param))
						param.clear();
					if (!param.empty())
					{
						if (std::atoi(param.c_str()) <= 0)
							return true;
						else if (std::atoi(param.c_str()) > 100)
							channel._maxUsers = 100;
						else
							channel._maxUsers = std::atoi(param.c_str());

						channel._hasLimit = true;
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
								"@localhost MODE " + returnRealNameOfChannel(channelName) + " +" + mode + " " + to_string(channel._maxUsers) + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
					else
					{
							std::string errMsg = ":ircserver 461 " + client._nickName + " " + returnRealNameOfChannel(channelName) + " l :Not enough parameters\r\n";
							send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
					}
				}
				else
				{
					if (channel._hasLimit)
					{
						channel._maxUsers = -1;
						channel._hasLimit = false;
						std::string modeChangeMsg = ":" + client._nickName + "!" + client._userName + \
									"@localhost MODE " + returnRealNameOfChannel(channelName) + " -" + mode + "\r\n";
						broadcastMessageToChannel(modeChangeMsg, channelName);
					}
				}
				continue;
	

			default:
				std::string errMsg = ":ircserver 472 " + client._nickName + " " + std::string(1, mode) + " :is unknown mode char to me\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
				break;
			
		}
	}
	return true;
}