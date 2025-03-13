/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 13:49:36 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 11:18:34 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::commandNickCheck(const std::string &nickName)
{
	Client client = returnClientStruct(_clientFd);
	std::string errMsg;
	if (!client.hasPass)
	{
		errMsg = ":ircserver 451 * :You're not authenticated\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (nickName.find_first_of("#: &") != std::string::npos)
	{
		if (client.hasNick)
			errMsg = ":ircserver 432 " + nickName + " " + nickName + " :Erroneus nickname\r\n";
		else
			errMsg = ":ircserver 432 * " + nickName + " :Erroneus nickname\r\n";

		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	else if (nickName[0] == '\0')
	{
		if (client.hasNick)
			errMsg = ":ircserver 431 " + client._nickName + " :No nickname given\r\n";
		else
			errMsg = ":ircserver 431 * :No nickname given\r\n";

		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return (false);
	}
	for (std::map<int, Client>::const_iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		const Client& clientIt = it->second;
		
		std::string nickNameCpy = nickName;
		std::string nickNameCpyU = clientIt._nickName;
		std::transform(nickNameCpy.begin(), nickNameCpy.end(), nickNameCpy.begin(), tolower);
		std::transform(nickNameCpyU.begin(), nickNameCpyU.end(), nickNameCpyU.begin(), tolower);
		
		if (nickNameCpy == nickNameCpyU)
		{
			if (client.hasNick)
				errMsg = ":ircserver 433 " + client._nickName + " " + nickName + " :Nickname is already in use\r\n";
			else
				errMsg = ":ircserver 433 * " + nickName + " :Nickname is already in use\r\n";

			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);

			return (false);
		}
	}

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
		nickUpdate = ":" + oldNick + "!~" + client._userName + "@localhost NICK :" + nickName +"\r\n";
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
	std::cout << "Client registration: " << green << nickName << reset << "\n";
}