/*
 * main.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include "../include/Game.h"

using namespace std;

#define SCREEN_WIDTH 1024*2
#define SCREEN_HEIGHT 768*2
#define TIMER 50

Game *game;

bool keysPressed[4];

void handleSpecialUpInput(int key,int x,int y)
{
	switch(key)
	{
		case GLUT_KEY_RIGHT:
			keysPressed[0] = false;
			break;

		case GLUT_KEY_LEFT:
			keysPressed[1] = false;
			break;

		case GLUT_KEY_UP:
			keysPressed[2] = false;
			break;

		case GLUT_KEY_DOWN:
			keysPressed[3] = false;
			break;
	}
}

void handleSpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_RIGHT:
        	keysPressed[0] = true;
        	if(keysPressed[2])
        	{
        		game->movePlayer(45);
        		//player->setAngle(45);
        		//player->moveForward();
        	}
        	else if(keysPressed[3])
        	{
        		game->movePlayer(315);
        		//player->setAngle(315);
        		//player->moveForward();
        	}
        	else
        	{
        		game->movePlayer(0);
        		//player->setAngle(0);
        		//player->moveForward();
        	}
            break;

        case GLUT_KEY_LEFT:
        	keysPressed[1] = true;
        	if(keysPressed[2])
			{
        		game->movePlayer(135);
				//player->setAngle(135);
				//player->moveForward();
			}
			else if(keysPressed[3])
			{
				game->movePlayer(225);
				//player->setAngle(225);
				//player->moveForward();
			}
			else
			{
				game->movePlayer(180);
				//player->setAngle(180);
				//player->moveForward();
			}
            break;

        case GLUT_KEY_UP:
        	keysPressed[2] = true;
        	if(keysPressed[0])
			{
        		game->movePlayer(45);
				//player->setAngle(45);
				//player->moveForward();
			}
			else if(keysPressed[1])
			{
				game->movePlayer(135);
				//player->setAngle(135);
				//player->moveForward();
			}
			else
			{
				game->movePlayer(90);
				//player->setAngle(90);
				//player->moveForward();
			}
            break;

        case GLUT_KEY_DOWN:
        	keysPressed[3] = true;
        	if(keysPressed[0])
			{
        		game->movePlayer(315);
				//player->setAngle(315);
				//player->moveForward();
			}
			else if(keysPressed[1])
			{
				game->movePlayer(225);
				//player->setAngle(225);
				//player->moveForward();
			}
			else
			{
				game->movePlayer(270);
				//player->setAngle(270);
				//player->moveForward();
			}
            break;
    }


}

void handleKeyPress(unsigned char key, int x, int y)
{
	switch(key)
	{
        case 27:
            exit(0);
            break;

        case 32:
        	game->shoot();
        	break;
	}
}

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
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w/(double)h, 1.0, 200.0);
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f);

    game->draw();

    glutSwapBuffers();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER, update, 0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow("OpenGL");
	initRendering();

	for(int i=0;i<4;i++) keysPressed[i] = false;

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeyPress);
	glutSpecialFunc(handleSpecialInput);
	glutSpecialUpFunc(handleSpecialUpInput);
	glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER, update, 0);

    //soccer = new Soccer();
    //ground = new Ground(GROUND_WIDTH, GROUND_HEIGHT, 0, 0, soccer);
    //ball = new Ball(GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground);
    //player = new Player(0, 2, GROUND_WIDTH/2, GROUND_HEIGHT/2, 0, soccer, ground, ball);
    //ball->hit(4, -0.3, 0);

    if(argc == 2)
    {
    	game = new Game("127.0.0.1", atoi(argv[1]), CREATOR, 0, 0);
    }
    else if(argc == 3)
    {
    	game = new Game("127.0.0.1", atoi(argv[1]), JOINER, 0, 1);
    	game->join("127.0.0.1", atoi(argv[2]));
    }
    else
    {
    	std::cout << "Incorrect number of arguments." << std::endl;
    	return -1;
    }

	glutMainLoop();

    return 0;
}
