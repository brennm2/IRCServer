/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 18:44:14 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/24 14:42:55 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandPrivMSG(std::istringstream &lineStream)
{
	//#TODO Do the multiple PRIVMSG
	std::string target, message, firstWord;
	lineStream >> target;
	lineStream >> firstWord;
	std::getline(lineStream, message);

	if (!privMsgSintaxCheck(firstWord, target))
		return;
	//Se for mensagem direta para um client
	if (target[0] != '#')
	{
		//Verifica se ha esse cliente ou server, se nao, retorna error 401
		if (!checkIfClientInServerByNick(target))
		{
			std::string noNickMsg = ":ircserver 401 " + target + " :No such nick/channel\r\n";
			send(_clientFd, noNickMsg.c_str(), noNickMsg.size(), 0);
			return ;
		}
		else
		{
			firstWord.erase(0, 1);
			std::string sendMsg = firstWord + message;
			//Envia para a mensagem para a pessoa que enviou e para o target
			broadcastMessagePrivate(sendMsg, target);
		}
		//#TODO RPL_AWAY (301)
	}
	else
	{
		//Verifica se existe o server, se nao, error 402
		if (!checkIfChannelExist(target))
		{
			Client client = returnClientStruct(_clientFd);
			std::string errMsg = ":ircserver 402 " + client._nickName + " " + target + " :" + "\x03" + "No such server\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		}
		//Se tudo passar, envia a mensagem para o canal
		else
		{
			Client client = returnClientStruct(_clientFd);

			firstWord.erase(0, 1);
			std::string sendMsg = firstWord + message;

			std::string channelMessage = ":" + client._nickName + "!" + client._userName + "@" + "localhost" + " PRIVMSG " \
				+ target + " :" + sendMsg + "\r\n";
			broadcastMessageToChannelExceptSender(channelMessage, target, _clientFd);
		}
		//#TODO ERR_CANNOTSENDTOCHAN (404) (depende dos MODES)
		;
	}
}