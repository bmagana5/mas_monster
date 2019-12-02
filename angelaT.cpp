/* Angela Tante
 * CMPS 3350
 *
 * hi this is my file!
*/

#include "fonts.h"
#include <stdio.h>
#include <GL/glx.h>
#include <math.h>

void printAngela(Rect r)
{
    	r.bot = 340;
	r.left = 275;
	ggprint8b(&r, 50, 0x00ffff00, "Angela");
}

/*
void showAngelaPicture (int x, int y, float tx, float ty, GLuint picid) 
{
    int wid = 40;
    float fx = 0.0f;
    static float angle = 0.0f;

    angle += 0.01;
    fx = sin(angle);
    y = y -40;

    glPushMatrix();
    glTranslatef(x + (int)(fx*30.0), y, 0);
    glColor3s(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, picid);
    glBegin(GL_QUADS);
        glTexCoord2f(tx, ty+0.5); glVertex2i(-wid, -wid);
        glTexCoord2f(tx, ty); glVertex2i(-wid, wid);
        glTexCoord2f(tx+0.125, ty); glVertex2i(wid, wid);
        glTexCoord2f(tx+0.125, ty+0.5); glVertex2i(wid, -wid);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}
*/
/* Quiz Debugging 
 *
 * 1. Recognize that the bug exists 
 * 2. Isolate the source of the bug
 * 3. Identify the cause of the bug
 * 4. Determine the fix for the bug
 * 5. Apply the fix then test your program.
 *
*/
/*
void showNewLogo(GLuint logoid, int widt, int xoff, int yoff)
{
    glBindTexture(GL_Texture_2D, logoid);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(-widt+xoff, -widt+yoff);
}*/

void showStump(GLuint stumpid, int xoff, int yoff)
{
    int wid = 64;

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, stumpid);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid+xoff, -wid+yoff);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid+xoff, wid+yoff);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(wid+xoff, wid+yoff);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(wid+xoff, -wid+yoff);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}
