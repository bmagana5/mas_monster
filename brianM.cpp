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

void moveCharacter(Player *player, const Global *g)
{
	int addgrav = 1;
	Player *p = player;
	p->pos[0] += p->vel[0];
	p->pos[1] += p->vel[1];
	//Check for collision with window edges
	if ((p->pos[0] > (g->xres*0.5 - p->img.width / p->frame_count)
				&& p->vel[0] > 0.0))
		p->pos[0] = g->xres*0.5 - (p->img.width / p->frame_count);
	if (addgrav)
		p->vel[1] -= 0.75;
}

void animateCharacter(Player *player, struct timespec *moveTime, 
		struct timespec *timeCurrent)
{
	recordTime(timeCurrent);
	double timeSpan = timeDiff(moveTime, timeCurrent);
	if (timeSpan > player->delay) {
		++player->currentFrame;
		if (player->currentFrame >= player->frame_count)
			player->currentFrame -= player->frame_count;
		recordTime(moveTime);
	}
}
