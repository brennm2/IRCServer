/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 20:11:46 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 17:10:17 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"
#include <iomanip>

void Ircserv::tellTimeOption(const int &clientFd)
{
	std::time_t now = std::time(NULL);
	std::tm* nowTime = std::localtime(&now);
	std::ostringstream dateStream;
	dateStream << std::setw(2) << std::setfill('0') << nowTime->tm_mday << '-'
				<< std::setw(2) << std::setfill('0') << (nowTime->tm_mon + 1) << '-'
				<< (nowTime->tm_year + 1900);

	std::ostringstream timeStream;
	timeStream << std::setw(2) << std::setfill('0') << nowTime->tm_hour << ':'
				<< std::setw(2) << std::setfill('0') << nowTime->tm_min;

	std::string msgTime = "\x03" "03Current Date: " + dateStream.str() + " and Time: " + timeStream.str() + "\r\n";
	send(clientFd, msgTime.c_str(), msgTime.size(), 0);
}

void tellQuoteOption(const int &clientFd)
{
	std::srand(std::time(0));

	std::vector<std::string> quotes;
	quotes.push_back("\x03" "03Tenho mais força na perna esquerda mas escrevo com a direita.\r\n");
	quotes.push_back("\x03" "03No gods or kings. Only man.\r\n");
	quotes.push_back("\x03" "03Why do you ask \"what\", when the delicious question is \"when\"?\r\n");
	quotes.push_back("\x03" "03Hey look, buddy, I'm an Engineer. That means I solve problems.\r\n");
	quotes.push_back("\x03" "03Isso tem tinta ai dentro?\r\n");
	quotes.push_back("\x03" "03The cake is a lie!\r\n");

	int randomIndex = std::rand() % quotes.size();
	std::string msgQuote = quotes[randomIndex];
	send(clientFd, msgQuote.c_str(), msgQuote.size(), 0);
}

void Ircserv::tellMsg(const int &clientFd, const int &msgCount)
{
	std::string msgResponse = "\x03" "03Current count of messages sent: " + to_string(msgCount) + "\r\n";
	send(clientFd, msgResponse.c_str(), msgResponse.size(), 0);
}

void Ircserv::commandBot(const std::string &option)
{
	Client client = returnClientStruct(_clientFd);

	if (option == "HELP")
	{
		std::string msgResponse = "Commands that you can use:\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
		msgResponse = "\x03" "07◎ TELLTIME\n	- Tells the current time\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
		msgResponse = "\x03" "07◎ TELLQUOTE\n	- Tells a random quote\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
		msgResponse = "\x03" "07◎ TELLMSG\n	- Tells the count of message\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
	}
	else if (option == "TELLQUOTE")
		tellQuoteOption(_clientFd);
	else if (option == "TELLTIME")
		tellTimeOption(_clientFd);
	else if (option == "TELLMSG")
		tellMsg(_clientFd, client._msgCount);
	else
	{
		std::string msgResponse = "\x03" "07I didn't catch that, try using /BOT HELP\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);

	}
}