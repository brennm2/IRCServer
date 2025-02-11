/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:49 by bde-souz          #+#    #+#             */
/*   Updated: 2025/02/11 17:56:52 by bde-souz         ###   ########.fr       */
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
#include <poll.h>
#include <ctime>
#include <stdlib.h>
#include <fcntl.h>
#include <cerrno>


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
		
		struct Client
		{
			int			_fd;
			std::string _nickName;
			std::string _userName;
			std::string _realName;
			bool		isFirstTime;
			bool		hasPass;
			bool		hasNick;
			bool		hasUser;
			bool		hasFinalReg;

		Client() :_fd(-1), _nickName(), \
		_userName(), _realName(), isFirstTime(true), \
		hasPass(false), hasNick(false), hasUser(false), hasFinalReg(false) {}
};
		
		
		std::map<std::string, std::vector<Client> > _channels;
		
		std::map<std::string, std::string > _channelTopics;
		
		std::map<int, Client> _clientsMap;



	std::string		_password;
	unsigned int	_port;



	int				_clientFd;
	int				_serverFd;
	time_t			_startTimer;
	std::tm*		now;



		bool _checkStartPass(const std::string& pass);
		bool _checkStartPort(const unsigned int port);
	

		std::string _getChannelTopic(std::string channel);
		void 		_changeChannelTopic(std::string &channel, std::string &newTopic);

	public:


		void createServer(const std::string& pass, unsigned int port);
		void acceptClients();

		//Lida com as mensagens
		void bufferReader(int clientFd, char *buffer);


		//Commands
		void commandJoin(const std::string &channel);
			void commandUser(std::istringstream &lineStream);
		//-------------------mudkip------------------
		void commandPart(std::string &channelName); 
		void checkCommandPart(std::istringstream &lineStream);
		void commandTopic(std::string &channelName, std::string &newTopic);
		void checkCommandTopic(std::istringstream &lineStream);
		void commandPrivMSG(std::istringstream &lineStream);

	//Help Functions
	bool checkIfClientInChannel(std::map<std::string, std::vector<Client> > channelMap, \
		std::string channel, int clientFd);
	bool checkIfClientInServer(int clientFd);
	bool checkIfClientInServerByNick(std::string clientNick);
	void clientFinalRegistration(int clientFd);
	bool checkIfChannelExist(std::string channel);
	Client returnClientStruct(int clientFd);
		void makeUserList(std::string channel);
	int  returnClientFd(std::string clientNick);

	bool privMsgSintaxCheck(std::string firstWord, std::string target);


	void broadcastMessageToChannel(const std::string& message, std::string channel);
	void broadcastMessageToChannelExceptSender(const std::string& message, std::string channel, int senderFd);
	void broadcastMessage(const std::string& message, int sender_fd);
	void broadcastMessagePrivate(const std::string &message, const std::string &target);

	std::string to_string(int value);


	//CommandNick----
	void commandNick(int clientFd, const std::string &str);
	void nickReplyMsg001(std::string nickName, int clientFd);
	void nickReplyMsg002(std::string nickName, int clientFd);
	void nickReplyMsg003(std::string nickName, int clientFd);
	void nickReplyMsg004(std::string nickName, int clientFd);
	void nickReplyMsg005(std::string nickName, int clientFd);

	//Command PASS
	bool commandPass(std::string password);
	bool clientCanUseCommands(int clientFd);

	//Command Ping
	void commandPing(std::string token);

	//Command Join
	bool commandJoinCheck(const std::string &channel);

	//Command MTDO
	void commandMtdo();

	

	//Debug
	void debugShowChannelsInfo();
	void debugShowLastClient(void);
	void debugShowAllClients(void);
	void debugShowSpecificClient(Client client);


		//Visual Functions
		void visualLoadingServer(void);

		//utils diogo
		std::vector<Client>::iterator LookClientInChannel(std::string channel);
		
	

		
} ;
