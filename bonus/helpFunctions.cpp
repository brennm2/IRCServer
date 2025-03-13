/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpFunctions.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:42:06 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 17:07:51 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool Ircserv::checkIfChannelExist(std::string channel)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);

		if (channelCpyU == channelCpy)
		{
			return (true);
		}
		channelIt++;
	}
	return (false);
}

bool Ircserv::checkIfClientInServer(int clientFd)
{
	std::map<int, Client>::const_iterator clientIt = _clientsMap.find(clientFd);
	if (clientIt != _clientsMap.end())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool Ircserv::checkIfClientInServerByNick(std::string clientNick)
{
	std::map<int, Client>::const_iterator clientIt = _clientsMap.begin();

	std::string nickCpy = clientNick;
	std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);

	while (clientIt != _clientsMap.end())
	{
		Client tempclient = clientIt->second;

		std::string nickCpyU = tempclient._nickName;
		std::transform(nickCpyU.begin(), nickCpyU.end(), nickCpyU.begin(), ::tolower);
		if (nickCpyU == nickCpy)
			return (true);
		clientIt++;
	}
	return (false);
}

bool Ircserv::checkIfClientInChannel(const std::string& channel, int clientFd)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				if (clientIt->_fd == clientFd)
					return true;
			}
		}
		++channelIt;
	}
	return false;
}


bool Ircserv::checkIfClientInChannelByNick(const std::string& channel, const std::string& clientNick)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);

		if (channelCpyU == channelCpy)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			
			std::string nickCpy = clientNick;
			std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);

			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				std::string nickCpyU = clientIt->_nickName;
				std::transform(nickCpyU.begin(), nickCpyU.end(), nickCpyU.begin(), ::tolower);
				if (nickCpyU == nickCpy)
					return true;
			}
		}
		++channelIt;
	}
	return false;
}

void Ircserv::makeUserList(std::string channel)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();
	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);
	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
		{
			const std::vector<Client>& clients = channelIt->_clients;

			std::string nameList = ":ircserver 353 " + _clientsMap[_clientFd]._nickName + " @ " + channel + " :";
			for (std::vector<Client>::const_iterator itClient = clients.begin(); \
				itClient != clients.end(); itClient++)
			{
				if (isOperator(itClient->_fd, channel))
					nameList += '@' + itClient->_nickName + " ";
				else
					nameList += itClient->_nickName + " ";
			}
			nameList += "\r\n";
			broadcastMessageToChannel(nameList, channel);

			std::string endOfNames = ":ircserver 366 " + _clientsMap[_clientFd]._nickName + " " + channel + " :End of /NAMES list.\r\n";
			send(_clientFd, endOfNames.c_str(), endOfNames.size(), 0);

			return;
		}
		++channelIt;
	}
	throw std::runtime_error("No server was found!");
}

Ircserv::channelsStruct& Ircserv::returnChannelStruct(const std::string &channel)
{
	std::vector<channelsStruct>::iterator it = _channels.begin();
	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);
	
	while(it != _channels.end())
	{
		std::string channelCpyU = it->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
			return *it;
		else
			it++;
	}
	throw std::runtime_error("Channel not found in returnChannelStruct");
	
}


Ircserv::Client Ircserv::returnClientStruct(int clientFd)
{
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
	if (it != _clientsMap.end())
		return it->second;
	else
		throw std::runtime_error("Client not found in returnClientStruct");
	
}

Ircserv::Client& Ircserv::returnClientStructToModify(int clientFd)
{
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);

	if (it != _clientsMap.end()) 
		return it->second;
	else
		throw std::runtime_error("Client not found in returnClientStruct");
}


int Ircserv::returnClientFd(std::string clientNick)
{
	std::map<int, Client>::iterator clientIt = _clientsMap.begin();
	std::string nickCpy = clientNick;
	std::transform(nickCpy.begin(), nickCpy.end(), nickCpy.begin(), ::tolower);

	if (clientIt != _clientsMap.end())
	{
		while(clientIt != _clientsMap.end())
		{
			std::string nickCpyU = clientIt->second._nickName;
			std::transform(nickCpyU.begin(), nickCpyU.end(), nickCpyU.begin(), ::tolower);

			if (nickCpyU == nickCpy)
				return(clientIt->second._fd);
			clientIt++;
		}
		throw std::runtime_error("No nickName found in function returnClientFd, nick->" + clientNick + "<- End Nick");
	}
	else
		throw std::runtime_error("No client map in function returnClientFd, nick->" + clientNick + "<- End Nick");
}



void Ircserv::broadcastMessageToChannel(const std::string& message, std::string channel)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();
	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				int clientFd = clientIt->_fd;
				send(clientFd, message.c_str(), message.size(), 0);
			}
			return;
		}
		++channelIt;
	}
	throw std::runtime_error("No server found in the broadcastMessageToChannel");
}

void Ircserv::broadcastMessageToChannelExceptSender(const std::string& message, std::string channel, int senderFd)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);
	while (channelIt != _channels.end())
	{
		std::string channelCpyU = channelIt->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);

		if (channelCpyU == channelCpy)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				int clientFd = clientIt->_fd; 
				if (clientFd != senderFd)
				{
					send(clientFd, message.c_str(), message.size(), MSG_NOSIGNAL);
				}
			}
			return;
		}
		++channelIt;
	}
	throw std::runtime_error("No server found in the Broadcast Message");
}



