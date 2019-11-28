#ifndef GLOBAL_H
#define GLOBAL_H

#include <GL/glx.h>
#include "Texture.h"

class Global {
	public: 
		int done;
		int xres, yres;

		Texture tex;
		//names of texutres
		GLuint bigfootTexture;
		GLuint silhouetteTexture;
		GLuint forestTexture;
		GLuint forestTransTexture;
		GLuint creditsTexture;
		GLuint graceloveTexture;
		GLuint brianTexture;
		GLuint krystalTexture;
		GLuint angelaTexture;
		GLuint logoTexture;
		GLuint obsTexture;
		int play;
		int showBigfoot;
		int forest;
		int silhouette;
		int trees;
		int showCredits;
		int highScore;
		int showPauseScreen;
		int  mainMenu;
		char buf[2048];
		char tmpbuf[256];
		Global();
		~Global();
};

#endif
