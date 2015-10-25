/*
 * Player.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include "../include/Player.h"
#include <math.h>

void Player::applyRotation(float angle, float x, float y)
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

void Player::restoreRotation()
{
    glPopMatrix();
}

Player::Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer)
{
	this->texture = texture;
	this->totalPostures = soccer->getTotalPostures(texture);
	this->posture = 0;
	this->mobility = mobility;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->angle = angle;
	this->soccer = soccer;
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::moveForward()
{
	static int stateCounter = 0;
	if(++stateCounter != 2)
		return;
	stateCounter = 0;
	posture = (posture + 2) % totalPostures;
	pos_x += mobility * cos(angle * 3.1415 / 180);
	pos_y += mobility * sin(angle * 3.1415 / 180);
}

void Player::setAngle(float angle)
{
	this->angle = angle;
}

void Player::draw()
{
	glBindTexture(GL_TEXTURE_2D, soccer->getPlayerTex(texture)[posture]);

	int width = 88 / 5;
	int height = 98 / 5;

	applyRotation(angle - 90, pos_x, pos_y);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(pos_x - width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 0); glVertex3f(pos_x + width/2, pos_y - height/2, 0);
		glTexCoord2f(1, 1); glVertex3f(pos_x + width/2, pos_y + height/2, 0);
		glTexCoord2f(0, 1); glVertex3f(pos_x - width/2, pos_y + height/2, 0);
	glEnd();
	restoreRotation();
}

