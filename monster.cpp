//3350
//program: rainforest.cpp
//author:  Gordon Griesel
//date:    2013 to 2018
//
//This program demonstrates the use of OpenGL and XWindows
//
//Texture maps are displayed.
//Press B to see bigfoot roaming his forest.
//
//This program was edited and completed by:
//Angela Tante
//Brian Magana
//Gracelove Simons
//Krystal Raines
//
// top
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "defs.h"
#include "timers.h"
#include "Image.h"
#include "Player.h"
#include "Obstacle.h"
#include "Global.h"
#include "Texture.h"

#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif

//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define ALPHA 1
//----------------------------------------------------------------------------
//user defined prototypes
extern void showCredits(Rect);
extern void highScore(char *, char *);
extern void parseScores(Rect, char *, char *);
extern void showPicture(GLuint, int, int);
extern void showMenu(Rect);
extern void showLogo(GLuint, int, int, int);
extern void showPause(Rect, GLuint, int, int);
extern void new_clock(Rect);
extern bool checkCollision(int, int, float, int, int, float);
extern void drawcircle(Vec);
extern void moveCharacter(Player *, const Global *);
extern void animateCharacter(Player *, struct timespec *, struct timespec *);
extern void showStump(GLuint, int, int);
extern void showPotato(GLuint, int, int);
extern void showButter(GLuint, int, int);
#ifdef USE_OPENAL_SOUND
extern void initAudio(char (*)[32], ALuint *, ALuint *, int);
extern void cleanupAudio(ALuint *, ALuint *, int);
#endif
//-----------------------------------------------------------------------------
//Setup timers
//clock_gettime(CLOCK_REALTIME, &timePause);
const double physicsRate = 1.0 / 30.0;
double physicsCountdown=0.0;
double timeSpan=0.0;
unsigned int upause=0;
struct timespec timeStart, timeEnd, timeCurrent;
struct timespec timePause, moveTime;
//-----------------------------------------------------------------------------

//define audio globals
#ifdef USE_OPENAL_SOUND
const int TOTAL_SOUNDS = 3;
char audio[TOTAL_SOUNDS][32] = {
	"../audio/monstermash.wav",
	"../audio/bigjump.wav",
	"../audio/smalljump.wav"};
ALuint alBuffer[TOTAL_SOUNDS];
ALuint alSource[TOTAL_SOUNDS];
#endif

Global g;

Image img[15] = {
    "./images/bigfoot.png",
    "./images/creepyforest.jpg",
    "./images/forestTrans.png",
    "./images/umbrella.png",
    "./images/scroll2.jpg",
    "./images/imag3.png",
    "./images/brianpic.gif",
    "./images/krystalPic.png",
    "./images/angelapic.png",
    "./images/monsterDash_logo_blkbg.gif",
    "./images/pixelforest.jpg",
    "./images/blackbox.jpeg",
    "./images/stump.gif",
    "./images/potato.gif",
    "./images/butter.gif"
};

// pass in Global object to use resolution x and y values to
// determine player size in game
//Player player("images/new_drac_run_sprite.gif");
Player player("images/new_drac_run_sprite.gif", &g);

Obstacle ob[3] = {
    "./images/stump.gif",
    "./images/potato.gif",
    "./images/butter.gif"};

GLuint glTexture;
GLuint brTexture;
GLuint krTexture;
GLuint agTexture;
GLuint lgTexture;
GLuint obsTexture;
GLuint stumpTexture;
GLuint potatoTexture;
GLuint butterTexture;

class Bigfoot {
    public:
	Vec pos;
	Vec vel;
} bigfoot;

class Collision {
    public:
	Vec pos;
	//float width;
	//float radius;
} collision, collision1;

