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
	SDL_Surface *playerSprite;
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
			/*playerSprite = IMG_Load(filePath.c_str());
			if(playerSprite == NULL)
				std::cerr << "Error loading image '" << filePath << "'" << std::endl;

			glGenTextures(1, &playerTex[i][j]);
			glBindTexture(GL_TEXTURE_2D, playerTex[i][j]);

			int mode;

			switch (playerSprite->format->BytesPerPixel) {
				case 4:
					if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
						mode = GL_BGRA;
					else
						mode = GL_RGBA;
					break;

				case 3:
					if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
						mode = GL_BGR;
					else
						mode = GL_RGB;
					break;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, mode, playerSprite->w, playerSprite->h, 0, mode, GL_UNSIGNED_BYTE, playerSprite->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			SDL_FreeSurface(playerSprite);*/
		}
	}

	//Load ball textures
	SDL_Surface *ballSprite;
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
		/*ballSprite = IMG_Load(filePath.c_str());
		if(ballSprite == NULL)
			std::cerr << "Error loading image '" << filePath << "'" << std::endl;

		glGenTextures(1, &ballTex[j]);
		glBindTexture(GL_TEXTURE_2D, ballTex[j]);

		int mode;

		switch (ballSprite->format->BytesPerPixel) {
			case 4:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					mode = GL_BGRA;
				else
					mode = GL_RGBA;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					mode = GL_BGR;
				else
					mode = GL_RGB;
				break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, mode, ballSprite->w, ballSprite->h, 0, mode, GL_UNSIGNED_BYTE, ballSprite->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(ballSprite);*/
	}

	//Load ground texture
	SDL_Surface *groundSprite;
	for(int j = 0; j < ballTexSample; j++)
	{
		std::string filePath = "../res/ground/1.jpg";
		groundTex = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
		(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
		/*groundSprite = IMG_Load(filePath.c_str());
		if(groundSprite == NULL)
			std::cerr << "Error loading image '" << filePath << "'" << std::endl;

		glGenTextures(1, &groundTex);
		glBindTexture(GL_TEXTURE_2D, groundTex);

		int mode;

		switch (groundSprite->format->BytesPerPixel) {
			case 4:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					mode = GL_BGRA;
				else
					mode = GL_RGBA;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					mode = GL_BGR;
				else
					mode = GL_RGB;
				break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, mode, groundSprite->w, groundSprite->h, 0, mode, GL_UNSIGNED_BYTE, groundSprite->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(groundSprite);*/
	}
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
