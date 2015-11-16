/*
 * MultiPlayer.h
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#ifndef INCLUDE_MULTIPLAYER_H_
#define INCLUDE_MULTIPLAYER_H_

#include <GL/gl.h>
#include <GL/glut.h>
#include <string>

using namespace std;

enum multiplayer_command {CREATE_GAME, JOIN_GAME, NO_MULTIPLAYER_COMMAND};

class MultiPlayer {
	string serverIp;
	string serverPort;
	string team;
	string player;
	int activeBox;
public:
	MultiPlayer();
	virtual ~MultiPlayer();

	void draw();
	multiplayer_command mouseClick(int x, int y);
	void keyPress(char ch);
	string getServerIp();
	int getServerPort();
	int getTeam();
	int getPlayer();
};

#endif /* INCLUDE_MULTIPLAYER_H_ */
