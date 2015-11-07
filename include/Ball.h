/*
 * Ball.h
 *
 *  Created on: 25-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_BALL_H_
#define INCLUDE_BALL_H_

#include "../include/Soccer.h"
#include "../include/Ground.h"
#include <iostream>

using namespace std;

class Ball {
	bool onShoot;
	int position;
	int totalPositions;
	bool isPass;
	float pos_x;
	float pos_y;
	float angle;
	float u; //Initial velocity
	float a; //Acceleration
	float d; //DIstance travelled
	Ground *ground;
	Soccer *soccer;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
public:
	Ball();
	Ball(float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground);
	virtual ~Ball();

	void draw();
	void updatePosition();
	void hit(float u, float a, float angle);
	float getPosX();
	void setPosX(float pos_x);
	float getPosY();
	void setPosY(float pos_y);
	void setAccn(float a);
	void setPosition(float x, float y);
	float getAngle();
	void setAngle(float angle);
	bool isOnShoot();
	float getU();
	float getA();
	float getD();
	void setIsPass(bool isPass);
	bool isBallPassed();

	void operator=(Ball& ball);
};

#endif /* INCLUDE_BALL_H_ */
