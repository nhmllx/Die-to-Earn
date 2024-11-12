#ifndef NMALLEAUX_H
#define NMALLEAUX_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

//extern class timers;
extern GLuint enemyTex;
const int MAX_ENEMIES = 30;


class Animate {
    public:
        double physicsRate;
        double oobillion;
        struct timespec timeStart, timeEnd, timeCurrent;
        struct timespec spriteTime;
        Animate() {
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
} anim;

int frameno = 0;
float delayer = 0.01f;
class Enemy {
    public:
        float pos[2];	//position
        float last_pos[2];	//last position
        float vel[2];	//velocity
        int w;
        int h;

        Enemy() {
            pos[0] = 1200 / 2; // enemy x position
            pos[1] = 800 / 2; // enemy y position
            vel[0] = 0.0f;
            vel[1] = 0.0f;
            w = 100;
            h = 100;

        }

} enemies[MAX_ENEMIES];


void enemyAnimate(void) //call in walk.cpp main
  {
  anim.recordTime(&anim.timeCurrent);
        double timeSpan = anim.timeDiff(&anim.spriteTime, &anim.timeCurrent);
        if (timeSpan > delayer) {
            ++frameno;
            if (frameno >= 18)
                frameno -= 18;
            anim.recordTime(&anim.spriteTime);
        }
        //for (int i=0; i<20; i++) {
          //  g.box[i][0] -= 2.0 * (0.5 / delayer);
         //   if (g.box[i][0] < -10.0)
           //     g.box[i][0] += g.xres + 10.0;
       // }
  }
void enemyRender(GLuint etex) 
{

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, etex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    float ww = 173.0f;
    float hh = 44.0f;

    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        /*glPushMatrix();
          glTranslatef(enemies[i].pos[0], enemies[i].pos[1], 0.0f);
          glBegin(GL_QUAS);
          glTexCoord2f(0, 1); glVertex2f(-enemies[i].w, -enemies[i].h);
          glTexCoord2f(0, 0); glVertex2f(-enemies[i].w,  enemies[i].h);
          glTexCoord2f(1, 0); glVertex2f( enemies[i].w,  enemies[i].h);
          glTexCoord2f(1, 1); glVertex2f( enemies[i].w, -enemies[i].h);
          glEnd();
          glPopMatrix();*/
        glPushMatrix();
        // glColor3f(1.0, 1.0, 1.0);
        // glBindTexture(GL_TEXTURE_2D, g.walkTexture);
        //
        // glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255,255,255,255);

        int ix = frameno % 9; // Get the current sprite frame (0, 1, or 2)

        // Calculate texture coordinates based on the current frame
        float tx = (float)(ix * ww) / 519.0f; // Adjust tx based on the frame
        float ty = 0.0f; // Only one row, so ty is always 0

        glBegin(GL_QUADS);
        glTexCoord2f(tx, ty+1.0f); 
        glVertex2f(-enemies[i].w - ww, -enemies[i].h - hh); // Top-left
        glTexCoord2f(tx, ty); 
        glVertex2f(-enemies[i].w - ww, enemies[i].h + hh); // Bottom-left
        glTexCoord2f(tx + 1.0f / 3.0f, ty); 
        glVertex2f(enemies[i].w + ww, enemies[i].h + hh);
        glTexCoord2f(tx + 1.0f / 3.0f, ty + 1.0f); 
        glVertex2f(enemies[i].w + ww, -enemies[i].h - hh); // Top-right
        glEnd();
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}    
#endif

