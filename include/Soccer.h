/*
 * Soccer.h
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#ifndef INCLUDE_SOCCER_H_
#define INCLUDE_SOCCER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
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
	GLuint goalTex;
	GLuint arrowTex;
	Mix_Music *crowdChant;
	Mix_Chunk *shootEffect;

public:
	Soccer();
	virtual ~Soccer();

	GLuint *getPlayerTex(int player);
	int getTotalPlayerPostures(int player);
	GLuint *getBallTex();
	int getTotalBallPositions();
	GLuint getGroundTex();
	GLuint getGoalTex();
	GLuint getArrowTex();
	Mix_Music *getCrowdChant();
	Mix_Chunk *getShootEffect();
};

#endif /* INCLUDE_SOCCER_H_ */
