#include "Ircserv.hpp"


bool Ircserv::checkIfClientInServer(std::map<std::string, std::vector<int> > channelMap, std::string channel, int clientFd)
{
	//se existe um channel no channelMap
	if (channelMap.find(channel) != channelMap.end())
	{
		std::vector<int>& clients = channelMap[channel];
		std::vector<int>::iterator it = clients.begin();

		while(it != clients.end())
		{
			if (*it == clientFd)
			{
				std::cout << "IT: " << *it << "\n";
				return (true);
			}
			it++;
		}
	}
	std::cout << "NAO HA ESSE CLIENTE NO SERVER" << "\n";
	return (false);
}