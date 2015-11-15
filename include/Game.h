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
#include <time.h>
#include "../include/Player.h"
#include "../include/Ball.h"
#include "../include/Ground.h"

#define GROUND_WIDTH 192*2
#define GROUND_HEIGHT 108*2
#define PLAYERS_PER_TEAM 3
#define ANGLE_ROTATION 2
#define SHOOT_RATE 0.25
#define ZERO_THRESHOLD 1

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

enum effect_type {SHOOT_EFFECT, NONE_EFFECT};

struct State
{
	effect_type effectType;
	float timeSpent;
	Player Team1[PLAYERS_PER_TEAM];
	Player Team2[PLAYERS_PER_TEAM];
	Ball ball;
};

enum packet_type {CONNECT, STATE, CONTROL, QUERY};
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
	std::queue<effect_type> *effectQ[4];
	std::queue<Control> *controlQ;
	OnlinePlayers *onlinePlayers;
	char ip[16];
	int port;
	char serverIp[16];
	int serverPort;
	game_type type;
	clock_t startTime;
	float timeSpent;
	bool isGoalSequenceRunning;
	bool matchCompleted;
	bool team1Won;
	bool team2Won;
	int lastGoalScoringTeam;
	void displayGoalWord();
	void displayResult();
	void whenNotInPossessionStrategy(int teamId, int playerId);
	void whenInPossessionStrategy(int teamId, int playerId);
	void whenOpponentInPossessionStrategy(int teamId, int playerId);
	bool goalKeeperMoreTowardsFirstBar(int teamId, Player *goalkeeper);
	bool isPlayerMovingTowardsGoal(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos);
	bool isPlayerMovingTowardsOwnGoal(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos);
	bool playerInDBox(Player *player, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos);
	bool isOpponentNearby(Player *player, int teamId, int playerId);
	bool isTeamMateInABetterPositionToScore(Player *player, Player *teamMate, int teamId, int playerId, pair<pair<float,float>, pair<float,float> > goalPos);
public:
	Game(int port, game_type type);
	virtual ~Game();

	void reset(int teamInAttack);
	void selectPlayer();
	void startServer();
	game_type getType();
	int getMyPlayerTeam();
	int getMyPlayerId();
	void movePlayer(int playerTeam, int playerId, float angle);
	void moveBall();
	void applyBallDeflection(float oldX, float oldY, float newX, float newY);
	bool isBallInPossession();
	bool isMyTeamInPossession();
	int possessorPlayerTeam();
	int possessorPlayerId();
	void setBallFree(); //No possession by any player
	void updateShootAngle(float amount, float towards, int playerTeam, int playerId);
	void updateShootPower(float amount, int playerTeam, int playerId);
	void shoot(int playerTeam, int playerId);
	void pass(int playerTeam, int playerId);
	void join();
	void setServerAddr();
	void query();
	string makePlayerString(); //Indicates which players are bots and which are humans
	void parseAndDisplayPlayerString(string playerString);
	bool addPlayer(char *ip, int port, int teamNo, int playerId);
	void applyState(State *state);
	void applyControl(Control control);
	void insertControl(Control control);
	Control removeControl();
	void insertEffect(effect_type type);
	effect_type removeEffect(int teamNo, int playerId);
	void draw();
	void increaseTeam1Goals();
	int getTeam1Goals();
	void increaseTeam2Goals();
	int getTeam2Goals();
	void initiateGoalSequence(int goalState);
	void initiateEndSequence();
	float getTimeSpent();
	float computeTimeSpent();
	void playCrowdChant();
	void playShootEffect();
	void playCrowdCheer();
	void blowWhistle();
	void computeNewPositionForOutfieldBotPlayers();
	bool isMatchCompleted();
	void setLastGoalScoringTeam(int lastGoalScoringTeam);
	int getLastGoalScoringTeam();

	friend void serverRunner(Game *game);
	friend bool sendPacket(Game *game, Packet *packet, char *destIp, int destPort, string *response);
	friend void controlSender(Game *game, char *destIp, int destPort);
	friend void communicate(int newSockFd, Game *game);
};

std::string getIp();

#endif /* INCLUDE_GAME_H_ */
