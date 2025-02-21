#include "Ircserv.hpp"

void Ircserv::checkCommandMode(std::istringstream &lineStream)
{
	std::string target, modes, parameters;
	lineStream >> target >> modes;
	std::getline(lineStream, parameters);

	if (target.empty())
	{
		std::string errMsg = ":ircserver 461 MODE :Not enough parameters\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}
	
	std::vector<channelsStruct>::iterator It = _channels.begin();
	while (It != _channels.end())
	{
		if (It->_channelName == target)
		break;
		++It;
	}

	if (target[0] == '#') // It's a channel
	{
		if (It == _channels.end())
		{
			std::string errMsg = ":ircserver 403 " + target + " :No such channel!\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeChannel(target, modes, parameters);
	}

	else // It's a user
	{
		if (_clientsMap.find(_clientFd) == _clientsMap.end())
		{
			std::string errMsg = ":ircserver 401 " + target + " :No such nick\r\n";
			send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
			return;
		}
		commandModeUser(target, modes, parameters);
	}
}

void Ircserv::commandModeChannel(std::string &channelName, std::string &modes, std::string &parameters)
{
	std::vector<channelsStruct>::iterator channelIt = _channels.begin();
	while (channelIt != _channels.end())
	{
		if (channelIt->_channelName == channelName)
		break;
		++channelIt;
	}

	if (channelName.empty() || channelName[0] != '#' || channelIt == _channels.end())
	{
		std::string errMsg = ":ircserver 403 " + channelName + " :No such channel!\r\n";
		send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
		return;
	}

	// Check if client is in the channel
	
}