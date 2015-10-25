/*
 * Ball.h
 *
 *  Created on: 25-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_BALL_H_
#define INCLUDE_BALL_H_

#include "../include/Soccer.h"

class Ball {
	int position;
	int totalPositions;
	float pos_x;
	float pos_y;
	float angle;
	float u; //Initial velocity
	float a; //Acceleration
	float d; //DIstance travelled
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
public:
	Ball(float pos_x, float pos_y, float angle, Soccer *soccer);
	virtual ~Ball();

	void draw();
	void updatePosition();
	void hit(float u, float a, float angle);
	float getPosX();
	float getPosY();
	float setPosition(float x, float y);
};

#endif /* INCLUDE_BALL_H_ */
