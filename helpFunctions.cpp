#include "Ircserv.hpp"


bool Ircserv::checkIfClientInChannel(std::map<std::string, std::vector<Client> > channelMap, std::string channel, int clientFd)
{
	// Verifica se o canal existe no channelMap
	std::map<std::string, std::vector<Client>>::const_iterator channelIt = channelMap.find(channel);
	if (channelIt != channelMap.end())
	{
		const std::vector<Client>& clients = channelIt->second;
		for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
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
				nameList+= itVector->_nickName;
		}
		nameList += "\r\n";
		send(_clientFd, nameList.c_str(), nameList.size(), 0);

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