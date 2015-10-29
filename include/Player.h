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
	Ball *ball;
	Ground *ground;
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
	float max(float x, float y);
public:
	Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground, Ball *ball = NULL);
	virtual ~Player();

	void moveForward();
	void shoot();
	void setAngle(float angle);
	float getPosX();
	float getPosY();
	void draw();
};

#endif /* INCLUDE_PLAYER_H_ */
