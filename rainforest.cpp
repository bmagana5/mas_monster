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
//
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
#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif
#include "log.h"
//#include "ppm.h"
#include "fonts.h"

//defined types
typedef double Flt;
typedef double Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
			     (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
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
#ifdef USE_OPENAL_SOUND
extern void initAudio(char (*)[32], ALuint *, ALuint *, int);
extern void cleanupAudio(ALuint *, ALuint *, int);
#endif
//-----------------------------------------------------------------------------
//Setup timers
//clock_gettime(CLOCK_REALTIME, &timePause);
const double physicsRate = 1.0 / 30.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeEnd, timeCurrent;
struct timespec timePause, moveTime;
double physicsCountdown=0.0;
double timeSpan=0.0;
unsigned int upause=0;
double timeDiff(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec ) +
	(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
    memcpy(dest, source, sizeof(struct timespec));
}
void recordTime(struct timespec *t) {
	clock_gettime(CLOCK_REALTIME, t);	
}
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


class Image {
    public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(){}
	Image(const char *fname) {
		readImage(fname);
	}
	void readImage(const char *fname) {
	    if (fname[0] == '\0')
		return;
	    //printf("fname **%s**\n", fname);
	    int ppmFlag = 0;
	    char name[40];
	    strcpy(name, fname);
	    int slen = strlen(name);
	    char ppmname[80];
	    if (strncmp(name+(slen-4), ".ppm", 4) == 0)
		ppmFlag = 1;
	    if (ppmFlag) {
		strcpy(ppmname, name);
	    } else {
		name[slen-4] = '\0';
		//printf("name **%s**\n", name);
		sprintf(ppmname,"%s.ppm", name);
		//printf("ppmname **%s**\n", ppmname);
		char ts[100];
		//system("convert eball.jpg eball.ppm");
		sprintf(ts, "convert %s %s", fname, ppmname);
		system(ts);
	    }
	    //sprintf(ts, "%s", name);
	    FILE *fpi = fopen(ppmname, "r");
	    if (fpi) {
		char line[200];
		fgets(line, 200, fpi);
		fgets(line, 200, fpi);
		//skip comments and blank lines
		while (line[0] == '#' || strlen(line) < 2)
		    fgets(line, 200, fpi);
		sscanf(line, "%i %i", &width, &height);
		fgets(line, 200, fpi);
		//get pixel data
		int n = width * height * 3;			
		data = new unsigned char[n];			
		for (int i=0; i<n; i++)
		    data[i] = fgetc(fpi);
		fclose(fpi);
	    } else {
		printf("ERROR opening image: %s\n",ppmname);
		exit(0);
	    }
	    if (!ppmFlag)
		unlink(ppmname);
	}
};
Image img[11] = {
    "./images/bigfoot.png",
    "./images/creepyforest.jpg",
    "./images/forestTrans.png",
    "./images/umbrella.png",
    "./images/scroll2.jpg",
    "./images/imag3.png",
    "./images/brianpic.gif",
    "./images/krystalPic.png",
    "./images/angelapic.png",
    "./images/monsterDash2.png",
    "./images/pixelforest.jpg"};

class Player {
	public:
		int move, currentFrame, frame_count;
		float delay;
		GLuint glTexture;
		Image img;
		Player(const char *file)
		{
			img.readImage(file);
			move = 0;
			currentFrame = 0;
			// this value represents frames in spritesheet; unique
			frame_count = 8;
			delay = 0.1;
		}
};
Player player("images/drac_run_spriteSheet.gif");

class Texture {
	public:
		Image *backImage;
		GLuint backTexture;
		float xc[2];
		float yc[2];
};

class Global {
    public:
	int done;
	int xres, yres;

	Texture tex;
	//names of texutres
	GLuint bigfootTexture;
	GLuint silhouetteTexture;
	GLuint forestTexture;
	GLuint forestTransTexture;
	GLuint umbrellaTexture;
	GLuint creditsTexture;
	GLuint graceloveTexture;
	GLuint brianTexture;
	GLuint krystalTexture;
	GLuint angelaTexture;
	GLuint logoTexture;
	int showBigfoot;
	int forest;
	int silhouette;
	int trees;
	int showRain;
	int showUmbrella;
	int deflection;
	int showCredits;
	int highScore;
	int showPauseScreen;
	int  mainMenu;
	char buf[2048];
	char tmpbuf[256];
	Global() {
	    mainMenu = 1;
	    logOpen();
	    done=0;
	    xres=800;
	    yres=600;
	    showBigfoot=0;
	    forest=1;
	    silhouette=1;
	    trees=1;
	    showRain=0;
	    showUmbrella=0;
	    deflection=0;
	    showCredits=0;
	    highScore=0;
	}
	~Global() {
	    logClose();
	}
} g;

class Bigfoot {
    public:
	Vec pos;
	Vec vel;
} bigfoot;

class Raindrop {
    public:
	int type;
	int linewidth;
	int sound;
	Vec pos;
	Vec lastpos;
	Vec vel;
	Vec maxvel;
	Vec force;
	float length;
	float color[4];
	Raindrop *prev;
	Raindrop *next;
} *rainhead = NULL;
int ndrops=1;
int totrain=0;
int maxrain=0;
void deleteRain(Raindrop *node);
void cleanupRaindrops(void);
//
#define UMBRELLA_FLAT  0
#define UMBRELLA_ROUND 1
class Umbrella {
    public:
	int shape;
	Vec pos;
	Vec lastpos;
	float width;
	float width2;
	float radius;
} umbrella;

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
	    XStoreName(dpy, win, "3350 - Animation Template");
	}
	void setupScreenRes(const int w, const int h) {
	    g.xres = w;
	    g.yres = h;
	}
	void reshapeWindow(int width, int height) {
	    //window has been resized.
	    setupScreenRes(width, height);
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
#ifdef USE_OPENAL_SOUND
void initSounds(void);
#endif
void checkMouse(XEvent *e);
int checkKeys(XEvent *e);
void init();
void physics(void);
void render(void);


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

GLuint glTexture;
GLuint brTexture;
GLuint krTexture;
GLuint agTexture;
GLuint lgTexture;

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
    //create opengl texture elements
    glGenTextures(1, &g.bigfootTexture);
    glGenTextures(1, &g.silhouetteTexture);
    glGenTextures(1, &g.forestTexture);
    glGenTextures(1, &g.umbrellaTexture);
    glGenTextures(1, &g.creditsTexture);
    glGenTextures(1, &g.graceloveTexture);
    glGenTextures(1, &g.brianTexture);
    glGenTextures(1, &g.krystalTexture);
    glGenTextures(1, &g.angelaTexture);
    glGenTextures(1, &g.logoTexture);
    glGenTextures(1, &g.tex.backTexture);
    glGenTextures(1, &player.glTexture);
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
	    0, GL_RGB, GL_UNSIGNED_BYTE, img[5].data);
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
    //umbrella
    //
    glBindTexture(GL_TEXTURE_2D, g.umbrellaTexture);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
    //must build a new set of data...
    silhouetteData = buildAlphaData(&img[3]);
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
    unsigned char *ltData = buildAlphaData(&img[9]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, ltData);
    free(ltData);
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
}

