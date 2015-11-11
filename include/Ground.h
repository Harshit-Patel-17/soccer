/*
 * Ground.h
 *
 *  Created on: 29-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_GROUND_H_
#define INCLUDE_GROUND_H_

#include "../include/Soccer.h"
#include <iostream>

using namespace std;

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
	pair<pair<float,float>, pair<float, float> > team1GoalPos;
	pair<pair<float,float>, pair<float, float> > team2GoalPos;
	pair<pair<float,float>, pair<float, float> > team1DBox;
	pair<pair<float,float>, pair<float, float> > team2DBox;

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
	pair<pair<float,float>, pair<float, float> > getTeam1GoalPos();
	pair<pair<float,float>, pair<float, float> > getTeam2GoalPos();
	pair<pair<float,float>, pair<float, float> > getTeam1DBox();
	pair<pair<float,float>, pair<float, float> > getTeam2DBox();
};

#endif /* INCLUDE_GROUND_H_ */
