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
#include <math.h>
#include <float.h>
#include <iostream>

using namespace std;

std::mutex stateMutex;
std::mutex controlQMutex;
std::mutex effectQMutex;

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

void controlSender(Game *game, char *destIp, int destPort)
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
		packet.type = CONTROL;
		packet.control = game->removeControl();

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

		case CONTROL:
			//std::cout << "State packet received." << std::endl;
			game->applyControl(packet.control);
			packet.type = STATE;
			stateMutex.lock();
			packet.state = *(game->state);
			packet.state.effectType = game->removeEffect();
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
		/*if(i == 0)
			team1[i] = new Player(0, 2, GROUND_WIDTH/2 + 10, GROUND_HEIGHT/2, 0, soccer, ground, ball);
		else
			team1[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball);
		team2[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball);*/

		if(i==2) //goalkeeper
		{
			team1[i] = new Player(0, 2, 50.0f, GROUND_HEIGHT/2, 0, soccer, ground, ball);
			team2[i] = new Player(1, 2, 334.0f, GROUND_HEIGHT/2, 0, soccer, ground, ball);
		}
		else if(i==0)
		{
			team1[i] = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball); // kickOff team player 1
			team2[i] = new Player(1, 2, GROUND_WIDTH/2 + 20.0, GROUND_HEIGHT/2, 0, soccer, ground, ball);
		}
		else
		{
			team1[i] = new Player(0, 2, (GROUND_WIDTH/2) - 20.0, GROUND_HEIGHT/2 - 50.0, 0, soccer, ground, ball); // kickOff team player 2
			team2[i] = new Player(1, 2, GROUND_WIDTH/2 + 20.0, GROUND_HEIGHT/2 - 50.0, 0, soccer, ground, ball);
		}

		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}

	team1[0]->possess();

	this->myPlayerTeam = myPlayerTeam;
	this->myPlayerId = myPlayerId;

	if(myPlayerTeam == 0)
		myPlayer = team1[myPlayerId];
	else
		myPlayer = team2[myPlayerId];

	effectQ = new std::queue<effect_type>;

	controlQ = new std::queue<Control>;

	onlinePlayers = new OnlinePlayers();

	this->type = type;

	this->team1Goals = this->team2Goals = 0;

	this->startTime = clock();

	this->timeSpent = 0;

	playCrowdChant();
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

void Game::startServer()
{
	std::thread server(serverRunner, this);
	server.detach();
}

game_type Game::getType()
{
	return type;
}

int Game::getMyPlayerTeam()
{
	return myPlayerTeam;
}

int Game::getMyPlayerId()
{
	return myPlayerId;
}

void Game::movePlayer(int playerTeam, int playerId, float angle)
{
	Player *player;

	if(playerTeam == 0)
		player = team1[playerId];
	else
		player = team2[playerId];

	player->setAngle(angle);
	player->moveForward();



	float dx = player->getPosX() - ball->getPosX();
	float dy = player->getPosY() - ball->getPosY();
	float dist = sqrt(dx*dx + dy*dy);

	if(dist < HIT_THRESHOLD)
	{
		setBallFree();
		player->possess();
	}
}

void Game::moveBall()
{
	int playerTeam, playerId;

	// if goalkeeper is in possession,he should pass
	if(team1[2]->InPossession() || team2[2]->InPossession())
	{
		if(team1[2]->InPossession())
			playerTeam = 0;
		else
			playerTeam = 1;

		playerId = 2;
		//sleep(2);

		pass(playerTeam, playerId);
		return;
	}

	// if within the threshold,goalkeeper should take claim of the ball
	for(int i=0;i<2;i++)
	{
		Player *player;

		if(i==0)
			player = team1[2];
		else
			player = team2[2];

		float dx = player->getPosX() - ball->getPosX();
		float dy = player->getPosY() - ball->getPosY();
		float dist = sqrt(dx*dx + dy*dy);
		if(dist < GK_CLAIM_THRESHOLD && !ball->isBallPassed())
		{
			setBallFree();
			player->possess();
			ball->setIsPass(false);
			ball->setPosX(player->getPosX());
			ball->setPosY(player->getPosY());
			ball->setAccn(-1000.0);
			return;
		}
	}

	// if anybody comes in way of the pass, he should intercept
	for(int team=0;team<2;team++)
	{
		for(int i=0;i<PLAYERS_PER_TEAM-1;i++)
		{
			pair<int,int> ballPassedBy = ball->getBallPassedBy();
			if(ballPassedBy.first == team && ballPassedBy.second == i && ball->isBallPassed()) continue;

			Player *player;

			if(team == 0)
			{
				player = team1[i];
			}
			else
			{
				player = team2[i];
			}

			float dx = player->getPosX() - ball->getPosX();
			float dy = player->getPosY() - ball->getPosY();
			float dist = sqrt(dx*dx + dy*dy);

			if(dist < HIT_THRESHOLD && ball->isBallPassed())
			{
				setBallFree();
				player->possess();
				ball->setIsPass(false);
				ball->setPosX(player->getPosX());
				ball->setPosY(player->getPosY());
				ball->setAccn(-1000.0);
				return;
			}
		}
	}

	float u = ball->getU();
	float a = ball->getA();
	float d = ball->getD();
	float angle = ball->getAngle();

	float oldX = ball->getPosX();
	float oldY = ball->getPosY();

	if(u*u + 2*a*d < 0)
			return;

	float dx_unit = cos(angle * 3.1415 / 180);
	float dy_unit = sin(angle * 3.1415 / 180);

	float v = sqrt(u*u + 2*a*d);
	float newX = oldX + v * dx_unit;
	float newY = oldY + v * dy_unit;

	ball->updatePosition();


	if(!(ball->isBallPassed()))
		applyBallDeflection(oldX, oldY, newX, newY);
	if(fabs(dy_unit) >= Y_CHANGE_THRESHOLD
			&& newY > GK_MIN_Y && newY < GK_MAX_Y)
	{
		team2[2]->positionGoalkeeper();
		team1[2]->positionGoalkeeper();
	}
	else
	{
		team1[2]->setPosture(0);
		team2[2]->setPosture(0);
		team1[2]->setAngle(180);
		team2[2]->setAngle(180);
	}
}

