//Francis Mateo
//File descriptor
//#include <asteroids.cpp>

#include <iostream>
#include "fonts.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <stdlib.h>

extern GLuint bulletTex;
extern float cy;

int mateo_show = 0;
const int MAX_PARTICLES = 800;
const int MAX_BULLETS = 50;

int n = 0;
int nn = 0;
int coord[2] = {105, 228};
float bulletDelay = 0.05f; // delay between bullets in seconds
clock_t lastBulletTime = clock();
int beam_flag = 0;

class Projectile {
public:
	float pos[2];	//position
	float last_pos[2];	//last position
	float vel[2];	//velocity
	int w, h;	//size
	
	Projectile() {

		//xres=1200;
	    //yres=800;

		pos[0] = 1000;// / 1.4; // position of the center
		pos[1] = 800 / 3.5; // position of the center
		vel[0] = vel[1] = 0.0f;
		w = 100;
		h = 100;

	}

} particle[MAX_PARTICLES], bullets[MAX_BULLETS], beam;

class Bullets {


};

//#define rnd() (float)rand() / (float)RAND_MAX
//static float initVel = rand(); 

void make_particles(XEvent *e, int yres) {

	int y = yres - e->xbutton.y;

	std::vector<std::pair<float, float>> velocities = {

		{0, 6}, {2, 5}, {4, 4},         
    	{5, 2}, {6, 0}, {5, -2}, {4, -4},   
    	{2, -5}, {0, -6}, {-2, -5}, {-4, -4},  
    	{-5, -2}, {-6, 0}, {-5, 2}, {-4, 4},   
    	{-2, 5}   
    };
		
    for (int x = 0; x < (int)velocities.size(); x++) {
		
    	if (n < MAX_PARTICLES) {
            particle[n].pos[0] = e->xbutton.x;
            particle[n].pos[1] = y;
            particle[n].w = 4;
            particle[n].h = 4;
            particle[n].vel[0] = velocities[x].first;
            particle[n].vel[1] = velocities[x].second;

            n++;
        }
    }

	return;
}

void make_particles2(float x, float yres) {
    float y = yres;  

    std::vector<std::pair<float, float>> velocities = {
        {0, 6}, {2, 5}, {4, 4},         
        {5, 2}, {6, 0}, {5, -2}, {4, -4},   
        {2, -5}, {0, -6}, {-2, -5}, {-4, -4},  
        {-5, -2}, {-6, 0}, {-5, 2}, {-4, 4},   
        {-2, 5}   
    };
    
    for (int xIndex = 0; xIndex < (int)velocities.size(); xIndex++) {
        if (n < MAX_PARTICLES) {
            particle[n].pos[0] = x;  // x is now passed directly
            particle[n].pos[1] = y;  // y is set to yres or some chosen value
            particle[n].w = 4;
            particle[n].h = 4;
            particle[n].vel[0] = velocities[xIndex].first;
            particle[n].vel[1] = velocities[xIndex].second;

            n++;
        }
    }

    return;
}
void make_ammo(float x, float y) {

   clock_t currentTime = clock();
   float elapsedTime = float(currentTime - lastBulletTime) / CLOCKS_PER_SEC;
    
    // create a new bullet only if the delay has passed
    if (elapsedTime > bulletDelay && nn < MAX_BULLETS) {
        bullets[nn].pos[0] = x;    
        bullets[nn].pos[1] = y;    
        bullets[nn].last_pos[0] = x; 
        bullets[nn].last_pos[1] = y;
        bullets[nn].vel[0] = 5.0f;    
        bullets[nn].vel[1] = 0.0f;    
        bullets[nn].w = 25;           
        bullets[nn].h = 20;           
        nn++;                         
        lastBulletTime = currentTime; 
    }
}

