/*
 * Game.h
 *
 *  Created on: 30-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_GAME_H_
#define INCLUDE_GAME_H_

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <string.h>
#include <queue>
#include "../include/Player.h"
#include "../include/Ball.h"
#include "../include/Ground.h"

#define GROUND_WIDTH 192*2
#define GROUND_HEIGHT 108*2
#define PLAYERS_PER_TEAM 3

struct PlayerDetails
{
	char ip[16];
	int port;
	int teamNo;
	int playerNo;
	PlayerDetails()
	{
		ip[0] = 0;
		port = 0;
		teamNo = -1;
		playerNo = -1;
	}
};

struct OnlinePlayers
{
	int count;
	PlayerDetails playerDetails[3];
	OnlinePlayers()
	{
		count = 0;
	}
	void add(char *ip, int port, int teamNo, int playerNo)
	{
		if(count == 3)
		{
			std::cerr << "No more players can be joined." << std::endl;
			return;
		}
		strcpy(playerDetails[count].ip, ip);
		playerDetails[count].port = port;
		playerDetails[count].teamNo = teamNo;
		playerDetails[count].playerNo = playerNo;
		count++;
	}
};

struct State
{
	Player Team1[PLAYERS_PER_TEAM];
	Player Team2[PLAYERS_PER_TEAM];
	Ball ball;
};

enum packet_type {CONNECT, STATE, CONTROL};
enum game_type {CREATOR, JOINER};
enum control_type {MOVE, SHOOT, PASS, NONE};

struct Control
{
	control_type type;
	int teamNo;
	int playerId;
	float angle;
};

struct Packet
{
	char ip[16];
	int port;
	packet_type type;
	int teamNo;
	int playerId;

	State state;
	Control control;
};

class Game
{
	Soccer *soccer;
	Ground *ground;
	Ball *ball;
	Player *team1[PLAYERS_PER_TEAM];
	Player *team2[PLAYERS_PER_TEAM];
	int team1Goals;
	int team2Goals;
	Player *myPlayer;
	int myPlayerTeam;
	int myPlayerId;
	State *state;
	std::queue<Control> *controlQ;
	OnlinePlayers *onlinePlayers;
	char ip[16];
	int port;
	game_type type;
public:
	Game(const char *ip, int port, game_type type, int myPlayerTeam, int myPlayerId);
	virtual ~Game();

	void startServer();
	game_type getType();
	int getMyPlayerTeam();
	int getMyPlayerId();
	void movePlayer(int playerTeam, int playerId, float angle);
	void moveBall();
	void applyBallDeflection(float oldX, float oldY, float newX, float newY);
	bool isBallInPossession();
	int possessorPlayerTeam();
	int possessorPlayerId();
	void setBallFree(); //No possession by any player
	void updateShootAngle(float amount, float towards);
	void updateShootPower(float amount);
	void shoot(int playerTeam, int playerId);
	void pass(int playerTeam, int playerId);
	void join(char *ip, int port);
	void applyState(State *state);
	void applyControl(Control control);
	void insertControl(Control control);
	Control removeControl();
	void draw();
	int getTeam1Goals();
	int getTeam2Goals();

	friend void serverRunner(Game *game);
	friend bool sendPacket(Game *game, Packet *packet, char *destIp, int destPort);
	friend void controlSender(Game *game, char *destIp, int destPort);
	friend void communicate(int newSockFd, Game *game);
};

#endif /* INCLUDE_GAME_H_ */
