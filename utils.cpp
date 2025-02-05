/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogosan <diogosan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 14:30:28 by diogosan          #+#    #+#             */
/*   Updated: 2025/02/05 17:36:01 by diogosan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

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