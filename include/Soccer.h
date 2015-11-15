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

enum weather_type {DAY, NIGHT, STORM};

class Soccer
{
	GLuint *playerTex[PLAYER_TEX];
	GLuint shadowTex;
	GLuint *ballTex;
	GLuint groundTex;
	GLuint goalTex;
	GLuint arrowTex;
	GLuint goalWordTex;
	GLuint teamWonTex[2];
	GLuint drawMatchTex;
	Mix_Music *crowdChant;
	Mix_Music *thunderEffect;
	Mix_Chunk *shootEffect;
	Mix_Chunk *crowdCheer;
	Mix_Chunk *whistle;

public:
	Soccer();
	virtual ~Soccer();

	GLuint *getPlayerTex(int player);
	GLuint getShadowTex();
	int getTotalPlayerPostures(int player);
	GLuint *getBallTex();
	int getTotalBallPositions();
	GLuint getGroundTex();
	GLuint getGoalTex();
	GLuint getArrowTex();
	GLuint getGoalWordTex();
	GLuint *getTeamWonTex();
	GLuint getDrawMatchTex();
	Mix_Music *getCrowdChant();
	Mix_Music *getThunderEffect();
	Mix_Chunk *getShootEffect();
	Mix_Chunk *getCrowdCheer();
	Mix_Chunk *getWhistle();
};

#endif /* INCLUDE_SOCCER_H_ */
