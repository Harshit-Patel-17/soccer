/*
 * Player.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include "../include/Player.h"
#include <math.h>

float Player::max(float x, float y)
{
	if(x >= y)
		return x;
	else
		return y;
}

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

Player::Player()
{

}

Player::Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground, Ball *ball)
{
	this->texture = texture;
	this->totalPostures = soccer->getTotalPlayerPostures(texture);
	this->posture = 0;
	this->mobility = mobility;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->angle = angle;
	this->ball = ball;
	this->soccer = soccer;
	this->ground = ground;
	this->ball = ball;
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

/*void Player::moveForward()
{
	static int stateCounter = 0;
	if(++stateCounter != 2)
		return;
	stateCounter = 0;
	posture = (posture + 2) % totalPostures;
	pos_x += mobility * cos(angle * 3.1415 / 180);
	pos_y += mobility * sin(angle * 3.1415 / 180);

	if(ball != NULL)
	{
		float dx = ball->getPosX() - pos_x;
		float dy = ball->getPosY() - pos_y;
		float dist = sqrt(dx*dx + dy*dy);
		if(dist <= HIT_THRESHOLD)
			ball->hit(3, -0.3, angle);
	}
}*/

void Player::moveForward()
{
	float minX = ground->getMinX();
	float maxX = ground->getMaxX();
	float minY = ground->getMinY();
	float maxY = ground->getMaxY();

	static int stateCounter = 0;
	if(++stateCounter != 2)
		return;
	stateCounter = 0;
	posture = (posture + 2) % totalPostures;

	if(possession)
	{
		float dx = ball->getPosX() - pos_x;
		float dy = ball->getPosY() - pos_y;
		float dist = sqrt(dx*dx + dy*dy);
		if(dist <= HIT_THRESHOLD)
			ball->hit(3, -0.3, angle);
		else
		{
			pos_x = pos_x + mobility * dx / dist;
			pos_y = pos_y + mobility * dy / dist;
		}
	}
	else
	{
		pos_x = pos_x + mobility * cos(angle * 3.1415 / 180);
		pos_y = pos_y + mobility * sin(angle * 3.1415 / 180);
	}

	if(pos_x > maxX)
		pos_x = maxX;

	if(pos_x < minX)
		pos_x = minX;

	if(pos_y > maxY)
		pos_y = maxY;

	if(pos_y < minY)
		pos_y = minY;
}

void Player::shoot()
{
	if(ball == NULL)
		return;
	ball->hit(8, -0.3, angle);
	possession = false;
}

int Player::getTexture()
{
	return texture;
}

void Player::setTexture(int texture)
{
	this->texture = texture;
}

int Player::getTotalPostures()
{
	return totalPostures;
}

void Player::setTotalPostures(int totalPostures)
{
	this->totalPostures = totalPostures;
}

int Player::getPosture()
{
	return posture;
}

void Player::setPosture(int posture)
{
	this->posture = posture;
}

float Player::getMobility()
{
	return mobility;
}

void Player::setMobility(float mobility)
{
	this->mobility = mobility;
}

float Player::getAngle()
{
	return angle;
}

void Player::setAngle(float angle)
{
	this->angle = angle;
}

float Player::getPosX()
{
	return pos_x;
}

void Player::setPosX(float pos_x)
{
	this->pos_x = pos_x;
}

float Player::getPosY()
{
	return pos_y;
}

void Player::setPosY(float pos_y)
{
	this->pos_y = pos_y;
}

void Player::possess()
{
	this->possession = true;
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

void Player::operator=(Player& player)
{
	this->texture = player.texture;
	this->totalPostures = player.totalPostures;
	this->posture = player.posture;
	this->mobility = player.mobility;
	this->pos_x = player.pos_x;
	this->pos_y = player.pos_y;
	this->angle = player.angle;
}
