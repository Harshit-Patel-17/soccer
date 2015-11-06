/*
 * Soccer.h
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_SOCCER_H_
#define INCLUDE_SOCCER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_endian.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <string>

#define PLAYER_TEX 2

class Soccer
{
	GLuint *playerTex[PLAYER_TEX];
	GLuint *ballTex;
	GLuint groundTex;

public:
	Soccer();
	virtual ~Soccer();

	GLuint *getPlayerTex(int player);
	int getTotalPlayerPostures(int player);
	GLuint *getBallTex();
	int getTotalBallPositions();
	GLuint getGroundTex();
};

#endif /* INCLUDE_SOCCER_H_ */