#ifdef USE_OPENAL_SOUND
void initSounds()
{
	initAudio(audio, alBuffer, alSource, TOTAL_SOUNDS);
}
#endif

void init() {
    umbrella.pos[0] = 220.0;
    umbrella.pos[1] = (double)(g.yres-200);
    VecCopy(umbrella.pos, umbrella.lastpos);
    umbrella.width = 200.0;
    umbrella.width2 = umbrella.width * 0.5;
    umbrella.radius = (float)umbrella.width2;
    umbrella.shape = UMBRELLA_FLAT;
    MakeVector(-150.0,180.0,0.0, bigfoot.pos);
    MakeVector(6.0,0.0,0.0, bigfoot.vel);
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
	    if (!g.highScore) {
		highScore(g.buf, g.tmpbuf);
		g.highScore ^= 1;
	    } else {
		g.highScore = 0;
	    }	    
	    break;
	case XK_space:
	    recordTime(&moveTime);
	    g.showBigfoot ^= 1;
	    player.move ^= 1;
	    /*if (g.showBigfoot) {
		bigfoot.pos[0] = -250.0;
	    }*/
	    break;
	case XK_c:
	    g.showCredits ^= 1;
	    break;
	case XK_d:
	    g.deflection ^= 1;
	    break;
	case XK_p:
	    g.showPauseScreen ^= 1;
	    break;
	case XK_s:
	    g.silhouette ^= 1;
	    printf("silhouette: %i\n", g.silhouette);
	    break;
	case XK_t:
	    g.trees ^= 1;
	    break;
	case XK_u:
	    g.showUmbrella ^= 1;
	    break;
	case XK_f:
	    umbrella.shape ^= 1;
	    break;
	case XK_r:
	    g.showRain ^= 1;
	    break;
	case XK_Left:
	    VecCopy(umbrella.pos, umbrella.lastpos);
	    umbrella.pos[0] -= 10.0;
	    break;
	case XK_Right:
	    VecCopy(umbrella.pos, umbrella.lastpos);
	    umbrella.pos[0] += 10.0;
	    break;
	case XK_Up:
	    VecCopy(umbrella.pos, umbrella.lastpos);
	    umbrella.pos[1] += 10.0;
	    break;
	case XK_Down:
	    VecCopy(umbrella.pos, umbrella.lastpos);
	    umbrella.pos[1] -= 10.0;
	    break;
	case XK_equal:
	    if (++ndrops > 40)
		ndrops=40;
	    break;
	case XK_minus:
	    if (--ndrops < 0)
		ndrops = 0;
	    break;
	case XK_n:
	    break;
	case XK_w:
	    if (shift) {
		//shrink the umbrella
		umbrella.width *= (1.0 / 1.05);
	    } else {
		//enlarge the umbrella
		umbrella.width *= 1.05;
	    }
	    //half the width
	    umbrella.width2 = umbrella.width * 0.5;
	    umbrella.radius = (float)umbrella.width2;
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

void cleanupRaindrops()
{
    Raindrop *s;
    while (rainhead) {
	s = rainhead->next;
	free(rainhead);
	rainhead = s;
    }
    rainhead=NULL;
}

void deleteRain(Raindrop *node)
{
    //remove a node from linked list
    //Log("deleteRain()...\n");
    if (node->prev == NULL) {
	if (node->next == NULL) {
	    //Log("only 1 item in list.\n");
	    rainhead = NULL;
	} else {
	    //Log("at beginning of list.\n");
	    node->next->prev = NULL;
	    rainhead = node->next;
	}
    } else {
	if (node->next == NULL) {
	    //Log("at end of list.\n");
	    node->prev->next = NULL;
	} else {
	    //Log("in middle of list.\n");
	    node->prev->next = node->next;
	    node->next->prev = node->prev;
	}
    }
    free(node);
    node = NULL;
}

void moveBigfoot()
{
    //move bigfoot...
    int addgrav = 1;
    //Update position
    bigfoot.pos[0] += bigfoot.vel[0];
    bigfoot.pos[1] += bigfoot.vel[1];
    //Check for collision with window edges
    if ((bigfoot.pos[0] < -140.0 && bigfoot.vel[0] < 0.0) ||
	    (bigfoot.pos[0] >= (float)g.xres+140.0 &&
	     bigfoot.vel[0] > 0.0))
    {
	bigfoot.vel[0] = -bigfoot.vel[0];
	addgrav = 0;
    }
    if ((bigfoot.pos[1] < 150.0 && bigfoot.vel[1] < 0.0) ||
	    (bigfoot.pos[1] >= (float)g.yres && bigfoot.vel[1] > 0.0)) {
	bigfoot.vel[1] = -bigfoot.vel[1];
	addgrav = 0;
    }
    //Gravity?
    if (addgrav)
	bigfoot.vel[1] -= 0.75;
}


void createRaindrop(const int n)
{
    //create new rain drops...
    int i;
    for (i=0; i<n; i++) {
	Raindrop *node = (Raindrop *)malloc(sizeof(Raindrop));
	if (node == NULL) {
	    Log("error allocating node.\n");
	    exit(EXIT_FAILURE);
	}
	node->prev = NULL;
	node->next = NULL;
	node->sound=0;
	node->pos[0] = rnd() * (float)g.xres;
	node->pos[1] = rnd() * 100.0f + (float)g.yres;
	VecCopy(node->pos, node->lastpos);
	node->vel[0] = 
	    node->vel[1] = 0.0f;
	node->color[0] = rnd() * 0.2f + 0.8f;
	node->color[1] = rnd() * 0.2f + 0.8f;
	node->color[2] = rnd() * 0.2f + 0.8f;
	node->color[3] = rnd() * 0.5f + 0.3f; //alpha
	node->linewidth = random(8)+1;
	//larger linewidth = faster speed
	node->maxvel[1] = (float)(node->linewidth*16);
	node->length = node->maxvel[1] * 0.2f + rnd();
	//put raindrop into linked list
	node->next = rainhead;
	if (rainhead != NULL)
	    rainhead->prev = node;
	rainhead = node;
	++totrain;
    }
}

void checkRaindrops()
{
    if (random(100) < 50) {
	createRaindrop(ndrops);
    }
    //
    //move rain droplets
    Raindrop *node = rainhead;
    while (node) {
	//force is toward the ground
	node->vel[1] += gravity;
	VecCopy(node->pos, node->lastpos);
	node->pos[0] += node->vel[0] * timeslice;
	node->pos[1] += node->vel[1] * timeslice;
	if (fabs(node->vel[1]) > node->maxvel[1])
	    node->vel[1] *= 0.96;
	node->vel[0] *= 0.999;
	node = node->next;
    }
    //}
    //
    //check rain droplets
int n=0;
node = rainhead;
while (node) {
    n++;
#ifdef USE_SOUND
    if (node->pos[1] < 0.0f) {
	//raindrop hit ground
	if (!node->sound && play_sounds) {
	    //small chance that a sound will play
	    int r = random(50);
	    if (r==1) {
		//play sound here...


	    }
	    //sound plays once per raindrop
	    node->sound=1;
	}
    }
#endif //USE_SOUND
    //collision detection for raindrop on umbrella
    if (g.showUmbrella) {
	if (umbrella.shape == UMBRELLA_FLAT) {
	    if (node->pos[0] >= (umbrella.pos[0] - umbrella.width2) &&
		    node->pos[0] <= (umbrella.pos[0] + umbrella.width2)) {
		if (node->lastpos[1] > umbrella.lastpos[1] ||
			node->lastpos[1] > umbrella.pos[1]) {
		    if (node->pos[1] <= umbrella.pos[1] ||
			    node->pos[1] <= umbrella.lastpos[1]) {
			if (node->linewidth > 1) {
			    Raindrop *savenode = node->next;
			    deleteRain(node);
			    node = savenode;
			    continue;
			}
		    }
		}
	    }
	}
	if (umbrella.shape == UMBRELLA_ROUND) {
	    float d0 = node->pos[0] - umbrella.pos[0];
	    float d1 = node->pos[1] - umbrella.pos[1];
	    float distance = sqrt((d0*d0)+(d1*d1));
	    //Log("distance: %f  umbrella.radius: %f\n",
	    //	stance,umbrella.radius);
	    if (distance <= umbrella.radius &&
		    node->pos[1] > umbrella.pos[1]) {
		if (node->linewidth > 1) {
		    if (g.deflection) {
			//deflect raindrop
			double dot;
			Vec v, up = {0,1,0};
			VecSub(node->pos, umbrella.pos, v);
			VecNormalize(v);
			node->pos[0] =
			    umbrella.pos[0] + v[0] * umbrella.radius;
			node->pos[1] =
			    umbrella.pos[1] + v[1] * umbrella.radius;
			dot = VecDot(v,up);
			dot += 1.0;
			node->vel[0] += v[0] * dot * 1.0;
			node->vel[1] += v[1] * dot * 1.0;
		    } else {
			Raindrop *savenode = node->next;
			deleteRain(node);
			node = savenode;
			continue;
		    }
		}
	    }
	}
    }
    if (node->pos[1] < -20.0f) {
	//rain drop is below the visible area
	Raindrop *savenode = node->next;
	deleteRain(node);
	node = savenode;
	continue;
    }
    node = node->next;
}
if (maxrain < n)
    maxrain = n;
    }

void physics()
{
	if (g.showBigfoot) {
		moveBigfoot();
		for (int i = 0; i < 2; i++) 
			g.tex.xc[i] += 0.003;
	}
	if (g.showRain)
		checkRaindrops();
	if (player.move) {
		// enable character move
		recordTime(&timeCurrent);
		double timeSpan = timeDiff(&moveTime, &timeCurrent);
		if (timeSpan > player.delay) {
			++player.currentFrame;
			if (player.currentFrame >= player.frame_count)
				player.currentFrame -= player.frame_count;
			recordTime(&moveTime);
		}
	}
}

void drawUmbrella()
{
    //Log("drawUmbrella()...\n");
    if (umbrella.shape == UMBRELLA_FLAT) {
	glColor4f(1.0f, 0.2f, 0.2f, 0.5f);
	glLineWidth(8);
	glBegin(GL_LINES);
	glVertex2f(umbrella.pos[0]-umbrella.width2, umbrella.pos[1]);
	glVertex2f(umbrella.pos[0]+umbrella.width2, umbrella.pos[1]);
	glEnd();
	glLineWidth(1);
    } else {
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glPushMatrix();
	glTranslatef(umbrella.pos[0],umbrella.pos[1],umbrella.pos[2]);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glBindTexture(GL_TEXTURE_2D, g.umbrellaTexture);
	glBegin(GL_QUADS);
	float w = umbrella.width2;
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();
    }
}

void drawRaindrops()
{
    Raindrop *node = rainhead;
    while (node) {
	glPushMatrix();
	glTranslated(node->pos[0],node->pos[1],node->pos[2]);
	glColor4fv(node->color);
	glLineWidth(node->linewidth);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, node->length);
	glEnd();
	glPopMatrix();
	node = node->next;
    }
    glLineWidth(1);
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
	unsigned int c = 0x00ffff44;
	
	glBindTexture(GL_TEXTURE_2D, g.forestTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;

	glEnd();
	int widt = 80, xoff = 320, yoff = 300;
	//showLogo
	glBindTexture(GL_TEXTURE_2D, g.logoTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-widt+xoff, -widt+yoff);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-widt+xoff, widt+yoff);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(widt+xoff, widt+yoff);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(widt+xoff, -widt+yoff);
	glEnd();

	//print menu options
	ggprint8b(&r, 40, c, "SPACE - Start Game");
	ggprint8b(&r, 40, c, "   C - Credits");
	ggprint8b(&r, 40, c, "    P - Pause");
	ggprint8b(&r, 40, c, "E - Score Board");


    }

    if (g.showBigfoot) {
	//glClearColor(1.0, 1.0, 1.0, 1.0);
    	//glClear(GL_COLOR_BUFFER_BIT);
	
 	//BACKGROUND GOES HERE
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);	
		glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);	
		glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);	
		glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
	glEnd();

	//PLAYER CHARACTER GOES HERE
	//glClearColor(0.1, 0.1, 0.1, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	
	float cx = g.xres*0.13;
	float cy = g.yres*0.13;
	//show ground
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.2);
		glVertex2i(0, g.yres*0.05);
		glVertex2i(g.xres, g.yres*0.05);
		glColor3f(0.4, 0.4, 0.4);
		glVertex2i(g.xres, 0);
		glVertex2i(0, 0);
	glEnd();
	float h = g.yres*0.08;
	float w = g.xres*0.08;
	//float h = player.img.height;
	//float w = player.img.width / player.frame_count;
	//
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, player.glTexture);
	//
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);
	// denominator corresponds to # of frames
	// player.currentFrame is incremented in physics()
	// this ensures that ix remains between 0 and (player.frame_count - 1)
	int ix = player.currentFrame % player.frame_count;
	int iy = 0;
	if (player.currentFrame >= player.frame_count)
		iy = 1;
	float tx = (float)ix / (float)player.frame_count;
	float ty = (float)iy / 2.0;
	// x_off is percentage that each frame takes up in the sprite sheet
	float x_off = 1.0 / (float)player.frame_count;
	glBegin(GL_QUADS);
		glTexCoord2f(tx, ty+1.0);	glVertex2i(cx-w, cy-h);
		glTexCoord2f(tx, ty);		glVertex2i(cx-w, cy+h);
		glTexCoord2f(tx+x_off, ty);	glVertex2i(cx+w, cy+h);
		glTexCoord2f(tx+x_off, ty+1.0);	glVertex2i(cx+w, cy-h);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	/*glPushMatrix();
	//glTranslatef(bigfoot.pos[0], bigfoot.pos[1], bigfoot.pos[2]);
	if (!g.silhouette) {
	    glBindTexture(GL_TEXTURE_2D, g.bigfootTexture);
	} else {
	    glBindTexture(GL_TEXTURE_2D, g.silhouetteTexture);
	    glEnable(GL_ALPHA_TEST);
	    glAlphaFunc(GL_GREATER, 0.0f);
	    glColor4ub(255,255,255,255);
	}
	glBegin(GL_QUADS);
	if (bigfoot.vel[0] > 0.0) {
	    glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid,-wid);
	    glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid, wid);
	    glTexCoord2f(1.0f, 0.0f); glVertex2i( wid, wid);
	    glTexCoord2f(1.0f, 1.0f); glVertex2i( wid,-wid);
	} else {
	    glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid,-wid);
	    glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid, wid);
	    glTexCoord2f(0.0f, 0.0f); glVertex2i( wid, wid);
	    glTexCoord2f(0.0f, 1.0f); glVertex2i( wid,-wid);
	}
	glEnd();
	glPopMatrix();*/
	//
	/*if (g.trees && g.silhouette) {
	    glBindTexture(GL_TEXTURE_2D, g.forestTransTexture);
	    glBegin(GL_QUADS);
	    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
	    glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
	    glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);
	    glEnd();
	}*/
	glDisable(GL_ALPHA_TEST);
    }
    //dumb bitch edit
    if(g.showCredits)
    {
	//keep trees in background
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
    	glClearColor(1.0, 1.0, 1.0, 1.0);
    	//glClearColor(1.0, 1.0, 1.0, 1.0);
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
    if (g.highScore)
    {
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
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
	glEnd();
	
	r.bot = 300;
	r.left = 300;
	char tmp[20] = "";
	parseScores(r, g.buf, tmp);
    }
    if (g.showPauseScreen)
    {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
    	//glClearColor(1.0, 1.0, 1.0, 1.0);
    	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g.forestTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, g.yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, g.yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, 0);

	glEnd();
	r.bot = 300;
	r.left = 300;
	ggprint8b(&r, 40, 0x00ffff44, "P - Resume Game");
	ggprint8b(&r, 40, 0x00ffff44, "  R - Restart");
    }
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    if (g.showRain)
	drawRaindrops();
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    //
    if (g.showUmbrella)
	drawUmbrella();
    glBindTexture(GL_TEXTURE_2D, 0);
}

