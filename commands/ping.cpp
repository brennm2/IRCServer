/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 18:11:50 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/25 18:32:44 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandPing(std::string token)
{
	Client client = returnClientStruct(_clientFd);

	if (token.find("LAG", 0) != std::string::npos)
		token.erase(0, 3);

	std::string pongMsg = ":" + client._nickName + "!" + client._userName + "@" \
	 + "localhost " + "PONG localhost " + token + "\r\n";
	send(_clientFd, pongMsg.c_str(), pongMsg.size(), 0);




	// #TODO ERR_NEEDMOREPARAMS (461)
	// #TODO ERR_NOORIGIN (409)

}