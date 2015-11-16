/*
 * main.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include <iostream>
#include <time.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "../include/Game.h"
#include "../include/SceneManager.h"

using namespace std;

#define JOYSTICK_THRESHOLD 200
#define REFRESH_TIMER 50
#define IO_TIMER 25

int screen_width = SCREEN_WIDTH;
int screen_height = SCREEN_HEIGHT;

/*Game *game;

enum keys {RIGHT_KEY, LEFT_KEY, UP_KEY, DOWN_KEY, PASS_KEY, SHOOT_KEY, ESC_KEY};

bool keysPressed[7];
clock_t shootPressStart,shootPressEnd;
bool isShootKeyPressed = false;
clock_t startTime,endTime;

void handleKeyboardInput(int value)
{
	Control control;
	control.playerId = game->getMyPlayerId();
	control.teamNo = game->getMyPlayerTeam();

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

		if(game->getType() == CREATOR)
			game->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			game->insertControl(control);

		game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
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

		if(game->getType() == CREATOR)
			game->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			game->insertControl(control);

		game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
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

		if(game->getType() == CREATOR)
			game->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			game->insertControl(control);

		game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
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

		if(game->getType() == CREATOR)
			game->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			game->insertControl(control);

		game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
	}

	if(keysPressed[ESC_KEY])
	{
		exit(0);
	}
	else if(keysPressed[SHOOT_KEY])
	{
		control.type = SHOOT;
		if(game->getType() == CREATOR)
			game->shoot(game->getMyPlayerTeam(), game->getMyPlayerId());
		else
			game->insertControl(control);

		game->updateShootPower(SHOOT_RATE, game->getMyPlayerTeam(), game->getMyPlayerId());
	}
	else if(keysPressed[PASS_KEY])
	{
		control.type = PASS;
		if(game->getType() == CREATOR)
			game->pass(game->getMyPlayerTeam(), game->getMyPlayerId());
		else
			game->insertControl(control);
	}

	glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
}


void handleJoystickInput(unsigned int buttomMask, int xaxis, int yaxis, int zaxis)
{
	Control control;
	control.playerId = game->getMyPlayerId();
	control.teamNo = game->getMyPlayerTeam();

	control.type = MOVE;
	if(xaxis > JOYSTICK_THRESHOLD || xaxis < -JOYSTICK_THRESHOLD)
	{
		control.angle = atan(-yaxis / xaxis) * 180 / 3.1415;
		if(xaxis < 0)
			control.angle += 180;
		if(game->getType() == CREATOR)
			game->movePlayer(control.teamNo, control.playerId, control.angle);
		else
			game->insertControl(control);

		game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
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
				if(game->getType() == CREATOR)
					game->movePlayer(control.teamNo, control.playerId, control.angle);
				else
					game->insertControl(control);
			}
			else
			{
				control.angle = atan(-yaxis / xaxis) * 180 / 3.1415;
				if(xaxis < 0)
					control.angle += 180;
				if(game->getType() == CREATOR)
					game->movePlayer(control.teamNo, control.playerId, control.angle);
				else
					game->insertControl(control);
			}

			game->updateShootAngle(ANGLE_ROTATION, control.angle, game->getMyPlayerTeam(), game->getMyPlayerId());
		}
	}

	switch(buttomMask)
	{
		case GLUT_JOYSTICK_BUTTON_A:
			//std::cout << "A" << std::endl;
			break;

		case GLUT_JOYSTICK_BUTTON_B:
			control.type = SHOOT;
			if(game->getType() == CREATOR)
				game->shoot(game->getMyPlayerTeam(), game->getMyPlayerId());
			else
				game->insertControl(control);

			game->updateShootPower(SHOOT_RATE, game->getMyPlayerTeam(), game->getMyPlayerId());
			break;

		case GLUT_JOYSTICK_BUTTON_C:
			control.type = PASS;
			if(game->getType() == CREATOR)
				game->pass(game->getMyPlayerTeam(), game->getMyPlayerId());
			else
				game->insertControl(control);
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
	control.playerId = game->getMyPlayerId();
	control.teamNo = game->getMyPlayerTeam();

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
	}
}

void handleKeyPress(unsigned char key, int x, int y)
{
	Control control;
	control.playerId = game->getMyPlayerId();
	control.teamNo = game->getMyPlayerTeam();

	switch(key)
	{
        case 27:
        	keysPressed[ESC_KEY] = true;
            break;

        case 32:
        	keysPressed[SHOOT_KEY] = true;
        	break;

        case 's':
        	keysPressed[PASS_KEY] = true;
        	break;
	}
}*/

void initRendering()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	//glEnable(GL_NORMALIZE);
	glClearColor(0.87f, 0.53f, 0.16f, 1.0f);
}

