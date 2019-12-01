#ifndef PLAYER_CPP
#define PLAYER_CPP

#include "Player.h"
#include "Image.h"
#include "Global.h"

Player::Player(const char *file)
{
	img.readImage(file);
	move = 0;
	currentFrame = 0;
	// this value represents frames in a sprite sheet
	// unique, so adjust according to # of frames in sprite
	frame_count = 8;
	delay = 0.1;
	// using image height and width to determine player ingame dimmensions
	height = img.height*0.6;
	width = img.width / frame_count;
	size_with_screen = 0;
	/*xpos = ;
	ypos = ;*/
}

Player::Player(const char *file, const Global *g)
{
	img.readImage(file);
	move = 0;
	currentFrame = 0;
	// this value represents frames in a sprite sheet
	// unique, so adjust according to # of frames in sprite
	frame_count = 8;
	delay = 0.1;
	/* adjust height and width of character here
	 relative to game screen */
	size_rate = 0.06;
	height = g->yres*size_rate;
	width = g->xres*size_rate;
	size_with_screen = 1;
	/*xpos = ;
	ypos = ;*/
}

void Player::resize(Global *g)
{
	if (size_with_screen)
	{
		height = g->yres*size_rate;
		width = g->xres*size_rate;
	}
}
#endif
