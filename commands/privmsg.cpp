/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 18:44:14 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/25 18:53:07 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

bool Ircserv::privMsgSintaxCheck(std::string firstWord, std::string target)
{
	if (target[0] == ':' || target[0] == '\0')
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

void Ircserv::commandPrivMSG(std::istringstream &lineStream)
{
	std::string target, message, tartg;
	lineStream >> target;
	lineStream >> std::ws;
	std::getline(lineStream, message);
	std::vector<std::string> targetsVec = splitString(target, ',');

	std::cout << "MESSAGE->" << message << "\n";
	if (!privMsgSintaxCheck(message, target))
		return;
	
	//Se for mensagem direta para um client
	for(std::vector<std::string>::const_iterator it = targetsVec.begin(); \
			it != targetsVec.end(); it++)
	{
		target = *it;
		if (target[0] != '#')
		{
			if (!checkIfClientInServerByNick(*it))
			{
				std::string noNickMsg = ":ircserver 401 " + *it + " " + target + " :No such nick/channel\r\n";
				send(_clientFd, noNickMsg.c_str(), noNickMsg.size(), 0);
			}
			else
			{
				if (message[0] == ':')
					message.erase(0, 1);
				//Envia para a mensagem para a pessoa que enviou e para o target
				broadcastMessagePrivate(message, *it);
			}
		//#TODO RPL_AWAY (301)
		}
		else
		{
			if (!checkIfChannelExist(*it))
			{
				std::cout << "target->" << *it <<"<-" << "\n";
				Client client = returnClientStruct(_clientFd);
				std::string errMsg = ":ircserver 402 " + client._nickName + " " + *it + " :No such server\r\n";
				send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			}
			else
			{
				Client client = returnClientStruct(_clientFd);
				if(message[0] == ':')
					message.erase(0, 1);
				std::string channelMessage = ":" + client._nickName + "!" + client._userName + "@" + "localhost" + " PRIVMSG " \
					+ *it + " :" + message + "\r\n";
				broadcastMessageToChannelExceptSender(channelMessage, *it, _clientFd);
			}
			//#TODO ERR_CANNOTSENDTOCHAN (404) (depende dos MODES)
			;
		}
	}
}