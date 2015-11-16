/*
 * SceneManager.cpp
 *
 *  Created on: 16-Nov-2015
 *      Author: harshit
 */

#include <math.h>
#include "../include/SceneManager.h"

#define JOYSTICK_THRESHOLD 200
#define IO_TIMER 25

enum keys {RIGHT_KEY, LEFT_KEY, UP_KEY, DOWN_KEY, PASS_KEY, SHOOT_KEY, ESC_KEY};

bool keysPressed[7];

SceneManager sceneManager;

void handleKeyboardInput(int value)
{
	if(sceneManager.scene != GAME)
		return;

	Control control;
	control.playerId = (sceneManager.game)->getMyPlayerId();
	control.teamNo = (sceneManager.game)->getMyPlayerTeam();

	control.type = MOVE;
	if(keysPressed[RIGHT_KEY])
	{
		if(keysPressed[UP_KEY])
		{
			control.angle = 45;
		}
		else if(keysPressed[DOWN_KEY])
		{
			control.angle = 315;
		}
		else
		{
			control.angle = 0;
		}

		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}
	else if(keysPressed[LEFT_KEY])
	{
		if(keysPressed[UP_KEY])
		{
			control.angle = 135;
		}
		else if(keysPressed[DOWN_KEY])
		{
			control.angle = 225;
		}
		else
		{
			control.angle = 180;
		}

		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}
	else if(keysPressed[UP_KEY])
	{
		if(keysPressed[RIGHT_KEY])
		{
			control.angle = 45;
		}
		else if(keysPressed[LEFT_KEY])
		{
			control.angle = 135;
		}
		else
		{
			control.angle = 90;
		}

		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}
	else if(keysPressed[DOWN_KEY])
	{
		if(keysPressed[RIGHT_KEY])
		{
			control.angle = 315;
		}
		else if(keysPressed[LEFT_KEY])
		{
			control.angle = 225;
		}
		else
		{
			control.angle = 270;
		}

		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}

	if(keysPressed[ESC_KEY])
	{
		exit(0);
	}
	else if(keysPressed[SHOOT_KEY])
	{
		control.type = SHOOT;
		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->shoot((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootPower(SHOOT_RATE, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}
	else if(keysPressed[PASS_KEY])
	{
		control.type = PASS;
		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->pass((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
		else
			(sceneManager.game)->insertControl(control);
	}

	glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
}


void handleJoystickInput(unsigned int buttomMask, int xaxis, int yaxis, int zaxis)
{
	Control control;
	control.playerId = (sceneManager.game)->getMyPlayerId();
	control.teamNo = (sceneManager.game)->getMyPlayerTeam();

	control.type = MOVE;
	if(xaxis > JOYSTICK_THRESHOLD || xaxis < -JOYSTICK_THRESHOLD)
	{
		control.angle = atan(-yaxis / xaxis) * 180 / 3.1415;
		if(xaxis < 0)
			control.angle += 180;
		if((sceneManager.game)->getType() == CREATOR)
			(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			(sceneManager.game)->insertControl(control);

		(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
	}
	else
	{
		if(yaxis > JOYSTICK_THRESHOLD || yaxis < -JOYSTICK_THRESHOLD)
		{
			if(xaxis == 0)
			{
				control.angle = 90;
				if(yaxis > 0)
					control.angle += 180;
				if((sceneManager.game)->getType() == CREATOR)
					(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
				else
					(sceneManager.game)->insertControl(control);
			}
			else
			{
				control.angle = atan(-yaxis / xaxis) * 180 / 3.1415;
				if(xaxis < 0)
					control.angle += 180;
				if((sceneManager.game)->getType() == CREATOR)
					(sceneManager.game)->movePlayer(control.teamNo, control.playerId, control.angle);
				else
					(sceneManager.game)->insertControl(control);
			}

			(sceneManager.game)->updateShootAngle(ANGLE_ROTATION, control.angle, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
		}
	}

	switch(buttomMask)
	{
		case GLUT_JOYSTICK_BUTTON_A:
			//std::cout << "A" << std::endl;
			break;

		case GLUT_JOYSTICK_BUTTON_B:
			control.type = SHOOT;
			if((sceneManager.game)->getType() == CREATOR)
				(sceneManager.game)->shoot((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
			else
				(sceneManager.game)->insertControl(control);

			(sceneManager.game)->updateShootPower(SHOOT_RATE, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
			break;

		case GLUT_JOYSTICK_BUTTON_C:
			control.type = PASS;
			if((sceneManager.game)->getType() == CREATOR)
				(sceneManager.game)->pass((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
			else
				(sceneManager.game)->insertControl(control);
			break;

		case GLUT_JOYSTICK_BUTTON_D:
			//std::cout << "D" << std::endl;
			break;
	}
}

void handleSpecialUpInput(int key,int x,int y)
{
	switch(key)
	{
		case GLUT_KEY_RIGHT:
			keysPressed[RIGHT_KEY] = false;
			break;

		case GLUT_KEY_LEFT:
			keysPressed[LEFT_KEY] = false;
			break;

		case GLUT_KEY_UP:
			keysPressed[UP_KEY] = false;
			break;

		case GLUT_KEY_DOWN:
			keysPressed[DOWN_KEY] = false;
			break;
	}
}

void handleSpecialInput(int key, int x, int y)
{
	Control control;
	control.type = MOVE;
	control.playerId = (sceneManager.game)->getMyPlayerId();
	control.teamNo = (sceneManager.game)->getMyPlayerTeam();

    switch(key)
    {
        case GLUT_KEY_RIGHT:
        	keysPressed[RIGHT_KEY] = true;
            break;

        case GLUT_KEY_LEFT:
        	keysPressed[LEFT_KEY] = true;
            break;

        case GLUT_KEY_UP:
        	keysPressed[UP_KEY] = true;
            break;

        case GLUT_KEY_DOWN:
        	keysPressed[DOWN_KEY] = true;
            break;
    }
}

void handleKeyUp(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			keysPressed[ESC_KEY] = false;
			break;

		case 32:
			keysPressed[SHOOT_KEY] = false;
			break;

		case 's':
			keysPressed[PASS_KEY] = false;
			break;

		/*case 's':
			if(isShootKeyPressed)
			{
				shootPressEnd = clock();
				double cpu_time_used = ((double) (shootPressEnd - shootPressStart)) / CLOCKS_PER_SEC;
				float initial_velocity = cpu_time_used + 5.0;
				printf("Key up %c",key);
				printf("%f ",initial_velocity);
				//(sceneManager.game)->shoot(8.0);
				isShootKeyPressed = false;
			}
			break;*/
	}
}

void handleKeyPress(unsigned char key, int x, int y)
{
	if(sceneManager.scene == GAME)
	{
		Control control;
		control.playerId = (sceneManager.game)->getMyPlayerId();
		control.teamNo = (sceneManager.game)->getMyPlayerTeam();

		switch(key)
		{
			case 27:
				keysPressed[ESC_KEY] = true;
				//exit(0);
				break;

			case 32:
				keysPressed[SHOOT_KEY] = true;
				/*control.type = SHOOT;
				if((sceneManager.game)->getType() == CREATOR)
					(sceneManager.game)->shoot((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
				else
					(sceneManager.game)->insertControl(control);

				(sceneManager.game)->updateShootPower(SHOOT_RATE, (sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());*/
				break;

			case 's':
				keysPressed[PASS_KEY] = true;
				/*control.type = PASS;
				if((sceneManager.game)->getType() == CREATOR)
					(sceneManager.game)->pass((sceneManager.game)->getMyPlayerTeam(), (sceneManager.game)->getMyPlayerId());
				else
					(sceneManager.game)->insertControl(control);*/
				break;
		}
	}
	else if(sceneManager.scene == MULTIPLAYER)
	{
		(sceneManager.multiplayer)->keyPress(key);
	}
}

void welcomeMouseFunc(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
		return;

	game_command gameCommand;
	multiplayer_command multiplayerCommand;
	if(sceneManager.scene == WELCOME)
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			gameCommand = (sceneManager.welcome)->mouseClick(x, y);
			switch(gameCommand)
			{
			case SINGLE_PLAYER:
				sceneManager.flushAllScenes();
				sceneManager.game = new Game(3000, CREATOR, sceneManager.gameWeather);
				(sceneManager.game)->startServer();
				sceneManager.loadScene(GAME);
				break;

			case MULTI_PLAYER:
				sceneManager.flushAllScenes();
				sceneManager.multiplayer = new MultiPlayer();
				sceneManager.loadScene(MULTIPLAYER);
				break;

			case QUIT:
				exit(0);
				break;

			case NO_COMMAND:
				break;
			}
			break;
		}
	}
	else if(sceneManager.scene == MULTIPLAYER)
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			multiplayerCommand = (sceneManager.multiplayer)->mouseClick(x, y);
			switch(multiplayerCommand)
			{
			case CREATE_GAME:
				sceneManager.game = new Game(3000, CREATOR, sceneManager.gameWeather);
				(sceneManager.game)->startServer();
				sceneManager.loadScene(GAME);
				break;

			case JOIN_GAME:
				sceneManager.game = new Game(4000, JOINER, sceneManager.gameWeather);
				(sceneManager.game)->startServer();
				(sceneManager.game)->setServerAddr((sceneManager.multiplayer)->getServerIp().c_str(), (sceneManager.multiplayer)->getServerPort());
				(sceneManager.game)->selectPlayer((sceneManager.multiplayer)->getTeam(), (sceneManager.multiplayer)->getPlayer());
				(sceneManager.game)->join();
				sceneManager.loadScene(GAME);
				break;

			case NO_MULTIPLAYER_COMMAND:
				break;
			}
			break;
		}
	}

	//sceneManager.loadScene(GAME);
}

void welcomeMouseMotionFunc(int x, int y)
{
	sceneManager.mouseMotion(x, y);
}

SceneManager::SceneManager() {
	game = NULL;
	welcome = NULL;
	multiplayer = NULL;
}

SceneManager::~SceneManager() {
}

void SceneManager::flushAllScenes()
{
	if(game != NULL)
	{
		delete game;
		game = NULL;
	}
	if(welcome != NULL)
	{
		delete welcome;
		welcome = NULL;
	}
	if(multiplayer != NULL)
	{
		delete multiplayer;
		multiplayer = NULL;
	}
}

void SceneManager::loadScene(scene_type scene)
{
	this->scene = scene;

	switch(scene)
	{
	case GAME:
		//game = new Game(3000, CREATOR, DAY);
		glutIgnoreKeyRepeat(1);
		glutKeyboardFunc(handleKeyPress);
		glutKeyboardUpFunc(handleKeyUp);
		glutSpecialFunc(handleSpecialInput);
		glutSpecialUpFunc(handleSpecialUpInput);
		glutJoystickFunc(handleJoystickInput, IO_TIMER);
		glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
		glutMouseFunc(NULL);
		glutPassiveMotionFunc(NULL);
		break;

	case WELCOME:
		welcome = new Welcome();
		glutIgnoreKeyRepeat(1);
		glutKeyboardFunc(NULL);
		glutKeyboardUpFunc(NULL);
		glutSpecialFunc(NULL);
		glutSpecialUpFunc(NULL);
		glutJoystickFunc(NULL, IO_TIMER);
		glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
		glutMouseFunc(welcomeMouseFunc);
		glutPassiveMotionFunc(welcomeMouseMotionFunc);
		break;

	case MULTIPLAYER:
		//multiplayer = new MultiPlayer();
		glutIgnoreKeyRepeat(0);
		glutKeyboardFunc(NULL);
		glutKeyboardUpFunc(handleKeyPress);
		glutSpecialFunc(NULL);
		glutSpecialUpFunc(NULL);
		glutJoystickFunc(NULL, IO_TIMER);
		glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
		glutMouseFunc(welcomeMouseFunc);
		glutPassiveMotionFunc(welcomeMouseMotionFunc);
		break;
	}
}

void SceneManager::drawScene()
{
	switch(scene)
	{
	case GAME:
		game->draw();
		break;

	case WELCOME:
		welcome->draw();
		break;

	case MULTIPLAYER:
		multiplayer->draw();
		break;
	}
}

void SceneManager::mouseMotion(int x, int y)
{
	if(welcome != NULL)
		welcome->mouseMotion(x, y);
}

void SceneManager::setGameWeather(weather_type weather)
{
	gameWeather = weather;
}
