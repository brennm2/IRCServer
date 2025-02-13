/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:54 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/13 15:09:05 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


void Ircserv::createServer(const std::string& pass, unsigned int port)
{
	_startTimer = std::time(0);
	now = std::localtime(&_startTimer);

	_password = "123";
	(void) pass;
	//visualLoadingServer();
	// if (!_checkStartPass(pass))
	// 	throw std::runtime_error("Wrong Password");

	_port = port;
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

	// Set non-blocking mode for the server socket
	int flags = fcntl(_serverFd, F_GETFL, 0);
	if (flags == -1 || fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(_serverFd);
		throw std::runtime_error("Error setting server socket to non-blocking mode!");
	}

	// Server config
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; //Aceita qualquer tipo de endereco
	std::cout << _port << std::endl;
	server_addr.sin_port = htons(_port);


	// Bind the server to the socket
	if (bind(_serverFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close (_serverFd);
		std::ostringstream textMsg;
		textMsg << "Error while binding server to the socket number: " << _port;
		throw std::runtime_error(textMsg.str());
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
	std::vector<pollfd> poll_fds;
	
	// Add server socket to poll list
	pollfd server_pollfd;
	server_pollfd.fd = _serverFd;
	server_pollfd.events = POLLIN; // Listen for incoming connections
	poll_fds.push_back(server_pollfd);

	std::cout << green << "Server is listening for connections...\n" << reset;
	
	while (true)
	{
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
		if (poll_count == -1)
		{
			if (errno == EINTR)
				continue; // Retry if interrupted by a signal
			std::cerr << red << "Poll error: " << strerror(errno) << "\n" << reset;
			break; // Stop server on unexpected error
		}

		// Iterate through all file descriptors
		std::vector<int> removeIndices;
		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			if (poll_fds[i].revents & (POLLIN | POLLHUP)) // Check if there's incoming data
			{
				if (poll_fds[i].fd == _serverFd)
				{
					// Accept new client
					sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int clientFd = accept(_serverFd, (struct sockaddr*)&client_addr, &client_len);
					if (clientFd < 0)
					{
						std::cerr << red << "Error accepting client\n" << reset;
						continue;
					}

					// Set non-blocking mode for the client socket
					int flags = fcntl(clientFd, F_GETFL, 0);
					if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
					{
						std::cerr << red << "Error setting client socket to non-blocking mode!\n" << reset;
						close(clientFd);
						continue;
					}
					
					std::cout << green << "New client connected! FD: " << clientFd << "\n" << reset;
					
					//Mensagem de boas vindas
					//"\x03" -> indica que e um codigo de cor
					//"01,02Teste" -> primeiro numero e a cor da letra e o segundo e a cor de fundo
					// obs: nao precisa ter cor de fundo
					const char *welcomeMsg = "\x03""04,01Welcome test!\n";
					send(clientFd, welcomeMsg, strlen(welcomeMsg), 0);

					// Add new client to poll list
					pollfd client_pollfd;
					client_pollfd.fd = clientFd;
					client_pollfd.events = POLLIN; // Ready to read
					poll_fds.push_back(client_pollfd);

					// Add client to map
					_clientsMap[clientFd] = Client(); // Initialize empty client
				}
				else
				{
					// Existing client sent data
					char buffer[512];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytes_received = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					
					if (bytes_received <= 0)
					{
						std::cout << red << "Client disconnected: " << poll_fds[i].fd << "\n" << reset;
						close(poll_fds[i].fd);
						_clientsMap.erase(poll_fds[i].fd);
						removeIndices.push_back(i);
						continue;
					}

					std::cout << green << "Received from " << poll_fds[i].fd << ": " << reset << buffer;
					
					// Process the message
					bufferReader(poll_fds[i].fd, buffer);
				}
			}
		}
		// Remove disconnected clients **after** iterating
		for (int i = removeIndices.size() - 1; i >= 0; --i)
			poll_fds.erase(poll_fds.begin() + removeIndices[i]);
	}
}


void Ircserv::bufferReader(int clientFd, char *buffer)
{
	std::istringstream stringSplit(buffer);
	std::string line;

	while (std::getline(stringSplit, line))
	{
		if (line.empty())
			continue;

		std::istringstream lineStream(line);
		std::string command;
		lineStream >> command;

		std::cout << "Command: " << command << "\n";
		_clientFd = clientFd;
		
		if (command == "JOIN")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			std::string channelName;
			lineStream >> channelName;
			
			commandJoin(channelName);
		}
		else if (command == "PART")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			checkCommandPart(lineStream); //do all that PART has to do
		}
		else if (command == "TOPIC")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			checkCommandTopic(lineStream);
		}
		else if (command == "NICK")
		{
			std::string nickName;
			lineStream >> nickName;
			
			commandNick(clientFd, nickName);
		}
		else if (command == "USER")
		{
			commandUser(lineStream); // Process the full user info
		}
		else if (command == "PRIVMSG")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			commandPrivMSG(lineStream);
		}
		else if (command == "PASS")
		{
			std::string password;
			lineStream >> password;
			if(!commandPass(password))
				return ;
		}
		else if (command == "PING")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			std::string token;
			lineStream >> token;
			commandPing(token);
		}
		else if (command == "MTDO")
		{
			if (!clientCanUseCommands(clientFd))
				continue ;
			commandMtdo();
		}
		else if (command == "QUIT")
		{
			// if (!clientCanUseCommands(clientFd))
			// 	continue ;
			
			commandQuit(lineStream);
			return ;
		}

		else if (command == "DDEBUG")
		{
			debugShowAllClients();
			debugShowChannelsInfo();
		}
		// else
		// {
		// 	std::string errorMsg = "Error: Unknown command " + command + "\n";
		// 	send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
		// }

		lineStream.clear();
		Client client = returnClientStruct(clientFd);
		if (client.hasNick && client.hasUser && client.hasPass && !client.hasFinalReg)
			clientFinalRegistration(clientFd);
	}

}

