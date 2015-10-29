/*
 * Ground.cpp
 *
 *  Created on: 29-Oct-2015
 *      Author: harshit
 */

#include "../include/Ground.h"

Ground::Ground(float width, float height, float bottomLeftX, float bottomLeftY, Soccer *soccer)
{
	this->width = width;
	this->height = height;
	this->bottomLeftX = bottomLeftX;
	this->bottomLeftY = bottomLeftY;
	this->soccer = soccer;
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

