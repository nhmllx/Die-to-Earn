//Francis Mateo
//File descriptor

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
#include <cstdlib>
#include <chrono>

extern GLuint bulletTex;
extern float cy;
extern float cx;
extern void get_data(float en[][4], int* en_health[]);

int mateo_show = 0;
const int MAX_PARTICLES = 800;
const int MAX_BULLETS = 50;

int n = 0;
int nn = 0;
int coord[2] = {105, 228};
float bullet_delay = 0.05f; // delay between bullets in seconds
clock_t last_bullet_time = clock();

float beam_render_duration = 1.0f; 
float beam_cooldown_duration = 1.0f; 
clock_t beam_start_time = 0; 
clock_t beam_cooldown_start = 0; 
int beam_cooldown = 0; 
int beam_flag = 0;

//float enemy_data[30][4];
//int * en_health[30];
extern int count;
//int enemy_hw[30][2];


class Projectile {
public:
	float pos[2];	//position
	float last_pos[2];	//last position
	float vel[2];	//velocity
	int w, h;	//size
    unsigned int color;
    int active;
	
	Projectile() {

		//xres=1200;
	    //yres=800;

		pos[0] = 700;// / 1.4; // position of the center
		pos[1] = 800 / 3.5; // position of the center
		vel[0] = vel[1] = 0.0f;
		w = 100;
		h = 100;
        active = 0;

	}

} particle[MAX_PARTICLES], bullets[MAX_BULLETS], beam;

class Bullets {


};

//#define rnd() (float)rand() / (float)RAND_MAX
//static float initVel = rand(); 


void make_fireworks(XEvent *e, float yres) {

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


void make_particles(float x, float yres) {
    float y = yres;  

  srand(static_cast<unsigned>(time(0)));

    std::vector<std::pair<float, float>> velocities = {
        {0, 6}, {2, 5}, {4, 4},         
        {5, 2}, {6, 0}, {5, -2}, {4, -4},   
        {2, -5}, {0, -6}, {-2, -5}, {-4, -4},  
        {-5, -2}, {-6, 0}, {-5, 2}, {-4, 4},   
        {-2, 5}   
    };

    // randomize velocities
      for (auto& velocity : velocities) {

        // add a random velocity within a [-1.5, 1.5] range of original values
        velocity.first += (rand() % 300 - 150) / 100.0f;  
        velocity.second += (rand() % 300 - 150) / 100.0f;
    }
    
    for (int xx = 0; xx < (int)velocities.size(); xx++) {

        if (n < MAX_PARTICLES) {
            particle[n].pos[0] = x;  // x is now passed directly
            particle[n].pos[1] = y;  // y is set to yres 
            particle[n].w = 4;
            particle[n].h = 4;
            particle[n].vel[0] = velocities[xx].first;
            particle[n].vel[1] = velocities[xx].second;

            particle[n].color = rand();

            n++;
        }
    }

    return;
}

void make_ammo(float x, float y) {

   clock_t currentTime = clock();
   float elapsedTime = float(currentTime - last_bullet_time) / CLOCKS_PER_SEC;
    
    // create a new bullet only if the delay has passed
    if (elapsedTime > bullet_delay && nn < MAX_BULLETS) {
        bullets[nn].pos[0] = x;    
        bullets[nn].pos[1] = y;    
        bullets[nn].last_pos[0] = x; 
        bullets[nn].last_pos[1] = y;
        bullets[nn].vel[0] = 5.0f;    
        bullets[nn].vel[1] = 0.0f;    
        bullets[nn].w = 25;           
        bullets[nn].h = 20;   
        bullets[nn].active = 1;    // set active flag to 1 to indicate that it's active        
        nn++;                         
        last_bullet_time = currentTime; 
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


void f_collisions() {

    float enemy_data[30][4];
    int* en_health[30];
   
    get_data(enemy_data, en_health); // get enemy data

    for (int i = 0; i < nn; i++) { 

        for (int j = 0; j < count; j++) { 

            if (bullets[i].active) {

              if (en_health[j] != NULL) {// && *en_health[j] > 0) {

                if (bullets[i].pos[1] < enemy_data[j][1] + enemy_data[j][3] &&
                    bullets[i].pos[1] > enemy_data[j][1] - enemy_data[j][3] &&
                    bullets[i].pos[0] > enemy_data[j][0] - enemy_data[j][2] &&
                    bullets[i].pos[0] < enemy_data[j][0] + enemy_data[j][2]) {
                

                    std::cout << "Collision: Bullet " << i << " with Enemy " << j << std::endl;
                    (*en_health[j])--;


                    for (int k = i; k < nn - 1; k++) {
                        bullets[k] = bullets[k + 1];
                    }
                    nn--; 
                    i--;  
                    break;
                }
              }
            }
        }
    }
}



int currentFrame = 0;         // Current column/frame
const int totalFrames = 12; 
int car_pos;

void f_render(GLuint atex, GLuint btex) {


    beam.h = 120;
    beam.w = 800;
    beam.pos[1] = cy - 15;
    //beam.pos[0] = 700;
    beam.pos[0] = cx + 700;

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

  clock_t currentTime = clock();
  float elapsedTime = float(currentTime - beam_start_time) / CLOCKS_PER_SEC;

  if (elapsedTime < beam_render_duration) {
       
        // render the beam
        glBindTexture(GL_TEXTURE_2D, btex);
        glAlphaFunc(GL_GREATER, 0.0f);

        glColor3f(1.0, 1.0, 1.0); // Set color to white to avoid interference

        glPushMatrix();
        glTranslatef(beam.pos[0], beam.pos[1], 0.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(u_start, 1); glVertex2f(-beam.w, -beam.h); // Top-left
            glTexCoord2f(u_start, 0); glVertex2f(-beam.w,  beam.h); // Bottom-left
            glTexCoord2f(u_end, 0);   glVertex2f( beam.w,  beam.h); // Bottom-right
            glTexCoord2f(u_end, 1);   glVertex2f( beam.w, -beam.h); // Top-right
        glEnd();
        glPopMatrix();

        currentFrame = (currentFrame + 1) % totalFrames; // update frame
    } else {
        // stop rendering and start cooldown
        beam_flag = 0;
        beam_cooldown = 1;
        beam_cooldown_start = currentTime;
    }
}
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_ALPHA_TEST);

if (beam_cooldown) {
    // Calculate elapsed time since cooldown started
    clock_t currentTime = clock();
    float cooldownElapsed = float(currentTime - beam_cooldown_start) / CLOCKS_PER_SEC;

    if (cooldownElapsed >= beam_cooldown_duration) {
        // End cooldown
        beam_cooldown = 0;
    }
}


    //int g_color = 80 + (rand() % 130);
    //int r_color = 200 + (rand() % 50);

  for(int x = 0; x < n; x++) {

		glPushMatrix();
		//glColor3ub(particle[x].color);

        // ex: particle[x].color = 0x349823
        // red: 0x34, green: 0x98, blue: 0x23
        glColor3ub((particle[x].color >> 16) & 0xFF, 
                   (particle[x].color >> 8) & 0xFF, 
                    particle[x].color & 0xFF);
        
		
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