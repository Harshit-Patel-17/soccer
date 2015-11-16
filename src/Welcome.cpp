/*
 * Welcome.cpp
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#include "../include/Welcome.h"
#include <string>

Welcome::Welcome() {
	//Load welcome texture
	{
		std::string filePath = "../res/misc/welcome.png";
		welcomeTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load single player texture
	{
		std::string filePath = "../res/misc/singlePlayer.png";
		singlePlayerTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load multi player texture
	{
		std::string filePath = "../res/misc/multiPlayer.png";
		multiPlayerTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load quit texture
	{
		std::string filePath = "../res/misc/quit.png";
		quitTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	aboveSinglePlayer = false;
	aboveMultiPlayer = false;
	aboveQuit = false;
}

Welcome::~Welcome() {
	glDeleteTextures(1, &welcomeTex);
	glDeleteTextures(1, &singlePlayerTex);
	glDeleteTextures(1, &multiPlayerTex);
	glDeleteTextures(1, &quitTex);
}


void Welcome::draw()
{
	int screen_width = glutGet(GLUT_WINDOW_WIDTH);
	int screen_height = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_width, screen_height, 0, 1, -1);
	//glTranslatef(0.0f, 0.0f, -150.0f);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(0.0f, 0.0f , screen_width, screen_height);
	glEnable(GL_TEXTURE_2D);

	int x = screen_width / 4;
	int y = screen_height / 4;
	int height = screen_height / 2;
	int width = height * 818 / 1000;
	//int height = 818 / 6;

	glBindTexture(GL_TEXTURE_2D, welcomeTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y + height, 0);
		glTexCoord2f(1, 0); glVertex3f(x + width, y + height, 0);
		glTexCoord2f(1, 1); glVertex3f(x + width, y, 0);
		glTexCoord2f(0, 1); glVertex3f(x, y, 0);
	glEnd();

	x = screen_width / 2;
	y = screen_height / 4;
	width = screen_width / 4;
	height = width * 106 / 770;

	if(aboveSinglePlayer)
		glColor3f(1.0f, 1.0f, 0.7f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, singlePlayerTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y + height, 0);
		glTexCoord2f(1, 0); glVertex3f(x + width, y + height, 0);
		glTexCoord2f(1, 1); glVertex3f(x + width, y, 0);
		glTexCoord2f(0, 1); glVertex3f(x, y, 0);
	glEnd();

	y += height;

	if(aboveMultiPlayer)
		glColor3f(1.0f, 1.0f, 0.7f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, multiPlayerTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y + height, 0);
		glTexCoord2f(1, 0); glVertex3f(x + width, y + height, 0);
		glTexCoord2f(1, 1); glVertex3f(x + width, y, 0);
		glTexCoord2f(0, 1); glVertex3f(x, y, 0);
	glEnd();

	y += height;
	width = height * 113 / 48;
	if(aboveQuit)
		glColor3f(1.0f, 1.0f, 0.7f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, quitTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y + height, 0);
		glTexCoord2f(1, 0); glVertex3f(x + width, y + height, 0);
		glTexCoord2f(1, 1); glVertex3f(x + width, y, 0);
		glTexCoord2f(0, 1); glVertex3f(x, y, 0);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
}

void Welcome::mouseMotion(int x, int y)
{
	int screen_width = glutGet(GLUT_WINDOW_WIDTH);
	int screen_height = glutGet(GLUT_WINDOW_HEIGHT);

	int startX = screen_width/2;
	int endX = screen_width * 3 / 4;
	int startY = screen_height / 4;
	int height = screen_width * 106 / (4 * 770);

	if(x > startX && x < endX && y > startY && y < startY + height)
		aboveSinglePlayer = true;
	else
		aboveSinglePlayer = false;

	if(x > startX && x < endX && y > startY + height && y < startY + 2*height)
		aboveMultiPlayer = true;
	else
		aboveMultiPlayer = false;

	if(x > startX && x < endX && y > startY + 2*height && y < startY + 3*height)
		aboveQuit = true;
	else
		aboveQuit = false;
}

game_command Welcome::mouseClick(int x, int y)
{
	int screen_width = glutGet(GLUT_WINDOW_WIDTH);
	int screen_height = glutGet(GLUT_WINDOW_HEIGHT);

	int startX = screen_width/2;
	int endX = screen_width * 3 / 4;
	int startY = screen_height / 4;
	int height = screen_width * 106 / (4 * 770);

	if(x > startX && x < endX && y > startY && y < startY + height)
		return SINGLE_PLAYER;

	if(x > startX && x < endX && y > startY + height && y < startY + 2*height)
		return MULTI_PLAYER;

	if(x > startX && x < endX && y > startY + 2*height && y < startY + 3*height)
		return QUIT;

	return NO_COMMAND;
}
