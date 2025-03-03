/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:10:19 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/03 18:32:56 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void Ircserv::signalCatcher(void)
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGQUIT, signalHandler);
}

void Ircserv::signalHandler(int signal)
{
	if(signal == SIGINT)
		std::cout << red << "\nSigInt Ativado" << "\n" << reset;
	else if (signal == SIGQUIT)
		std::cout << red << "\nSigQuit Ativado" << "\n" << reset;
	endServer = true;
}

bool Ircserv::eofChecker(const std::string &buffer)
{
	printAsciiValues(buffer);
	if (buffer.find('\04') != std::string::npos)
	{
		std::cout << red <<  "FOI ENCONTRADO NPOS" << "\n" << reset;
		return true;
	}
	else
		return (false);
}


bool Ircserv::checkIfBufferHasEnd(const std::string &line)
{
	if (line.find('\n') != std::string::npos)
	{
		Client client = returnClientStructToModify(_clientFd);
		std::cout << red <<  "FOI ENCONTRADO BARRA N" << "\n" << reset;
		client.bufferIsReady = true;
		client.buffer += line;
		return (true);
	}
	else
	{
		std::cout << "CLIENTFD -> " << _clientFd << "\n";
		Client client = returnClientStructToModify(_clientFd);
		client.buffer += line;
		return (false);
	}
}