void Ircserv::broadcastMessage(const std::string& message, int senderFd)
{
	(void) senderFd;
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		int clientFd = it->first;
		Client client = it->second;

		send(clientFd, message.c_str(), message.size(), 0);
	}
}

void Ircserv::broadcastMessagePrivate(const std::string &message, const std::string &target)
{
	Client clientSender = returnClientStruct(_clientFd);
	Client clientTarget = returnClientStruct(returnClientFd(target));

	std::string nickNameSender = clientSender._nickName;
	std::string targetMessage = ":" + nickNameSender + " PRIVMSG " + target + " :" + message + "\r\n";
	send(clientTarget._fd, targetMessage.c_str(), targetMessage.size(), 0);
}



std::string Ircserv::to_string(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

void Ircserv::clientFinalRegistration(int clientFd)
{
	Client &client = _clientsMap[clientFd];

	std::string msgConfirmation;
	msgConfirmation = ":ircserver 001 " + client._nickName + " Welcome to the server, " + client._nickName + "!\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);

	msgConfirmation = ":ircserver 002 " + client._nickName + " Your host is localhost, running version 1.0!\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);

	std::string timer = to_string(now->tm_mday ) + '-' \
		+ to_string(now->tm_mon + 1) + '-' \
		+ to_string(now->tm_year + 1900) + " at " \
		+ to_string(now->tm_hour) + ':' \
		+ to_string(now->tm_min);
	msgConfirmation = ":ircserver 003 " + client._nickName + " This server was created " + timer + " \r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);

	msgConfirmation = ":ircserver 004 " + client._nickName + ": localhost 1.0 o iklt\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);

	msgConfirmation = ":ircserver 005 " + client._nickName + " I, T, K, O, L :are supported by this server\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);

	commandMotd();
	client.hasFinalReg = true;
}

void Ircserv::removeClientFromChannel(const std::string& channelName, int clientFd)
{
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channelName)
		{
			std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				if (clientIt->_fd == clientFd)
				{
					clients.erase(clientIt);
					std::cout << "Client with FD: " << clientFd << " removed from channel: " << channelName << "\n";

					if (clients.empty())
					{
						_channels.erase(channelIt);
						std::cout << "Channel: " << channelName << " deleted because is empty\n";
					}
					return;
				}
			}
			std::cerr << "Client with FD: " << clientFd << " not found on the channel: " << channelName << "\n";
			return;
		}
		++channelIt;
	}
	std::cerr << "Channel: " << channelName << " not found\n";
}


void Ircserv::disconnectClientFromEveryChannel(int clientFd)
{
	Client client = returnClientStruct(clientFd);
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while(channelIt != _channels.end())
	{
		std::vector<Client>::iterator clientIt = channelIt->_clients.begin();
		while(clientIt != channelIt->_clients.end())
		{
			if (clientIt->_fd == clientFd)
			{
				std::string leaveMsg = ":" + client._nickName + "!" + client._userName + \
					"@localhost PART " + channelIt->_channelName + " :Leaving\r\n";
				broadcastMessageToChannelExceptSender(leaveMsg, channelIt->_channelName, clientFd);
				clientIt = channelIt->_clients.erase(clientIt);
			}
			else
				clientIt++;
		}
		if (channelIt->_clients.empty())
		{
			std::cout << "Empty server: " << channelIt->_channelName << " - Deleting..." << "\n";
			channelIt = _channels.erase(channelIt);
		}
		else
			channelIt++;
	}
}



void Ircserv::removeClientFromEveryChannel(int clientFd)
{
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		std::vector<Client>& clients = channelIt->_clients;
		for (std::vector<Client>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
		{
			if (clientIt->_fd == clientFd)
			{
				std::cout << "Client: " << clientIt->_nickName << " removed from channel: " << channelIt->_channelName << "\n";
				clients.erase(clientIt);

				if (clients.empty())
				{
					std::cout << "Channel: " << channelIt->_channelName << " deleted because is empty\n";
					channelIt = _channels.erase(channelIt);
				}
				else
				{
					++channelIt;
				}
				return;
			}
		}
		++channelIt;
	}
}

std::vector<std::string> Ircserv::splitString(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string tempStr;

	while(std::getline(ss, tempStr, delimiter))
	{
		if (!tempStr.empty())
		{
			if (tempStr[tempStr.size() - 1] == '\r')
				tempStr.erase(tempStr.size() - 1);
			tokens.push_back(tempStr);
		}
	}
	return (tokens);
}

std::string Ircserv::_getChannelTopic(std::string channel)
{
	std::vector<channelsStruct>::iterator It = _channels.begin();
	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);
	
	while (It != _channels.end())
	{
		std::string channelCpyU = It->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
			return It->_channelTopic;
		It++;
	}
	return NULL;
}


void Ircserv::_changeChannelTopic(std::string &channel, std::string &newTopic)
{
	std::vector<channelsStruct>::iterator It = _channels.begin();

	std::string channelCpy = channel;
	std::transform(channelCpy.begin(), channelCpy.end(), channelCpy.begin(), ::tolower);

	while (It != _channels.end())
	{
		std::string channelCpyU = It->_channelName;
		std::transform(channelCpyU.begin(), channelCpyU.end(), channelCpyU.begin(), ::tolower);
		if (channelCpyU == channelCpy)
		{
			It->_channelTopic = newTopic;
			It->_topicSetter = _clientsMap[_clientFd]._nickName; // Store who set the topic
			It->_topicSetTime = time(NULL); // Store the current timestamp
			return;
		}
		++It;
	}

}