class X11_wrapper {
    private:
	Display *dpy;
	Window win;
    public:
	X11_wrapper() {
	    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	    //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	    XSetWindowAttributes swa;
	    setupScreenRes(640, 480);
	    dpy = XOpenDisplay(NULL);
	    if (dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(EXIT_FAILURE);
	    }
	    Window root = DefaultRootWindow(dpy);
	    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	    if (vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(EXIT_FAILURE);
	    } 
	    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	    swa.colormap = cmap;
	    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask;
	    win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
		    vi->depth, InputOutput, vi->visual,
		    CWColormap | CWEventMask, &swa);
	    GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	    glXMakeCurrent(dpy, win, glc);
	    setTitle();
	}
	~X11_wrapper() {
	    XDestroyWindow(dpy, win);
	    XCloseDisplay(dpy);
	}
	void setTitle() {
	    //Set the window title bar.
	    XMapWindow(dpy, win);
	    XStoreName(dpy, win, "MAS - Monster Dash");
	}
	void setupScreenRes(const int w, const int h) {
	    g.xres = w;
	    g.yres = h;
	}
	void reshapeWindow(int width, int height) {
	    //window has been resized.
	    setupScreenRes(width, height);
	    // added the following to resize floor and character
	    player.resize(&g);
	    g.resizeObjects();
	    //
	    glViewport(0, 0, (GLint)width, (GLint)height);
	    glMatrixMode(GL_PROJECTION); glLoadIdentity();
	    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	    glOrtho(0, g.xres, 0, g.yres, -1, 1);
	    setTitle();
	}
	void checkResize(XEvent *e) {
	    //The ConfigureNotify is sent by the
	    //server if the window is resized.
	    if (e->type != ConfigureNotify)
		return;
	    XConfigureEvent xce = e->xconfigure;
	    if (xce.width != g.xres || xce.height != g.yres) {
		//Window size did change.
		reshapeWindow(xce.width, xce.height);
	    }
	}
	bool getXPending() {
	    return XPending(dpy);
	}
	XEvent getXNextEvent() {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    return e;
	}
	void swapBuffers() {
	    glXSwapBuffers(dpy, win);
	}
} x11;

//function prototypes
void initOpengl(void);
void checkMouse(XEvent *e);
int checkKeys(XEvent *e);
void init();
void physics(void);
void render(void);

#ifdef USE_OPENAL_SOUND
void initSounds(void);
#endif

int main()
{
    initOpengl();
    init();
#ifdef USE_OPENAL_SOUND
    initSounds();
#endif
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
    int done = 0;
    while (!done) {
	while (x11.getXPending()) {
	    //XEvent e;
	    //XNextEvent(dpy, &e);
	    XEvent e = x11.getXNextEvent();
	    x11.checkResize(&e);
	    checkMouse(&e);
	    done = checkKeys(&e);
	}
	//
	//Below is a process to apply physics at a consistent rate.
	//1. Get the current time.
	clock_gettime(CLOCK_REALTIME, &timeCurrent);
	//2. How long since we were here last?
	timeSpan = timeDiff(&timeStart, &timeCurrent);
	//3. Save the current time as our new starting time.
	timeCopy(&timeStart, &timeCurrent);
	//4. Add time-span to our countdown amount.
	physicsCountdown += timeSpan;
	//5. Has countdown gone beyond our physics rate? 
	//       if yes,
	//           In a loop...
	//              Apply physics
	//              Reducing countdown by physics-rate.
	//              Break when countdown < physics-rate.
	//       if no,
	//           Apply no physics this frame.
	while (physicsCountdown >= physicsRate) {
	    //6. Apply physics
	    physics();
	    //7. Reduce the countdown by our physics-rate
	    physicsCountdown -= physicsRate;
	}
	//Always render every frame.
	render();
	x11.swapBuffers();
    }
    //cleanupXWindows();
    cleanup_fonts();
#ifdef USE_OPENAL_SOUND
    cleanupAudio(alBuffer, alSource, TOTAL_SOUNDS);
#endif
    return 0;
}

