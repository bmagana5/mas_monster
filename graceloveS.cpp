// Gracelove Simons
//
// Debugging Quiz
// 1. reproduce bug
// 2. locate the bad code
// 3. fix the code
// 4. test the fix and the whole program

#include "fonts.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glx.h>
#include <math.h>
#include "Player.h"
#include "defs.h"
#include "Global.h"
#include "timers.h"
void displayGracelove(Rect r)
{
    	r.bot = 250;
	r.left = 300;
	ggprint8b(&r, 50, 0x00ffff00, "Gracelove");
}

void showPicture(GLuint textid, int xoff, int yoff)
{
    int wid = 40;

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, textid);
    glBegin(GL_QUADS);
    	glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid+xoff, -wid+yoff);
    	glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid+xoff, wid+yoff);
    	glTexCoord2f(1.0f, 0.0f); glVertex2i(wid+xoff, wid+yoff);
    	glTexCoord2f(1.0f, 1.0f); glVertex2i(wid+xoff, -wid+yoff);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    //glDisable(GL_ALPHA_TEST);
}

void showLogo(GLuint textid2, int widt, int xoff, int yoff)
{
	int len = 200;
	// show logo
	glBindTexture(GL_TEXTURE_2D, textid2);
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

/*
void showMenu(Rect r, const Global &g)
{
    	//r.bot = 155;
	//r.left = 260;
	//r.bot = g.yres*0.3;
	//r.left = g.xres*0.4;
	//ggprint8b(&r, 40, 0x00ffff00, "RETURN - Start Game");
	//ggprint8b(&r, 40, 0x00ffff00, "     C - Credits  ");
	//ggprint8b(&r, 40, 0x00ffff00, "  E - Score Board ");
}*/

void showPause(Rect r, GLuint textid3, int xres, int yres)
{
        r.bot = yres - 20;
        r.left = 10;
        r.center = 0;

	glPushMatrix();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, textid3);
        glBegin(GL_QUADS);
        	glTexCoord2f(0.0f, 1.0f); 	glVertex2i(0, 0);
        	glTexCoord2f(0.0f, 0.0f); 	glVertex2i(0, yres);
        	glTexCoord2f(1.0f, 0.0f); 	glVertex2i(xres, yres);
        	glTexCoord2f(1.0f, 1.0f); 	glVertex2i(xres, 0);

        glEnd();

        r.bot = 300;
        r.left = 250;
        ggprint8b(&r, 40, 0x00ffff44, "P - Resume Game");
        ggprint8b(&r, 40, 0x00ffff44, "     R - Restart");
        ggprint8b(&r, 40, 0x00ffff44, " M - Main Menu");
	glPopMatrix();
}

void drawcircle(Vec ob1, float r)
{
	// draw the radius on each image
	// need center: (x,y)
	// radius: r
	// get new points by changing the angle w/ sin and cos
	//
	glBegin(GL_LINE_LOOP);
	int num_segs = 10;
	//float r = 15.0;
	for (int i=0; i<num_segs; i++) {
	    float theta = 2.0f * 3.1415296f * float(i) / float(num_segs);

	    float x = r*cosf(theta);
	    float y = r*sinf(theta);
	    //printf("%f		%f\n", x, y);

	    glVertex2f(x + ob1[0], y + ob1[1]);
	}
	glEnd();
}

bool checkcollision(Vec obj1, float rad1, Vec obj2, float rad2)
{
	int xd, yd, Distance;

	xd = obj2[0]-obj1[0];
	yd = obj2[1]-obj1[1];
	Distance = sqrt(xd*xd + yd*yd);

	if (rad1 + rad2 >= Distance)
		return true;
	else
		return false;
}

void showDied(Rect r, Global *g, int end_score)
{
	r.bot = g->yres*0.65;
	r.left = g->xres*0.45;
	r.center = 0;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g->forestTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);	glVertex2i(0,0);
		glTexCoord2f(0.0f, 0.0f);	glVertex2i(0,g->yres);
		glTexCoord2f(1.0f, 0.0f);	glVertex2i(g->xres,g->yres);
		glTexCoord2f(1.0f, 1.0f);	glVertex2i(g->xres,0);
	glEnd();
	char score[32];
	char time[32];
	sprintf(score, "Score: %d", end_score);
	sprintf(time, "Time: %s", g->run_time);
	ggprint8b(&r, 50, 0x00ffff00, "You Died!!");
	ggprint8b(&r, 50, 0x00ffff00, score);
	ggprint8b(&r, 50, 0x00ffff00, time);
	ggprint8b(&r, 100, 0x00ffff00, "Press X to continue...");
}

void showEndMenu(Rect r, GLuint ftexture, int xres, int yres)
{
	r.bot = yres*0.5;
	r.left = xres*0.5;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, ftexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);	glVertex2i(0,0);
		glTexCoord2f(0.0f, 0.0f);	glVertex2i(0,yres);
		glTexCoord2f(1.0f, 0.0f);	glVertex2i(xres,yres);
		glTexCoord2f(1.0f, 1.0f);	glVertex2i(xres,0);
	glEnd();
	ggprint8b(&r, 50, 0x00ffff00, "Return to Main Menu - M ");
	ggprint8b(&r, 50, 0x00ffff00, "     Restart Game - R   ");

}
