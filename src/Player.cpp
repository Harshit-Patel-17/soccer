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

Player::Player(int texture, float mobility, float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground, bool isBot, Ball *ball)
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
	this->possession = false;
	this->isBot = isBot;
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
	//if(playerId == 1 && playerTeam == 1)
	//cout<<"here2";
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
		cout<<pos_x<<" "<<pos_y<<endl;
		pos_x = pos_x + mobility * cos(angle * 3.1415 / 180);
		pos_y = pos_y + mobility * sin(angle * 3.1415 / 180);
		cout<<angle<<" "<<mobility * cos(angle * 3.1415 / 180)<<" "<<mobility * sin(angle * 3.1415 / 180)<<endl;
		cout<<mobility<<endl;
		cout<<pos_x<<" "<<pos_y<<endl;
	}

	if(pos_x > maxX)
	{
		if(pos_y < ground->getGoalPosY() - ground->getGoalWidth()/2 || pos_y > ground->getGoalPosY() + ground->getGoalWidth()/2)
			pos_x = maxX;
		else
			if(pos_x > maxX + ground->getGoalDepth())
				pos_x = maxX + ground->getGoalDepth();
	}

	if(pos_x < minX)
	{
		if(pos_y < ground->getGoalPosY() - ground->getGoalWidth()/2 || pos_y > ground->getGoalPosY() + ground->getGoalWidth()/2)
				pos_x = minX;
		else
			if(pos_x < minX - ground->getGoalDepth())
				pos_x = minX - ground->getGoalDepth();
	}

	if(pos_y > maxY)
		pos_y = maxY;

	if(pos_y < minY)
		pos_y = minY;
}

void Player::positionGoalkeeper()
{
	if(ball->getPosY() < pos_y)
	{
		angle = 270;
	}
	else if(ball->getPosY() > pos_y)
	{
		angle = 90;
	}
	else
	{
		angle = 0;
		return;
	}

	posture = (posture + 2) % totalPostures;


	pos_x = pos_x + mobility * cos(angle * 3.1415 / 180);
	pos_y = pos_y + mobility * sin(angle * 3.1415 / 180);

	if(pos_y > GK_MAX_Y)
		pos_y = GK_MAX_Y;
	if(pos_y < GK_MIN_Y)
			pos_y = GK_MIN_Y;
}

void Player::shoot()
{
	if(possession == false)
		return;
	ball->hit(8.0, -0.3, angle);
	possession = false;
}

void Player::pass(int playerTeam, int playerId, Player *destPlayer, int destPlayerId, pair<float,float> dest, pair<float,float> src)
{
	if(possession == false)
		return;

	float dist = sqrt((src.first - dest.first)*(src.first - dest.first)
			+ (src.second - dest.second)*(src.second - dest.second));

	float u = 8.0;
	float accn = -1.0*((u*u)/(2.0*dist));

	float angle = atan((src.second - dest.second)/(src.first - dest.first));
	if((dest.first - src.first) < 0.0)
		angle += 3.1415;

	possession = false;
	ball->hit(u, accn, (angle*180.0)/3.1415);

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

void Player::release()
{
	this->possession = false;
}

bool Player::InPossession()
{
	return possession;
}

void Player::setIsBot(bool isBot)
{
	this->isBot = isBot;
}

bool Player::getIsBot()
{
	return this->isBot;
}

void Player::draw()
{
	glBindTexture(GL_TEXTURE_2D, soccer->getPlayerTex(texture)[posture]);

	int width = 88 / 5;
	int height = 98 / 5;

	applyRotation(angle + 90, pos_x, pos_y);
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
	this->possession = player.possession;
}