void handleResize(int w, int h)
{
	screen_width = w;
	screen_height = h;
}

/*void drawText(string message,float x,float y)
{
	int i, len;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRasterPos2f(x,y);

	glDisable(GL_TEXTURE);
	glDisable(GL_TEXTURE_2D);
	for (i = 0, len = message.size(); i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)message[i]);
	}
	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);
}*/

/*void showScoreAndTime()
{
	string message = "TEAM1 " + std::to_string(game->getTeam1Goals()) + "-" +
			std::to_string(game->getTeam2Goals()) + " TEAM2 ";
	float time_spent;
	if(game->getType() == CREATOR)
		time_spent = game->computeTimeSpent();
	else
		time_spent = game->getTimeSpent();
	//endTime = clock();
	//float time_spent = (float)(endTime - startTime) / CLOCKS_PER_SEC;
	//time_spent = floorf(time_spent * 100) / 100 - 0.40;
	//time_spent *= 100;
	int minutes = ((int)time_spent)/60;
	int seconds = ((int)time_spent)%60;
	if(seconds < 10)
		message += std::to_string(minutes) + ":0" + std::to_string(seconds);
	else
		message += std::to_string(minutes) + ":" + std::to_string(seconds);
	drawText(message,-3.5f, 1.7f);

	if(minutes >= 10)
		game->initiateEndSequence();
}*/

void drawScene()
{
	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)screen_width/(double)screen_height, 1.0, 200.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glOrtho(0.0f, screen, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f);

    /*if(game->getWeather() == STORM)
    {
		static int i = 0;

		if(i % 100 != 0 && i % 102 != 0)
			glColor3f(0.5f, 0.5f, 0.5f);
		else
			glColor3f(1.0f, 1.0f, 1.0f);

		i++;
		if(i > 102) i = 0;
    }
    else if(game->getWeather() == DAY)
    {
    	glColor3f(1.0f, 1.0f, 0.7f);
    }
    else if(game->getWeather() == NIGHT)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
	}

    game->draw();

    showScoreAndTime();*/

    sceneManager.drawScene();

    glutSwapBuffers();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(REFRESH_TIMER, update, 0);
}

int main(int argc, char *argv[])
{
	/*for(int i = 0; i < 7; i++)
		keysPressed[i] = false;

	int choice;
	std::cout << "1. Create game" << std::endl;
	std::cout << "2. Join game" << std::endl;
	std::cout << "Enter choice(1/2): ";
	std::cin >> choice;*/

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow("OpenGL");
	//glutFullScreen();
	initRendering();

	glutDisplayFunc(drawScene);
	/*glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(handleKeyPress);
	glutKeyboardUpFunc(handleKeyUp);
	glutSpecialFunc(handleSpecialInput);
	glutSpecialUpFunc(handleSpecialUpInput);
	glutJoystickFunc(handleJoystickInput, IO_TIMER);*/
	glutReshapeFunc(handleResize);
	//glutTimerFunc(IO_TIMER, handleKeyboardInput, 0);
    glutTimerFunc(REFRESH_TIMER, update, 0);

    /*weather_type weather;
    switch(choice)
    {
    case 1:
    	std::cout << std::endl << "Select condition:" << std::endl;
    	std::cout << "1. Day" << std::endl;
    	std::cout << "2. Night" << std::endl;
    	std::cout << "3. Storm" << std::endl;
    	std::cout << "Enter choice(1-3): ";
    	std::cin >> choice;
    	switch(choice)
    	{
    	case 1:
    		weather = DAY;
    		break;

    	case 2:
    		weather = NIGHT;
    		break;

    	case 3:
    		weather = STORM;
    		break;
    	}
    	game = new Game(atoi(argv[1]), CREATOR, weather);
    	game->startServer();
    	break;

    case 2:
    	game = new Game(atoi(argv[1]), JOINER, DAY);
    	game->startServer();
    	game->setServerAddr();
    	game->query();
    	break;

    default:
    	std::cout << "Incorrect choice..." << std::endl;
    	break;
    }*/

    /*if(argc == 2)
    {

    }
    else if(argc == 3)
    {
    	int teamNo, playerId;
    	char IP[16];
    	cout << "Player team (0/1): ";
    	std::cin >> teamNo;
    	cout << "Player Id (0/1): ";
    	std::cin >> playerId;
    	cout << "Creator IP: ";
    	cin >> IP;
    	game = new Game(atoi(argv[1]), JOINER, teamNo, playerId);
    	game->startServer();
    	game->query(IP,atoi(argv[2]));
    	//game->join(IP, atoi(argv[2]));
    }
    else
    {
    	std::cout << "Incorrect number of arguments." << std::endl;
    	return -1;
    }*/

    sceneManager.loadScene(WELCOME);

	glutMainLoop();

    return 0;
}
