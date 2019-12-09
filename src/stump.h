#ifndef STUMP_H
#define STUMP_H

#include <GL/glx.h>
#include "Image.h" 
#include "Global.h" 
#include "../defs.h" 
class Stump 
{
    public:
        int move, currentFrame; //frame_count;
        float delay;
        GLuint glTexture;
        Image img;
        Vec pos;
        Vec vel;
        float size_rate;
        int height;
        int width;
        int size_with_screen;
        int xoff;
        int yoff;
        Stump(const char *);
        Stump(const char *, const Global *);
        void resize(Global *);

};
#endif
