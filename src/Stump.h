#ifndef STUMP_H
#define STUMP_H

#include "Image.h"
#include "Global.h"

class Stump {
	public:
		Image img;
		Vec pos, vel;
		float size_rate;
		int height, width;
		int size_with_screen;
		int move;
		Stump(const char *);
		Stump(const char *, const Global *);
		void resize(Global *);
};

#endif
