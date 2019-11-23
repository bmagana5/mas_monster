#ifndef OBSTACLE_CPP
#define OBSTACLE_CPP

#include "Obstacle.h"

Obstacle::Obstacle(const char *file)
{
	img.readImage(file);
}

#endif
