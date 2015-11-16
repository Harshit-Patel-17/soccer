/*
 * Welcome.h
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#ifndef INCLUDE_WELCOME_H_
#define INCLUDE_WELCOME_H_

#include <GL/gl.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>

#define SCREEN_WIDTH 1024*2
#define SCREEN_HEIGHT 768*2

enum game_command {SINGLE_PLAYER, MULTI_PLAYER, QUIT, NO_COMMAND};

class Welcome {
	GLuint welcomeTex;
	GLuint singlePlayerTex;
	GLuint multiPlayerTex;
	GLuint quitTex;
	bool aboveSinglePlayer;
	bool aboveMultiPlayer;
	bool aboveQuit;
public:
	Welcome();
	virtual ~Welcome();

	void draw();
	void mouseMotion(int x, int y);
	game_command mouseClick(int x, int y);
};

#endif /* INCLUDE_WELCOME_H_ */