unsigned char *buildAlphaData(Image *img)
{
    //add 4th component to RGB stream...
    int i;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    unsigned char a, b, c;
    unsigned char t0 = *(data+0);
    unsigned char t1 = *(data+1);
    unsigned char t2 = *(data+2);
    for (i=0; i<img->width * img->height * 3; i+=3) {
	a = *(data+0);
	b = *(data+1);
	c = *(data+2);
	*(ptr+0) = a;
	*(ptr+1) = b;
	*(ptr+2) = c;
	//-----------------------------------------------
	//get largest color component...
	//*(ptr+3) = (unsigned char)((
	//		(int)*(ptr+0) +
	//		(int)*(ptr+1) +
	//		(int)*(ptr+2)) / 3);
	//d = a;
	//if (b >= a && b >= c) d = b;
	//if (c >= a && c >= b) d = c;
	//*(ptr+3) = d;
	//-----------------------------------------------
	//this code optimizes the commented code above.
	if (a==t0 && b==t1 && c==t2)
		*(ptr+3) = 0;
	else
		*(ptr+3) = (a|b|c);
	//-----------------------------------------------
	ptr += 4;
	data += 3;
    }
    return newdata;
}


void initOpengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, g.xres, g.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    //
    //load the images file into a ppm structure.
    //
    //	bigfootImage     = ppm6GetImage("./images/bigfoot.ppm");
    //	forestImage      = ppm6GetImage("./images/forest.ppm");
    //	forestTransImage = ppm6GetImage("./images/forestTrans.ppm");
    //	umbrellaImage    = ppm6GetImage("./images/umbrella.ppm");
    //create opegl texture elements
    glGenTextures(1, &g.bigfootTexture);
    glGenTextures(1, &g.silhouetteTexture);
    glGenTextures(1, &g.forestTexture);
    glGenTextures(1, &g.creditsTexture);
    glGenTextures(1, &g.graceloveTexture);
    glGenTextures(1, &g.brianTexture);
    glGenTextures(1, &g.krystalTexture);
    glGenTextures(1, &g.angelaTexture);
    glGenTextures(1, &g.logoTexture);
    glGenTextures(1, &g.tex.backTexture);
    glGenTextures(1, &player.glTexture);
    glGenTextures(1, &g.obsTexture);
    glGenTextures(1, &g.stumpTexture);
    glGenTextures(1, &g.potatoTexture);
    glGenTextures(1, &g.butterTexture);
    //-------------------------------------------------------------------------
    //bigfoot
    //
    int w = img[0].width;
    int h = img[0].height;
    //
    glBindTexture(GL_TEXTURE_2D, g.bigfootTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
	    GL_RGB, GL_UNSIGNED_BYTE, img[0].data);
    //-------------------------------------------------------------------------
    //credits
    glBindTexture(GL_TEXTURE_2D, g.creditsTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
	    img[1].width, img[1].height,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[1].data);
    //------------------------------------------------------------------------
    //
    //silhouette
    //this is similar to a sprite graphic
    //
    glBindTexture(GL_TEXTURE_2D, g.silhouetteTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
    //must build a new set of data...
    unsigned char *silhouetteData = buildAlphaData(&img[0]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
    free(silhouetteData);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
    //	GL_RGB, GL_UNSIGNED_BYTE, bigfootImage->data);
    //-------------------------------------------------------------------------
    //
    //forest
    glBindTexture(GL_TEXTURE_2D, g.forestTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
	    img[1].width, img[1].height,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[1].data);
    //-------------------------------------------------------------------------
    //
    //forest transparent part
    //
    glBindTexture(GL_TEXTURE_2D, g.forestTransTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
    //must build a new set of data...
    w = img[2].width;
    h = img[2].height;
    unsigned char *ftData = buildAlphaData(&img[2]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, ftData);
    free(ftData);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
    //GL_RGB, GL_UNSIGNED_BYTE, bigfootImage->data);
    //-------------------------------------------------------------------------
    // Gracelove
    w = img[5].width;
    h = img[5].height;
    glBindTexture(GL_TEXTURE_2D, g.graceloveTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[5].data);
    glTexture = g.graceloveTexture;
    //-------------------------------------------------------------------------
    //Brian 
    w = img[6].width;
    h = img[6].height;
    glBindTexture(GL_TEXTURE_2D, g.brianTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[6].data);
    brTexture = g.brianTexture;
    //-------------------------------------------------------------------------
    //Krystal 
    w = img[7].width;
    h = img[7].height;
    glBindTexture(GL_TEXTURE_2D, g.krystalTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[7].data);
    krTexture = g.krystalTexture;
    //-------------------------------------------------------------------------
    //Angela 
    w = img[8].width;
    h = img[8].height;
    glBindTexture(GL_TEXTURE_2D, g.angelaTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[8].data);
    agTexture = g.angelaTexture;
    //-------------------------------------------------------------------------
    // logo 
    w = img[9].width;
    h = img[9].height;
    glBindTexture(GL_TEXTURE_2D, g.logoTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    unsigned char *clearLogoData = buildAlphaData(&img[9]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, clearLogoData);
    free(clearLogoData);
    lgTexture = g.logoTexture;
    //-------------------------------------------------------------------------
    // moving background
    g.tex.backImage = &img[10];
    w = g.tex.backImage->width;
    h = g.tex.backImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
		    GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 0.25;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;
    //------------------------------------------------------------------------
    // player character
    w = player.img.width;
    h = player.img.height;
    glBindTexture(GL_TEXTURE_2D, player.glTexture);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //
    // must build a new set of data to include alpha val in rgba...
    unsigned char *playerData = buildAlphaData(&player.img);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		    GL_RGBA, GL_UNSIGNED_BYTE, playerData);
    //------------------------------------------------------------------------
    // stump

    w = 64;
    h = 64;
    glBindTexture(GL_TEXTURE_2D, g.stumpTexture);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // alpha data

    unsigned char *clearStumpData = buildAlphaData(&img[12]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, clearStumpData);
    free(clearStumpData);
    stumpTexture = g.stumpTexture;
    //------------------------------------------------------------------------
    // potato

    w = 32;
    h = 32;
    glBindTexture(GL_TEXTURE_2D, g.potatoTexture);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // alpha data

    unsigned char *clearPotatoData = buildAlphaData(&img[13]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, clearPotatoData);
    free(clearPotatoData);
    potatoTexture = g.potatoTexture;
    //------------------------------------------------------------------------
    // butter

    w = 32;
    h = 32;
    glBindTexture(GL_TEXTURE_2D, g.butterTexture);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // alpha data

    unsigned char *clearButterData = buildAlphaData(&img[14]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, clearButterData);
    free(clearButterData);
    butterTexture = g.butterTexture;
    //------------------------------------------------------------------------
 
    // obstacle 
    w = 10;
    h = 10;
    glBindTexture(GL_TEXTURE_2D, g.obsTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[11].data);
    obsTexture = g.obsTexture;
}

#ifdef USE_OPENAL_SOUND
void initSounds()
{
	initAudio(audio, alBuffer, alSource, TOTAL_SOUNDS);
}
#endif

void init() {
    collision.pos[0] = 100;
    collision.pos[1] = 550;
    collision1.pos[0] = 50;
    collision1.pos[1] = 550;
    // initialize position and velocity of player
    // please handle the next function call with care :)
    MakeVector(g.xres*0.01, g.floor.center[1] + g.floor.height - player.height*2.0*0.85, 
		    0.0, player.pos);
    //MakeVector(g.xres*0.01, g.yres*0.008, 0.0, player.pos);
    MakeVector(6.0,0.0,0.0, player.vel);
}

void checkMouse(XEvent *e)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    static int savex = 0;
    static int savey = 0;
    //
    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button is down
        }
        if (e->xbutton.button==3) {
            //Right button is down
        }
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        //Mouse moved
        savex = e->xbutton.x;
        savey = e->xbutton.y;
    }
}

