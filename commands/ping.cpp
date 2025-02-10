/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 18:11:50 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/10 18:52:03 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandPing(std::string token)
{
	Client client = returnClientStruct(_clientFd);
	// _buffer = ":" + _nick + "!" + _username + "@" + _hostname + " " + command;
	std::string pongMsg = ":" + client._nickName + "!" + client._userName + "@" \
	 + "localhost " + "PONG " + token + "\r\n";
	send(_clientFd, pongMsg.c_str(), pongMsg.size(), 0);

	std::cout << "pongMsg: " << pongMsg << "\n";

	// #TODO ERR_NEEDMOREPARAMS (461)
	// #TODO ERR_NOORIGIN (409)

}