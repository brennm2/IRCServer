/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 13:26:55 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/19 16:21:43 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void printAsciiValues(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        char c = *it;
        std::cout << "Character: " << c << " ASCII: " << static_cast<int>(c) << std::endl;
    }
}



void Ircserv::commandKick(std::istringstream &lineStream)
{
	Client client = returnClientStruct(_clientFd);
	std::string channel, clients;

	lineStream >> channel;
	getline(lineStream, clients);
	std::vector<std::string> clientsVec = splitString(clients, ' ');

	for (std::vector<std::string>::const_iterator it = clientsVec.begin(); \
			it != clientsVec.end(); it++)
	{
		std::string tempClient = *it;
		if (tempClient[0] == ':')
			tempClient.erase(0, 1);
		std::cout << "Client ->" << tempClient << "\n";
		
		std::string kickMsg = ":" + client._nickName + "!" + client._userName + "@localhost KICK " \
			+ channel + " " + tempClient + "\r\n";
		//send(_clientFd, kickMsg.c_str(), kickMsg.size(), 0);
		broadcastMessageToChannel(kickMsg, channel);
		removeClientFromChannel(channel, returnClientFd(tempClient));
	}
}