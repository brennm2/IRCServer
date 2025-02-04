/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpFunctions.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:42:06 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/04 18:42:24 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


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

bool Ircserv::checkIfClientInChannel(std::map<std::string, std::vector<Client> > channelMap, std::string channel, int clientFd)
{
	// Verifica se o canal existe no channelMap
	std::map<std::string, std::vector<Client>>::const_iterator channelIt = channelMap.find(channel);
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
	std::cout << red << "NENHUM CLIENTE ENCONTRADO NO CHANNEL" << "\n" << reset ;
	return false;
}

void Ircserv::makeUserList(std::string channel)
{
	std::map<std::string, std::vector<Client>>::const_iterator channelIt = _channels.find(channel);
	if (channelIt != _channels.end())
	{
		const std::vector<Client>& clients = channelIt->second;

		std::string nameList = ":ircserver 353 " + _clientsMap[_clientFd]._nickName + " @ " + channel + " :";
		for (std::vector<Client>::const_iterator itVector = clients.begin(); \
			itVector != clients.end(); ++itVector)
		{
			// if (_clientFd != itVector->_fd)
				nameList+= itVector->_nickName + " ";
		}
		nameList += "\r\n";
		broadcastMessageToChannel(nameList, channel);
		// send(_clientFd, nameList.c_str(), nameList.size(), 0);

		std::string endOfNames = ":ircserver 366 " + _clientsMap[_clientFd]._nickName + " " + channel + " :End of /NAMES list.\r\n";
		send(_clientFd, endOfNames.c_str(), endOfNames.size(), 0);

	}
	else
		throw std::runtime_error("No server was founded!");
}



Client Ircserv::returnClientStruct(int clientFd)
{
	// Procura o cliente no mapa
	std::map<int, Client>::iterator it = _clientsMap.find(clientFd);
	
	if (it != _clientsMap.end()) // Retorna a estrutura Client se encontrada
		return it->second;
	else // Lança uma exceção ou retorna um valor padrão se o cliente não for encontrado
		throw std::runtime_error("Client not found");
	
}


void Ircserv::broadcastMessageToChannel(const std::string& message, std::string channel)
{
	std::map<std::string, std::vector<Client>>::const_iterator channelIt = _channels.find(channel);

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
