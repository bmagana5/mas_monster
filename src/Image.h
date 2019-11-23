#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

class Image {
	public:
		int width, height;
		unsigned char *data;
		~Image();
		Image();
		Image(const char *fname);
		void readImage(const char *fname);
};

#endif