void ammo_pos() {

     for (int i = 0; i < nn; i++) {
        bullets[i].last_pos[0] = bullets[i].pos[0];
        bullets[i].pos[0] += bullets[i].vel[0]; // only update x-position

        // remove bullet if it goes off-screen
        if (bullets[i].pos[0] > 1500) {

            // shift the remaining bullets down in the array
            for (int j = i; j < nn - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            nn--; 
            i--;  
        }
    }
}


const float GRAVITY = -0.05;
const float SLOWSPEED = 1.0;

void update_particles() {

	//int i = 0;
    for (int x = 0; x < n; x++) {

        particle[x].vel[1] += GRAVITY;
        particle[x].last_pos[0] = particle[x].pos[0];
        particle[x].last_pos[1] = particle[x].pos[1];
        particle[x].pos[0] += particle[x].vel[0];
        particle[x].pos[1] += particle[x].vel[1];

	    //particle[x].vel[0] *= SLOWSPEED;
        //particle[x].vel[1] *= SLOWSPEED;

        if (particle[x].pos[1] < 0) {
            particle[x] = particle[--n];
        }
    }
}

int currentFrame = 0;         // Current column/frame
const int totalFrames = 12; 
int car_pos;

void f_render(GLuint atex, GLuint btex) {


    beam.h = 145;
    beam.w = 800;
    beam.pos[1] = cy;

    float u_start = currentFrame / static_cast<float>(totalFrames);
    float u_end = (currentFrame + 1) / static_cast<float>(totalFrames);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, atex);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    
    glColor3f(1.0, 1.0, 1.0); // Set color to white to avoid interference

    for (int i = 0; i < nn; i++) {
        glPushMatrix();
        glTranslatef(bullets[i].pos[0], bullets[i].pos[1], 0.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-bullets[i].w, -bullets[i].h);
            glTexCoord2f(0, 0); glVertex2f(-bullets[i].w,  bullets[i].h);
            glTexCoord2f(1, 0); glVertex2f( bullets[i].w,  bullets[i].h);
            glTexCoord2f(1, 1); glVertex2f( bullets[i].w, -bullets[i].h);
        glEnd();
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
 //////////////////////////////////////////

if (beam_flag) {
  glBindTexture(GL_TEXTURE_2D, btex);
   // glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    
    glColor3f(1.0, 1.0, 1.0); // Set color to white to avoid interference

   // for (int i = 0; i < nn; i++) {
        glPushMatrix();
    glTranslatef(beam.pos[0], beam.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(u_start, 1); glVertex2f(-beam.w, -beam.h); // Top-left
        glTexCoord2f(u_start, 0); glVertex2f(-beam.w,  beam.h); // Bottom-left
        glTexCoord2f(u_end, 0);   glVertex2f( beam.w,  beam.h); // Bottom-right
        glTexCoord2f(u_end, 1);   glVertex2f( beam.w, -beam.h); // Top-right
    glEnd();
    glPopMatrix();

    // Update frame for the next render call
    currentFrame = (currentFrame + 1) % totalFrames;
    //}
}

    glBindTexture(GL_TEXTURE_2D, 0);


    glDisable(GL_ALPHA_TEST);

    int g_color = 80 + (rand() % 130);
    int r_color = 200 + (rand() % 50);

  //glEnable(GL_TEXTURE_2D);

    //glColor3f(1.0, 1.0, 1.0);
   // glBindTexture(GL_TEXTURE_2D, atex);
    //
    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GREATER, 0.0f);
   // glColor4ub(255,255,255,255);

  for(int x = 0; x < n; x++) {

		glPushMatrix();
		 glColor3ub(r_color, g_color, 0);
		
		glTranslatef(particle[x].pos[0], particle[x].pos[1], 0.0f);
    	glBegin(GL_QUADS);
			glTexCoord2f(0, 1); glVertex2f(-particle[x].w, -particle[x].h);
			glTexCoord2f(0, 0); glVertex2f(-particle[x].w,  particle[x].h); // check when the x of this vertex == x posiiotn?
			glTexCoord2f(1, 0); glVertex2f( particle[x].w,  particle[x].h);
			glTexCoord2f(1, 1); glVertex2f( particle[x].w, -particle[x].h);
		glEnd();
		glPopMatrix();		
  }

    glBindTexture(GL_TEXTURE_2D, 0);

}