void Game::applyBallDeflection(float oldX, float oldY, float newX, float newY)
{
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			float dx, dy, dist, theta, p1, p2, x, y;

			if(j == 0)
			{
				if(possessorPlayerTeam() == 0 && possessorPlayerId() == i)
					continue;
				dx = newX - state->Team1[i].getPosX();
				dy = newY - state->Team1[i].getPosY();
			}
			else
			{
				if(possessorPlayerTeam() == 1 && possessorPlayerId() == i)
					continue;
				dx = newX - state->Team2[i].getPosX();
				dy = newY - state->Team2[i].getPosY();
			}
			dist = sqrt(dx*dx + dy*dy);

			if(dist < ZERO_THRESHOLD)
				return;

			if(dist < 2*HIT_THRESHOLD)
			{
				float ballAngle = ball->getAngle() * 3.1415 / 180;
				float ballDirection = (-dx * cos(ballAngle) - dy * sin(ballAngle)) / dist;
				if(ballDirection < 0)
					continue;

				if(dx < ZERO_THRESHOLD && dx > -ZERO_THRESHOLD/2)
					theta = (dy > 0) ? 3.1415/2 : -3.1415/2;
				else
				{
					theta = atan(dy/dx);
					if(dx < 0)
						theta = 3.1415 + theta;
				}

				float cost = cos(theta);
				float sint = sin(theta);

				p1 = cost*cost - sint*sint;
				p2 = 2*cost*sint;

				x = p1*(oldX - newX) + p2*(oldY - newY) + newX;
				y = p1*(newY - oldY) + p2*(oldX - newX) + newY;

				float new_dx = x - newX;
				float new_dy = y - newY;

				if(new_dx < ZERO_THRESHOLD && new_dx > -ZERO_THRESHOLD)
					theta = (new_dy > 0) ? 3.1415/2 : -3.1415/2;
				else
				{
					theta = atan(new_dy / new_dx);
					if(new_dx < 0)
						theta = 3.1415 + theta;
				}

				ball->setAngle(theta * 180 / 3.1415);
				return;
			}
		}
	}
}

bool Game::isBallInPossession()
{
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		if(team1[i]->InPossession())
			return true;
		if(team2[i]->InPossession())
			return true;
	}
	return false;
}

int Game::possessorPlayerTeam()
{
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		if(team1[i]->InPossession())
			return 0;
		if(team2[i]->InPossession())
			return 1;
	}
	return -1;
}

int Game::possessorPlayerId()
{
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		if(team1[i]->InPossession())
			return i;
		if(team2[i]->InPossession())
			return i;
	}
	return -1;
}

void Game::setBallFree()
{
	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		//if(team1[i]->InPossession())
			team1[i]->release();
		//if(team1[i]->InPossession())
			team2[i]->release();
	}
}

void shootTimer(int ms, Player *player, Ball *ball, Game *game)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	game->insertEffect(SHOOT_EFFECT);
	game->playShootEffect();
	player->shoot(ball->getShootAngle(), ball->getShootPower());
	ball->setIsShoot(false);
}

void Game::updateShootAngle(float amount, float towards, int playerTeam, int playerId)
{
	Player *player;

	if(playerTeam == 0)
		player = team1[playerId];
	else
		player = team2[playerId];

	if(!player->InPossession())
		return;

	ball->updateShootAngle(amount, towards);
}

void Game::updateShootPower(float amount, int playerTeam, int playerId)
{
	Player *player;

	if(playerTeam == 0)
		player = team1[playerId];
	else
		player = team2[playerId];

	if(!player->InPossession())
		return;

	ball->updateShootPower(amount);
}

