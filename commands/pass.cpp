/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:38:00 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/11 15:23:12 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"


bool Ircserv::clientCanUseCommands(int clientFd)
{
	Client client = returnClientStruct(clientFd);

	if (!client.hasPass || !client.hasNick || !client.hasUser)
	{
		std::string errorMsg = ":ircserver 451 :" + client._nickName + " You have not registered\r\n";
		send (clientFd, errorMsg.c_str(), errorMsg.size(), 0);

		std::cout << "Has Pass:" << client.hasPass << "\n";
		std::cout << "Has Nick:" << client.hasNick << "\n";
		std::cout << "Has User:" << client.hasUser << "\n";

		return (false);
	}
	else
		return (true);
}

bool Ircserv::commandPass(std::string password)
{
	Client &client = _clientsMap[_clientFd];

	// debugShowSpecificClient(client);
	if (client.hasPass == true)
	{
		std::string errorMsg = ":ircserver 462 :" + client._nickName + " You may not reregister\r\n";
		send (_clientFd, errorMsg.c_str(), errorMsg.size(), 0);
		return (false);
	}
	else if (password == _password)
	{
		client.hasPass = true;
		std::string acceptMsg = "You may now have the privilege to register\r\n";
		send (_clientFd, acceptMsg.c_str(), acceptMsg.size(), 0);
		return (true);
	}
	else if (password[0] == '\0')
	{
		std::string errorMsg = ":ircserver 461 : Not enough parameters!\r\n";
		send(_clientFd, errorMsg.c_str(), errorMsg.size(), 0);
		return (false);
	}
	else
	{
		std::string errorMsg = ":ircserver 464 : Password incorrect!\r\n";
		send(_clientFd, errorMsg.c_str(), errorMsg.size(), 0);
		return false;
	}
}