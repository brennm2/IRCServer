/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:54 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/13 12:18:04 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool Ircserv::endServer = false;

void Ircserv::createServer(const std::string& pass, unsigned int port)
{
	_startTimer = std::time(0);
	now = std::localtime(&_startTimer);

	//visualLoadingServer();
	
	_password = pass;
	_port = port;
	
	// Create a TCP socket
	this->_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		throw std::runtime_error("Error while creating the socket!");
	}

	// Set socket options, SO_REUSEADDR allows the socket to reuse the address immediately after a restart
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Error while setting socket options!");
	}

	// Set non-blocking mode for the server socket
	int flags = fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	if (flags == -1)
	{
		close(_serverFd);
		throw std::runtime_error("Error setting server socket to non-blocking mode!");
	}

	// Server config
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET; // IPv4
	server_addr.sin_addr.s_addr = INADDR_ANY; // Accepts any ip address
	server_addr.sin_port = htons(_port); // Converts port number to network byte order


	// Assign the socket to the port, giving and address so that clients can use to find the server
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
	Ircserv serv;
	
	// Add server socket to poll list
	pollfd server_pollfd; // Pollfd struct for the server socket
	server_pollfd.fd = _serverFd;
	server_pollfd.events = POLLIN; // Listen for incoming connections
	server_pollfd.revents = 0;
	poll_fds.push_back(server_pollfd); // poll_fds stores all the fds that poll will monitor

	std::cout << green << "Server is listening for connections...\n" << reset;
	
	// Main loop
	while (true && !endServer)
	{
		signalCatcher();
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1); // Wait for activity and handle multiple fds
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
			if (poll_fds[i].revents & POLLIN) // Handle incoming connections
			{
				if (poll_fds[i].fd == _serverFd) // Accept a new client
				{
					sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int clientFd = accept(_serverFd, (struct sockaddr*)&client_addr, &client_len);
					if (clientFd < 0)
					{
						std::cerr << red << "Error accepting client\n" << reset;
						continue;
					}

					// Set non-blocking mode for the client socket
					int flags = fcntl(clientFd, F_SETFL, O_NONBLOCK);
					if (flags == -1)
					{
						std::cerr << red << "Error setting client socket to non-blocking mode!\n" << reset;
						close(clientFd);
						continue;
					}
					
					std::cout << green << "New client connected! FD: " << clientFd << "\n" << reset;

					// Add new client to poll list
					pollfd client_pollfd;
					client_pollfd.fd = clientFd;
					client_pollfd.events = POLLIN; // Initially only listening for input
					client_pollfd.revents = 0;
					poll_fds.push_back(client_pollfd);

					// Add client to map
					_clientsMap[clientFd] = Client(); // Initialize empty client
				}
				else
				{
					// Existing client sent data
					char buffer[512];
					memset(buffer, 0, sizeof(buffer));
					ssize_t bytes_received = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0); // Receive data from client
					
					// Handle client disconncetion	
					if (bytes_received <= 0)
					{
						int tempFd = poll_fds[i].fd;
						disconnectClientFromEveryChannel(tempFd);
						std::cout << red << "Client disconnected: " << poll_fds[i].fd << "\n" << reset;
						close(poll_fds[i].fd);
						_clientsMap.erase(poll_fds[i].fd);
						removeIndices.push_back(i);
						continue;
					}
					// Process the message
					bufferReader(poll_fds[i].fd, buffer);
				}
			}
			
			// Check if the client has data to send
			if (poll_fds[i].revents & POLLOUT) //Handle outgoing messages
			{
				Client &client = _clientsMap[poll_fds[i].fd];
				if (!client.outgoingBuffer.empty()) // Send buffered messages
				{
					ssize_t bytesSent = send(poll_fds[i].fd, client.outgoingBuffer.c_str(), client.outgoingBuffer.size(), 0);
					
					if (bytesSent > 0) // Remove sent data from buffer
					{
						client.outgoingBuffer.erase(0, bytesSent); // Remove sent part
						if (client.outgoingBuffer.empty())
						{
							poll_fds[i].events &= ~POLLOUT; //Disable POLLOUT if nothing left to send
						}
					}
					else if (bytesSent < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) // Handle send errors
					{
						std::cerr << red << "Error sending data to client: " << poll_fds[i].fd << "\n" << reset;
						close(poll_fds[i].fd);
						_clientsMap.erase(poll_fds[i].fd);
						removeIndices.push_back(i);
					}
				}
			}
		}
		// Remove disconnected clients **after** iterating
		for (int i = removeIndices.size() - 1; i >= 0; --i)
			poll_fds.erase(poll_fds.begin() + removeIndices[i]);
	}
	return ;
}

void betterPrint(std::string str)
{
	if (str.find(13) != std::string::npos)
	{
		str.erase(str.size() - 1);
	}
}

void Ircserv::bufferReader(int clientFd, char *buffer)
{
	_clientFd = clientFd;
	if (checkIfBufferHasEnd(buffer))
	{
		std::string line;
		std::istringstream stringSplit(returnClientBuffer(clientFd));
		while (std::getline(stringSplit, line))
		{
			if (line.empty())
				continue;

			if (checkIfClientHasEndedBuffer(clientFd))
			{
				std::istringstream lineStream(line);
				//std::istringstream lineStream(returnClientBuffer(clientFd));
				std::cout << green << "Received from " << clientFd << ": " << reset << lineStream.str() << "\n";

				std::string command;
				lineStream >> command;

				if (command == "JOIN")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					std::string channelName, key;
					lineStream >> channelName;
					lineStream >> key;
					
					commandJoin(channelName, key);
				}
				else if (command == "PART")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					std::string channels, reason;

					lineStream >> channels;
					lineStream >> std::ws;
					getline(lineStream, reason);
					checkCommandPart(channels, reason);
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
				else if (command == "PASS")
				{
					std::string password;
					lineStream >> password;
					if(!commandPass(password))
						return ;
				}
				else if (command == "PRIVMSG")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					commandPrivMSG(lineStream);
				}
				else if (command == "PING")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					std::string token;
					lineStream >> token;
					commandPing(token);
				}
				else if (command == "MOTD")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					commandMotd();
				}
				else if (command == "QUIT")
					return commandQuit(lineStream);
				else if (command == "KICK")
				{
					if (!clientCanUseCommands(clientFd))
						continue ;
					commandKick(lineStream);
				}
				else if (command == "DDEBUG")
				{
					debugShowAllClients();
					debugShowChannelsInfo();
				}
				else if (command == "MODE")
				{
					if (!clientCanUseCommands(clientFd))
						continue;
					checkCommandMode(lineStream);
				}
				else if(command == "INVITE")
				{
					if (!clientCanUseCommands(clientFd))
						continue;
					std::string clients, channels;
					lineStream >> clients;
					lineStream >> channels;
					commandInvite (clients, channels);
				}
				else
					commandUnknown(command);

				lineStream.clear();
				Client client = returnClientStruct(clientFd);
				if (client.hasNick && client.hasUser && client.hasPass && !client.hasFinalReg)
					clientFinalRegistration(clientFd);
			}
		}
	}

	clientHasSendedBuffer(clientFd);

}

//Commands



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
