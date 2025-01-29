/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:54 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/29 18:52:48 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


void Ircserv::createServer(void)
{
	//visualLoadingServer();


	_port = 0; // esta aqui apenas para tirar error das flags de compilacao

	//Create server
	this->_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		throw std::runtime_error("Error while creating the socket!");
	}

	// Set socket options
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Error while setting socket options!");
	}

	// Server config
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; //Aceita qualquer tipo de endereco
	server_addr.sin_port = htons(6667); //#TODO Porta que vai ser utilizada, Server->_port;


	// Bind the server to the socket
	if (bind(_serverFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close (_serverFd);
		throw std::runtime_error("Error while binding server to the socket!");
	}

	// Listen for connections
	if (listen(_serverFd, 5) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Error while listening connections!");
	}

	std::cout << green << "IRC Server up and ready to go!" << reset << "\n";
}

void Ircserv::acceptClients()
{
	while (true)
	{
		sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		this->_clientFd = accept(this->_serverFd, (struct sockaddr*)&client_addr, &client_len);
		if (_clientFd < 0)
		{
			std::cerr << red << "Error while accepting connection" << "\n" << reset;
			continue;
		}
		else
			std::cout << green << "Client Connected!" << "\n" << reset;
		
		//Mensagem de boas vindas
		//"\x03" -> indica que e um codigo de cor
		//"01,02Teste" -> primeiro numero e a cor da letra e o segundo e a cor de fundo
		// obs: nao precisa ter cor de fundo
		const char *welcomeMsg = "\x03""04,01Welcome test!\n";
		send(_clientFd, welcomeMsg, strlen(welcomeMsg), 0);

		//Receber mensagens
		char buffer[512];
		while(true)
		{
			std::memset(buffer, 0, sizeof(buffer));
			ssize_t bytes_received = recv(_clientFd, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received <= 0)
			{
				std::cout << red << "User disconected!" << "\n" << reset;
				break ;
			}
			std::cout << green << "Recebido: " << reset << buffer;

			bufferReader(buffer);
			
		}
		close (_clientFd);
	}
	close (_serverFd);
}


void Ircserv::bufferReader(char *buffer)
{
	std::istringstream stringSplit(buffer);
	std::string command;
	std::string channel;


	stringSplit >> command;
	stringSplit >> channel;


	if (command == "JOIN")
	{
		std::cout << "JOIN CHANNEL" << "\n";
		commandJoin(channel);
	}
	else if (command == "NICK");
	{
		// getNick(channel);
		;
	}
	// if (stringSplit == "JOIN")

}



//Commands
void Ircserv::commandJoin(const std::string &channel)
{
	//Se nao existir, cria um novo
	if (_channels.find(channel) == _channels.end())
	{
		_channels.insert(std::pair<std::string, int>(channel, 0));
	}
	_channels[channel].push_back(_clientFd);

	std::string joinMsg;
	//joinMsg = "\r\n";
	joinMsg = std::to_string(_clientFd) + " joined " + channel + "!\r\n";
	send(_clientFd, joinMsg.c_str(), joinMsg.size(), 0);
	// joinMsg = "teste mensagem\r\n";

	// debugShowChannelInfo();
}





// Visual Functions
void Ircserv::visualLoadingServer(void)
{
	std::cout << cyan << "Loading Server" << std::flush;
	sleep(1);
	std::cout << "." << std::flush;
	sleep(1);
	std::cout << "." << std::flush;
	sleep(1);
	std::cout << "." << reset << std::flush;
	sleep(1);
	std::cout << "\n";
}


//Debug Functions




// 00 - Branco  
// 01 - Preto  
// 02 - Azul Marinho  
// 03 - Verde  
// 04 - Vermelho  
// 05 - Marrom  
// 06 - Roxo  
// 07 - Laranja  
// 08 - Amarelo  
// 09 - Verde Claro  
// 10 - Azul Claro  
// 11 - Ciano  
// 12 - Azul  
// 13 - Rosa  
// 14 - Cinza  
// 15 - Cinza Claro  