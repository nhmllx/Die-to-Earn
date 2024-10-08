//Francis Mateo
//File descriptor
//#include <asteroids.cpp>

//#include <iostream>
#include "fonts.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
//#include "walk.cpp"
//extern int main();
//#include "asteroids.cpp"

int mateo_show = 0;
const int MAX_PARTICLES = 500;
//const float random = 100.0f;
int n = 0;

class Particles {
public:
	float pos[2];	//position
	float last_pos[2];	//last position
	float vel[2];	//velocity
	int w, h;	//size
	
	Particles() {

		//pos[0] = g.xres / 2; // position of the center
		//pos[1] = g.yres / 2; // position of the center
		vel[0] = vel[1] = 0.0f;
		w = 50;
		h = 10;

	}

} particle[MAX_PARTICLES];



void show_my_feature(int x, int y) {

    //draw a rectangle
   // rectangle(0, 0, 640, 480);
    //draw a text

  Rect r;
  r.bot = y - 20;
	r.left = x;
	r.center = 0;
	ggprint8b(&r, 16, 0x0000ff00, "Francis' feature");

}

//#define rnd() (float)rand() / (float)RAND_MAX
static float initVel = rand(); 

void check_particles(XEvent *e, int yres) {


	  int y = yres - e->xbutton.y;
		
    for (int x = 0; x < 2; x++) {
		  if (n < MAX_PARTICLES) {

			  particle[n].pos[0] = e->xbutton.x;
			  particle[n].pos[1] = y;
			  particle[n].h = 4;
			  particle[n].w = 4;
			  particle[n].vel[0] = initVel;
			  particle[n].vel[1] = initVel;

			  n++;
		}
	}	
	return;
}
const float GRAVITY = -0.7;

void update_particles() {

	//int i = 0;
    for (int x = 0; x < n; x++) {

        particle[x].vel[1] += GRAVITY;
        particle[x].last_pos[0] = particle[x].pos[0];
        particle[x].last_pos[1] = particle[x].pos[1];
        particle[x].pos[0] += particle[x].vel[0];
        particle[x].pos[1] += particle[x].vel[1];

        if (particle[x].pos[1] < 0) {
            particle[x] = particle[--n];
        }
    }
}

void render_particles() {

  int i = 0;
  for(int x = 0; x < n; x++) {
		glPushMatrix();
		glColor3ub(100 + i, 200 + i, 220 - i);
		//i+=10;
		if(i >= 50){
			i = -20;
		}
		glTranslatef(particle[x].pos[0], particle[x].pos[1], 0.0f);
    	glBegin(GL_QUADS);
			glVertex2f(-particle[x].w, -particle[x].h);
			glVertex2f(-particle[x].w,  particle[x].h); // check when the x of this vertex == x posiiotn?
			glVertex2f( particle[x].w,  particle[x].h);
			glVertex2f( particle[x].w, -particle[x].h);
		glEnd();
		glPopMatrix();
		//glColor3ub(100 + i, 200 + i, 220 - i);
		i+=10;
		if(i >= 50){
			i = -20;
		}
  }

}