#ifndef GLOBAL_CPP
#define GLOBAL_CPP

#include "Global.h"
#include "../log.h"

Global::Global()
{
	mainMenu = 1;
	logOpen();
	done=0;
	xres=800; // these next two set window resolution size
	yres=600;
	play = 0;
	showBigfoot=0;
	forest=1;
	silhouette=1;
	trees=1;
	showCredits=0;
	highScore=0;
	
	floor.width = xres; 	  // floor has width of x-axis res
	floor.height = yres*0.04; // height of floor is only 4% of y-axis res
	floor.center[0] = floor.width/2;
	floor.center[1] = floor.height/2;
}

Global::~Global()
{
	logClose();
}

#endif
