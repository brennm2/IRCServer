/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogosan <diogosan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:49 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/05 11:07:50 by diogosan         ###   ########.fr       */
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
#include <sstream>


// COLORS //
#define blue "\033[34m"
#define red "\033[31m"
#define green "\033[32m"
#define yellow "\033[33m"
#define magenta "\033[35m"
#define cyan "\033[36m"
#define reset "\033[0m"


struct Client
{
	std::string _nickName;
	std::string _userName;
	std::string _realName;
};


class Ircserv 
{
private:
	// nome do canal - FDs dos usuarios
	std::map<std::string, std::vector<int> > _channels;
	std::map<int, Client> _clientsMap;



	std::string		_password = "colhapos";
	unsigned int	_port;



	int				_clientFd;
	int				_serverFd;

	bool _checkStartPass(const std::string& pass);
	bool _checkStartPort(const unsigned int port);



public:
	void createServer(const std::string& pass, unsigned int port);
	void acceptClients();
	void broadcastMessage(const std::string& message, int sender_fd);

	//Lida com as mensagens
	void bufferReader(char *buffer);


	//Commands
	void commandJoin(const std::string &channel);
	void commandNick(int clientFd, const std::string &str);
	void commandUser(std::istringstream &lineStream);

	//Help Functions
	bool checkIfClientInServer(std::map<std::string, std::vector<int> > channelMap, \
		std::string channel, int clientFd);


	//Debug
	void debugShowChannelInfo();
	void debugShowLastClient(void);
	void debugShowAllClients(void);


	//Visual Functions
	void visualLoadingServer(void);

	
} ;
