/*
 * Ground.h
 *
 *  Created on: 29-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_GROUND_H_
#define INCLUDE_GROUND_H_

#include "../include/Soccer.h"

class Ground {
	//GLuint texture;
	Soccer *soccer;
	float width;
	float height;
	float bottomLeftX;
	float bottomLeftY;
	float viewX;
	float viewY;
	float goalPosY;
	float goalWidth;
	float goalDepth;

	void applyRotation(float angle, float x, float y);
	void restoreRotation();
public:
	Ground(float width, float height, float bottomLeftX, float bottomLeftY, Soccer *soccer);
	virtual ~Ground();

	float getMinX();
	float getMaxX();
	float getMinY();
	float getMaxY();
	float getGoalPosY();
	void draw();
	void drawGoals();
	float getGroundHeight();
	float getGroundWidth();
	float getGoalWidth();
	float getGoalDepth();
};

#endif /* INCLUDE_GROUND_H_ */
