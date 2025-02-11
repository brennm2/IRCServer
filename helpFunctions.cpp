/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpFunctions.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:42:06 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/11 17:57:07 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool Ircserv::checkIfChannelExist(std::string channel)
{
	std::map<std::string, std::vector<Client> >::const_iterator channelIt = _channels.find(channel);

	if (channelIt != _channels.end())
		return (true);
	else
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

bool Ircserv::checkIfClientInChannel(std::map<std::string, std::vector<Client> > channelMap, std::string channel, int clientFd)
{
	// Verifica se o canal existe no channelMap
	std::map<std::string, std::vector<Client> >::const_iterator channelIt = channelMap.find(channel);
	if (channelIt != channelMap.end())
	{
		const std::vector<Client>& clients = channelIt->second;
		for (std::vector<Client>::const_iterator clientIt = clients.begin(); \
			clientIt != clients.end(); ++clientIt)
		{
			if (clientIt->_fd == clientFd)
			{
				// std::cout << "Client FD: " << clientIt->_fd << " found in channel: " << channel << "\n";
				return true;
			}
		}
	}
	// std::cout << "Client FD: " << clientFd << " not found in channel: " << channel << "\n";
	//std::cout << red << "NENHUM CLIENTE ENCONTRADO NO CHANNEL" << "\n" << reset ;
	return false;
}

void Ircserv::makeUserList(std::string channel)
{
	std::map<std::string, std::vector<Client> >::const_iterator channelIt = _channels.find(channel);
	if (channelIt != _channels.end())
	{
		const std::vector<Client>& clients = channelIt->second;

		std::string nameList = ":ircserver 353 " + _clientsMap[_clientFd]._nickName + " @ " + channel + " :";
		for (std::vector<Client>::const_iterator itVector = clients.begin(); \
			itVector != clients.end(); ++itVector)
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

Ircserv::Client Ircserv::returnClientStruct(int clientFd)
{
	// Procura o cliente no mapa
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
	if (it != _clientsMap.end()) // Retorna a estrutura Client se encontrada
		return it->second;
	else // Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
		throw std::runtime_error("Client not found");
	
}

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
		throw std::runtime_error("No nickName found in function returnClientFd");
	}
	else
		throw std::runtime_error("No client map in function returnClientFd");
}



void Ircserv::broadcastMessageToChannel(const std::string& message, std::string channel)
{
	std::map<std::string, std::vector<Client> >::const_iterator channelIt = _channels.find(channel);

	if(channelIt != _channels.end())
	{
		const std::vector<Client>& clients = channelIt->second;
		for (std::vector<Client>::const_iterator clientIt = clients.begin(); \
			clientIt != clients.end(); ++clientIt)
		{
			int clientFd = clientIt->_fd;				// Pega o file descriptor do cliente
			send(clientFd, message.c_str(), message.size(), 0);
		}
	}
	else
		throw std::runtime_error("No server found in the broadcastMessageToChannel");
}

void Ircserv::broadcastMessageToChannelExceptSender(const std::string& message, std::string channel, int senderFd)
{
	std::map<std::string, std::vector<Client> >::const_iterator channelIt = _channels.find(channel);

	if(channelIt != _channels.end())
	{
		const std::vector<Client>& clients = channelIt->second;
		for (std::vector<Client>::const_iterator clientIt = clients.begin(); \
			clientIt != clients.end(); ++clientIt)
		{
			int clientFd = clientIt->_fd;				// Pega o file descriptor do cliente
			if (clientFd != senderFd)
				send(clientFd, message.c_str(), message.size(), 0);
		}
	}
	else
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


bool Ircserv::privMsgSintaxCheck(std::string firstWord, std::string target)
{
	if (target[0] == ':')
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 411 " + client._nickName + " :" + "\x03" + "04No recipient given\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return false;
	}
	//Se a primeira letra nao for ':', entao retorna error de sintax (esse error nao existe no IRC, error 407 (too many targets))
	else if (firstWord[0] != ':')
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 407 " + client._nickName + " :" + "\x03" + "04Sintax Error (/PRIVMSG NICK :MESSAGE)\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	//Se a primeira letra for ':' e nao existir mais nada a frente, entao nao existe texto (error 412)
	else if (firstWord[0] == ':' && firstWord[1] == '\0')
	{
		Client client = returnClientStruct(_clientFd);
		std::string errMsg = ":ircserver 412 " + client._nickName + " :" + "\x03" + "04No text to send\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else
		return true;
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
	std::cout << "Client nickname:" << client._nickName << "\n";
	nickReplyMsg002(client._nickName, clientFd);
	nickReplyMsg003(client._nickName, clientFd);
	nickReplyMsg004(client._nickName, clientFd);
	nickReplyMsg005(client._nickName, clientFd);
	commandMtdo();
	client.hasFinalReg = true;
}