void Game::shoot(int playerTeam, int playerId)
{
	Player *player;

	if(playerTeam == 0)
		player = team1[playerId];
	else
		player = team2[playerId];

	if(ball->isOnShoot())
		return;

	if(!player->InPossession())
		return;

	ball->setIsPass(false);
	ball->setShootAngle(player->getAngle());
	ball->setShootPower(MIN_SHOOT_POWER);
	ball->setIsShoot(true);
	std::thread timer(shootTimer, 700, player, ball, this);
	timer.detach();
	//player->shoot();
}

void Game::pass(int playerTeam, int playerId)
{
	ball->setIsPass(true);
	ball->setBallPassedBy(make_pair(playerTeam, playerId));
	Player *player, *destPlayer;
	int destPlayerId;

	if(playerTeam == 0)
	{
		player = team1[playerId];
		if(playerId == 2)
		{
			destPlayerId = 0;
			destPlayer = team1[destPlayerId];
		}
		else
		{
			destPlayerId = 1-playerId;
			destPlayer = team1[destPlayerId]; //change logic if pass needs to be made to the goalkeeper
		}
	}
	else
	{
		player = team2[playerId];
		if(playerId == 2)
		{
			destPlayerId = 0;
			destPlayer = team2[destPlayerId];
		}
		else
		{
			destPlayerId = 1-playerId;
			destPlayer = team2[destPlayerId]; //change logic if pass needs to be made to the goalkeeper
		}
	}

	pair<float, float> dest = make_pair(destPlayer->getPosX(),destPlayer->getPosY());
	pair<float, float> src = make_pair(ball->getPosX(), ball->getPosY());

	player->pass(playerTeam, playerId, destPlayer, destPlayerId, dest, src);
}

int Game::getTeam1Goals()
{
	return team1Goals;
}

int Game::getTeam2Goals()
{
	return team2Goals;
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

	std::thread controlSenderThread(controlSender, this, ip, port);
	controlSenderThread.detach();
}


void Game::applyState(State *state)
{
	stateMutex.lock();
	//int team = possessorPlayerTeam();
	//int id = possessorPlayerId();

	switch(state->effectType)
	{
		case NONE_EFFECT:
			break;

		case SHOOT_EFFECT:
			playShootEffect();
			break;
	}

	timeSpent = state->timeSpent;
	*ball = state->ball;
	this->state->ball = state->ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		*(team1[i]) = state->Team1[i];
		*(team2[i]) = state->Team2[i];
	}

	stateMutex.unlock();
}

void Game::applyControl(Control control)
{
	switch(control.type)
	{
	case NONE:
		break;

	case MOVE:
		movePlayer(control.teamNo, control.playerId, control.angle);
		updateShootAngle(ANGLE_ROTATION, control.angle, control.teamNo, control.playerId);
		break;

	case SHOOT:
		shoot(control.teamNo, control.playerId);
		updateShootPower(SHOOT_RATE, control.teamNo, control.playerId);
		break;

	case PASS:
		pass(control.teamNo, control.playerId);
		break;
	}
}

void Game::insertControl(Control control)
{
	controlQMutex.lock();
	controlQ->push(control);
	controlQMutex.unlock();
}

Control Game::removeControl()
{
	Control control;
	control.type = NONE;
	control.teamNo = myPlayerTeam;
	control.playerId = myPlayerId;

	controlQMutex.lock();
	if(controlQ->size() != 0)
	{
		control = controlQ->front();
		controlQ->pop();
	}
	controlQMutex.unlock();

	return control;
}

void Game::insertEffect(effect_type type)
{
	effectQMutex.lock();
	effectQ->push(type);
	effectQMutex.unlock();
}

effect_type Game::removeEffect()
{
	effect_type returnVal = NONE_EFFECT;

	effectQMutex.lock();
	if(effectQ->size() != 0)
	{
		returnVal = effectQ->front();
		effectQ->pop();
	}
	effectQMutex.unlock();

	return returnVal;
}

void Game::draw()
{
	glTranslatef(-ball->getPosX(), -ball->getPosY(), -200.0f);

	ground->draw();

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		team1[i]->draw();
		team2[i]->draw();
	}


	moveBall();
	ball->draw();

	ground->drawGoals();

	stateMutex.lock();
	state->timeSpent = computeTimeSpent();
	state->ball = *ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{
		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}
	stateMutex.unlock();
}

float Game::getTimeSpent()
{
	return timeSpent;
}

float Game::computeTimeSpent()
{
	clock_t endTime = clock();
	float time_spent = (float)(endTime - startTime) / CLOCKS_PER_SEC;
	time_spent = floorf(time_spent * 100) / 100 - 0.40;
	time_spent *= 100;
	return time_spent;
}

void Game::playCrowdChant()
{
	Mix_PlayMusic(soccer->getCrowdChant(), -1);
}

void Game::playShootEffect()
{
	Mix_PlayChannel(-1, soccer->getShootEffect(), 0);
}
