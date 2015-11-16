/*
 * MultiPlayer.cpp
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#include "../include/MultiPlayer.h"
#include <ctype.h>

void drawText(string message,float x,float y)
{
	int i, len;

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

	//glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -5.0f);
	glRasterPos2f(x,y);

	glDisable(GL_TEXTURE);
	glDisable(GL_TEXTURE_2D);
	for (i = 0, len = message.size(); i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)message[i]);
	}
	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

MultiPlayer::MultiPlayer() {
	activeBox = 0;
}

MultiPlayer::~MultiPlayer() {
	// TODO Auto-generated destructor stub
}

void MultiPlayer::draw()
{
	int screen_width = glutGet(GLUT_WINDOW_WIDTH);
	int screen_height = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_width, screen_height, 0, 1, -1);

	drawText("IP", 100, 100);
	drawText("Port", 100, 150);
	drawText("Team", 100, 200);
	drawText("Player", 100, 250);

	glRectf(150, 82, 300, 112);
	glRectf(150, 132, 300, 162);
	glRectf(150, 182, 300, 212);
	glRectf(150, 232, 300, 262);

	drawText(serverIp, 150, 100);
	drawText(serverPort, 150, 150);
	drawText(team, 150, 200);
	drawText(player, 150, 250);

	glColor3f(0.8f, 0.8f, 0.8f);
	glRectf(100, 270, 200, 300);
	glRectf(250, 270, 350, 300);

	drawText("create", 130, 285);
	drawText("join", 280, 285);

	//drawText(serverIp, 100, 100);
}

multiplayer_command MultiPlayer::mouseClick(int x, int y)
{
	if(x > 150 && x < 300 && y > 82 && y < 112)
		activeBox = 0;
	else if(x > 150 && x < 300 && y > 132 && y < 162)
		activeBox = 1;
	else if(x > 150 && x < 300 && y > 182 && y < 212)
		activeBox = 2;
	else if(x > 150 && x < 300 && y > 232 && y < 262)
		activeBox = 3;
	else if(x > 100 && x < 200 && y > 270 && y < 300)
		return CREATE_GAME;
	else if(x > 250 && x < 350 && y > 270 && y < 300)
		return JOIN_GAME;

	return NO_MULTIPLAYER_COMMAND;
}

void MultiPlayer::keyPress(char ch)
{
	if(activeBox == 0)
	{
		if(serverIp.length() >= 15 && ch != '\b')
			return;

		if(isdigit(ch) || ch == '.' )
			serverIp += ch;

		if(ch == '\b')
			serverIp.erase(serverIp.length() - 1);
	}
	else if(activeBox == 1)
	{
		if(isdigit(ch) )
			serverPort += ch;

		if(ch == '\b')
			serverPort.erase(serverPort.length() - 1);
	}
	else if(activeBox == 2)
	{
		if(team.length() >= 1 && ch != '\b')
			return;

		if(ch == '1' || ch == '0' )
			team += ch;

		if(ch == '\b')
			team.erase(team.length() - 1);
	}
	else if(activeBox == 3)
	{
		if(player.length() >= 1 && ch != '\b')
			return;

		if(ch == '1' || ch == '0' )
			player += ch;

		if(ch == '\b')
			player.erase(player.length() - 1);
	}
}

string MultiPlayer::getServerIp()
{
	return serverIp;
}

int MultiPlayer::getServerPort()
{
	return atoi(serverPort.c_str());
}

int MultiPlayer::getTeam()
{
	return atoi(team.c_str());
}

int MultiPlayer::getPlayer()
{
	return atoi(player.c_str());
}
