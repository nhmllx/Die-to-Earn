/*! \file imalleaux.cpp
 * \a source file to show contribution
 * History: Written by Isaiah Malleaux, 9/24.
 */
#ifndef IMALLEAUX_H
#define IMALLEAUX_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <vector>
#include <chrono>
#include <ctime>
#include "fonts.h"

struct Firework {
    float pos[2];
    float vel[2];
    float last_pos[2];
    int w, h;
    int color;
};

const int MAX_FIREWORKS = 800;
Firework firework[MAX_FIREWORKS];
int p = 0;
const float GRAVITY = -0.05;


void make_fireworks(float x, float y) {
    srand(static_cast<unsigned>(time(0)));

    std::vector<std::pair<float, float>> velocities;
    for (int angle = 0; angle < 360; angle += 22) {
        float rad = angle * 3.14159f / 180.0f;
        velocities.emplace_back(cos(rad) * 6, sin(rad) * 6);
    }

    for (auto& velocity : velocities) {
        velocity.first += static_cast<float>(rand() % 300 - 150) / 100.0f;
        velocity.second += static_cast<float>(rand() % 300 - 150) / 100.0f;
    }

    for (size_t i = 0; i < velocities.size(); ++i) {
        if (p < MAX_FIREWORKS) {
            firework[p].pos[0] = x;
            firework[p].pos[1] = y;
            firework[p].w = 4;
            firework[p].h = 4;
            firework[p].vel[0] = velocities[i].first;
            firework[p].vel[1] = velocities[i].second;
            firework[p].color = rand();
            ++p;
        }
    }
}


void restartProgram(const char *programName) {
    printf("Restarting program...\n");
    // Attempt to execute the program
    if (execl(programName, programName, NULL) == -1) {
        // If execl fails, print the error and exit
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}

void scroll(float val[])
{
    val[0] += 0.001;
    val[1] += 0.001;
}
// new render
void render2(float x[], float y[], GLuint bt, int xres,int yres)
{

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    //draw background
    glBindTexture(GL_TEXTURE_2D, bt);
    glBegin(GL_QUADS);//background
    glTexCoord2f(x[0], y[1]); glVertex2i(0, 0);
    glTexCoord2f(x[0], y[0]); glVertex2i(0, yres);
    glTexCoord2f(x[1], y[0]); glVertex2i(xres, yres);
    glTexCoord2f(x[1], y[1]); glVertex2i(xres, 0);
    glEnd();
    //draw background
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, bt);
    glBegin(GL_QUADS);
    glTexCoord2f(x[0], y[1]); glVertex2i(0, 0);
    glTexCoord2f(x[0], y[0]); glVertex2i(0, yres);
    glTexCoord2f(x[1], y[0]); glVertex2i(xres, yres);
    glTexCoord2f(x[1], y[1]); glVertex2i(xres, 0);
    glEnd();
}
//UI elements
void render3(float x[], float y[], GLuint bt, int xres,int yres)
{
    // Rect r;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, bt);
    glBegin(GL_QUADS);
    glTexCoord2f(x[0], y[1]); glVertex2i(0, 0);
    glTexCoord2f(x[0], y[0]); glVertex2i(0, yres);
    glTexCoord2f(x[1], y[0]); glVertex2i(xres, yres);
    glTexCoord2f(x[1], y[1]); glVertex2i(xres, 0);
    glEnd();
    //draw background
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, bt);
    glBegin(GL_QUADS);
    glTexCoord2f(x[0], y[1]); glVertex2i(0, 0);
    glTexCoord2f(x[0], y[0]); glVertex2i(0, yres);
    glTexCoord2f(x[1], y[0]); glVertex2i(xres, yres);
    glTexCoord2f(x[1], y[1]); glVertex2i(xres, 0);
    glEnd();

}
float lineOffset = 0.0f;

