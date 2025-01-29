/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:49 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/29 18:50:46 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <map>
#include <vector>

// COLORS //
#define blue "\033[34m"
#define red "\033[31m"
#define green "\033[32m"
#define yellow "\033[33m"
#define magenta "\033[35m"
#define cyan "\033[36m"
#define reset "\033[0m"


class Ircserv 
{
private:
	// nome do canal - FDs dos usuarios
	std::map<std::string, std::vector<int> > _channels;
	std::map<int, std::string> _users;



	std::string		_password;
	unsigned int	_port;



	int				_clientFd;
	int				_serverFd;

public:
	void createServer();
	void acceptClients();

	//Lida com as mensagens
	void bufferReader(char *buffer);


	//Commands
	void commandJoin(const std::string &channel);



	//Debug
	void debugShowChannelInfo();


	//Visual Functions
	void visualLoadingServer(void);
} ;
