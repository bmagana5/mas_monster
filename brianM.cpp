// Brian Magana
// 
// four steps of debugging:
// 	1. identify the bug
// 		- figure out where it's located and why it's happening
// 	2. isolate the bug
// 		- separate buggy code from main code and test it
// 	3. fix it
// 	4. review that the fix is working properly
#include "fonts.h"
#include "timers.h"
#include "Player.h"
#include "Global.h"
#include "stump.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <GL/glx.h>
#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif

void printBriansName(Rect r)
{
    	r.bot = 200;
	r.left = 275;
	ggprint8b(&r, 50, 0x00ffff00, "Brian");
}

void parseScores(Rect r, char *buf, char *tmp)
{
	int i = 0;
	while (i < (int)strlen(buf)) {
		if (buf[i] == '*') {
			i++;
			while (buf[i] != '&') {
				strncat(tmp, &buf[i], 1);
				i++;
			}
			strcat(tmp, "\n");
			ggprint12(&r, 20, 0xffff0000, tmp);
			strcpy(tmp, "");
		} else i++;
	}
}

#ifdef USE_OPENAL_SOUND

void initAudio(char audio[][32], ALuint *alBuffer, ALuint *alSource, int n)
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: alutInit()\n");
		exit(1);
	}
	alGetError();

	//Setup for listener
	float vec[6] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListeneri(AL_GAIN, 1.0f);

	//set up buffers to hold sound info
	for (int i = 0; i < n; i++) {
		alBuffer[i] = 
			alutCreateBufferFromFile(audio[i]);
	}
	// source refers to the sound
	// generate n source, and store in matching buffers.
	alGenSources(n, alSource);
	for (int i = 0; i < n; i++) {
		alSourcei(alSource[i], AL_BUFFER, alBuffer[i]);
	}
	for (int i = 0; i < n; i++) {
		// set volume and pitch to normal
		if (i == 0) {
			alSourcef(alSource[i], AL_GAIN, 0.5f);
			alSourcef(alSource[i], AL_LOOPING, AL_TRUE);
		} else {
			alSourcef(alSource[i], AL_GAIN, 1.0f);
			alSourcef(alSource[i], AL_LOOPING, AL_FALSE);
		}
		alSourcef(alSource[i], AL_PITCH, 1.0f);
		if (alGetError() != AL_NO_ERROR) {
			printf("ERROR: setting source\n");
			exit(1);
		}
	}
	/*for (int i = 1; i < n; i++) {
		alSourcePlay(alSource[i]);
		usleep(500000);
	}*/
}
#endif

#ifdef USE_OPENAL_SOUND
void cleanupAudio(ALuint *alBuffer, ALuint *alSource, int n)
{
	// close out sources
	for (int i = 0; i < n; i++)  
		alDeleteSources(1, &(alSource[i]));
	// delete out buffers
	for (int i = 0; i < n; i++)  
		alDeleteBuffers(1, &(alBuffer[i]));
	// close OpenAL
	ALCcontext *Context = alcGetCurrentContext();
	ALCdevice *Device = alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}
#endif

void startGame(Global &g, Player *player) 
{
	Player *p = player;
    // please handle the next function call with care :)
	MakeVector(g.xres*0.01, 
			g.floor.center[1] + g.floor.height + p->height, 0.0,
			p->pos);
    MakeVector(p->width*0.4, 0.0, 0.0, p->vel);
	p->score = 0;
	g.time_reset = 1; // flag to reset game time
}

void generateObstacle(const Global &g, Stump *stump)
{
	Stump *s = stump;
	MakeVector(g.xres+s->width,
			g.floor.center[1] + g.floor.height + s->height, 0.0,
			s->pos);
	MakeVector(-s->width*0.4, 0.0, 0.0, s->vel);
}

void moveCharacter(Player *player, const Global &g)
{
	int addgrav = 1;
	Player *p = player;
	p->pos[0] += p->vel[0];
	p->pos[1] += p->vel[1];
	//Check for collision with top of floor 
	if ((p->pos[0] > g.xres*0.3 && p->vel[0] > 0.0))
		p->pos[0] = g.xres*0.3;
	if (addgrav) {
		if (p->pos[1] >= g.floor.center[1] + g.floor.height + p->height)
			p->vel[1] -= 1.5;
		else if (p->pos[1] < (g.floor.center[1] + g.floor.height) + p->height) {	
			p->pos[1] = (g.floor.center[1] + g.floor.height) + p->height; 
			if (p->jumping)
				p->jumping = 0;
		}
	}
}

