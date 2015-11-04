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
public:
	Ground(float width, float height, float bottomLeftX, float bottomLeftY, Soccer *soccer);
	virtual ~Ground();

	float getMinX();
	float getMaxX();
	float getMinY();
	float getMaxY();
	void draw();
	float getGroundHeight();
	float getGroundWidth();
};

#endif /* INCLUDE_GROUND_H_ */
