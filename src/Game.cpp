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
std::mutex effectQMutex[3];

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
			game->addPlayer(packet.ip, packet.port, packet.teamNo, packet.playerId);
			//game->onlinePlayers->add(packet.ip,  packet.port, packet.teamNo, packet.playerId);
			strcpy(buffer, "Connect packet received");
			count = write(newSockFd, buffer, strlen(buffer));
			break;

		case CONTROL:
			//std::cout << "State packet received." << std::endl;
			game->applyControl(packet.control);
			packet.type = STATE;
			stateMutex.lock();
			packet.state = *(game->state);
			packet.state.effectType = game->removeEffect(packet.control.teamNo, packet.control.playerId);
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
			team1[i] = new Player(0, 2, 50.0f, GROUND_HEIGHT/2, 0, soccer, ground, true, ball);
			team2[i] = new Player(1, 2, 334.0f, GROUND_HEIGHT/2, 0, soccer, ground, true, ball);
		}
		else if(i==0)
		{
			team1[i] = new Player(0, 2, GROUND_WIDTH/2 - 10, GROUND_HEIGHT/2, 0, soccer, ground, true, ball); // kickOff team player 1
			team2[i] = new Player(1, 2, GROUND_WIDTH/2 + 20.0, GROUND_HEIGHT/2, 0, soccer, ground, true, ball);
		}
		else
		{
			team1[i] = new Player(0, 2, (GROUND_WIDTH/2) - 20.0, GROUND_HEIGHT/2 - 50.0, 0, soccer, ground, true, ball); // kickOff team player 2
			team2[i] = new Player(1, 2, GROUND_WIDTH/2 + 20.0, GROUND_HEIGHT/2 - 50.0, 0, soccer, ground, true, ball);
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

	for(int i = 0; i < 3; i++)
		effectQ[i] = new std::queue<effect_type>;
	myPlayer->setIsBot(false);

	controlQ = new std::queue<Control>;

	onlinePlayers = new OnlinePlayers();

	this->type = type;

	this->team1Goals = this->team2Goals = 0;

	this->startTime = clock();

	this->timeSpent = 0;

	playCrowdChant();

	isGoalSequenceRunning = false;

	blowWhistle();

	matchCompleted = false;
	team1Won = false;
	team2Won = false;
}

