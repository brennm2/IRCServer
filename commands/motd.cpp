/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 16:38:58 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/06 15:33:27 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::commandMotd()
{
	std::vector<std::string> art;
    art.push_back("  .''. ");
    art.push_back(" (~~~~) ");
    art.push_back("   || ");
    art.push_back(" __||__ ");
    art.push_back("/______\\ ");
    art.push_back("  |  |' _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ ");
    art.push_back("  |  |'|o| - - - - - - - - - - - - - - - - - - - - - - - - -|| ");
    art.push_back("  |  |'| |                                                  || ");
    art.push_back("  |  |'| |                      . ' .                       || ");
    art.push_back("  |  |'| |                  . '       ' .                   || ");
    art.push_back("  |  |'| |              . '    .-\"'\"'-.    ' .              || ");
    art.push_back("  |  |'| |          . '      ,\"       \".      ' .           || ");
    art.push_back("  |  |'| |      . '        /:           :\\        ' .       || ");
    art.push_back("  |  |'| |  . '            ;  .          ;            ' .   || ");
    art.push_back("  |  |'| |    ' .          \\: . .       :/          . '     || ");
    art.push_back("  |  |'| |        ' .        `. . .    ,/       . '         || ");
    art.push_back("  |  |'| |            ' .      `-.,,.-'     . '             || ");
    art.push_back("  |  |'| |                ' .           . '                 || ");
    art.push_back("  |  |'| |                    ' .   . '                     || ");
    art.push_back("  |  |'| |                        '                         || ");
    art.push_back("  |  |'|o|-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_|| ");
    art.push_back("  |  |' ");
    art.push_back("  |  |' ");
    art.push_back("  |  |' ");
    art.push_back("  |  |' ");
    art.push_back("  '~~'  ");

	Client client = returnClientStruct(_clientFd);
	std::string mtdoMessage = ":ircserver 375 :" + client._nickName + " - Message of the day - \r\n";
	send(_clientFd, mtdoMessage.c_str(), mtdoMessage.size(), 0);


	for (std::vector<std::string>::const_iterator it = art.begin(); \
			it != art.end(); it++)
	{
		mtdoMessage = ":ircserver 372 :" + client._nickName + *it + "\r\n";
		send(_clientFd, mtdoMessage.c_str(), mtdoMessage.size(), 0);
	}

	mtdoMessage = ":ircserver 376 :" + client._nickName + " \r\n";
	send(_clientFd, mtdoMessage.c_str(), mtdoMessage.size(), 0);
}


