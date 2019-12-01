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
		float size_rate;
		int height;
		int width;
		int size_with_screen;
		Player(const char *file);
		Player(const char *file, const Global *g);
		void resize(Global *g);
};

#endif
