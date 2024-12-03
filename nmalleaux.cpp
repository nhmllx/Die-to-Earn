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
#include <iostream>
extern void make_particles(float, float);
extern GLuint enemyTex;
extern float cy;
extern float cx;
extern int lane;
extern int f;
extern float frames[];
extern float hearts_frame;
int count = 10;

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
int gap = 500;
float delayer = 0.01f;
class Enemy {
    public:
        float pos[2];    
        float vel[2];    
        int w, h;        
        float collisionBox[4]; // left, bottom, right, top
        int health;

        Enemy() {
            health = 2;
            //    pos[0] = rand() % 1200; 
            pos[0] = (rand() % (1200 / gap)) * gap;
            // pos[1] = rand() % 280; 
            if (rand() % 2 == 0) {
                pos[1] = 228.571426;
            } else {
                pos[1] = 105.0;
            }
            vel[0] = rand() % 5 + 1; 
            vel[1] = 0.0f;           
            w = 20;                  
            h = 20;                  
            updateCollisionBox();
        }

        void updateCollisionBox() {
            // Calculate the bounding box based on position and dimensions
            collisionBox[0] = pos[0] - w / 2;  // left
            collisionBox[1] = pos[1] - h / 2;  // bottom
            collisionBox[2] = pos[0] + w / 2;  // right
            collisionBox[3] = pos[1] + h / 2;  // top
        }
} enemies[10];
class Boss {
    public:
        int health;
        float pos[2];    
        float vel[2];    
        int w, h;        
        float collisionBox[4]; // left, bottom, right, top

        Boss() {
            health = 10;
            pos[0] =  18000; 
            pos[1] =  75;  
            vel[0] = rand() % 5 + 1; 
            vel[1] = 0.0f;           
            w = 20;                  
            h = 20;                  
            updateCollisionBox();
        }

        void updateCollisionBox() {
            // Calculate the bounding box based on position and dimensions
            collisionBox[0] = pos[0] - w / 2;  // left
            collisionBox[1] = pos[1] - h / 2;  // bottom
            collisionBox[2] = pos[0] + w / 2;  // right
            collisionBox[3] = pos[1] + h / 2;  // top
        }
} b;

// Define a target bounding box region (left, bottom, right, top) centered on the screen
/*if (lane == 0)
  {

  }*/
float targetBox[4];
void updateCbox(float x, float y)
{
    targetBox[0] = x-1;
    targetBox[1] = y;
    targetBox[2] = x + 85;
    targetBox[3] = y;
}
bool checkIfEnemyReachedTarget(int i) {
    if (enemies[i].collisionBox[2] >= targetBox[0] &&  
            enemies[i].collisionBox[0] <= targetBox[2] &&  
            enemies[i].collisionBox[3] >= targetBox[1] &&  
            enemies[i].collisionBox[1] <= targetBox[3]) {
        // f++;
        printf("f: %i\n", f);  
        return true;  
    }
  //  if (enemies[i].health == 3) {

    //    enemies[i].health = 2;
        
      //  return true;
    //}
    return false;
}

bool isEnemyDead(int i) {

    return enemies[i].health <= 0;
}

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
        enemies[i].updateCollisionBox();  // Update collision box based on position

        // Check if the enemy reached the target box
        if (checkIfEnemyReachedTarget(i) || enemies[i].pos[0] <= -100) {

            if(checkIfEnemyReachedTarget(i)){

                make_particles(enemies[i].pos[0],enemies[i].pos[1]);
                //enemies[i].health = 2;
            }

            enemies[i].pos[0] = 1250;// kill off enemy by moving it off screen
                                 //make_particles2(enemies[i].pos[0],enemies[i].pos[1]);

    }

}


}

void get_data(float en[][4], int* health[], float* pos[]) {

    for (int x = 0; x < count; x++) {

        en[x][0] = enemies[x].pos[0];
        en[x][1] = enemies[x].pos[1];

        en[x][2] = enemies[x].w;
        en[x][3] = enemies[x].h;

        health[x] = &enemies[x].health;
        pos[x] = &enemies[x].pos[0];

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

void enemyKiller() {
    if (count > 0) {
        count--;
    }
}
void bossRender(GLuint btex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, btex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 1950.0f / 13.0f; 
    float spriteHeight = 150.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();
    glScalef(1.75f, 1.75f, 1.0f);  // Scale by 2x in both x and y directions

    int ix = frameno % 13;
    float tx = (float)(ix * spriteWidth) / 1950.0f;  // left part of the sprite
    float ty = 0.0f;
    float flipped = tx + spriteWidth / 1950.0f;  // right part of the sprite

    // face sprite toward player 
    float temp = tx;
    tx = flipped;
    flipped = temp;

    glBegin(GL_QUADS);
    glTexCoord2f(flipped, ty + 1.0f);  // top left
    glVertex2f(b.pos[0] - spriteWidth / 2, b.pos[1] - spriteHeight / 2);

    glTexCoord2f(flipped, ty);  // bottom left
    glVertex2f(b.pos[0] - spriteWidth / 2, b.pos[1] + spriteHeight / 2);

    glTexCoord2f(tx, ty);  // bottom right
    glVertex2f(b.pos[0] + spriteWidth / 2, b.pos[1] + spriteHeight / 2);

    glTexCoord2f(tx, ty + 1.0f);  // top right
    glVertex2f(b.pos[0] + spriteWidth / 2, b.pos[1] - spriteHeight / 2);
    glEnd();

    glPopMatrix();
    if (b.pos[0] > 560) {
        // printf("boss pos: %f\n", b.pos[0]);
        b.pos[0] = b.pos[0] - 10;
    }
    //     if (b.pos[0] < 920) {
    //       b.pos[0] = 920;
    // }    

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}


#endif


