/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:54 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/28 18:45:40 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void Ircserv::createServer(void)
{
	_port = 0; // esta aqui apenas para tirar error das flags de compilacao

	//Create server
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		throw std::runtime_error("Error while creating the socket!");
	}


	// Server config
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; //Aceita qualquer tipo de endereco
	server_addr.sin_port = htons(6667); //#TODO Porta que vai ser utilizada, Server->_port;


	// Bind the server to the socket
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close (server_fd);
		throw std::runtime_error("Error while binding server to the socket!");
	}

	// Listen for connections
	if (listen(server_fd, 5) < 0)
	{
		close(server_fd);
		throw std::runtime_error("Error while listening connections!");
	}

	std::cout << green << "IRC Server up and ready to go!" << "\n";
}