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
	time_reset = 0;
	showBigfoot=0;
	forest=1;
	showCredits=0;
	highScore=0;
	
	floor.center[0] = xres*0.5;
	floor.center[1] = yres*0.02;
	floor.width = floor.center[0]; // floor width is half of x-axis res
	floor.height = floor.center[1]; // height of floor will be 4% of y-axis res
	
}

Global::~Global()
{
	logClose();
}

void Global::resizeObjects()
{
	floor.center[0] = xres*0.5;
	floor.center[1] = yres*0.02;
	floor.width = floor.center[0]; // floor width is half of x-axis res
	floor.height = floor.center[1]; // height of floor will be 4% of y-axis res
}
#endif