void Game::reset()
{
	ball->setPosX(GROUND_WIDTH/2);
	ball->setPosY(GROUND_HEIGHT/2);
	ball->setU(0);

	state->ball = *ball;

	for(int i = 0; i < PLAYERS_PER_TEAM; i++)
	{

		if(i==2) //goalkeeper
		{
			team1[i]->setPosX(50.0f);
			team1[i]->setPosY(GROUND_HEIGHT/2);
			team1[i]->setPosture(0);
			team2[i]->setPosX(334.0f);
			team2[i]->setPosY(GROUND_HEIGHT/2);
			team2[i]->setPosture(0);
		}
		else if(i==0)
		{
			team1[i]->setPosX(GROUND_WIDTH/2 - 10);
			team1[i]->setPosY(GROUND_HEIGHT/2);
			team1[i]->setPosture(0);
			team2[i]->setPosX(GROUND_WIDTH/2 + 20.0);
			team2[i]->setPosY(GROUND_HEIGHT/2);
			team2[i]->setPosture(0);
		}
		else
		{
			team1[i]->setPosX(GROUND_WIDTH/2 - 20.0);
			team1[i]->setPosY(GROUND_HEIGHT/2 - 50.0);
			team1[i]->setPosture(0);
			team2[i]->setPosX(GROUND_WIDTH/2 + 20.0);
			team2[i]->setPosY(GROUND_HEIGHT/2 - 50.0);
			team2[i]->setPosture(0);
		}

		state->Team1[i] = *(team1[i]);
		state->Team2[i] = *(team2[i]);
	}

	team1[0]->possess();
	isGoalSequenceRunning = false;
	blowWhistle();
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
	//if(playerId == 1 && playerTeam == 1) cout<<"here1";
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

bool Game::isMyTeamInPossession()
{
	Player *player;
	for(int i=0;i<PLAYERS_PER_TEAM;i++)
	{
		if(myPlayerTeam == 0) player = team1[i];
		else player = team2[i];
		if(player->InPossession()) return true;
	}

	return false;
}

bool Game::goalKeeperMoreTowardsFirstBar(int teamId, Player *goalkeeper)
{
	pair<pair<float,float>, pair<float, float> > goalPos;
	float dx, dy, distToFirstPost, distToSecondPost;
	if(teamId == 0)
	{
		goalPos = ground->getTeam1GoalPos();
	}
	else
	{
		goalPos = ground->getTeam2GoalPos();
	}
	dx = goalkeeper->getPosX() - (goalPos.first).first;
	dy = goalkeeper->getPosY() - (goalPos.first).second;
	distToFirstPost = sqrt(dx*dx + dy*dy);

	dx = goalkeeper->getPosX() - (goalPos.second).first;
	dy = goalkeeper->getPosY() - (goalPos.second).second;
	distToSecondPost = sqrt(dx*dx + dy*dy);

	if(distToFirstPost > distToSecondPost) return true;
	else return false;
}

bool Game::isPlayerMovingTowardsGoal(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos)
{
	float dX = (goalPos.first).first - player->getPosX();
	float dY = (goalPos.first).second - player->getPosY();
	float angle1 = atan(dY/dX);
	if(dX < 0.0)
		angle1 += 3.1415;
	angle1 = (angle1*180.0)/3.1415;

	dX = (goalPos.second).first - player->getPosX();
	dY = (goalPos.second).second - player->getPosY();
	float angle2 = atan(dY/dX);
	if(dX < 0.0)
		angle2 += 3.1415;
	angle2 = (angle2*180.0)/3.1415;


	//cout<<player->getAngle()<<" "<<angle1<<" "<<angle2<<endl;

	if(angle1 < player->getAngle() && player->getAngle() < angle2)
		return true;
	else if(teamId == 0 && -10.0 < player->getAngle() && player->getAngle() < 10.0)
		return true;
	else if(teamId == 1 && 170.0 < player->getAngle() && player->getAngle() < 190.0)
		return true;
	else
		return false;

}

bool Game::isPlayerMovingTowardsOwnGoal(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos)
{
	float dX = (goalPos.first).first - player->getPosX();
	float dY = (goalPos.first).second - player->getPosY();
	float angle1 = atan(dY/dX);
	if(dX < 0.0)
		angle1 += 3.1415;
	angle1 = (angle1*180.0)/3.1415;

	dX = (goalPos.second).first - player->getPosX();
	dY = (goalPos.second).second - player->getPosY();
	float angle2 = atan(dY/dX);
	if(dX < 0.0)
		angle2 += 3.1415;
	angle2 = (angle2*180.0)/3.1415;


	//cout<<player->getAngle()<<" "<<angle1<<" "<<angle2<<endl;

	if(angle2 < player->getAngle() && player->getAngle() < angle1)
		return true;
	else if(teamId == 1 && -10.0 < player->getAngle() && player->getAngle() < 10.0)
			return true;
	else if(teamId == 0 && 170.0 < player->getAngle() && player->getAngle() < 190.0)
			return true;
	else
		return false;
}

bool Game::playerInDBox(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos)
{
	bool isPlayerInDBox;
	pair<pair<float, float>, pair<float,float> > DBox;
	if(teamId == 0)
	{
		DBox = ground->getTeam2DBox();
	}
	else
	{
		DBox = ground->getTeam1DBox();
	}
	//cout<<(DBox.first).first<<" "<<(DBox.first).second<<" "<<(DBox.second).first<<" "<<(DBox.second).second<<endl;
	//cout<<teamId<<" "<<playerId<<endl;
	//cout<<player->getPosX()<<" "<<player->getPosY()<<endl;
	if(teamId == 0 && player->getPosX() > (DBox.first).first && (DBox.first).second < player->getPosY() && player->getPosY() < (DBox.second).second)
	{
		//cout<<"here";
		isPlayerInDBox = true;
	}
	if(teamId == 1 && player->getPosX() < (DBox.first).first && (DBox.first).second < player->getPosY() && player->getPosY() < (DBox.second).second)
	{
		//cout<<"here";
		isPlayerInDBox = true;
	}
	if(isPlayerInDBox)
		return true;
	else
		return false;
}

bool Game::isOpponentNearby(Player *player, int teamId, int playerId)
{
	Player *opponentPlayer;
	float minDist = 1000.0;
	for(int i=0;i<PLAYERS_PER_TEAM-1;i++)
	{
		if(teamId == 0)
		{
			opponentPlayer = team2[i];
		}
		else
		{
			opponentPlayer = team1[i];
		}
		float dX = player->getPosX() - opponentPlayer->getPosX();
		float dY = player->getPosY() - opponentPlayer->getPosY();
		float dist = sqrt(dX*dX + dY*dY);

		minDist = min(minDist, dist);
	}

	if(minDist < 30.0) return true;
	else return false;
}

bool Game::isTeamMateInABetterPositionToScore(Player *player, Player *teamMate, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos)
{
	Player *opponentGoalkeeper;
	float angle;
	if(teamId == 0)
	{
		opponentGoalkeeper = team2[2];
	}
	else
	{
		opponentGoalkeeper = team1[2];
	}
	float dX = opponentGoalkeeper->getPosX() - ball->getPosX();
	float dY = opponentGoalkeeper->getPosY() - ball->getPosY();

	angle = atan(dY/dX);
	if(dX < 0.0)
		angle += 3.1415;
	angle = (angle*180.0)/3.1415;

	if(fabs(angle - player->getAngle()) < 10.0)
	{
		return true;
	}

	return false;
}

void Game::whenInPossessionStrategy(int teamId, int playerId)
{
	Player *player, *teamMate, *opponentGoalkeeper;
	pair<pair<float,float>, pair<float, float> > goalPos;
	float angle, goalPosX, goalPosY, playerPosX, playerPosY;

	Control control;
	control.playerId = playerId;
	control.teamNo = teamId;

	if(teamId == 0)
	{
		player = team1[playerId];
		teamMate = team1[1-playerId];
		goalPos = ground->getTeam2GoalPos();
		opponentGoalkeeper = team2[2];
	}
	else
	{
		player = team2[playerId];
		teamMate = team2[1-playerId];
		goalPos = ground->getTeam1GoalPos();
		opponentGoalkeeper = team1[2];
	}

	if(playerInDBox(player, teamId, playerId, goalPos))
	{
		if(isTeamMateInABetterPositionToScore(player, teamMate, teamId, playerId, goalPos))
		{
			pass(teamId, playerId);
		}
		else
		{
			float add;
			if(goalKeeperMoreTowardsFirstBar(1-teamId, opponentGoalkeeper))
			{
				goalPosX = (goalPos.first).first;
				goalPosY = (goalPos.first).second;
				if(teamId == 0)
					add = 10.0;
				else
					add = -30.0;
			}
			else
			{
				goalPosX = (goalPos.second).first;
				goalPosY = (goalPos.second).second;
				if(teamId == 0)
					add = -10.0;
				else
					add = 30.0;
			}
			playerPosX = player->getPosX();
			playerPosY = player->getPosY();

			angle = atan((playerPosY - goalPosY)/(playerPosX - goalPosX));
			if((goalPosX - playerPosX) < 0.0)
				angle += 3.1415;
			angle = (angle*180.0)/3.1415;
			angle += add;
			player->setAngle(angle);

			if(teamId == 0)
			{
				shoot(teamId, playerId);
				updateShootPower(SHOOT_RATE, teamId, playerId);
			}
			else
			{
				control.type = MOVE;
				control.angle = angle;
				insertControl(control);
				control.type = SHOOT;
				control.angle = angle;
				insertControl(control);

				updateShootPower(SHOOT_RATE, teamId, playerId);
			}
		}
	}
	else if(isOpponentNearby(player, teamId, playerId))
	{
		if(teamId == 0)
		{
			pass(teamId, playerId);
		}
		else
		{
			control.type = PASS;
			control.angle = angle;
			insertControl(control);

		}
	}
	else
	{
		if(goalKeeperMoreTowardsFirstBar(playerId, opponentGoalkeeper))
		{
			goalPosX = (goalPos.first).first;
			goalPosY = (goalPos.first).second;
		}
		else
		{
			goalPosX = (goalPos.second).first;
			goalPosY = (goalPos.second).second;
		}
		playerPosX = player->getPosX();
		playerPosY = player->getPosY();

		angle = atan((playerPosY - goalPosY)/(playerPosX - goalPosX));
		if((goalPosX - playerPosX) < 0.0)
			angle += 3.1415;

		if(teamId == 0)
		{
			movePlayer(teamId, playerId, (angle*180.0)/3.1415);
			movePlayer(teamId, playerId, (angle*180.0)/3.1415);
		}
		else
		{
			control.angle = (angle*180.0)/3.1415;
			insertControl(control);
			insertControl(control);
		}
	}
}

void Game::whenNotInPossessionStrategy(int teamId, int playerId)
{
	Player *player, *teamMate, *opponentGoalkeeper, *myGoalkeeper;
	pair<pair<float,float>, pair<float, float> > opponentGoalPos, myGoalPos;
	float goalPosX, goalPosY, distToMyTeamMate, dX, dY;
	float angle;

	Control control;
	control.playerId = playerId;
	control.teamNo = teamId;


	if(teamId == 0)
	{
		player = team1[playerId];
		teamMate = team1[1-playerId];
		opponentGoalPos = ground->getTeam2GoalPos();
		myGoalPos = ground->getTeam1GoalPos();
		opponentGoalkeeper = team2[2];
		myGoalkeeper = team1[2];
	}
	else
	{
		player = team2[playerId];
		teamMate = team2[1-playerId];
		opponentGoalPos = ground->getTeam1GoalPos();
		myGoalPos = ground->getTeam2GoalPos();
		opponentGoalkeeper = team1[2];
		myGoalkeeper = team2[2];
	}


	float playerPosX = player->getPosX();
	float playerPosY = player->getPosY();
	float teamMatePosX = teamMate->getPosX();
	float teamMatePosY = teamMate->getPosY();

	dX = teamMatePosX - playerPosX;
	dY = teamMatePosY - playerPosY;
	distToMyTeamMate = sqrt(dX*dX + dY*dY);

	if((distToMyTeamMate > 80.0 || distToMyTeamMate < 60.0) && (!isPlayerMovingTowardsGoal(teamMate, teamId, playerId, opponentGoalPos))
			&& (!isPlayerMovingTowardsOwnGoal(teamMate, teamId, playerId, myGoalPos)))
	{
		if(playerInDBox(teamMate, teamId, 1-playerId, opponentGoalPos))
		{
			if(distToMyTeamMate < 40.0)
			{
				if(goalKeeperMoreTowardsFirstBar(1-teamId, opponentGoalkeeper))
				{
					goalPosX = (opponentGoalPos.first).first;
					goalPosY = (opponentGoalPos.first).second;
				}
				else
				{
					goalPosX = (opponentGoalPos.second).first;
					goalPosY = (opponentGoalPos.second).second;
				}

				angle = atan((playerPosY - goalPosY)/(playerPosX - goalPosX));
				if((goalPosX - playerPosX) < 0.0)
					angle += 3.1415;

				if(teamId == 1)
				{
					control.angle = (angle*180.0)/3.1415;
					insertControl(control);
				}
				else
				{
					movePlayer(teamId, playerId, (angle*180.0)/3.1415);
				}
			}
			else if(distToMyTeamMate > 60.0)
			{
				angle = atan(dY/dX);
				if(dX < 0.0)
					angle += 3.1415;

				control.angle = (angle*180.0)/3.1415;

				if(teamId == 1)
				{
					insertControl(control);
					insertControl(control);
				}
				else
				{
					movePlayer(teamId, playerId, (angle*180.0)/3.1415);
					movePlayer(teamId, playerId, (angle*180.0)/3.1415);
				}
				return;
			}
			else
			{
				return;
			}

		}
		if(distToMyTeamMate < 30.0)
		{
			return;
		}
		else if(distToMyTeamMate < 80.0)
		{
			control.angle = teamMate->getAngle();
			if(teamId == 1)
			{
				insertControl(control);
				insertControl(control);
			}
			else
			{
				movePlayer(teamId, playerId, teamMate->getAngle());
				movePlayer(teamId, playerId, teamMate->getAngle());
			}
			return;
		}
		else
		{
			angle = atan(dY/dX);
			if(dX < 0.0)
				angle += 3.1415;
			control.angle = (angle*180.0)/3.1415;

			if(teamId == 1)
			{
				insertControl(control);
				insertControl(control);
			}
			else
			{
				movePlayer(teamId, playerId, (angle*180.0)/3.1415);
				movePlayer(teamId, playerId, (angle*180.0)/3.1415);
			}
			return;
		}
	}

	if((dX < 5.0 || dX > -5.0) && (!isPlayerMovingTowardsGoal(teamMate, teamId, playerId, opponentGoalPos))
			&& (!isPlayerMovingTowardsOwnGoal(teamMate, teamId, playerId, myGoalPos)))
	{
		return;
	}

	if(isPlayerMovingTowardsGoal(teamMate, teamId, playerId, opponentGoalPos))
	{
		if((teamId == 0 && dX < -10.0)||(teamId == 1 && dX > 10.0))
		{
			return;
		}
	}
	else
	{
		if((teamId == 0 && dX > 10.0)||(teamId == 1 && dX < -10.0))
		{
			return;
		}
	}

	if(isPlayerMovingTowardsGoal(teamMate, teamId, playerId, opponentGoalPos))
	{
		if(goalKeeperMoreTowardsFirstBar(1-teamId, opponentGoalkeeper))
		{
			goalPosX = (opponentGoalPos.first).first;
			goalPosY = (opponentGoalPos.first).second;
		}
		else
		{
			goalPosX = (opponentGoalPos.second).first;
			goalPosY = (opponentGoalPos.second).second;
		}
	}
	else
	{
		if(goalKeeperMoreTowardsFirstBar(1-teamId, myGoalkeeper))
		{
			goalPosX = (myGoalPos.first).first;
			goalPosY = (myGoalPos.first).second;
		}
		else
		{
			goalPosX = (myGoalPos.second).first;
			goalPosY = (myGoalPos.second).second;
		}
	}

	angle = atan((playerPosY - goalPosY)/(playerPosX - goalPosX));
	if((goalPosX - playerPosX) < 0.0)
		angle += 3.1415;

	control.angle = (angle*180.0)/3.1415;

	if(teamId == 1)
	{
		insertControl(control);
		insertControl(control);
	}
	else
	{
		movePlayer(teamId, playerId, (angle*180.0)/3.1415);
		movePlayer(teamId, playerId, (angle*180.0)/3.1415);
	}
}

void Game::computeNewPositionForOutfieldBotPlayers()
{
	Player *player, *teamMate;
	for(int team=0;team<2;team++)
	{
		for(int playerId=0;playerId<PLAYERS_PER_TEAM-1;playerId++)
		{
			if(team == 0)
			{
				player = team1[playerId];
				teamMate = team1[1-playerId];
			}
			else
			{
				player = team2[playerId];
				teamMate = team2[1-playerId];
			}
			if(!(player->getIsBot())) continue;

			if(isMyTeamInPossession() && team == myPlayerTeam)
			{
				if(teamMate->InPossession())
				{
					whenNotInPossessionStrategy(team, playerId);
				}
				else
				{
					whenInPossessionStrategy(team, playerId);
				}
			}
			else
			{
				//whenOpponentInPossessionStrategy(team, playerId);
			}

		}
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

	computeNewPositionForOutfieldBotPlayers();

	if(u*u + 2*a*d < 0)
			return;

	float dx_unit = cos(angle * 3.1415 / 180);
	float dy_unit = sin(angle * 3.1415 / 180);

	float v = sqrt(u*u + 2*a*d);
	float newX = oldX + v * dx_unit;
	float newY = oldY + v * dy_unit;

	ball->updatePosition();

	initiateGoalSequence(ball->isInGoal());

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

	if(!player->InPossession())
		return;

	if(ball->isOnShoot())
		return;

	ball->setIsShoot(true);
	ball->setIsPass(false);
	ball->setShootAngle(player->getAngle());
	ball->setShootPower(MIN_SHOOT_POWER);
	std::thread timer(shootTimer, 700, player, ball, this);
	timer.detach();
	//player->shoot();
}

void Game::pass(int playerTeam, int playerId)
{
	ball->setIsPass(true);
	ball->setBallPassedBy(make_pair(playerTeam, playerId));
	Player *player, *destPlayer, *tempPlayer;
	int destPlayerId;

	if(playerTeam == 0)
	{
		player = team1[playerId];
		if(playerId == 2)
		{
			tempPlayer = team1[0];
			float dX = player->getPosX() - tempPlayer->getPosX();
			float dY = player->getPosY() - tempPlayer->getPosY();
			float distToPlayer0 = sqrt(dX*dX + dY*dY);

			tempPlayer = team1[1];
			dX = player->getPosX() - tempPlayer->getPosX();
			dY = player->getPosY() - tempPlayer->getPosY();
			float distToPlayer1 = sqrt(dX*dX + dY*dY);

			if(distToPlayer0 < distToPlayer1)
			{
				destPlayerId = 0;
				destPlayer = team1[destPlayerId];
			}
			else
			{
				destPlayerId = 1;
				destPlayer = team1[destPlayerId];
			}
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
			tempPlayer = team2[0];
			float dX = player->getPosX() - tempPlayer->getPosX();
			float dY = player->getPosY() - tempPlayer->getPosY();
			float distToPlayer0 = sqrt(dX*dX + dY*dY);

			tempPlayer = team2[1];
			dX = player->getPosX() - tempPlayer->getPosX();
			dY = player->getPosY() - tempPlayer->getPosY();
			float distToPlayer1 = sqrt(dX*dX + dY*dY);

			if(distToPlayer0 < distToPlayer1)
			{
				destPlayerId = 0;
				destPlayer = team2[destPlayerId];
			}
			else
			{
				destPlayerId = 1;
				destPlayer = team2[destPlayerId];
			}
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

void Game::increaseTeam1Goals()
{
	team1Goals++;
}

int Game::getTeam1Goals()
{
	return team1Goals;
}

void Game::increaseTeam2Goals()
{
	team2Goals++;
}

int Game::getTeam2Goals()
{
	return team2Goals;
}

void goalSequenceTimer(Game *game)
{
	game->playCrowdCheer();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	game->reset();
}

void Game::initiateGoalSequence(int goalState)
{
	if(goalState == -1)
		return;

	if(isGoalSequenceRunning)
		return;

	isGoalSequenceRunning = true;

	switch(goalState)
	{
		case 0:
			increaseTeam2Goals();
			break;

		case 1:
			increaseTeam1Goals();
			break;
	}

	std::thread timer(goalSequenceTimer, this);
	timer.detach();
}

void Game::initiateEndSequence()
{
	if(matchCompleted)
		return;

	matchCompleted = true;

	if(team1Goals > team2Goals)
		team1Won = true;
	else if(team2Goals > team1Goals)
		team2Won = true;
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

void Game::addPlayer(char *ip, int port, int teamNo, int playerId)
{
	Player *player;

	if(teamNo == 0)
		player = team1[playerId];
	else
		player = team2[playerId];

	onlinePlayers->add(ip, port, teamNo, playerId);
	player->setIsBot(false);
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
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			if(i == 0 && j == 0)
				continue;
			if(i == 0)
			{
				if(team1[j]->getIsBot())
					continue;
			}
			else
			{
				if(team2[j]->getIsBot())
					continue;
			}
			int index = i*2 + j - 1;
			effectQMutex[index].lock();
			effectQ[index]->push(type);
			effectQMutex[index].unlock();
		}
	}
}

effect_type Game::removeEffect(int teamNo, int playerId)
{
	effect_type returnVal = NONE_EFFECT;

	int index = teamNo*2 + playerId - 1;

	effectQMutex[index].lock();
	if(effectQ[index]->size() != 0)
	{
		returnVal = effectQ[index]->front();
		effectQ[index]->pop();
	}
	effectQMutex[index].unlock();

	return returnVal;
}

void Game::draw()
{
	if(matchCompleted)
	{
		glColor3f(0.3f, 0.3f, 0.3f);
		glTranslatef(-GROUND_WIDTH/2, -GROUND_HEIGHT/2, -200.0f);
	}
	else
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

	if(isGoalSequenceRunning)
		displayGoalWord();

	if(matchCompleted)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		displayResult();
	}

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

void Game::displayGoalWord()
{
	glBindTexture(GL_TEXTURE_2D, soccer->getGoalWordTex());

	int width = 777 / 8;
	int height = 389 / 8;
	int pos_x = ball->getPosX();
	int pos_y = ball->getPosY();
	int dist = 50;

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(pos_x - width/2, pos_y + dist - height/2, 0);
		glTexCoord2f(1, 0); glVertex3f(pos_x + width/2, pos_y + dist - height/2, 0);
		glTexCoord2f(1, 1); glVertex3f(pos_x + width/2, pos_y + dist + height/2, 0);
		glTexCoord2f(0, 1); glVertex3f(pos_x - width/2, pos_y + dist + height/2, 0);
	glEnd();
}

void Game::displayResult()
{
	int width, height, pos_x, pos_y;

	pos_x = GROUND_WIDTH/2;
	pos_y = GROUND_HEIGHT/2;

	if(team1Won)
	{
		width = 469 / 8;
		height = 118 / 8;
		glBindTexture(GL_TEXTURE_2D, soccer->getTeamWonTex()[0]);
	}
	else if(team2Won)
	{
		width = 469 / 8;
		height = 118 / 8;
		glBindTexture(GL_TEXTURE_2D, soccer->getTeamWonTex()[1]);
	}
	else
	{
		width = 482 / 8;
		height = 118 / 8;
		glBindTexture(GL_TEXTURE_2D, soccer->getDrawMatchTex());
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(pos_x - width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 0); glVertex3f(pos_x + width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 1); glVertex3f(pos_x + width/2, pos_y + height/2, 0);
		glTexCoord2f(0, 1); glVertex3f(pos_x - width/2, pos_y + height/2, 0);
	glEnd();
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

void Game::playCrowdCheer()
{
	Mix_PlayChannel(-1, soccer->getCrowdCheer(), 0);
}

void Game::blowWhistle()
{
	Mix_PlayChannel(-1, soccer->getWhistle(), 0);
}

bool Game::isMatchCompleted()
{
	return matchCompleted;
}