int checkKeys(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    if (e->type != KeyPress && e->type != KeyRelease)
	return 0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    if (e->type == KeyRelease) {
	if (key == XK_Shift_L || key == XK_Shift_R)
	    shift=0;
	return 0;
    }
    if (key == XK_Shift_L || key == XK_Shift_R) {
	shift=1;
	return 0;
    }

    switch (key) {
	case XK_e:
	    if (!g.play && !g.showCredits && !g.showPauseScreen) {
		if (!g.highScore) {
			highScore(g.buf, g.tmpbuf);
			g.highScore ^= 1;
		} else g.highScore = 0;
		g.forest ^= 1;
	    }	    
	    break;
	case XK_Return:
	    if (!g.play && !g.showPauseScreen && !g.showCredits && !g.highScore) {
		    g.play ^= 1;
		    player.move ^= 1;
		    g.forest ^= 1;
		    recordTime(&moveTime);
	    }
	    break;
	case XK_space:
	    // hardcoded value for now...
	    if (!player.jumping) {
		    player.jumping = 1;
		    player.vel[1] = 18.0;
	    }
	    break;
	case XK_c:
	    if (!g.play && !g.showPauseScreen && !g.highScore) {
	    	g.showCredits ^= 1;
		g.forest ^= 1;
	    }
	    break;
	case XK_d:
	    break;
	case XK_p:
	    if (!g.forest && !g.showCredits && !g.highScore) {
		    g.showPauseScreen ^= 1;
		    g.play ^= 1;
		    player.move ^= 1;
	    }
	    break;
	case XK_s:
	    break;
	case XK_t:
	    break;
	case XK_u:
	    break;
	case XK_f:
	    break;
	case XK_r:
	    if (g.showPauseScreen) {
		    // reset game 
	    }
	    break;
	case XK_Left:
	    break;
	case XK_Right:
	    break;
	case XK_Up:
	    break;
	case XK_Down:
	    break;
	case XK_equal:
	    break;
	case XK_minus:
	    break;
	case XK_n:
	    break;
	case XK_w:
	    break;
	case XK_Escape:
	    return 1;
	    //break;
    }
    return 0;
}

