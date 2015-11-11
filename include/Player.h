/*
 * Player.h
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_

#include "../include/Soccer.h"
#include "../include/Ball.h"
#include "../include/Ground.h"
#include <iostream>

using namespace std;

#define HIT_THRESHOLD 5
#define Y_CHANGE_THRESHOLD 0.005
#define GK_MIN_Y 83
#define GK_MAX_Y 133
#define GK_CLAIM_THRESHOLD 12

class Player
{
	bool possession;
	int texture;
	int totalPostures;
	int posture;
	float mobility;
	float pos_x;
	float pos_y;
	float angle;
	bool isBot;
	Ball *ball;
	Ground *ground;
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
	float max(float x, float y);
public:
	Player();
	Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground, bool isBot, Ball *ball = NULL);
	virtual ~Player();

	void moveForward();
	void positionGoalkeeper();
	void shoot(float shootAngle, float shootPower);
	void pass(int playerTeam, int playerId, Player *destPlayer, int destPlayerId, pair<float, float> dest, pair<float, float> src);
	int getTexture();
	void setTexture(int texture);
	int getTotalPostures();
	void setTotalPostures(int totalPostures);
	int getPosture();
	void setPosture(int posture);
	float getMobility();
	void setMobility(float mobility);
	float getAngle();
	void setAngle(float angle);
	float getPosX();
	void setPosX(float pos_x);
	float getPosY();
	void setPosY(float pos_x);
	void possess();
	void release();
	bool InPossession();
	void draw();
	void setIsBot(bool isBot);
	bool getIsBot();

	void operator=(Player& player);
};

#endif /* INCLUDE_PLAYER_H_ */
