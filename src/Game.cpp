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
#include <condition_variable>
#include <chrono>

std::mutex stateMutex;
std::condition_variable serverRunning;
std::mutex serverMutex;

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
	std::cout << destIp << ":" << destPort << std::endl;

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
	while(connect(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Socket connect error." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		//close(sockFd);
		//return;
	}

	do
	{
		stateMutex.lock();
		packet.state = *(game->state);
		stateMutex.unlock();

		//Send message to server
		//std::cout << "Sending state packet." << std::endl;
		count = write(sockFd, (char *)&packet, sizeof(Packet));
		if(count < 0)
		{
			std::cerr << "Socket write error." << std::endl;
			break;
		}

		//Receive response from server
		bzero(buffer, bufferSize);
		count = read(sockFd, (char *)&packet, sizeof(Packet));
		if(count < 0)
		{
			std::cerr << "Socket read error." << std::endl;
			break;
		}

		game->applyState(&packet.state);

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

	Packet packet;

	do
	{
		count = read(newSockFd, (char *)&packet, sizeof(Packet));
		if(count < 0) {
			std::cerr << "Socket reading error." << std::endl;
			break;
		}

		memset(buffer, 0, bufferSize);

		switch(packet.type)
		{
		case CONNECT:
			//std::cout << "Connect packet received." << std::endl;
			game->onlinePlayers->add(packet.ip,  packet.port, packet.teamNo, packet.playerId);
			strcpy(buffer, "Connect packet received");
			count = write(newSockFd, buffer, strlen(buffer));
			break;

		case STATE:
			//std::cout << "State packet received." << std::endl;
			game->applyState(&packet.state);
			stateMutex.lock();
			packet.state = *(game->state);
			stateMutex.unlock();
			count = write(newSockFd, (char *)&packet, sizeof(Packet));
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

	while(1) {
		//Start listening to incoming connections
		std::cout << "Listening to incoming connection..." << std::endl;
		//serverRunning.notify_all();
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

Game::Game(const char *ip, int port, game_type type, int myPlayerTeam, int myPlayerId)
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
		if(i==2) // goalkeeper
		{
			team1[i] = new Player(0, 2, 40.0f, GROUND_HEIGHT/2, 0, soccer, ground, ball, false);
			team2[i] = new Player(0, 2, 344.0f, GROUND_HEIGHT/2, 0, soccer, ground, ball, false);
		}
		else
		{
			team1[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball, false);
			team2[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball, false);
		}

		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}

	team1[0]->setPossession();
	possessor = team1[0];

	if(myPlayerTeam == 0 && myPlayerId == 0)
		this->possession = true;
	else
		this->possession = false;

	this->myPlayerTeam = myPlayerTeam;
	this->myPlayerId = myPlayerId;

	state->teamNo = myPlayerTeam;
	state->playerId = myPlayerId;

	if(myPlayerTeam == 0)
		this->myPlayer = this->team1[myPlayerId];
	else
		this->myPlayer = this->team2[myPlayerId];

	onlinePlayers = new OnlinePlayers();

	this->type = type;

	std::thread server(serverRunner, this);
	server.detach();
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
	packet.teamNo = myPlayerTeam;
	packet.playerId = myPlayerId;

	sendPacket(this, &packet, ip, port);
	onlinePlayers->add(ip, port, 0, 0);

	//std::unique_lock<std::mutex> lk(serverMutex);
	//serverRunning.wait(lk);

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
	if(type == JOINER)
	{
		//*(this->state) = *state;

		*ball = state->ball;
		this->state->ball = state->ball;

		for(int i = 0; i < PLAYERS_PER_TEAM; i++)
		{
			if(this->state->teamNo != 0 || this->state->playerId != i)
			{
				*(team1[i]) = state->Team1[i];
				this->state->Team1[i] = state->Team1[i];
			}
			if(this->state->teamNo != 1 || this->state->playerId != i)
			{
				*(team2[i]) = state->Team2[i];
				this->state->Team2[i] = state->Team2[i];
			}
		}
	}
	else
	{
		//*ball = state->ball;
		//this->state->ball = state->ball;

		if(state->teamNo == 0)
		{
			*(team1[state->playerId]) = state->Team1[state->playerId];
			this->state->Team1[state->playerId] = state->Team1[state->playerId];
		}
		else
		{
			*(team2[state->playerId]) = state->Team2[state->playerId];
			this->state->Team2[state->playerId] = state->Team2[state->playerId];
		}
	}

	stateMutex.unlock();
}

void Game::draw()
{
	glTranslatef(-ball->getPosX(), -ball->getPosY(), -150.0f);

	ground->draw();
	ball->draw();

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		team1[i]->draw();
		team1[i]->setBall(ball);
		team2[i]->draw();
		team2[i]->setBall(ball);
	}

	stateMutex.lock();
	state->ball = *ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}
	stateMutex.unlock();
}
