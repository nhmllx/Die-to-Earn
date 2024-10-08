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
//added wastelands.jpg
Image img[3] = {"images/walk.gif", "images/car.png", "images/wastelands.jpg"};
//new class for main code
class Texture {
    public:
        Image *backImage;
        GLuint backTexture;
        float xc[2];
        float yc[2];
};
// In class Global
int flag
Texture tex;
// In Globals constructor
flag = 1;
// new render
void render2(void);

// I added something in main, but cannot show :(

//in void initOpengl(void)
    g.tex.backImage = &img[2];
    //create opengl texture elements
    glGenTextures(1, &g.tex.backTexture);
    int ww = g.tex.backImage->width;
    int hh = g.tex.backImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, ww, hh, 0,
            GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 0.25;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;

//in int checkKeys(XEvent *e)
case XK_Up:
    g.flag = 0;
//in void physics(void)
    g.tex.xc[0] += 0.001;
    g.tex.xc[1] += 0.001;
// new render
void render2()
{
   /* Rect r;
   // glClear(GL_COLOR_BUFFER_BIT);
    r.bot = g.yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ffff00, "vsync: %s", ((g.vsync)?"ON":"OFF"));
    ggprint8b(&r, 16, 0x00ffff00, "fps: %i", g.fps);
    printf("FRAMES: %i\n", g.fps );
    printf("VSYNC: %s\n", ((g.vsync) ? "ON":"OFF") );
*/

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    //draw background
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glBegin(GL_QUADS);//background
    glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);
    glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, g.yres);
    glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
    glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
    glEnd();
    //draw background
}
                       



