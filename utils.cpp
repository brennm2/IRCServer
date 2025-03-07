/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diodos-s <diodos-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:30:28 by diogosan          #+#    #+#             */
/*   Updated: 2025/03/05 12:09:34 by diodos-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"
#include <cstddef>
#include <string>
#include <vector>

// std::vector<Ircserv::Client>::iterator Ircserv::LookClientInChannel(std::string channel)
// {
// 	std::map<std::string, std::vector<Client> >::iterator It = _channels.find(channel);
// 	if (It != _channels.end())
// 	{
// 		std::vector<Client> clients = It->second;
// 		for (std::vector<Client>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
// 		{
// 			if (clientIt->_fd == _clientFd)
// 			{
// 				// std::cout << "Client FD: " << clientIt->_fd << " found in channel: " << channel << "\n";
// 				return clientIt;
// 			}
// 		}
// 	}
// 	return std::vector<Ircserv::Client>::iterator();
// }

std::string Ircserv::_getChannelTopic(std::string channel)
{
	std::vector<channelsStruct>::iterator It = _channels.begin();
	
	while (It != _channels.end())
	{
		if (It->_channelName == channel)
			return It->_channelTopic;
		It++;
	}
	return NULL;
}


void Ircserv::_changeChannelTopic(std::string &channel, std::string &newTopic)
{
	std::vector<channelsStruct>::iterator It = _channels.begin();
	while (It != _channels.end())
	{
		if (It->_channelName == channel)
		{
			It->_channelTopic = newTopic;
			It->_topicSetter = _clientsMap[_clientFd]._nickName; // Store who set the topic
			It->_topicSetTime = time(NULL); // Store the current timestamp
			return;
		}
		++It;
	}

}