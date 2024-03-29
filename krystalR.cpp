//Name: Krystal Raynes
//hi this is my source code
//--------------------------FOUR STEPS OF DEBUGGING---------------------------//
//1 Identify
//2 Isolate
//3 Fix
//4 Review
//-------------------------------CREDITS BEGIN--------------------------------//

#include "fonts.h"
#include "Global.h"
#include "timers.h"
#include <GL/glx.h>
extern void credits(Rect);

void showCredits(Rect r/*, int x, int y, float tx, float ty, GLuint textid*/) {
    credits(r);
}

void printKrystal(Rect r) {
    r.bot = 100;
    r.left = 310;
    ggprint8b(&r, 50, 0x00ffff00, "Krystal");
}

//-------------------------------CREDITS END----------------------------------//
//--------------------------------STUMP BEGIN---------------------------------//
#include "Image.h"
#include "stump.h"

Stump::Stump(const char *file) {
    img.readImage(file);
    move = 0;
    xoff = 550;
    yoff = 50;
    // this value represents frames in a sprite sheet
    // unique, so adjust according to # of frames in sprite
    //frame_count = 8;
    size_rate = 0.06;
    // using image height and width to determine player ingame dimmensions
    height = img.height*size_rate*0.5;
    width = 32;
    size_with_screen = 0;
    }

Stump::Stump(const char *file, const Global *g) {
    img.readImage(file);
    move = 0;
    xoff = 550;
    yoff = 50;
    /* adjust height and width of character here
       relative to game screen */
    size_rate = 0.075;
    height = g->yres*size_rate*0.5;
    width = g->xres*size_rate*0.5;
    size_with_screen = 1;
}
void Stump::resize(Global *g) {
    if (size_with_screen)
    {
        height = g->yres*size_rate*0.5;
        width = g->xres*size_rate*0.5;
    }	

}
//-----------------------------STUMP END--------------------------------------//

#include "butter.h"

Butter::Butter(const char *file) {
    img.readImage(file);
    move = 0;
    xoff = 550;
    yoff = 125;
    // this value represents frames in a sprite sheet
    // unique, so adjust according to # of frames in sprite
    //frame_count = 8;
    size_rate = 0.06;
    // using image height and width to determine player ingame dimmensions
    height = img.height*size_rate*0.5;
    width = 32;
    size_with_screen = 0;
    }

Butter::Butter(const char *file, const Global *g) {
    img.readImage(file);
    move = 0;
    xoff = 550;
    yoff = 125;
    //adjust height and width of character here relative to game screen
    size_rate = 0.075;
    height = g->yres*size_rate*0.5;
    width = g->xres*size_rate*0.5;
    size_with_screen = 1;
}
void Butter::resize(Global *g) {
    if (size_with_screen)
    {
        height = g->yres*size_rate*0.5;
        width = g->xres*size_rate*0.5;
    }
}
void generateButter(const Global &g, Butter *butter)
{
        Butter *b = butter;
        MakeVector(g.xres+b->width,
                        g.floor.center[1] + g.floor.height + b->height, 0.0,
                        b->pos);
        MakeVector(-b->width*0.4, 0.0, 0.0, b->vel);
}


void moveButter(Butter *butter, const Global &g)
{
        Butter *b = butter;
        if (b->move) {
                b->pos[0] += b->vel[0]; // velocity is initialized to a negative value
                //Check for collision with top of floor 
                if ((b->pos[0] + b->width < 0 && b->vel[0] < 0.0))
                        generateButter(g, b);
        }
}


//--------------------------------P BEGIN-----------------------------------//
//Written by: Gordon Griesel
//Purpose:    Example of HTTP request using SSL.
//            Used to contact a web server and receive a page as text.
//Build:
//  gcc lab3sget.c -Wall -olab3sget -lssl -lcrypto

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 443
#define USERAGENT "CMPS-3350"
const int MAX_READ_ERRORS = 100;


