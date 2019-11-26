#ifndef PLAYER_CPP
#define PLAYER_CPP

#include "Player.h"
#include "Image.h"

Player::Player(const char *file)
{
	img.readImage(file);
	move = 0;
	currentFrame = 0;
	// this value represents frames in a sprite sheet
	// unique, so adjust according to each sprite
	frame_count = 8;
	delay = 0.1;
}

#endif
