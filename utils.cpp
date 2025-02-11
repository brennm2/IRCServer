/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogosan <diogosan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:30:28 by diogosan          #+#    #+#             */
/*   Updated: 2025/02/06 15:30:59 by diogosan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"
#include <cstddef>
#include <string>
#include <vector>

std::vector<Ircserv::Client>::const_iterator Ircserv::LookClientInChannel(std::string channel)
{
	std::map<std::string, std::vector<Client>>::const_iterator It = _channels.find(channel);
	if (It != _channels.end())
	{
		const std::vector<Client>& clients = It->second;
		for (std::vector<Client>::const_iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
		{
			if (clientIt->_fd == _clientFd)
			{
				// std::cout << "Client FD: " << clientIt->_fd << " found in channel: " << channel << "\n";
				return clientIt;
			}
		}
	}
	return std::vector<Ircserv::Client>::const_iterator();
}

std::string Ircserv::_getChannelTopic(std::string channel)
{
	std::map<std::string, std::string>::const_iterator It = _channelTopics.find(channel);
	
	if (It != _channelTopics.end())
	{
		if (It->first == channel)
			return It->second;
	}
	return NULL;
}


void Ircserv::_changeChannelTopic(std::string &channel, std::string &newTopic)
{
	std::map<std::string, std::string>::iterator It = _channelTopics.find(channel);
	
	if (It != _channelTopics.end())
	{
		if (It->first == channel)
			It->second = newTopic;
	}

}