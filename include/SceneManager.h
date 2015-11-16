/*
 * SceneManager.h
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#ifndef INCLUDE_SCENEMANAGER_H_
#define INCLUDE_SCENEMANAGER_H_

#include "../include/Game.h"
#include "../include/Welcome.h"
#include "../include/MultiPlayer.h"

enum scene_type {WELCOME, GAME, MULTIPLAYER};

class SceneManager {
	scene_type scene;
	Game *game;
	Welcome *welcome;
	MultiPlayer *multiplayer;
public:
	SceneManager();
	virtual ~SceneManager();

	void flushAllScenes();
	void loadScene(scene_type scene);
	void drawScene();
	void mouseMotion(int x, int y);

	friend void handleKeyboardInput(int value);
	friend void handleJoystickInput(unsigned int buttomMask, int xaxis, int yaxis, int zaxis);
	friend void handleSpecialUpInput(int key,int x,int y);
	friend void handleSpecialInput(int key, int x, int y);
	friend void handleKeyUp(unsigned char key, int x, int y);
	friend void handleKeyPress(unsigned char key, int x, int y);
	friend void welcomeMouseFunc(int button, int state, int x, int y);
	friend void welcomeMouseMotionFunc(int x, int y);
};

extern SceneManager sceneManager;

#endif /* INCLUDE_SCENEMANAGER_H_ */
