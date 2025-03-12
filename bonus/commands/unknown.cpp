/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unknown.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 18:11:22 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/10 16:52:46 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandUnknown(const std::string &command)
{
	// printAsciiValues(command);
	if (command == "WHO" || command == "CAP" || command.empty())
		return ;
	Client client = returnClientStruct(_clientFd);

	std::string errMsg;
	if (client.hasNick)
		errMsg = ":ircserver 421 " + client._nickName + " " + command + " :Unknown command\r\n";
	else
		errMsg = ":ircserver 421 * " + command + " :Unknown command\r\n";

	send(_clientFd, errMsg.c_str(), errMsg.size(), 0);
}