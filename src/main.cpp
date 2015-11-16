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

void initRendering()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.87f, 0.53f, 0.16f, 1.0f);
}

void handleResize(int w, int h)
{
	screen_width = w;
	screen_height = h;
}

void drawScene()
{
	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)screen_width/(double)screen_height, 1.0, 200.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow("OpenGL");
	//glutFullScreen();
	initRendering();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
    glutTimerFunc(REFRESH_TIMER, update, 0);

    weather_type weather = NIGHT;
    if(argc == 2)
    {
    	if(strcmp(argv[1], "storm") == 0)
    		weather = STORM;
    	else if(strcmp(argv[1], "day") == 0)
    		weather = DAY;
    	else if(strcmp(argv[1], "night") == 0)
    	    weather = NIGHT;
    }

    sceneManager.setGameWeather(weather);
    sceneManager.loadScene(WELCOME);

	glutMainLoop();

    return 0;
}