Flt VecNormalize(Vec vec)
{
    Flt len, tlen;
    Flt xlen = vec[0];
    Flt ylen = vec[1];
    Flt zlen = vec[2];
    len = xlen*xlen + ylen*ylen + zlen*zlen;
    if (len == 0.0) {
	MakeVector(0.0,0.0,1.0,vec);
	return 1.0;
    }
    len = sqrt(len);
    tlen = 1.0 / len;
    vec[0] = xlen * tlen;
    vec[1] = ylen * tlen;
    vec[2] = zlen * tlen;
    return(len);
}

void moveBigfoot()
{
	player.pos[0] += player.vel[0];
	//Check for collision with window edges
	if ((player.pos[0] > (g.xres*0.5 - player.img.width / player.frame_count) 
				&& player.vel[0] > 0.0))
	{
		player.pos[0] = g.xres*0.5 - (player.img.width / player.frame_count);
	}
}

void physics()
{
	if (player.move) {
		animateCharacter(&player, &moveTime, &timeCurrent);
		//moveBigfoot();
		moveCharacter(&player, &g);
		if (player.pos[0] == (float)(g.xres*0.5 - player.width*2.0)) {
			for (int i = 0; i < 2; i++) 
				g.tex.xc[i] += 0.003;
		}
	}
}

