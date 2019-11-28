#ifndef GLOBAL_CPP
#define GLOBAL_CPP

#include "Global.h"
#include "../log.h"

Global::Global()
{
	mainMenu = 1;
	logOpen();
	done=0;
	xres=800;
	yres=600;
	play = 0;
	showBigfoot=0;
	forest=1;
	silhouette=1;
	trees=1;
	showCredits=0;
	highScore=0;
}

Global::~Global()
{
	logClose();
}

#endif