void highScore(char *buf, char *tmpbuf) 
{
    BIO *ssl_setup_bio(void);
    void show_cert_data(SSL *ssl, BIO *outbio, const char *hostname);
    void set_to_non_blocking(const int sock);
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    BIO *outbio = NULL;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    char req[1000];
    int req_len;
    char hostname[256] = "odin.cs.csubak.edu";
    char pagename[256] = "/~bmagana/3350/project/getscores.php";
    int port = PORT;
    int bytes, nreads, nerrs;
    //char buf[256];
    int ret;

    //Setup the SSL BIO
    outbio = ssl_setup_bio();
    //Initialize the SSL library
    if (SSL_library_init() < 0) 
        BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");
    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

    //Setup the socket used for connection.

    host = gethostbyname(hostname);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        BIO_printf(outbio, 
                "%s: Cannot connect to host %s [%s] on port %d.\n", 
                buf, hostname, inet_ntoa(addr.sin_addr), port);
        //hostname, inet_ntoa(addr.sin_addr), port);
    }

    //Connect using the SSL certificate.
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sd);
    SSL_connect(ssl);
    //Show the certificate data
    show_cert_data(ssl, outbio, hostname);
    //A non-blocking socket will make the ssl_read() not block.
    set_to_non_blocking(sd);
    //Send the http request to the host server.
    sprintf(req, "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", 
            pagename, hostname, USERAGENT);
    req_len = strlen(req);
    ret = SSL_write(ssl, req, req_len);
    if (ret <= 0) {
        fprintf(stderr, "ERROR: SSL_write\n");
        fflush(stderr);
    }
    //Get data returned from the server.
    //First, do priming read.
    //We can take this approach because our socket is non-blocking.
    //Start with an error condition.
    bytes = -1;
    memset(tmpbuf, '\0', (int)sizeof(tmpbuf));
    while (bytes <= 0) {
        bytes = SSL_read(ssl, tmpbuf, sizeof(tmpbuf));
        //A slight pause can cause fewer reads to be needed.
        usleep(10000);
    }
    //A successful priming read was accomplished.
    //Now read all the data.
    nreads = 1;
    //Allow for some read errors to happen, while getting the complete data.
    nerrs = 0;
    //zero out the main buf array before we go into reading from ssl
    memset(buf, '\0', (int)sizeof(buf));
    while (bytes >= 0 && nerrs < MAX_READ_ERRORS) {
        //write(STDOUT_FILENO, tmpbuf, bytes);
        memset(tmpbuf, '\0', (int)sizeof(tmpbuf));
        ++nreads;
        bytes = SSL_read(ssl, tmpbuf, (int)sizeof(tmpbuf));
        strcat(buf, tmpbuf);
        if (bytes == 0) ++nerrs; else nerrs = 0;
        //A slight pause can cause fewer reads to be needed.
        usleep(20000);
    }
    printf("\nn calls to ssl_read(): %i\n", nreads); fflush(stdout);
    //Cleanup.
    SSL_free(ssl);
    close(sd);
    SSL_CTX_free(ctx);
}

BIO *ssl_setup_bio(void)
{
    //Setup the ssl BIO, basic I/O abstraction.
    //https://www.openssl.org/docs/man1.1.0/man3/bio.html
    BIO *bio = NULL;
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    bio = BIO_new(BIO_s_file());
    bio = BIO_new_fp(stdout, BIO_NOCLOSE);
    return bio;
}

void show_cert_data(SSL *ssl, BIO *outbio, const char *hostname) 
{
    //Display ssl certificate data here.
    //Get the remote certificate into the X509 structure
    printf("--------------------------------------------------------------\n");
    printf("Certificate data...\n");
    X509 *cert;
    X509_NAME *certname;
    printf("calling SSL_get_peer_certificate(ssl)\n");
    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL)
        printf("Error: Could not get a certificate from: %s.\n", hostname);
    else
        printf("Retrieved the server's certificate from: %s.\n", hostname);
    //extract various certificate information
    certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);
    //display the cert subject here
    if (BIO_printf(outbio, 
                "Displaying the certificate subject data:\n") < 0)
        fprintf(stderr, "ERROR: BIO_printf\n");
    X509_NAME_print_ex(outbio, certname, 0, 0);
    if (BIO_printf(outbio, "\n\n") < 0)
        fprintf(stderr, "ERROR: BIO_printf\n");
    printf("------------------------------------------------------------\n");
}

void set_to_non_blocking(const int sock)
{
    //Set a socket to be non-blocking.
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        perror("ERROR: fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock, F_SETFL, opts) < 0) {
        perror("ERROR: fcntl(O_NONBLOCK)");
        exit(EXIT_FAILURE);
    }
}
//--------------------------------PHP END-------------------------------------//

//-----------------------------RUNNING CLOCK START----------------------------//
#include <unistd.h>
#include <ctime>
#include <cstring>
#include  <sstream>
#include <stdlib.h>
#include <iostream>


void new_clock(Global *g, struct timespec *gameclock)
{
    static int seconds = 0;
    static int minutes  = 0;
    static int hours = 0;
    if (g->time_reset) {
        seconds = 0;
        minutes = 0;
        hours = 0;
        g->time_reset = 0;
    }
    Rect r;
    r.bot = g->yres*0.95;
    r.left = g->xres*0.90;
    struct timespec currentTime;
    recordTime(&currentTime);
    seconds = (int)timeDiff(gameclock, &currentTime);
    if (seconds == 60) {
        recordTime(gameclock);
        seconds = 0;
        minutes++;
    }
    if (minutes == 60) {
        minutes = 0;
        hours++;
    }
    char str[80];
    //int n;
    sprintf (str,"%02i:%02i:%02i", hours, minutes, seconds);
    //system("clear");
    //printf ("time: %s %d\n", str, n);
    ggprint13(&r, 40, 0xDEADAFBB, "%s", str);
	sprintf(g->run_time, "%s", str);
}
//------------------------------RUNNING CLOCK END-----------------------------//
