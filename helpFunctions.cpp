/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpFunctions.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:42:06 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/25 16:31:30 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"



bool Ircserv::checkIfChannelExist(std::string channel)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
			return (true);
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
	while (clientIt != _clientsMap.end())
	{
		Client tempclient = clientIt->second;
		if (tempclient._nickName == clientNick)
			return (true);
		clientIt++;
	}
	return (false);
}

bool Ircserv::checkIfClientInChannel(const std::string& channel, int clientFd)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
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
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				if (clientIt->_nickName == clientNick)
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
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
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
	// Procura o cliente no mapa
	std::vector<channelsStruct>::iterator it = _channels.begin();
	
	while(it != _channels.end())
	{
		if (it->_channelName == channel)
			return *it;
		else
			it++;
	}
	// Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
	throw std::runtime_error("Channel not found in returnChannelStruct");
	
}




Ircserv::Client Ircserv::returnClientStruct(int clientFd)
{
	// Procura o cliente no mapa
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
	if (it != _clientsMap.end()) // Retorna a estrutura Client se encontrada
		return it->second;
	else // Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
		throw std::runtime_error("Client not found in returnClientStruct");
	
}

Ircserv::Client& Ircserv::returnClientStructToModify(int clientFd)
{
	// Procura o cliente no mapa
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
	if (it != _clientsMap.end()) // Retorna a estrutura Client se encontrada
		return it->second;
	else // Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
		throw std::runtime_error("Client not found in returnClientStruct");
}

// Ircserv::Client& Ircserv::returnClientStructToModifyInChannel(int clientFd, std::string channel)
// {
// 	// Procura o cliente no mapa
// 	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
// 	if (it != _clientsMap.end()) // Retorna a estrutura Client se encontrada
// 		return it->second;
// 	else // Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
// 		throw std::runtime_error("Client not found in returnClientStruct");
// }

int Ircserv::returnClientFd(std::string clientNick)
{
	std::map<int, Client>::iterator clientIt = _clientsMap.begin();

	if (clientIt != _clientsMap.end())
	{
		while(clientIt != _clientsMap.end())
		{
			if (clientNick == clientIt->second._nickName)
				return(clientIt->second._fd);
			clientIt++;
		}
		//Se nao encontrar nick name no ClientsMap, entao da throw
		throw std::runtime_error("No nickName found in function returnClientFd, nick->" + clientNick + "<- End Nick");
	}
	else
		throw std::runtime_error("No client map in function returnClientFd, nick->" + clientNick + "<- End Nick");
}



void Ircserv::broadcastMessageToChannel(const std::string& message, std::string channel)
{
	std::vector<channelsStruct>::const_iterator channelIt = _channels.begin();

	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				int clientFd = clientIt->_fd; // Pega o file descriptor do cliente
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

	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channel)
		{
			const std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				int clientFd = clientIt->_fd; // Pega o file descriptor do cliente
				if (clientFd != senderFd)
					send(clientFd, message.c_str(), message.size(), 0);
			}
			return;
		}
		++channelIt;
	}

	throw std::runtime_error("No server found in the Broad Cast Message");
}

void Ircserv::broadcastMessage(const std::string& message, int senderFd)
{
	(void) senderFd;
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) 
	{
		int clientFd = it->first;				// Pega o file descriptor do cliente
		Client client = it->second;				// Pega o objeto Client

		// if (clientFd != senderFd)			// Não envia para o próprio cliente
		// {
			// send(clientFd, message.c_str(), message.size(), 0);
		// }
		send(clientFd, message.c_str(), message.size(), 0);
	}
}

void Ircserv::broadcastMessagePrivate(const std::string &message, const std::string &target)
{
	Client clientSender = returnClientStruct(_clientFd);
	Client clientTarget = returnClientStruct(returnClientFd(target));

	std::string nickNameSender = clientSender._nickName;

	// std::cout << "MESSAGE: " << message << "\n";
	std::string targetMessage = ":" + nickNameSender + " PRIVMSG " + target + " :" + message + "\r\n";
	//	std::string senderMessage = ":" + clientTarget._nickName + " PRIVMSG " + target + " :" + message + "\r\n";

	send(clientTarget._fd, targetMessage.c_str(), targetMessage.size(), 0);
	//std::cout << "Mensagem enviada:" << "\n";
	//std::cout << targetMessage << "\n";

//	send(clientSender._fd, senderMessage.c_str(), senderMessage.size(), 0);
}


