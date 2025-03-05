/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:10:19 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/05 16:38:03 by bde-souz         ###   ########.fr       */
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

bool Ircserv::checkIfClientHasEndedBuffer(const int &clientFd)
{
	Client client = returnClientStruct(clientFd);

	if (client.bufferIsReady)
		return (true);
	else
		return (false);
}

void Ircserv::clientHasSendedBuffer(const int &clientFd)
{
	Client &client = returnClientStructToModify(clientFd);

	client.bufferIsReady = false;

}

std::string Ircserv::returnClientBuffer(const int &clientFd)
{
	Client &client = returnClientStructToModify(clientFd);
	std::string tempStr = client.buffer;
	client.buffer.clear();

	std::cout << "teste->" << tempStr << "\n";
	return tempStr;
}


bool Ircserv::checkIfBufferHasEnd(const std::string &line)
{
	//printAsciiValues(line);
	if (line.find(13) != std::string::npos)
	{
		Client &client = returnClientStructToModify(_clientFd);
		std::cout << red << "FOI ENCONTRADO O 13" << "\n" << reset;
		client.bufferIsReady = true;
		client.buffer += line;
		return (true);
	}
	else
	{
		std::cout << red << "NAO ENCONTROU" << "\n" << reset;
		//std::cout << "CLIENTFD -> " << _clientFd << "\n";
		Client client = returnClientStructToModify(_clientFd);
		client.buffer += line;
		return (false);
	}
}