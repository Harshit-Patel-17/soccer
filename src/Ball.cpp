/*
 * Ball.cpp
 *
 *  Created on: 25-Oct-2015
 *      Author: harshit
 */

#include "../include/Ball.h"
#include <math.h>

float Ball::min(float x, float y)
{
	if(x <= y)
		return x;
	else
		return y;
}

float Ball::max(float x, float y)
{
	if(x >= y)
		return x;
	else
		return y;
}

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

Ball::Ball()
{

}

Ball::Ball(float pos_x, float pos_y, float angle, Soccer *soccer, Ground *ground)
{
	this->isShoot = false;
	this->position = 0;
	this->totalPositions = soccer->getTotalBallPositions();
	this->isPass = false;
	this->ballPassedBy = make_pair(-1,-1);
	this->ground = ground;
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
	//updatePosition();
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

	if(isShoot)
	{
		glBindTexture(GL_TEXTURE_2D, soccer->getArrowTex());

		width = 101 / 8;
		height = 57 / 8;
		int dist = 10;

		applyRotation(shootAngle, pos_x, pos_y);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(pos_x + dist - width/2, pos_y - height/2, 0);
			glTexCoord2f(1, 0); glVertex3f(pos_x + dist + width/2, pos_y - height/2, 0);
			glTexCoord2f(1, 1); glVertex3f(pos_x + dist + width/2, pos_y + height/2, 0);
			glTexCoord2f(0, 1); glVertex3f(pos_x + dist - width/2, pos_y + height/2, 0);
		glEnd();
		restoreRotation();

		float powerBarWidth = 7;
		float powerBarHeight = 1;
		float powerBarLowerX = pos_x - powerBarWidth/2;
		float powerBarLowerY = pos_y - 10;
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		glRectf(powerBarLowerX, powerBarLowerY , powerBarLowerX + powerBarWidth, powerBarLowerY + powerBarHeight);
		glColor3f(1.0f, 0.0f, 0.0f);
		glRectf(powerBarLowerX, powerBarLowerY , powerBarLowerX + (shootPower - MIN_SHOOT_POWER) / (MAX_SHOOT_POWER - MIN_SHOOT_POWER) * powerBarWidth, powerBarLowerY + powerBarHeight);
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
	}
}

void Ball::updatePosition()
{
	float minX = ground->getMinX();
	float maxX = ground->getMaxX();
	float minY = ground->getMinY();
	float maxY = ground->getMaxY();

	float dx_unit = cos(angle * 3.1415 / 180);
	float dy_unit = sin(angle * 3.1415 / 180);

	if(u*u + 2*a*d < 0)
		return;

	float v = sqrt(u*u + 2*a*d);
	pos_x += v * dx_unit;
	pos_y += v * dy_unit;
	d += v;
	position = (position + (int)v) % totalPositions;

	float lowerGoalY = ground->getGoalPosY() - ground->getGoalWidth()/2;
	float upperGoalY = ground->getGoalPosY() + ground->getGoalWidth()/2;

	if(pos_x > maxX)
	{
		float y_intercept = pos_y + (maxX - pos_x) * tan(angle * 3.1415 / 180);
		if(y_intercept >= lowerGoalY && y_intercept <= upperGoalY)
		{
			if(pos_x > maxX + ground->getGoalDepth())
				pos_x = maxX + ground->getGoalDepth();

			if(pos_y < lowerGoalY)
				pos_y = lowerGoalY;

			if(pos_y > upperGoalY)
				pos_y = upperGoalY;
		}
		else
		{
			pos_x = maxX;
			angle = 180 - angle;
		}
	}

	if(pos_x < minX)
	{
		float y_intercept = pos_y + (minX - pos_x) * tan(angle * 3.1415 / 180);
		if(y_intercept >= lowerGoalY && y_intercept <= upperGoalY)
		{
			if(pos_x < minX - ground->getGoalDepth())
				pos_x = minX - ground->getGoalDepth();

			if(pos_y < lowerGoalY)
				pos_y = lowerGoalY;

			if(pos_y > upperGoalY)
				pos_y = upperGoalY;
		}
		else
		{
			pos_x = minX;
			angle = 180 - angle;
		}
	}

	if(pos_y > maxY)
	{
		pos_y = maxY;
		angle = -angle;
	}

	if(pos_y < minY)
	{
		pos_y = minY;
		angle = -angle;
	}
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

void Ball::setPosX(float pos_x)
{
	this->pos_x = pos_x;
}

float Ball::getPosY()
{
	return pos_y;
}

void Ball::setPosY(float pos_y)
{
	this->pos_y = pos_y;
}

void Ball::setPosition(float x, float y)
{
	this->pos_x = x;
	this->pos_y = y;
}

void Ball::operator=(Ball& ball)
{
	this->isShoot = ball.isShoot;
	this->shootAngle = ball.shootAngle;
	this->shootPower = ball.shootPower;
	this->isPass = ball.isPass;
	this->position = ball.position;
	this->totalPositions = ball.totalPositions;
	this->pos_x = ball.pos_x;
	this->pos_y = ball.pos_y;
	this->angle = ball.angle;
	this->u = ball.u;
	this->a = ball.a;
	this->d = ball.d;
}

bool Ball::isOnShoot()
{
	return isShoot;
}

float Ball::getU()
{
	return u;
}

float Ball::getA()
{
	return a;
}

float Ball::getD()
{
	return d;
}

float Ball::getAngle()
{
	return angle;
}

void Ball::setAngle(float angle)
{
	this->angle = angle;
}

void Ball::setIsShoot(bool isShoot)
{
	this->isShoot = isShoot;
}

void Ball::setIsPass(bool isPass)
{
	this->isPass = isPass;
}

void Ball::setShootAngle(float shootAngle)
{
	this->shootAngle = shootAngle;
}

float Ball::getShootAngle()
{
	return shootAngle;
}

void Ball::updateShootAngle(float amount, float towards)
{
	//shootAngle += amount;
	towards = (towards + 360) - int((towards + 360) / 360) * 360;
	shootAngle = (shootAngle + 360) - int((shootAngle + 360) / 360) * 360;

	std::cout << shootAngle << " " << towards << std::endl;

	float clockWiseAngle, antiClockWiseAngle;

	if(shootAngle >= towards)
		clockWiseAngle = shootAngle - towards;
	else
		clockWiseAngle = (360 - towards) + shootAngle;

	if(shootAngle >= towards)
		antiClockWiseAngle = (360 - shootAngle) + towards;
	else
		antiClockWiseAngle = towards - shootAngle;

	if(clockWiseAngle <= antiClockWiseAngle)
		shootAngle -= amount;
	else
		shootAngle += amount;

}

void Ball::setShootPower(float shootPower)
{
	this->shootPower = shootPower;
}

float Ball::getShootPower()
{
	return shootPower;
}

void Ball::updateShootPower(float amount)
{
	shootPower += min(amount, max(MAX_SHOOT_POWER - shootPower, 0));
}

bool Ball::isBallPassed()
{
	return isPass;
}

void Ball::setAccn(float a)
{
	this->a = a;
}

void Ball::setBallPassedBy(pair<int,int> ballPassedBy)
{
	this->ballPassedBy = ballPassedBy;
}

pair<int,int> Ball::getBallPassedBy()
{
	return this->ballPassedBy;
}
