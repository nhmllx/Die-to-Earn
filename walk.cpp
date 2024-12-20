//3350
//program: walk.cpp
//author:  Gordon Griesel
//date:    summer 2017 - 2018
//Updated by Company 6
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <ctime>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include <ctime>
#include <thread>

extern void restartProgram(const char *programName);
extern void make_particles(float, float);
extern void update_particles();
extern void render_particles();
extern void make_fireworks(float, float);
extern void update_fireworks();
extern void scroll(float val[]);
extern void render2(float x[], float y[], GLuint bt, int xres,int yres);
extern void render3(float x[], float y[], GLuint bt, int xres,int yres);
extern void make_ammo(float, float);
extern void ammo_pos();
extern void f_render(GLuint, GLuint, GLuint);
extern void enemyAnimate(void);
extern void enemyRender(GLuint);
extern void enemyKiller();
extern void bossRender(GLuint);
extern void HealthRender(GLuint);
extern void fuelRender(GLuint);
extern void speedometerRender(GLuint);
extern int beam_flag;
extern clock_t beam_start_time; 
extern int beam_cooldown; 
extern void f_collisions();
extern void updateCbox(float, float);
extern void check_kill_count();
extern int count;
extern float pspeed;
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



class Image {
    public:
        int width, height;
        unsigned char *data;
        ~Image() { delete [] data; }
        Image(const char *fname) {
            if (fname[0] == '\0')
                return;
            char name[40];
            strcpy(name, fname);
            int slen = strlen(name);
            name[slen-4] = '\0';
            char ppmname[80];
            sprintf(ppmname,"%s.ppm", name);
            char ts[100];
            sprintf(ts, "convert %s %s", fname, ppmname);
            system(ts);
            FILE *fpi = fopen(ppmname, "r");
            if (fpi) {
                char line[200];
                fgets(line, 200, fpi);
                fgets(line, 200, fpi);
                while (line[0] == '#')
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
            unlink(ppmname);
        }
};
Image img[14] = {"images/walk.gif","images/bg.png","images/wastelands.png",
    "images/car_move.png", "images/bomber.png", "images/enemy.png", 
    "images/speedometer.png", "images/beam.png", "images/health.png", 
    "images/fuel.png", "images/tank.png", "images/hearts1.png", 
    "images/ded.png", "./images/particle_tex3.png"};


//------------------------------------------------------
//Setup timers
class Timers {
    public:
        double physicsRate;
        double oobillion;
        struct timespec timeStart, timeEnd, timeCurrent;
        struct timespec walkTime;
        Timers() {
            physicsRate = 1.0 / 30.0;
            oobillion = 1.0 / 1e9;
        }
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
} timers;
//------------------------------------------------------
class Texture {
    public:
        Image *deadImage;
        GLuint deadTexture;
        float xa[2];
        float ya[2];
        Image *bgImage;
        GLuint bgTexture;
        float xb[2];
        float yb[2];
        Image *backImage;
        GLuint backTexture;
        float xc[2];
        float yc[2];
};
class Global {
    public:
        int done;
        int xres, yres;
        int walk;
        int walkFrame;
        int flag;
        char keys[ 0xffff ];
        double delay;
        Texture tex; //title
        Texture dead;
        GLuint walkTexture; //holds data for car sprites
        GLuint bulletTex;
        GLuint enemyTex;
        GLuint speedoTex;
        GLuint beamTex;
        GLuint healthTex;
        GLuint fuelTex;
        GLuint bossTex;
        GLuint heartsTex;
        GLuint particleTex;
        Vec box[20];
        Global() {
            memset(keys, 0, 0xffff);
            done=0;
            xres=1200;
            yres=800;

            flag = 1;
            walk=0;
            walkFrame=0;

            delay = 0.01;
            for (int i=0; i<20; i++) {
                box[i][0] = rnd() * xres;
                box[i][1] = rnd() * (yres-220) + 220.0;
                box[i][2] = 0.0;
            }
        }
} g;

class X11_wrapper {
    private:
        Display *dpy;
        Window win;
    public:
        X11_wrapper() {
            GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24,
                GLX_DOUBLEBUFFER, None };
            XSetWindowAttributes swa;
            setupScreenRes(g.xres, g.yres);
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
            Colormap cmap = XCreateColormap
                (dpy, root, vi->visual, AllocNone);
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
            XStoreName(dpy, win, "Walk Cycle");
        }
        void setupScreenRes(const int w, const int h) {
            g.xres = w;
            g.yres = h;
        }
        void reshapeWindow(int width, int height) {
            //window has been resized.
            setupScreenRes(width, height);
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
void render2(void);
void render3(void);
void update_hearts(int);

float hearts_frame = 0.0f;
float frame_w = 1.0f/15.0f;
float frames[5];
int hearts = 1;
int started = 0;
float current_speed = 0.00030f;

int enemy_kill_count = 0;
int kills_needed = 1000;  
int complete = 0;
int death_flag = 0;
int bullets_active = 0;

float cx = g.xres/4; //xpos of car
float cy = g.yres/3.5; // ypos of car
int main(int argc, char *argv[])
{
    (void)argc;

    frames[0] = hearts_frame;
    frames[1] = frame_w * 4;
    frames[2] = frame_w * 8;
    frames[3] = frame_w * 11;
    frames[4] = frame_w * 14;

    initOpengl();
    init();
    int done = 0;
    while (!done) {

        usleep(4000);
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.checkResize(&e);
            checkMouse(&e);
            done = checkKeys(&e);
            if (done == 2) {
                restartProgram(argv[0]);
            }

        }
        if (g.flag == 1 && !death_flag) 
        {
            render2(g.tex.xc, g.tex.yc, g.tex.backTexture, g.xres, g.yres);
        }
        if (g.flag == 0 && !death_flag) {

            started = 1;
            f_collisions();
            enemyAnimate();//nmalleaux.cpp
            updateCbox(cx, cy);

            if (bullets_active) 
                 make_ammo(cx + 20, cy);
            
            physics();
            render();
        }

        if(death_flag) {

            Rect r;

            render3(g.tex.xc, g.tex.yc, g.tex.deadTexture, g.xres, g.yres);

            char buf[100];
            unsigned int c = 0x00ff0000;

            r.bot = (g.yres/2) + 150;
            r.left = (g.xres/2) + 500;
            r.center = 0;

            glPushMatrix();\

                glTranslatef(g.xres / 2 + 500, g.yres / 2 + 150, 0);
            glScalef(1.7, 1.7, 1.0);
            glTranslatef(-g.xres / 2 - 500, -g.yres / 2 - 150, 0); 

            ggprint8b(&r, 16, c, "Total Kills");
            sprintf(buf, "     %d     ", enemy_kill_count);
            ggprint8b(&r, 16, c, buf);

            glPopMatrix();
        }
        x11.swapBuffers();
    }
    cleanup_fonts();
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
    unsigned char a,b,c;
    //use the first pixel in the image as the transparent color.
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
        *(ptr+3) = 1;
        if (a==t0 && b==t1 && c==t2)
            *(ptr+3) = 0;
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
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
    //load the images file into a ppm structure.
    //title screen------------------------------------------
    g.tex.backImage = &img[2];
    //create opengl texture elements
    glGenTextures(1, &g.tex.backTexture);
    int ww = g.tex.backImage->width;
    int hh = g.tex.backImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, ww, hh, 0,
            GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 1.0;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;
    // death screen--------------------------------------------
    g.tex.deadImage = &img[12];
    //create opengl texture elements
    glGenTextures(1, &g.tex.deadTexture);
    int www = g.tex.deadImage->width;
    int hhh = g.tex.deadImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.deadTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, www, hhh, 0,
            GL_RGB, GL_UNSIGNED_BYTE, g.tex.deadImage->data);
    g.dead.xa[0] = 0.0;
    g.dead.xa[1] = 1.0;
    g.dead.ya[0] = 0.0;
    g.dead.ya[1] = 1.0;
    //background-------------------------------------
    g.tex.bgImage = &img[1];
    //create opengl texture elements
    glGenTextures(1, &g.tex.bgTexture);
    int bw = g.tex.bgImage->width;
    int bh = g.tex.bgImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.bgTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, bw, bh, 0,
            GL_RGB, GL_UNSIGNED_BYTE, g.tex.bgImage->data);
    g.tex.xb[0] = 0.0;
    g.tex.xb[1] = 1.0;
    g.tex.yb[0] = 0.0;
    g.tex.yb[1] = 1.0;
    //--------------------------------------------------------------
    int w = img[3].width;
    int h = img[3].height;
    //create opengl texture elements
    glGenTextures(1, &g.walkTexture); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.walkTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *walkData = buildAlphaData(&img[3]);//car moving	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, walkData);

    //---------------------------------------------------------

    w = img[4].width;
    h = img[4].height;
    //
    //create opengl texture elements
    glGenTextures(1, &g.bulletTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.bulletTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *Tex = buildAlphaData(&img[4]);//The Bullets	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, Tex);
    //-----------------------------------------------------
    w = img[5].width;
    h = img[5].height;
    //create opengl texture elements
    glGenTextures(1, &g.enemyTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.enemyTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *enemydata = buildAlphaData(&img[5]);//The Enemy	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, enemydata);
    //-----------------------------------------------------------------
    w = img[6].width;
    h = img[6].height;
    //create opengl texture elements
    glGenTextures(1, &g.speedoTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.speedoTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *speedodata = buildAlphaData(&img[6]);//The speedometer	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, speedodata);

    \
        w = img[7].width;
    h = img[7].height;

    glGenTextures(1, &g.beamTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.beamTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *bTex = buildAlphaData(&img[7]);//The Bullets	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, bTex);
    //health---------------------------------
    w = img[8].width;
    h = img[8].height;

    glGenTextures(1, &g.healthTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.healthTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *hedata = buildAlphaData(&img[8]);//The heart
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, hedata);
    w = img[9].width;
    h = img[9].height;

    glGenTextures(1, &g.fuelTex); 
    //silhouette
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.fuelTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *fdata = buildAlphaData(&img[9]);//The heart
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, fdata);
    //------------------------------------------------------
    w = img[10].width;
    h = img[10].height;
    //create opengl texture elements
    glGenTextures(1, &g.bossTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.bossTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *bossdata = buildAlphaData(&img[10]);//The Boss	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, bossdata);

    w = img[11].width;
    h = img[11].height;

    glGenTextures(1, &g.heartsTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.heartsTex);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
    //must build a new set of data...
    unsigned char *hTex = buildAlphaData(&img[11]);//The Bullets	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, hTex);

    
    w = img[13].width;
    h = img[13].height;

    glGenTextures(1, &g.particleTex); 
    //this is similar to a sprite graphic
    glBindTexture(GL_TEXTURE_2D, g.particleTex);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
    //must build a new set of data...
    unsigned char *pTex = buildAlphaData(&img[13]);//The Bullets	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pTex);
}

void init() {

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
            //printf("look: %i\n", savex);
            // render();

            g.flag = 0;
            if (complete) {
                g.flag = 1;
                complete = 0;
            }

            //Left button is down
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
}
int keyf = 0;

float tem_frames[3];
int f = 0;
int particle_tex = 1;



int checkKeys(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    if (e->type != KeyRelease && e->type != KeyPress)
        return 0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    if (e->type == KeyRelease) {
        if (key == XK_Shift_L || key == XK_Shift_R)
            shift = 0;
        return 0;
    }

    if (key == XK_Shift_L || key == XK_Shift_R) {
        shift=1;
        return 0;
    }
    (void)shift;
    if (e->type == KeyPress)   g.keys[key] = 1; 
    if (e->type == KeyRelease) g.keys[key] = 0;
    if (e->type == KeyPress) {
        switch (key) {
            case XK_w:
                timers.recordTime(&timers.walkTime);
                g.walk ^= 1;
                break;
            case XK_Left: 

                if (!(cx <= 125))
                    cx -= 60;

                break;
            case XK_Right:

                if (!(cx >= (g.xres - 125)))
                    cx += 60;

                break;
            case XK_Up:
                keyf = 1;
                cy = 228;
                g.delay -= pspeed;
                if (g.delay < 0.005)
                    g.delay = 0.005;
                break;
            case XK_Return:
                g.flag = 0;
                break;
            case XK_Down:
                keyf = 1;
                cy = 105;
                g.delay -= pspeed;
                if (g.delay < 0.005)
                    g.delay = 0.005;
                break;
            case XK_s: 
                if (!beam_flag)
                    bullets_active = !bullets_active;
                    //make_ammo(cx + 20, cy);
                break;
            case XK_p: 
                // enemyKiller();
                    particle_tex = !particle_tex;
                break;
            case XK_d:

                if (beam_cooldown == 0 && beam_flag == 0) {
                    beam_flag = 1;
                    beam_start_time = clock();
                    bullets_active = 0;
                }

                break;
            case XK_equal:
                g.delay -= 0.005;
                if (g.delay < 0.005)
                    g.delay = 0.005;
                break;
            case XK_minus:
                g.delay += 0.005;
                break;
            case XK_z:
                return(2);
                break;
        }
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

void physics(void)
{
    if (keyf == 1) {
        timers.recordTime(&timers.timeCurrent);
        double timeSpan = timers.timeDiff
            (&timers.walkTime, &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance
            ++g.walkFrame;
            if (g.walkFrame >= 16)
                g.walkFrame -= 16;
            timers.recordTime(&timers.walkTime);
        }
        for (int i=0; i<20; i++) {
            g.box[i][0] -= 2.0 * (0.5 / g.delay);
            if (g.box[i][0] < -10.0)
                g.box[i][0] += g.xres + 10.0;
        }
    }

    if (keyf == 0) {
        g.delay += 0.0005;
        // }
}

update_particles();
ammo_pos();
}

//above check_keys()
float camerax = 0.0f;

float color_delay = 0.045f;
clock_t last_color_time = 0; 
unsigned int a = rand();

void render()
{
    Rect r;
    char buf[100];

    if (enemy_kill_count == kills_needed) 
        complete = 1;


    check_kill_count();

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(255, 255, 255);
    glBindTexture(GL_TEXTURE_2D, g.tex.bgTexture);
    //static float camerax = 0.0f;
    glBegin(GL_QUADS);
    glTexCoord2f(camerax+0, 1); glVertex2i(0,      0);
    glTexCoord2f(camerax+0, 0); glVertex2i(0,      g.yres);
    glTexCoord2f(camerax+1, 0); glVertex2i(g.xres, g.yres);
    glTexCoord2f(camerax+1, 1); glVertex2i(g.xres, 0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    if (started){
        camerax += current_speed;
    }

    float h = 76.0f;
    float w = 101.0f;
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.walkTexture);
    //
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);

    int ix = g.walkFrame % 3; // Get the current sprite frame

    // Calculate texture coordinates based on the current frame
    float tx = (float)(ix * w) / 303.0f; // Adjust 
    float ty = 0.0f; // Only one row, so ty is always 0

    glBegin(GL_QUADS);
    glTexCoord2f(tx, ty+1.0f); glVertex2i(cx - w, cy - h); // Top-left
    glTexCoord2f(tx, ty);  glVertex2i(cx - w, cy + h); // Bottom-left
    glTexCoord2f(tx + 1.0f / 3.0f, ty); glVertex2i(cx + w, cy + h); 
    glTexCoord2f(tx + 1.0f / 3.0f, ty + 1.0f); 
    glVertex2i(cx + w, cy - h); // Top-right
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    //hearts 
    float posOffset = cy + 30.0;
    float tw = 150.0f;
    float th = 160.0f;

    glBindTexture(GL_TEXTURE_2D, g.heartsTex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glColor3f(1.0, 1.0, 1.0); // Set color to white 

    float t1 = hearts_frame;
    float t2 = hearts_frame + frame_w;

    glPushMatrix();
    glTranslatef(cx - 75, posOffset , 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(t1, 1.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(t2, 1.0f); glVertex2f(tw, 0.0f);
    glTexCoord2f(t2, 0.0f); glVertex2f(tw, th);
    glTexCoord2f(t1, 0.0f); glVertex2f(0.0f, th);
    glEnd();
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    unsigned int c = 0x00ffff44;
    r.bot = g.yres - 20;
    r.left = 10;
    r.center = 0;

    ggprint8b(&r, 16, c, "S   Bullets");
    ggprint8b(&r, 16, c, "D   Beam shot");
    ggprint8b(&r, 16, c, "up arrow: accelerate");
    ggprint8b(&r, 16, c, "Movement: Arrow keys");

    c = 0x00ffff44;
    r.bot = (g.yres / 2) + 150;
    r.left = (g.xres / 2) + 500;
    r.center = 0;


    glPushMatrix();
    glTranslatef(g.xres / 2 + 500, g.yres / 2 + 150, 0); 
    glScalef(1.7, 1.7, 1.0); // Scale up by a size of 3
    glTranslatef(-g.xres / 2 - 500, -g.yres / 2 - 150, 0); 

    // Render the text
    ggprint8b(&r, 16, c, "Kill Count ");
    sprintf(buf, "%d / %d", enemy_kill_count, kills_needed);
    ggprint8b(&r, 16, c, buf);

    glPopMatrix();


    if (complete) {

        clock_t current_time = clock();
        float elapsed_time = 
	    float(current_time - last_color_time) / CLOCKS_PER_SEC;

        if (elapsed_time > color_delay) {

            a = rand();
            last_color_time = current_time;
        }


        r.bot = g.yres / 2;       
        r.left = g.xres / 2; 
        r.center = 1;             

        glPushMatrix();

        glTranslatef(g.xres / 2, g.yres / 2, 0);
        glScalef(3.0, 3.0, 1.0); 
        glTranslatef(-g.xres / 2, -g.yres / 2, 0);

        // Render the text
        ggprint16(&r, 0, a, "YOU WIN!");


        for (int i = 0; i < 10; i++)// call this 10 times 
        {
            float x = static_cast<float>(rand() % 800);//random x
            float y = static_cast<float>(rand() % 600);// random y
            make_particles(x, y);
            update_particles();
        }

        glPopMatrix();
    }

    f_render(g.bulletTex, g.beamTex, g.particleTex);
    enemyRender(g.enemyTex);
    HealthRender(g.healthTex);
    fuelRender(g.fuelTex);
    // bossRender(g.bossTex);
    speedometerRender(g.speedoTex);

}