//Commands
void Ircserv::commandJoin(const std::string &channel)
{
	if (!commandJoinCheck(channel))
		return ;

	//Se nao existir, cria um novo canal
	if (_channels.find(channel) == _channels.end())
	{
		std::cout << green << "Nao existe channel, criado um novo" << channel << "\n" << reset;
		_channels.insert(std::make_pair(channel, std::vector<Client>()));
		_channels[channel].push_back(returnClientStruct(_clientFd));

		std::string testeMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + _clientsMap[_clientFd]._userName + "@localhost JOIN " + channel + "\r\n";
		send(_clientFd, testeMsg.c_str(), testeMsg.size(), 0);

		std::cout << red << "Client Join command ->" << testeMsg << "\n" << reset;

		std::string msgTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + channel + " :My cool server yay!\r\n";
		send(_clientFd, msgTopic.c_str(), msgTopic.size(), 0);

		_channelTopics.insert(std::make_pair(channel ,"My cool server yay!"));
		makeUserList(channel);
		return ;
	}
	else
	{
		//Verifica se ja existe o clientFd igual no canal, caso nao tenha, coloque na lista
		if (!checkIfClientInChannel(_channels, channel, _clientFd))
		{
			_channels[channel].push_back(returnClientStruct(_clientFd));
		}
	}
	
	//Mensagem de confirmacao para dar JOIN
	std::string testeMsg = ":" + _clientsMap[_clientFd]._nickName + "!" + _clientsMap[_clientFd]._userName + "@localhost JOIN " + channel + "\r\n";
	send(_clientFd, testeMsg.c_str(), testeMsg.size(), 0);


	std::string msgTopic = ":ircserver 332 " + _clientsMap[_clientFd]._nickName + " " + channel + " :" + _getChannelTopic(channel) + "\r\n";
	send(_clientFd, msgTopic.c_str(), msgTopic.size(), 0);

	makeUserList(channel);

	return ;
}


bool Ircserv::_checkStartPass(const std::string& pass)
{
	return pass == _password;
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