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
#include <string>

class Soccer
{
	GLuint playerTex[15];

public:
	Soccer();
	virtual ~Soccer();

	GLuint *getPlayerTex();
};

#endif /* INCLUDE_SOCCER_H_ */
