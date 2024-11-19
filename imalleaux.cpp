/*! \file imalleaux.cpp
* \a source file to show contribution
*
* This file is your showcase of software engineering work.
* The file will contain your own contribution to the group project.
* File will contain a minimum of 200-lines of executable source code
*
*  Certain parts of a file do not count as source...
      - comments
      - header files
      - blank lines
      - repeated code

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
#include <GL/glx.h>
#include "fonts.h"

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
    //Rect r;
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


void speedometerRender(GLuint stex, float speedAngle) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stex);  // Bind the speedometer texture

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 260.0f;   // Width of the speedometer (260)
    float spriteHeight = 260.0f;  // Height of the speedometer (260)

    glColor3f(1.0, 1.0, 1.0);  

    glPushMatrix();

    // Position the speedometer in the center of the screen
    float posX = 1080.0f;  // Left side of the screen
    float posY = 680.0f;   // Top side of the screen

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
    glEnd();

    // Now, let's draw the speed line
    // The angle should be adjusted based on the speed (for example, 0.0f -> 0 degrees, max speed -> max angle)

    glPushMatrix();
    glRotatef(speedAngle, 0.0f, 0.0f, 1.0f);  // Rotate by the speed angle

    glLineWidth(3.0f);  // Line width for better visibility
    glColor3f(1.0f, 0.0f, 0.0f);  // Red line for the speed indicator

    glBegin(GL_LINES);
    // Draw the line from the center to the edge of the speedometer (radius = spriteWidth / 2)
    glVertex2f(0.0f, 0.0f);  // Starting point (center)
    glVertex2f(0.0f, spriteHeight / 2);  // End point (on the perimeter)
    glEnd();

    glPopMatrix();  // Pop the rotation matrix for the speed line

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}
                     
#endif