void moveObstacle(Stump *stump, const Global &g)
{
	Stump *s = stump;
	if (s->move) {
		s->pos[0] += s->vel[0]; // velocity is initialized to a negative value
		//Check for collision with top of floor 
		if ((s->pos[0] + s->width < 0 && s->vel[0] < 0.0))
			generateObstacle(g, s);
	}
}

void animateCharacter(Player *player, struct timespec *moveTime, 
		struct timespec *timeCurrent)
{
	Player *p = player;
	recordTime(timeCurrent);
	double timeSpan = timeDiff(moveTime, timeCurrent);
	if (timeSpan > p->delay) {
		++p->currentFrame;
		switch (p->currentFrame % p->frame_count) {
			// 3rd and 7th frame, player foot touches down
			// increment score by one for any of these two frames
			case 3:
			case 7: p->score++;
					break;
			default:
					break;
		}
		if (p->currentFrame >= p->frame_count)
			p->currentFrame -= p->frame_count;
		recordTime(moveTime);
	}
}

void animateSkeleton(Global *g, struct timespec *moveTime) 
{
	static int currentFrame = 0;
	static int totalFrames = 8;
	static float delay = 0.1f;
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
	int height = g->yres/8;
	float cx = g->xres*0.5;
	float cy = g->yres*0.15;
	glBindTexture(GL_TEXTURE_2D, g->skeletonTexture);
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

void checkPlayerCoords(Player *player)
{
	// this will draw characters on the screen to test 
	// collision box and character image alignment
	Player *p = player;
	Rect re;
	int color = 0xffffffff;
	// center
	re.bot = p->pos[1];
	re.left = p->pos[0];
	ggprint12(&re, 10, color, "x");
	// left bottom
	re.bot = p->pos[1] - p->height;
	re.left = p->pos[0] - p->width;
	ggprint12(&re, 10, color, "1");
	// right bottom
	re.bot = p->pos[1] - p->height;
	re.left = p->pos[0] + p->width;
	ggprint12(&re, 10, color, "2");
	// top right
	re.bot = p->pos[1] + p->height;
	re.left = p->pos[0] + p->width;
	ggprint12(&re, 10, color, "3");
	// top left
	re.bot = p->pos[1] + p->height;
	re.left = p->pos[0] - p->width;
	ggprint12(&re, 10, color, "4");
}

void checkObstacleCoords(Stump *stump)
{
	// this will draw characters on the screen to test 
	// collision box and stump image alignment
	Stump *s = stump;
	Rect re;
	int color = 0xffffffff;
	// center
	re.bot = s->pos[1];
	re.left = s->pos[0];
	ggprint12(&re, 10, color, "x");
	// left bottom
	re.bot = s->pos[1] - s->height;
	re.left = s->pos[0] - s->width;
	ggprint12(&re, 10, color, "1");
	// right bottom
	re.bot = s->pos[1] - s->height;
	re.left = s->pos[0] + s->width;
	ggprint12(&re, 10, color, "2");
	// top right
	re.bot = s->pos[1] + s->height;
	re.left = s->pos[0] + s->width;
	ggprint12(&re, 10, color, "3");
	// top left
	re.bot = s->pos[1] + s->height;
	re.left = s->pos[0] - s->width;
	ggprint12(&re, 10, color, "4");
}

void checkFloorCoords(Global *g)
{
	Rect re;
	int color = 0xffffffff;
	// center
	re.bot = g->floor.center[1];
	re.left = g->floor.center[0];
	ggprint8b(&re, 10, color, ".");
	// left bottom
	re.bot = g->floor.center[1] - g->floor.height;
	re.left =g->floor.center[0] - g->floor.width;
	ggprint8b(&re, 10, color, "1");
	// right bottom
	re.bot = g->floor.center[1] - g->floor.height;
	re.left = g->floor.center[0] + g->floor.width;
	ggprint8b(&re, 10, color, "2");
	// top right
	re.bot = g->floor.center[1] + g->floor.height;
	re.left = g->floor.center[0] + g->floor.width;
	ggprint8b(&re, 10, color, "3");
	// top left
	re.bot = g->floor.center[1] + g->floor.height;
	re.left = g->floor.center[0] - g->floor.width;
	ggprint8b(&re, 10, color, "4");
}

void displayScore(const Global &g, Player *p)
{
	Rect r;
	r.bot = g.yres*0.95;
	r.left = g.xres*0.7;
	char string[64];
	sprintf(string, "SCORE: %05d", p->score);
	ggprint13(&r, 40, 0xDEADAFBB, "%s", string);
}

