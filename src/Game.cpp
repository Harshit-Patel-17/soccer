/*
 * Game.cpp
 *
 *  Created on: 30-Oct-2015
 *      Author: harshit
 */

#include "../include/Game.h"
#include <ifaddrs.h>
#include <string>
#include <map>
#include <errno.h>
#include <mutex>

std::mutex stateMutex;

std::string getIp() {

	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;
	std::map<std::string, std::string> ip_addrs;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			ip_addrs[ifa->ifa_name] = addressBuffer;
		}
	}

	if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

	if(ip_addrs.find("eth0") != ip_addrs.end())
		return ip_addrs["eth0"];

	if(ip_addrs.find("wlan0") != ip_addrs.end())
			return ip_addrs["wlan0"];

	return ip_addrs["lo"];

}

bool connected(int sock)
{
     char buf;
     recv(sock, &buf, 1, MSG_PEEK);
     if(errno == EAGAIN || errno == EWOULDBLOCK)
    	 return false;
     return true;
}

bool sendPacket(Game *game, Packet *packet, char *destIp, int destPort)
{
	int sockFd, portNo, count;
	struct sockaddr_in serverAddr;
	const unsigned int bufferSize = sizeof(Packet);
	char buffer[bufferSize];

	portNo = destPort;

	//Create TCP socket
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0)
	{
		std::cerr << "Socket create error." << std::endl;
		close(sockFd);
		return false;
	}

	//Initialize socket structure
	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(destIp);
	serverAddr.sin_port = htons(portNo);

	//Connect to server
	if(connect(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Socket connect error." << std::endl;
		close(sockFd);
		return false;
	}

	//Send message to server
	count = write(sockFd, (char *)packet, sizeof(Packet));
	if(count < 0)
	{
		std::cerr << "Socket write error." << std::endl;
		close(sockFd);
		return false;
	}

	//Receive response from server
	bzero(buffer, bufferSize);
	count = read(sockFd, buffer, bufferSize-1);
	if(count < 0)
	{
		std::cerr << "Socket read error." << std::endl;
		close(sockFd);
		return false;
	}

	close(sockFd);
	return true;
}

void stateSender(Game *game, char *destIp, int destPort)
{
	int sockFd, portNo, count;
	struct sockaddr_in serverAddr;
	const unsigned int bufferSize = sizeof(Packet);
	char buffer[bufferSize];

	portNo = destPort;

	Packet packet;
	strcpy(packet.ip, game->ip);
	packet.port = game->port;
	packet.type = STATE;

	//Create TCP socket
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0)
	{
		std::cerr << "Socket create error." << std::endl;
		return;
	}

	//Initialize socket structure
	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(destIp);
	serverAddr.sin_port = htons(portNo);

	//Connect to server
	if(connect(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Socket connect error." << std::endl;
		close(sockFd);
		return;
	}

	do
	{
		stateMutex.lock();
		packet.state = *(game->state);
		stateMutex.unlock();

		//Send message to server
		count = write(sockFd, (char *)&packet, sizeof(Packet));
		if(count < 0)
		{
			std::cerr << "Socket write error." << std::endl;
			break;
		}

		//Receive response from server
		bzero(buffer, bufferSize);
		count = read(sockFd, buffer, bufferSize-1);
		if(count < 0)
		{
			std::cerr << "Socket read error." << std::endl;
			break;
		}

		usleep(10000);
	}while(1);

	close(sockFd);
	return;
}

void communicate(int newSockFd, Game *game)
{
	const unsigned int bufferSize = sizeof(Packet);
	char buffer[bufferSize];
	int count;
	bool persistent = false;

	std::thread *stateSenderThread;

	Packet packet;

	do
	{
		count = read(newSockFd, (char *)&packet, sizeof(Packet));
		if(count < 0) {
			std::cerr << "Socket reading error." << std::endl;
			break;
		}

		memset(buffer, 0, bufferSize);
		strcpy(buffer, "Packet received");
		count = write(newSockFd, buffer, strlen(buffer));
		if(count < 0) {
			std::cerr << "Socket writing error." << std::endl;
			break;
		}

		switch(packet.type)
		{
		case CONNECT:
			std::cout << "Connect packet received." << std::endl;
			game->onlinePlayers->add(packet.ip,  packet.port, 1);
			stateSenderThread = new std::thread(stateSender, game, packet.ip, packet.port);
			stateSenderThread->detach();
			break;

		case STATE:
			std::cout << "State packet received." << std::endl;
			if(game->type == JOINER)
				game->applyState(&packet.state);
			persistent = true;
			break;
		}
	}while(persistent);

	close(newSockFd);
}

