/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:56 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/16 00:57:57 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool isNumber(std::string &str)
{
	std::string::iterator it = str.begin();

	while (it != str.end())
	{
		if (!std::isdigit(*it))
			return false;
		it++;
	}
	return true;
}

void checkIfValidPort(std::string arg)
{
	std::string port = arg;

	if (!isNumber(port))
		throw std::runtime_error("Port must be a number");

	if (atoi(arg.c_str()) > 65535 || atof(arg.c_str()) > INT_MAX)
		throw std::runtime_error("Port not available in system range");
	
}

void checkIfValidPassWord(const std::string &pass)
{
	if (pass.size() > 8 || pass.size() < 3)
		throw std::runtime_error ("Invalid Password (invalid size, min 3 or max 8)");

	for(std::string::const_iterator it = pass.begin(); 
		it != pass.end(); it++)
	{
		if (!std::isprint(*it) || *it == ' ')
			throw std::runtime_error ("Invalid Password (invalid character)");
	}
}

int main(int ac, char **av)
{
	if (ac == 3)
	{
		signal(SIGPIPE, SIG_IGN);
		Ircserv Server;
		try
		{
			checkIfValidPort(av[1]);
			checkIfValidPassWord(av[2]);

			Server.createServer(av[2], std::atoi(av[1]));
			Server.acceptClients();
		}
		catch(const std::exception& e)
		{
			std::cerr << red << e.what() << '\n' << reset;
		}
	}
	else
	{
		std::cout << red << "Wrong arguments!" << "\n" << reset;
		std::cout << "You need to use ./ircserv" << \
			green << " <port> <password>"<< reset << "\n";
	}
	return (0);
}
