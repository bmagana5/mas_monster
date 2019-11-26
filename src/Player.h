#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glx.h>
#include "Image.h" 

class Player {
	public:
		int move, currentFrame, frame_count;
		float delay;
		GLuint glTexture;
		Image img;
		Player(const char *file);
};

#endif
