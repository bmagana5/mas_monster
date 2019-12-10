/* Angela Tante
 * CMPS 3350
 *
 * hi this is my file!
*/

#include "fonts.h"
#include <stdio.h>
#include <GL/glx.h>
#include <math.h>
#include "Global.h"
#include <time.h>
#include "timers.h"

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
    int wid = 32;

    static int x = 25;
    float fx = 0.0f;
    fx = 4.2;

    x -= fx;
    yoff = yoff-30;

    glPushMatrix();
    glTranslatef(x - (int)fx, 0, 0);
    glBindTexture(GL_TEXTURE_2D, stumpid);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid+xoff, -wid+yoff); //upper-left
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid+xoff, wid+yoff); //lower-left
    glTexCoord2f(1.0f, 0.0f); glVertex2i(wid+xoff, wid+yoff); //lower-right
    glTexCoord2f(1.0f, 1.0f); glVertex2i(wid+xoff, -wid+yoff); //upper-right
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void showPotato(GLuint potatoTex, int xoff, int yoff)
{
    int wid = 32;

    int x = 0;
    int y = 10;

    glPushMatrix();
    //glTranslatef(0, 100.0, 0);
    float fy = 0.0f;
    static float angle = 0.0f;

    angle += 0.05;
    fy = sin(angle);
    //y = y -40;

    glPushMatrix();
    glTranslatef(x, y + (int)(fy*10.0), 0);

    glBindTexture(GL_TEXTURE_2D, potatoTex);
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

void showButter(GLuint butterTex, int xoff, int yoff)
{
    int wid = 32;
    int x = 0;
    int y = 10;

    glPushMatrix();
    //glTranslatef(0, 100.0, 0);
    float fy = 0.0f;
    static float angle = 0.0f;

    angle += 0.05;
    fy = sin(angle);
    //y = y -40;

    glPushMatrix();
    glTranslatef(x, y + (int)(fy*10.0), 0);

    glBindTexture(GL_TEXTURE_2D, butterTex);
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

void dancingDrac (Global *g, struct timespec *moveTime)
{
  static int currentFrame = 0;
	static int totalFrames = 2;
	static float delay = 0.2f;
	struct timespec currentTime;
	recordTime(&currentTime);
	double timespan = timeDiff(moveTime, &currentTime);
	if (timespan > delay) {
		currentFrame++;
		if (currentFrame >= totalFrames)
			currentFrame -= totalFrames;
		recordTime(moveTime);
	}
	int width = g->xres/8;
	int height = g->yres/7;
	float cx = g->xres*0.20;
	float cy = g->yres*0.20;
	glBindTexture(GL_TEXTURE_2D, g->dancingDracTexture);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);
	int ix = currentFrame % totalFrames;
	int iy = 0;
	float tx = (float)ix / (float)totalFrames;
	float ty = (float)iy / 2.0;
	float x_off = 1.0 / (float)totalFrames;
	glBegin(GL_QUADS);
	glTexCoord2f(tx, ty+1.0);
	glVertex2i(cx-width, cy-height);
	glTexCoord2f(tx, ty);
	glVertex2i(cx-width, cy+height);
	glTexCoord2f(tx+x_off, ty);
	glVertex2i(cx+width, cy+height);
	glTexCoord2f(tx+x_off, ty+1.0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glVertex2i(cx+width, cy-height);
	glEnd();
}

void showStartTitle(GLuint startid, int xoff, int yoff)
{
    int widt = 16;
    int len = 90;

    glBindTexture(GL_TEXTURE_2D, startid);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f); glVertex2i(-len+xoff, -widt+yoff);
      glTexCoord2f(0.0f, 0.0f); glVertex2i(-len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 0.0f); glVertex2i(len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 1.0f); glVertex2i(len+xoff, -widt+yoff);
    glEnd();
    glDisable(GL_ALPHA_TEST);
}

void showCreditsTitle(GLuint creditsid, int xoff, int yoff)
{
    int widt = 16;
    int len = 70;

    glBindTexture(GL_TEXTURE_2D, creditsid);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f); glVertex2i(-len+xoff, -widt+yoff);
      glTexCoord2f(0.0f, 0.0f); glVertex2i(-len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 0.0f); glVertex2i(len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 1.0f); glVertex2i(len+xoff, -widt+yoff);
    glEnd();
    glDisable(GL_ALPHA_TEST);
}

void showScoreTitle(GLuint creditsid, int xoff, int yoff)
{
    int widt = 16;
    int len = 97;

    glBindTexture(GL_TEXTURE_2D, creditsid);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f); glVertex2i(-len+xoff, -widt+yoff);
      glTexCoord2f(0.0f, 0.0f); glVertex2i(-len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 0.0f); glVertex2i(len+xoff, widt+yoff);
      glTexCoord2f(1.0f, 1.0f); glVertex2i(len+xoff, -widt+yoff);
    glEnd();
    glDisable(GL_ALPHA_TEST);
}

void startButton(Global *g, struct timespec *moveTime)
{
  static int currentFrame = 0;
	static int totalFrames = 2;
	static float delay = 0.5f;
	struct timespec currentTime;
	recordTime(&currentTime);
	double timespan = timeDiff(moveTime, &currentTime);
	if (timespan > delay) {
		currentFrame++;
		if (currentFrame >= totalFrames)
			currentFrame -= totalFrames;
		recordTime(moveTime);
	}
	int width = g->xres/8;
	int height = g->yres/7;
	float cx = g->xres*0.20;
	float cy = g->yres*0.20;
	glBindTexture(GL_TEXTURE_2D, g->enterButtonTexture);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);
	int ix = currentFrame % totalFrames;
	int iy = 0;
	float tx = (float)ix / (float)totalFrames;
	float ty = (float)iy / 2.0;
	float x_off = 1.0 / (float)totalFrames;
	glBegin(GL_QUADS);
	glTexCoord2f(tx, ty+1.0);
	glVertex2i(cx-width, cy-height);
	glTexCoord2f(tx, ty);
	glVertex2i(cx-width, cy+height);
	glTexCoord2f(tx+x_off, ty);
	glVertex2i(cx+width, cy+height);
	glTexCoord2f(tx+x_off, ty+1.0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glVertex2i(cx+width, cy-height);
	glEnd();
}
