#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glx.h>
#include "Image.h"

class Texture {
	public: 
		Image *backImage;
		GLuint backTexture;
		float xc[2];
		float yc[2];
};

#endif
