#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glx.h>
#include "Image.h" 
#include "Global.h" 
#include "../defs.h" 

class Player {
	public:
		int move, currentFrame, frame_count;
		float delay;
		GLuint glTexture;
		Image img;
		Vec pos;
		Vec vel;
		int height;
		int width;
		Player(const char *file);
		Player(const char *file, const Global *g);
};

#endif