void render()
{
    Rect r;

    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //
    //draw a quad with texture
    //float wid = 120.0f;
    glColor3f(1.0, 1.0, 1.0);
    if (g.forest) {
	//show forest
	//unsigned int c = 0x00ffff44;
	
	glBindTexture(GL_TEXTURE_2D, g.forestTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);
	glEnd();

	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;

	int widt = 110, xoff = 320, yoff = 300;
	//showLogo
	showLogo(g.logoTexture, widt, xoff, yoff);
	

	//print menu options
	showMenu(r);

    }

    //render play
    if (g.play) {
	//do timer
	new_clock(r);	
	
 	//BACKGROUND GOES HERE
	/*glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0); // white*/
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glBegin(GL_QUADS);
	    glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);	
	    glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);	
	    glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);	
	    glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
	glEnd();
	//show ground
	glBegin(GL_QUADS);
		// dark grey
		glColor3f(0.2, 0.2, 0.2);
		glVertex2i(0, g.floor.height);
		glVertex2i(g.floor.width, g.floor.height);
		// lighter grey
		glColor3f(0.4, 0.4, 0.4);
		glVertex2i(g.floor.width, 0);
		glVertex2i(0, 0);
	glEnd();
	//PLAYER CHARACTER GOES HERE
	
	Player *p = &player;
	// center coordinates
	float cx = g.xres*0.1;
	float cy = g.yres*0.1;
	
	// gave Player obj 'player' height and width member variables
	// no longer need 'h' and 'w'
	//float h = g.yres*0.08;
	//float w = g.xres*0.08;
	
	glPushMatrix();
	glTranslatef(p->pos[0], p->pos[1], p->pos[2]);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, p->glTexture);
	// enable alpha testing
	glEnable(GL_ALPHA_TEST);
	// specify alpha comparison func and referenece value that incoming
	// alpha values are to be compared to
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);
	// denominator corresponds to # of frames
	// player.currentFrame is incremented in physics()
	// this ensures that ix remains between 0 and (player.frame_count - 1)
	int ix = p->currentFrame % p->frame_count;
	int iy = 0;
	if (p->currentFrame >= p->frame_count)
		iy = 1;
	// width percentage for sprite 
	float tx = (float)ix / (float)p->frame_count;
	float ty = (float)iy / 2.0;
	// x_off is percentage that each frame takes up in the sprite sheet
	float x_off = 1.0 / (float)p->frame_count;
	glBegin(GL_QUADS);
		glTexCoord2f(tx, ty+1.0);
		glVertex2i(cx-p->width*2.0, cy-p->height*2.0);
		glTexCoord2f(tx, ty);
		glVertex2i(cx-p->width*2.0, cy+p->height*2.0);
		glTexCoord2f(tx+x_off, ty);
		glVertex2i(cx+p->width*2.0, cy+p->height*2.0);
		glTexCoord2f(tx+x_off, ty+1.0);	
		glVertex2i(cx+p->width*2.0, cy-p->height*2.0);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	showPicture(obsTexture, 550, 100);
	showStump(stumpTexture, 550, 80);
    
    //showPotato(texture, xoff, yoff);
    showPotato(potatoTexture, 550, 50);
    showButter(butterTexture, 0, 0);


	/*player.pos[0] = tx;
	player.pos[1] = ty;
	player.pos[2] = 10;
	drawcircle(player.pos);*/
	//bool collision = checkCollision(100, 550, radius1, tx, ty, radius2);
	/*if (!collision)
	{
		//end game
	}*/
	glDisable(GL_ALPHA_TEST);
    }
    
    //render credits
    if (g.showCredits) {
	//keep trees in background
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
    	glClearColor(1.0, 1.0, 1.0, 1.0);
    	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g.forestTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);
	glEnd();
	
	//show the credits and credit pictures
	showCredits(r); //g.creditsTexture
	showPicture(glTexture, 400, 250);
	showPicture(brTexture, 220, 200);
	showPicture(krTexture, 400, 100);
	showPicture(agTexture, 220, 350);

    }
    if (g.highScore) {
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
    	//glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g.forestTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);
	//r.bot = g.yres - 20;
	//r.left = 10;
	r.center = 0;
	glEnd();
	
	r.bot = 300;
	r.left = 300;
	char tmp[20] = "";
	parseScores(r, g.buf, tmp);
    }
    if (g.showPauseScreen) {
	showPause(r, g.forestTexture, g.xres, g.yres);
    }
    //glDisable(GL_TEXTURE_2D);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);
    //glDisable(GL_BLEND);
    //glEnable(GL_TEXTURE_2D);
    //
    glBindTexture(GL_TEXTURE_2D, 0);
}

