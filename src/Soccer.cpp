/*
 * Soccer.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include "../include/Soccer.h"
#include <iostream>

static const int playerTexSample[PLAYER_TEX] = {39, 39};
static const int ballTexSample = 10;

Soccer::Soccer()
{
	//Load player textures
	for(int i = 0; i < PLAYER_TEX; i++)
	{
		playerTex[i] = new GLuint[playerTexSample[i]];
		for(int j = 0; j < playerTexSample[i]; j++)
		{
			std::string filePath = "../res/player" + std::to_string(i+1) + "/" + std::to_string(j+1) + ".png";
			playerTex[i][j] = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
			(
				filePath.c_str(),
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);
		}
	}

	//Load ball textures
	ballTex = new GLuint[ballTexSample];
	for(int j = 0; j < ballTexSample; j++)
	{
		std::string filePath = "../res/Ball/" + std::to_string(j+1) + ".png";
		ballTex[j] = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load ground texture
	{
		std::string filePath = "../res/ground/1.jpg";
		groundTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load goal texture
	{
		std::string filePath = "../res/goal/1.png";
		goalTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load arrow texture
	{
		std::string filePath = "../res/misc/arrow.png";
		arrowTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load goal texture
	{
		std::string filePath = "../res/misc/goalWord.png";
		goalWordTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load  team won texture
	for(int i = 0; i < 2; i++)
	{
		std::string filePath = "../res/misc/team" + std::to_string(i+1) + "won.png";
		teamWonTex[i] = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	//Load arrow texture
	{
		std::string filePath = "../res/misc/drawMatch.png";
		drawMatchTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	//Load crowd chant
	crowdChant = Mix_LoadMUS("../res/music/crowdChant.mp3");
	if(crowdChant == NULL)
		std::cerr << "Music file could not be opened" << std::endl;

	//Load shoot effect
	shootEffect = Mix_LoadWAV("../res/music/shoot3.wav");

	//Load crowd cheer
	crowdCheer = Mix_LoadWAV("../res/music/crowdCheer.wav");

	//Load whistle
	whistle = Mix_LoadWAV("../res/music/whistle.wav");
}

Soccer::~Soccer()
{
	for(int i = 0; i < PLAYER_TEX; i++)
	{
		for(int j = 0; j < playerTexSample[i]; j++)
			glDeleteTextures(1, &playerTex[i][j]);
	}

	for(int i = 0; i < ballTexSample; i++)
		glDeleteTextures(1, &ballTex[i]);

	glDeleteTextures(1, &groundTex);
	glDeleteTextures(1, &goalTex);
	glDeleteTextures(1, &arrowTex);
	Mix_CloseAudio();
	Mix_FreeChunk(shootEffect);
	Mix_FreeMusic(crowdChant);
}

GLuint *Soccer::getPlayerTex(int player)
{
	return playerTex[player];
}

int Soccer::getTotalPlayerPostures(int player)
{
	return playerTexSample[player];
}

GLuint *Soccer::getBallTex()
{
	return ballTex;
}

int Soccer::getTotalBallPositions()
{
	return ballTexSample;
}

GLuint Soccer::getGroundTex()
{
	return groundTex;
}

GLuint Soccer::getGoalTex()
{
	return goalTex;
}

GLuint Soccer::getArrowTex()
{
	return arrowTex;
}

GLuint Soccer::getGoalWordTex()
{
	return goalWordTex;
}

GLuint *Soccer::getTeamWonTex()
{
	return teamWonTex;
}

GLuint Soccer::getDrawMatchTex()
{
	return drawMatchTex;
}

Mix_Music *Soccer::getCrowdChant()
{
	return crowdChant;
}

Mix_Chunk *Soccer::getShootEffect()
{
	return shootEffect;
}

Mix_Chunk *Soccer::getCrowdCheer()
{
	return crowdCheer;
}

Mix_Chunk *Soccer::getWhistle()
{
	return whistle;
}
