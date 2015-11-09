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

#define MIN_SHOOT_POWER 5
#define MAX_SHOOT_POWER 10

using namespace std;

class Ball {
	bool isShoot;
	float shootAngle;
	float shootPower;
	int position;
	int totalPositions;
	bool isPass;
	pair<int,int> ballPassedBy;
	float pos_x;
	float pos_y;
	float angle;
	float u; //Initial velocity
	float a; //Acceleration
	float d; //DIstance travelled
	Ground *ground;
	Soccer *soccer;

	float min(float x, float y);
	float max(float x, float y);
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
	void setIsShoot(bool isShoot);
	void setShootAngle(bool shootAngle);
	float getShootAngle();
	void updateShootAngle(float amount, float towards);
	void setShootPower(float shootPower);
	float getShootPower();
	void updateShootPower(float amount);
	void setIsPass(bool isPass);
	bool isBallPassed();
	void setBallPassedBy(pair<int,int>);
	pair<int,int> getBallPassedBy();

	void operator=(Ball& ball);
};

#endif /* INCLUDE_BALL_H_ */
