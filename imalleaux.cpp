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

void fuelRender(GLuint ftex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ftex);  // Bind the fuel texture

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float sspriteWidth = 160.0f;
    float sspriteHeight = 160.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();  // Start a new transformation block

    // Position the fuel texture
    float posX = 280.0f;  // Left side of the screen
    float posY = 60.0f;   // Top side of the screen

    glTranslatef(posX, posY, 0.0f);  // Translate to desired position

    // Draw the fuel texture
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);  // Top left
    glVertex2f(-sspriteWidth / 2, -sspriteHeight / 2);

    glTexCoord2f(1.0f, 1.0f);  // Top right
    glVertex2f(sspriteWidth / 2, -sspriteHeight / 2);

    glTexCoord2f(1.0f, 0.0f);  // Bottom right
    glVertex2f(sspriteWidth / 2, sspriteHeight / 2);

    glTexCoord2f(0.0f, 0.0f);  // Bottom left
    glVertex2f(-sspriteWidth / 2, sspriteHeight / 2);

    glEnd();  // End of the quad

    glBindTexture(GL_TEXTURE_2D, 0);   // Unbind texture
    glDisable(GL_ALPHA_TEST);          // Disable alpha test

    glPopMatrix();  // Restore the matrix
}

void speedometerRender(GLuint stex, float speedAngle) {
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
    glRotatef(speedAngle, 0.0f, 0.0f, 1.0f);  // Rotate by the speed angle

    glLineWidth(3.0f);  // Line width for better visibility
    glColor3f(1.0f, 0.0f, 0.0f);  // Red line for speed indicator

    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);  // Starting point (center)
    glVertex2f(0.0f, spriteHeight/4);  // End point (on the perimeter)
    glEnd();

    glPopMatrix();  // End of line rotation

    glPopMatrix();  // Restore the matrix
}

void HealthRender(GLuint htex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, htex);  // Bind the health texture

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    float spriteWidth = 260.0f;
    float spriteHeight = 260.0f;

    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();  // Start a new transformation block

    // Position the health texture
    float posX = 100.0f;  // Left side of the screen
    float posY = 80.0f;   // Top side of the screen

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


