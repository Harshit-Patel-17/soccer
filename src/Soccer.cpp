/*
 * Soccer.cpp
 *
 *  Created on: 23-Oct-2015
 *      Author: harshit
 */

#include "../include/Soccer.h"
#include <iostream>

static const int playerTexSample[PLAYER_TEX] = {39, 14};

Soccer::Soccer()
{
	SDL_Surface *playerSprite;
	for(int i = 0; i < PLAYER_TEX; i++)
	{
		playerTex[i] = new GLuint[playerTexSample[i]];
		for(int j = 0; j < playerTexSample[i]; j++)
		{
			std::string filePath = "../res/player" + std::to_string(i+1) + "/" + std::to_string(j+1) + ".png";
			playerSprite = IMG_Load(filePath.c_str());
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

			std::cout << (int)playerSprite->format->BytesPerPixel << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, mode, playerSprite->w, playerSprite->h, 0, mode, GL_UNSIGNED_BYTE, playerSprite->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			SDL_FreeSurface(playerSprite);
		}
	}
}

Soccer::~Soccer()
{
	// TODO Auto-generated destructor stub
}

GLuint *Soccer::getPlayerTex(int player)
{
	return playerTex[player];
}

int Soccer::getTotalPostures(int player)
{
	return playerTexSample[player];
}