void Ircserv::nickReplyMsg001(std::string nickName, int clientFd)
{
	std::string msgConfirmation;
	msgConfirmation = ":ircserver 001 " + nickName + " Welcome to the server, " + nickName + "!\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);
}

void Ircserv::nickReplyMsg002(std::string nickName, int clientFd)
{
	std::string msgConfirmation;
	msgConfirmation = ":ircserver 002 " + nickName + " Your host is localhost, running version 1.0!\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);
}

void Ircserv::nickReplyMsg003(std::string nickName, int clientFd)
{
	std::string msgConfirmation;
	std::string timer = to_string(now->tm_mday ) + '-' \
		+ to_string(now->tm_mon + 1) + '-' \
		+ to_string(now->tm_year + 1900) + " at " \
		+ to_string(now->tm_hour) + ':' \
		+ to_string(now->tm_min);
	msgConfirmation = ":ircserver 003 " + nickName + " This server was created " + timer + " \r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);
}

void Ircserv::nickReplyMsg004(std::string nickName, int clientFd)
{
	std::string msgConfirmation;

	msgConfirmation = ":ircserver 004 " + nickName + ": localhost 1.0 o iklt\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);
}

void Ircserv::nickReplyMsg005(std::string nickName, int clientFd)
{
	std::string msgConfirmation;

	msgConfirmation = ":ircserver 005 " + nickName + " I, T, K, O, L :are supported by this server\r\n";
	send(clientFd, msgConfirmation.c_str(), msgConfirmation.size(), 0);
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

	nickReplyMsg001(client._nickName, clientFd);
	nickReplyMsg002(client._nickName, clientFd);
	nickReplyMsg003(client._nickName, clientFd);
	nickReplyMsg004(client._nickName, clientFd);
	nickReplyMsg005(client._nickName, clientFd);
	commandMtdo();
	client.hasFinalReg = true;
}

void Ircserv::removeClientFromChannel(const std::string& channelName, int clientFd)
{
	// Itera sobre o vetor de canais
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channelName)
		{
			// Encontra o cliente no vetor de clientes do canal
			std::vector<Client>& clients = channelIt->_clients;
			for (std::vector<Client>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
			{
				if (clientIt->_fd == clientFd)
				{
					// Remove o cliente do vetor
					clients.erase(clientIt);
					std::cout << "Cliente com FD " << clientFd << " removido do canal " << channelName << "\n";

					// Se o canal estiver vazio após a remoção, apaga o canal do vetor
					if (clients.empty())
					{
						_channels.erase(channelIt);
						std::cout << "Canal " << channelName << " removido pois está vazio\n";
					}
					return;
				}
			}
			std::cerr << "Cliente com FD " << clientFd << " não encontrado no canal " << channelName << "\n";
			return;
		}
		++channelIt;
	}
	std::cerr << "Canal " << channelName << " não encontrado\n";
}

void Ircserv::removeClientFromEveryChannel(int clientFd)
{
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		// Encontra o cliente no vetor de clientes do canal
		std::vector<Client>& clients = channelIt->_clients;
		for (std::vector<Client>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
		{
			if (clientIt->_fd == clientFd)
			{
				std::cout << "Cliente: " << clientIt->_nickName << " removido do canal " << channelIt->_channelName << "\n";
				clients.erase(clientIt);

				// Se o canal estiver vazio após a remoção, apaga o canal do vetor
				if (clients.empty())
				{
					std::cout << "Canal " << channelIt->_channelName << " removido pois está vazio\n";
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
			std::cout << green <<"Token pushback->" << tempStr << reset << "\n";
		}
	}
	return (tokens);
}

