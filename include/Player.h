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

class Player
{
	int texture;
	int totalPostures;
	int posture;
	float mobility;
	float pos_x;
	float pos_y;
	float angle;
	bool inPossession;
	Ball *ball;
	Ground *ground;
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
	float max(float x, float y);
public:
	Player();
	Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground, Ball *ball, bool isPossession);
	virtual ~Player();

	void moveForward();
	void shoot();
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
	void possess(Ball *ball);
	void setPossession();
	void setBall(Ball *ball);
	void draw();

	void operator=(Player& player);
};

#endif /* INCLUDE_PLAYER_H_ */
