// Gracelove Simons
//
// Debugging Quiz
// 1. reproduce bug
// 2. locate the bad code
// 3. fix the code
// 4. test the fix and the whole program

#include "fonts.h"
#include <stdio.h>
#include <GL/glx.h>
#include <math.h>
void displayGracelove(Rect r)
{
    	r.bot = 250;
	r.left = 300;
	ggprint8b(&r, 50, 0x00ffff00, "Gracelove");
}

void showPicture(GLuint textid, int xoff, int yoff)
{
    int wid = 40;
    
    //float fx = 0.0f;
    //static float angle = 0.0f;

    //angle += 0.01;
    //fx = sin(angle);
    //y = y - 40;

    glPushMatrix();
    //glTranslatef(x + (int)(fx*30.0),y,0);
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, textid);
    glBegin(GL_QUADS);
    	glTexCoord2f(0.0f, 1.0f);	glVertex2i(-wid+xoff, -wid+yoff);
    	glTexCoord2f(0.0f, 0.0f);	glVertex2i(-wid+xoff, wid+yoff);
    	glTexCoord2f(1.0f, 0.0f);	glVertex2i(wid+xoff, wid+yoff);
    	glTexCoord2f(1.0f, 1.0f);	glVertex2i(wid+xoff, -wid+yoff);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void showLogo(GLuint textid2, int widt, int xoff, int yoff)
{
	// show logo
	glBindTexture(GL_TEXTURE_2D, textid2);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);glVertex2i(-widt+xoff, -widt+yoff);
		glTexCoord2f(0.0f, 0.0f);glVertex2i(-widt+xoff, widt+yoff);
		glTexCoord2f(1.0f, 0.0f);glVertex2i(widt+xoff, widt+yoff);
		glTexCoord2f(1.0f, 1.0f);glVertex2i(widt+xoff, -widt+yoff);
	glEnd();

}


void showMenu(Rect r)
{
    	r.bot = 155;
	r.left = 260;
	ggprint8b(&r, 40, 0x00ffff00, "SPACE - Start Game");
	ggprint8b(&r, 40, 0x00ffff00, "     C - Credits  ");
	ggprint8b(&r, 40, 0x00ffff00, "  E - Score Board ");
}

void showPause(Rect r, GLuint textid3, int xres, int yres)
{
        r.bot = yres - 20;
        r.left = 10;
        r.center = 0;
        
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
        ggprint8b(&r, 40, 0x00ffff44, "     R - Restart  ");
}






