/*
 * Ball.cpp
 *
 *  Created on: 25-Oct-2015
 *      Author: harshit
 */

#include "../include/Ball.h"
#include <math.h>

void Ball::applyRotation(float angle, float x, float y)
{
    angle = angle * 3.1415 / 180;

    float T1[] = {1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                x, y, 0, 1};

    float T2[] = {1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                -x, -y, 0, 1};

    float R[] = {cos(angle), sin(angle), 0, 0,
                 -sin(angle), cos(angle), 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1};

    glPushMatrix();
    glMultMatrixf(T1);
    glMultMatrixf(R);
    glMultMatrixf(T2);
}

void Ball::restoreRotation()
{
    glPopMatrix();
}

Ball::Ball(float pos_x, float pos_y, float angle, Soccer *soccer)
{
	this->position = 0;
	this->totalPositions = soccer->getTotalBallPositions();
	this->soccer = soccer;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->angle = angle;
	this->u = 0;
	this->a = 0;
	this->d = 0;
}

Ball::~Ball() {
	// TODO Auto-generated destructor stub
}

void Ball::draw()
{
	glBindTexture(GL_TEXTURE_2D, soccer->getBallTex()[position]);

	int width = 62 / 8;
	int height = 62 / 8;

	applyRotation(angle + 180, pos_x, pos_y);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(pos_x - width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 0); glVertex3f(pos_x + width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 1); glVertex3f(pos_x + width/2, pos_y + height/2, 0);
		glTexCoord2f(0, 1); glVertex3f(pos_x - width/2, pos_y + height/2, 0);
	glEnd();
	restoreRotation();
}

void Ball::updatePosition()
{
	float dx_unit = cos(angle * 3.1415 / 180);
	float dy_unit = sin(angle * 3.1415 / 180);

	if(u*u + 2*a*d < 0)
		return;

	float v = sqrt(u*u + 2*a*d);
	pos_x += v * dx_unit;
	pos_y += v * dy_unit;
	d += v;
	position = (position + (int)v) % totalPositions;
}

void Ball::hit(float u, float a, float angle)
{
	this->u = u;
	this->a = a;
	this->d = 0;
	this->angle = angle;
}

float Ball::getPosX()
{
	return pos_x;
}

float Ball::getPosY()
{
	return pos_y;
}

float Ball::setPosition(float x, float y)
{
	this->pos_x = x;
	this->pos_y = y;
}
