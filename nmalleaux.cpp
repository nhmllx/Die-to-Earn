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
int count = 30;


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
        float pos[2];    
        float vel[2];    
        int w, h;        

        Enemy() {
            pos[0] = rand() % 1200; 
            pos[1] = rand() % 600;  
            vel[0] = rand() % 5 + 1; 
            vel[1] = 0.0f;           
            w = 20;                  
            h = 20;                  
        }
} enemies[30];

void enemyAnimate(void) {
    anim.recordTime(&anim.timeCurrent);
    double timeSpan = anim.timeDiff(&anim.spriteTime, &anim.timeCurrent);
    if (timeSpan > delayer) {
        ++frameno;
        if (frameno >= 18)
            frameno -= 18;
        anim.recordTime(&anim.spriteTime);
    }

    for (int i = 0; i < count; i++) {
        enemies[i].pos[0] -= enemies[i].vel[0]; 
    }
}


void enemyRender(GLuint etex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, etex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 900.0f / 9.0f;  
    float spriteHeight = 100.0f;        

    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < count; i++) {
        glPushMatrix();

        int ix = frameno % 9;  
        float tx = (float)(ix * spriteWidth) / 900.0f;  
        float ty = 0.0f;  
        float flipped = tx + spriteWidth / 900.0f;  

        glBegin(GL_QUADS);
        glTexCoord2f(flipped, ty + 1.0f);  // top left
        glVertex2f(enemies[i].pos[0] - spriteWidth / 2, enemies[i].pos[1] - spriteHeight / 2);
        glTexCoord2f(flipped, ty);  // bottom left
        glVertex2f(enemies[i].pos[0] - spriteWidth / 2, enemies[i].pos[1] + spriteHeight / 2);
        glTexCoord2f(tx, ty);  // bottom right
        glVertex2f(enemies[i].pos[0] + spriteWidth / 2, enemies[i].pos[1] + spriteHeight / 2);
        glTexCoord2f(tx, ty + 1.0f);  // top right
        glVertex2f(enemies[i].pos[0] + spriteWidth / 2, enemies[i].pos[1] - spriteHeight / 2);
        glEnd();

        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}
void enemyKiller()
{
    if (count > 0) {

    count--;
    }
}
void bossRender(GLuint btex)
{
}
#endif


