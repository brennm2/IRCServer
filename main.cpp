/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:56 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/11 11:33:47 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"
#include <climits>
#include <stdexcept>

//isprintable para password
// talvez tamanho de 8

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


// void server(void)
// {
// 	// creating socket
//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

//     // specifying the address
//     sockaddr_in serverAddress;
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(8080);
//     serverAddress.sin_addr.s_addr = INADDR_ANY;

//     // binding socket.
//     bind(serverSocket, (struct sockaddr*)&serverAddress,
//          sizeof(serverAddress));

// 	while (1)
// 	{
// 		// listening to the assigned socket
// 		listen(serverSocket, 5);

// 		// accepting connection request
// 		int clientSocket
// 			= accept(serverSocket, NULL, NULL);

// 		// recieving data
// 		char buffer[1024] = { 0 };
// 		recv(clientSocket, buffer, sizeof(buffer), 0);
// 		std::cout << "Message from client: " << buffer
// 				<< std::endl;

// 	}
//     // closing the socket.
//     close(serverSocket);

//     return ;
// }


int main(int ac, char **av)
{
	(void) av; //#TODO Debug
	if (ac == 3)
	{
		Ircserv Server;
		try
		{
			checkIfValidPort(av[1]);
			Server.createServer(av[2], std::atoi(av[1]));
			Server.acceptClients();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else
	{
		std::cout << "Wrong arguments!" << "\n";
		std::cout << "You need to use ./ircserv" << \
			green << " <port> <password>"<< reset << "\n";
	}
}