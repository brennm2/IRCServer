/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:56 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/29 12:17:03 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

//isprintable para password
// talvez tamanho de 8

void checkIfValidPort(char **av, Ircserv Server)
{
	(void) Server; //debug
	std::string tempAv1 = av[1];
	std::string tempAv2 = av[2];

	//check Av1
	if (!tempAv1.empty())
	{
		for (size_t i = 0; i < tempAv1.size() ; i++)
		{
			/* code */
		}
	}
	
}

void checkArguments(char **av, Ircserv Server)
{
	checkIfValidPort(av, Server);
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
			checkArguments(av, Server);
			Server.createServer();
			Server.acceptClients();
		}
		catch(const std::exception& e)
		{
			std::cerr << red << e.what() << '\n';
		}
	}
	else
	{
		std::cout << "Wrong arguments!" << "\n";
		std::cout << "You need to use ./ircserv" << \
			green << " <port> <password>"<< reset << "\n";
	}
}