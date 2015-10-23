/*
 * main.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include "../include/Player.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TIMER 50

Soccer *soccer;
Player *player;

void handleSpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_RIGHT:
            player->setAngle(0);
            player->moveForward();
            break;

        case GLUT_KEY_LEFT:
            player->setAngle(180);
            player->moveForward();
            break;

        case GLUT_KEY_UP:
            player->setAngle(90);
            player->moveForward();
            break;

        case GLUT_KEY_DOWN:
            player->setAngle(270);
            player->moveForward();
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
	}
}

void initRendering()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	//glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
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
    //glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f);
    glTranslatef(-50.0f, -50.0f, -150.0f);

    player->draw();

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow("OpenGL");
	initRendering();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeyPress);
	glutSpecialFunc(handleSpecialInput);
	glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER, update, 0);

    soccer = new Soccer();
    player = new Player(2, 10, 10, 0, soccer);

	glutMainLoop();

    return 0;
}
