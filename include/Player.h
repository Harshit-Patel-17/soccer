/*
 * Player.h
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_

#include "../include/Soccer.h"

class Player
{
	int texture;
	int totalPostures;
	int posture;
	float mobility;
	float pos_x;
	float pos_y;
	float angle;
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
public:
	Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer);
	virtual ~Player();

	void moveForward();
	void setAngle(float angle);
	void draw();
};

#endif /* INCLUDE_PLAYER_H_ */
