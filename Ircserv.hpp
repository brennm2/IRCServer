/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:49 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/05 18:51:13 by bde-souz         ###   ########.fr       */
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
#include <algorithm>
#include <csignal>


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
			std::string outgoingBuffer;
			std::string buffer;
			bool		isFirstTime;
			bool		hasPass;
			bool		hasNick;
			bool		hasUser;
			bool		hasFinalReg;
			bool		_isOperator;
			bool		bufferIsReady;

		Client() :_fd(-1), _nickName(), \
		_userName(), _realName(), outgoingBuffer(), buffer(), \
		isFirstTime(true), \
		hasPass(false), hasNick(false), hasUser(false), hasFinalReg(false), \
		_isOperator(false), bufferIsReady(false) {}
};

	private:
		struct channelsStruct
		{
			std::string			_channelName;
			std::string			_channelTopic;
			std::string			_channelPassword;
			std::vector<Client> _clients;
			std::vector<int>	_clientsFdInvite;
			std::vector<int>	_clientsBanned;
			bool				_isTopicLocked;
			bool				_isPrivate;
			bool				_hasPassword;
		std::string			_topicSetter;
		time_t				_topicSetTime;
		int					_maxUsers;

			channelsStruct() :_channelName(), _channelTopic(), _channelPassword(), \
			_clients(), _clientsFdInvite(), \
			_clientsBanned(), _isTopicLocked(false), _isPrivate(true), \
			_hasPassword(false), _topicSetter(), _topicSetTime(0), _maxUsers(-1) \
			{}
		};
			
	std::vector<channelsStruct> _channels;
	std::map<std::string, std::string > _channelTopics;
	std::map<int, Client> _clientsMap;
	std::vector<pollfd> poll_fds;
	std::string		_password;
	unsigned int	_port;
	static bool		endServer;



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
	void commandUser(std::istringstream &lineStream);
	//-------------------mudkip------------------
	void commandPart(std::string &channelName); 
	void checkCommandPart(std::istringstream &lineStream);
	void commandTopic(std::string &channelName, std::string &newTopic);
	void checkCommandTopic(std::istringstream &lineStream);
	void commandPrivMSG(std::istringstream &lineStream);

	//Help Functions
	bool checkIfClientInChannel(const std::string& channel, int clientFd);
	bool checkIfClientInChannelByNick(const std::string& channel, \
		const std::string& clientNick);
	bool checkIfClientInServer(int clientFd);
	bool checkIfClientInServerByNick(std::string clientNick);
	void clientFinalRegistration(int clientFd);
	bool checkIfChannelExist(std::string channel);
	Client returnClientStruct(int clientFd);
	Client& returnClientStructToModify(int clientFd);
	Client& returnClientStructToModifyInChannel(int clientFd, std::string channel);
	channelsStruct& returnChannelStruct(const std::string &channel);
	int	returnClientFd(std::string clientNick);
	void makeUserList(std::string channel);
	void createNewChannel(const std::string& channelName);
	void addClientToChannel(const std::string& channelName, const Client& client);
	std::vector<std::string> splitString(const std::string &str, char delimiter);

	


	bool privMsgSintaxCheck(std::string firstWord, std::string target);
	void removeClientFromChannel(const std::string& channelName, int clientFd);
	void removeClientFromEveryChannel(int clientFd);



	void broadcastMessageToChannel(const std::string& message, std::string channel);
	void broadcastMessageToChannelExceptSender(const std::string& message, std::string channel, int senderFd);
	void broadcastMessage(const std::string& message, int sender_fd);
	void broadcastMessagePrivate(const std::string &message, const std::string &target);

	std::string to_string(int value);


	//CommandNick----
	void commandNick(int clientFd, const std::string &str);
	bool commandNickCheck(const std::string &nickName);
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
	void commandJoin(const std::string &channel, const std::string &key);
	bool commandJoinCheck(const std::string &channel);
	bool commandJoinCheckExistingChannel(const std::string &tempChannel, \
		const Client &client, std::vector<std::string>::iterator &keyVecIt, \
			const std::vector<std::string>::const_iterator &endVec, bool emptyKeyFlag);
	bool checkIfClientCanJoinPrivChannel(const int &clientFd, const std::string &channel);
	bool checkIfClientCanJoinBannedChannel(const int &clientFd, const std::string &channel);
	bool checkIfChannelHasPassword(const std::string &channel);
	bool checkIfChannelHasCorrectPassword(const std::string &channel, const std::string &password);
	//Command MTDO
	void commandMtdo();

	//Command Quit
	void commandQuit(std::istringstream &lineStream);

	//Command Kick
	void commandKick(std::istringstream &lineStream);
	bool checkCommandKick(const std::string &channel, const std::vector<std::string> &clientsVec);

	// Command Mode
	void checkCommandMode(std::istringstream &lineStream);
	void commandModeChannel(std::string &channelName, std::string &modes, std::string &parameters);
	void changeClientToOperator(int clientFd, std::string channel);
	bool isOperator(const int clientFd, const std::string &channel);
	bool applyChannelModes(std::string &channelName, std::string &modes, std::string &parameters);
	// Command Invite
	void commandInvite(const std::string &clients, const std::string &channels);
	void placeClientInChannelInvite(const int &clientFd, const std::string &channel);
	void brodcastInviteMessage(const Client &clientTarget, const std::string &channels);
	bool checkCommandInvite(const std::string &target, const std::string &channel);

	

	//Debug
	void debugShowChannelsInfo();
	void debugShowLastClient(void);
	void debugShowAllClients(void);
	void debugShowSpecificClient(Client client);


	//Visual Functions
	void visualLoadingServer(void);

	//utils diogo
	std::vector<Client>::iterator LookClientInChannel(std::string channel);



	static void signalHandler(int signal);
	void signalCatcher(void);
	bool eofChecker(const std::string &buffer);
	bool checkIfBufferHasEnd(const std::string &line);
	bool checkIfClientHasEndedBuffer(const int &clientFd);
	std::string returnClientBuffer(const int &clientFd);
	void clientHasSendedBuffer(const int &clientFd);
} ;

void printAsciiValues(const std::string& str);
// void signalHandler(int signal);