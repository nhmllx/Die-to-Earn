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
            w = 20;
            h = 20;
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
}
void enemyRender(GLuint etex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, etex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 900.0f / 9.0f;  
    float spriteHeight = 100.0f;      

    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        glPushMatrix();
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255, 255, 255, 255);

        int ix = frameno % 9;  // Get the current sprite frame (0 to 8)

        float tx = (float)(ix * spriteWidth) / 900.0f;  // x offset for current sprite frame
        float ty = 0.0f;  // y offset (since all frames are in a single row)
        float flipped = tx + spriteWidth / 900.0f;  // Right boundary for the current sprite

        glBegin(GL_QUADS);
        glTexCoord2f(flipped, ty + 1.0f); // top left
        glVertex2f(enemies[i].pos[0] - spriteWidth / 2, enemies[i].pos[1] - spriteHeight / 2);
        glTexCoord2f(flipped, ty); // bottom left
        glVertex2f(enemies[i].pos[0] - spriteWidth / 2, enemies[i].pos[1] + spriteHeight / 2);
        glTexCoord2f(tx, ty); // bottom right
        glVertex2f(enemies[i].pos[0] + spriteWidth / 2, enemies[i].pos[1] + spriteHeight / 2);
        glTexCoord2f(tx, ty + 1.0f); // top right
        glVertex2f(enemies[i].pos[0] + spriteWidth / 2, enemies[i].pos[1] - spriteHeight / 2);

        glEnd();
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}
#endif