void handleInput(Display* display) {
    XEvent event;
    while (XPending(display)) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_Up) {
                lineOffset += 5.0f; // Increase offset 
            } else if (key == XK_Down) {
                lineOffset -= 5.0f; // Decrease offset 
            }

            // Clamp lineOffset within a range (e.g., 0 to 160.0f)
            if (lineOffset < -80.0f) lineOffset = -80.0f;
            if (lineOffset > 80.0f) lineOffset = 80.0f;
        }
    }
}

void fuelRender(GLuint ftex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ftex);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float sspriteWidth = 160.0f;
    float sspriteHeight = 160.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();

    float posX = 990.0f;
    float posY = 600.0f;

    glTranslatef(posX, posY, 0.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-sspriteWidth / 2, -sspriteHeight / 2);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(sspriteWidth / 2, -sspriteHeight / 2);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(sspriteWidth / 2, sspriteHeight / 2);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-sspriteWidth / 2, sspriteHeight / 2);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(lineOffset, 0.0f);
    glVertex2f(lineOffset + sspriteWidth / 2.5f, 0.0f);
    glEnd();

    glPopMatrix();
}

float currentSpeedAngle = 140.0f;
void speedometerRender(GLuint stex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stex);  // Bind the speedometer texture

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 260.0f;
    float spriteHeight = 260.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();  // Start a new transformation block

    // Position the speedometer
    float posX = 1110.0f;  // Left side of the screen
    float posY = 690.0f;   // Top side of the screen

    glTranslatef(posX, posY, 0.0f);  // Translate to desired position

    // Draw the speedometer texture
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);  // Top left
    glVertex2f(-spriteWidth / 2, -spriteHeight / 2);

    glTexCoord2f(1.0f, 1.0f);  // Top right
    glVertex2f(spriteWidth / 2, -spriteHeight / 2);

    glTexCoord2f(1.0f, 0.0f);  // Bottom right
    glVertex2f(spriteWidth / 2, spriteHeight / 2);

    glTexCoord2f(0.0f, 0.0f);  // Bottom left
    glVertex2f(-spriteWidth / 2, spriteHeight / 2);

    glEnd();  // End of the quad

    glBindTexture(GL_TEXTURE_2D, 0);   // Unbind texture
    glDisable(GL_ALPHA_TEST);          // Disable alpha test

    // Draw the speed line (red line)
    glPushMatrix();  // Start a new block for line drawing

    // Add the conditional update for currentSpeedAngle
    if (currentSpeedAngle > -140.0f) {
        currentSpeedAngle--;  // Decrement 
    }
    else
    {
        currentSpeedAngle++;
    }

    glRotatef(currentSpeedAngle, 0.0f, 0.0f, 1.0f);  // Rotate 

    glLineWidth(3.0f); 
    glColor3f(1.0f, 0.0f, 0.0f); 

    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);  // Starting point (center)
    glVertex2f(0.0f, spriteHeight / 4);  // End point (on the perimeter)
    glEnd();

    glPopMatrix();  // End of line rotation

    glPopMatrix();  
}



void HealthRender(GLuint htex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, htex);  // Bind the health texture

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 542.0f;
    float spriteHeight = 43.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();  // Start a new transformation block

    // Position the health texture
    float posX = 600.0f;  // Left side of the screen
    float posY = 750.0f;   // Top side of the screen

    glTranslatef(posX, posY, 0.0f);  // Translate to desired position

    // Draw the health texture
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);  // Top left
    glVertex2f(-spriteWidth / 2, -spriteHeight / 2);

    glTexCoord2f(1.0f, 1.0f);  // Top right
    glVertex2f(spriteWidth / 2, -spriteHeight / 2);

    glTexCoord2f(1.0f, 0.0f);  // Bottom right
    glVertex2f(spriteWidth / 2, spriteHeight / 2);

    glTexCoord2f(0.0f, 0.0f);  // Bottom left
    glVertex2f(-spriteWidth / 2, spriteHeight / 2);
    glEnd();  // End of the quad

    glBindTexture(GL_TEXTURE_2D, 0);   // Unbind texture
    glDisable(GL_ALPHA_TEST);          // Disable alpha test

    glPopMatrix();  // Restore the matrix
}


#endif


