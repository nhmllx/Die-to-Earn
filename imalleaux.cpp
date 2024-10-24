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
}
                       
#endif


