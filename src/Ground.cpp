/*
 * Ground.cpp
 *
 *  Created on: 29-Oct-2015
 *      Author: harshit
 */

#include "../include/Ground.h"
#include <math.h>

void Ground::applyRotation(float angle, float x, float y)
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

void Ground::restoreRotation()
{
    glPopMatrix();
}

Ground::Ground(float width, float height, float bottomLeftX, float bottomLeftY, Soccer *soccer)
{
	this->width = width;
	this->height = height;
	this->bottomLeftX = bottomLeftX;
	this->bottomLeftY = bottomLeftY;
	this->soccer = soccer;
	this->goalPosY = bottomLeftY + 0.5 * height;
	this->goalWidth = 50;
	this->goalDepth = 20;
	//this->texture = soccer->getGroundTex();
}

Ground::~Ground()
{
	// TODO Auto-generated destructor stub
}

float Ground::getMinX()
{
	return bottomLeftX + 0.1 * width;
}

float Ground::getMaxX()
{
	return bottomLeftX + 0.9 * width;
}

float Ground::getMinY()
{
	return bottomLeftY + 0.03 * height;
}

float Ground::getMaxY()
{
	return bottomLeftY + 0.97 * height;
}

void Ground::draw()
{
	glBindTexture(GL_TEXTURE_2D, soccer->getGroundTex());

	int x = bottomLeftX;
	int y = bottomLeftY;

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y, 0);
		glTexCoord2f(1, 0); glVertex3f(x + width, y, 0);
		glTexCoord2f(1, 1); glVertex3f(x + width, y + height, 0);
		glTexCoord2f(0, 1); glVertex3f(x, y + height, 0);
	glEnd();
}

void Ground::drawGoals()
{
	int goalWidth = 40;
	int goalHeight = (244.0 / 143) * goalWidth;

	glBindTexture(GL_TEXTURE_2D, soccer->getGoalTex());

	int x = bottomLeftX + 0.06 * width;
	int y = goalPosY;

	this->team1GoalPos = make_pair(make_pair(x + goalWidth/2, y - goalHeight/2),make_pair(x + goalWidth/2, y + goalHeight/2));

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x - goalWidth/2, y - goalHeight/2, 0);
		glTexCoord2f(1, 0); glVertex3f(x + goalWidth/2, y - goalHeight/2, 0);
		glTexCoord2f(1, 1); glVertex3f(x + goalWidth/2, y + goalHeight/2, 0);
		glTexCoord2f(0, 1); glVertex3f(x - goalWidth/2, y + goalHeight/2, 0);
	glEnd();

	x = bottomLeftX + 0.94 * width;
	y = goalPosY;

	this->team2GoalPos = make_pair(make_pair(x + goalWidth/2, y - goalHeight/2),make_pair(x + goalWidth/2, y + goalHeight/2));

	applyRotation(180, x, y);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x - goalWidth/2, y - goalHeight/2, 0);
		glTexCoord2f(1, 0); glVertex3f(x + goalWidth/2, y - goalHeight/2, 0);
		glTexCoord2f(1, 1); glVertex3f(x + goalWidth/2, y + goalHeight/2, 0);
		glTexCoord2f(0, 1); glVertex3f(x - goalWidth/2, y + goalHeight/2, 0);
	glEnd();
	restoreRotation();

	float x1 = bottomLeftX + 0.25 * width;
	float y1 = bottomLeftY + 0.25 * height;

	float x2 = bottomLeftX + 0.25 * width;
	float y2 = bottomLeftY + 0.75 * height;
	this->team1DBox = make_pair(make_pair(x1,y1), make_pair(x2,y2));

	x1 = x2 = bottomLeftX + 0.75 * width;
	this->team2DBox = make_pair(make_pair(x1,y1), make_pair(x2,y2));
}

float Ground::getGroundHeight()
{
	return height;
}

float Ground::getGroundWidth()
{
	return width;
}

float Ground::getGoalPosY()
{
	return goalPosY;
}

float Ground::getGoalWidth()
{
	return goalWidth;
}

float Ground::getGoalDepth()
{
	return goalDepth;
}

pair<pair<float,float>, pair<float, float> > Ground::getTeam1GoalPos()
{
	return team1GoalPos;
}

pair<pair<float,float>, pair<float, float> > Ground::getTeam2GoalPos()
{
	return team2GoalPos;
}

pair<pair<float,float>, pair<float, float> > Ground::getTeam1DBox()
{
	return team1DBox;
}

pair<pair<float,float>, pair<float, float> > Ground::getTeam2DBox()
{
	return team2DBox;
}