void serverRunner(Game *game)
{
	int sockFd, newSockFd, port;
	unsigned int clientLen;
	struct sockaddr_in serverAddr, clientAddr;

	//Create TCP socket
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0) {
		std::cerr << "Socket create error" << std::endl;
		return;
	}

	//Initialize socket structure
	bzero((char *) &serverAddr, sizeof(serverAddr));
	port = game->port;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	//Bind the host address with socket
	if(bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		std::cerr << "Socket bind error" << std::endl;
		return;
	}

	std::cout << "Listening to incoming connection..." << std::endl;

	while(1) {
		//Start listening to incoming connections
		listen(sockFd, 5);
		clientLen = sizeof(clientAddr);

		//Accept connection from client
		newSockFd = accept(sockFd, (struct sockaddr *) &clientAddr, &clientLen);
		if(newSockFd < 0) {
			std::cerr << "Socket accept error" << std::endl;
			return;
		}

		std::thread communicationThreadId(communicate, newSockFd, game);
		communicationThreadId.detach();
	}
	close(sockFd);
}

void PlayerState::buildFromPlayer(Player& player)
{
	this->texture = player.getTexture();
	this->totalPostures = player.getTotalPostures();
	this->posture = player.getPosture();
	this->mobility = player.getMobility();
	this->pos_x = player.getPosX();
	this->pos_y = player.getPosY();
	this->angle = player.getAngle();
}

void PlayerState::buildPlayer(Player& player)
{
	player.setTexture(this->texture);
	player.setTotalPostures(this->totalPostures);
	player.setPosture(this->posture);
	player.setMobility(this->mobility);
	player.setPosX(this->pos_x);
	player.setPosY(this->pos_y);
	player.setAngle(this->angle);
}

Game::Game(const char *ip, int port, game_type type)
{
	this->ip[0] = 0;
	strcpy(this->ip, ip);
	this->port = port;

	soccer = new Soccer();
	ground = new Ground(GROUND_WIDTH, GROUND_HEIGHT, 0, 0, soccer);
	ball = new Ball(GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground);

	state = new State;

	state->ball = *ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		team1[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, NULL);
		team2[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, NULL);

		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}

	team1[0]->possess(ball);
	myPlayer = team1[0];
	possession = true;
	possessor = myPlayer;

	onlinePlayers = new OnlinePlayers();

	this->type = type;

	server = new std::thread(serverRunner, this);
	server->detach();
}

Game::~Game() {
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		delete team1[i];
		delete team2[i];
	}

	delete state;
	delete ball;
	delete ground;
	delete soccer;
}

void Game::movePlayer(float angle)
{
	myPlayer->setAngle(angle);
	myPlayer->moveForward();
}

void Game::shoot()
{
	if(possession && myPlayer == possessor)
		myPlayer->shoot();
}

void Game::join(char *ip, int port)
{
	Packet packet;
	strcpy(packet.ip, this->ip);
	packet.port = this->port;
	packet.type = CONNECT;

	sendPacket(this, &packet, ip, port);
	onlinePlayers->add(ip, port, 1);

	std::thread stateSenderThread(stateSender, this, ip, port);
	stateSenderThread.detach();
}

void Game::sendState()
{
	/*Packet packet;
	packet.type = STATE;
	for(int i = 0; i < onlinePlayers->count; i++)
	{
		strcpy(packet.ip, this->ip);
		packet.port = this->port;
		//std::cout << onlinePlayers->playerDetails[i].ip << ":" << onlinePlayers->playerDetails[i].port << std::endl;
		packet.state = *state;
		sendPacket(this, &packet, onlinePlayers->playerDetails[i].ip, onlinePlayers->playerDetails[i].port, &clientSocket[i]);
	}*/
}

void Game::applyState(State *state)
{
	stateMutex.lock();
	*(this->state) = *state;

	*ball = state->ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		*(team1[i]) = state->Team1[i];
		*(team2[i]) = state->Team2[i];
	}
	stateMutex.unlock();
}

void Game::draw()
{
	glTranslatef(-ball->getPosX(), -ball->getPosY(), -150.0f);

	ground->draw();

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		team1[i]->draw();
		team2[i]->draw();
	}

	ball->draw();

	stateMutex.lock();
	state->ball = *ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}
	stateMutex.unlock();
}
