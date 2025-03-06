/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 13:49:36 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/06 15:16:14 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::commandNickCheck(const std::string &nickName)
{
	Client client = returnClientStruct(_clientFd);
	if (nickName.find_first_of("#: &") != std::string::npos)
	{
		std::string errMsg = ":ircserver 432 " + nickName + " " + nickName + " Erroneus nickname\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (nickName[0] == '\0')
	{
		std::string errMsg = ":ircserver 431 " + nickName + " No nickname given\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	//Check if nickname is in use
	for (std::map<int, Client>::const_iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		const Client& client = it->second;
		if (client._nickName == nickName)
		{
			std::string errMsg = ":ircserver 433 " + nickName + " Nickname is already in use\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);

			// std::string changeNickMsg = ":" + _clientsMap[clientFd]._nickName + "!" +  _clientsMap[clientFd]._userName + "@localhost NICK" + nickName +  "\r\n";
			return (false);
		}
	}

	//If nothing is bad, return true
	return (true);
}

void Ircserv::changeNickNameInChannels(const int &clientFd, const std::string &newNickName)
{
	for (std::vector<channelsStruct>::iterator channelIt = _channels.begin();
		channelIt != _channels.end(); ++channelIt)
	{
		for (std::vector<Client>::iterator clientsIt = channelIt->_clients.begin(); 
			clientsIt != channelIt->_clients.end(); ++clientsIt)
		{
			if (clientsIt->_fd == clientFd)
			{
				clientsIt->_nickName = newNickName;
				return;
			}
		}
	}
}



void Ircserv::commandNick(int clientFd, const std::string &nickName)
{
	if (!commandNickCheck(nickName))
		return ;

	Client &client = _clientsMap[clientFd];
	if (client.isFirstTime == false)
	{
		std::string oldNick = client._nickName;
		client._nickName = nickName;
		client._fd = clientFd;

		std::string nickUpdate;
		nickUpdate = ":" + oldNick + "!~b@localhost NICK :" + nickName +"\r\n";
		broadcastMessage(nickUpdate, 0);
		changeNickNameInChannels(clientFd, nickName);
	}
	else
	{
		client._nickName = nickName;
		client._fd = clientFd;
		client.isFirstTime = false;
		client.hasNick = true;
	}
	std::cout << "Registrado o client: " << green << nickName << reset << "\n";


}