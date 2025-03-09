/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 20:11:46 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/09 21:21:03 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Ircserv.hpp"

void Ircserv::tellTimeOption(const int &clientFd)
{
	std::string date = to_string(now->tm_mday ) + '-' \
		+ to_string(now->tm_mon + 1) + '-' \
		+ to_string(now->tm_year + 1900);

	std::string time = 	to_string(now->tm_hour) + ':' \
		+ to_string(now->tm_min);
	
	std::string msgTime = "\x03" "05Current Date: " + date + ", and Time: " + time + "\r\n";
	send(clientFd, msgTime.c_str(), msgTime.size(), 0);

}

void tellQuoteOption(const int &clientFd)
{
	std::srand(std::time(0));

	std::vector<std::string> quotes;
	quotes.push_back("\x03" "05Tenho mais força na perna esquerda mas escrevo com a direita.	(rumachad)\r\n");
	quotes.push_back("\x03" "05No gods or kings. Only man.	(Andrew Ryan)\r\n");
	quotes.push_back("\x03" "05Why do you ask \"what\", when the delicious question is \"when\"?.	(Lutece Twins)\r\n");
	quotes.push_back("\x03" "05Hey look, buddy, I'm an Engineer. That means I solve problems.	(Engineer)\r\n");
	quotes.push_back("\x03" "05The cake is a lie!\r\n");

	
	int randomIndex = std::rand() % quotes.size();
	std::string msgQuote = quotes[randomIndex];
	send(clientFd, msgQuote.c_str(), msgQuote.size(), 0);
}


void Ircserv::commandBot(const std::string &option)
{
	if (option == "HELP")
	{
		std::string msgResponse = "Commands that you can use:\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
		msgResponse = "\x03" "09◎ TELLTIME\n	- Tells the current time\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
		msgResponse = "\x03" "09◎ TELLQUOTE\n	- Tells a random quote\r\n";
		send(_clientFd, msgResponse.c_str(), msgResponse.size(), 0);
	}
	else if (option == "TELLQUOTE")
		tellQuoteOption(_clientFd);
	else if (option == "TELLTIME")
		tellTimeOption(_clientFd);
}