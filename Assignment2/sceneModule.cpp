/*
 * OpenGL demonstration program for UCI ICS Computer Graphics courses
 * sceneModule.cpp,v 2.2 2016/10/05 11:38 pm
 *
 */

#include "sceneModule.h"
#include "my_gl.h"

GLfloat angley = 20;    /* in degrees */
GLfloat anglex = 30;   /* in degrees */
GLfloat distanceX = 0.0;
GLfloat distanceY = 0.0;
GLfloat distanceZ = 5.0;

static float cubeColors[6][4] =
{
  {0.8, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.0, 1.0},
  {0.0, 0.8, 0.0, 1.0},
  {0.0, 0.0, 0.8, 1.0},
  {0.0, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.8, 1.0},
};

static float cubeVertexes[6][4][4] =
{
  {
    {-1.0, -1.0, -1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0}},

  {
    {-1.0, -1.0, -1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0}},

  {
    {-1.0, -1.0, -1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0}}
};

void drawScene(){

  for (int i = 0; i < 6; i++) {
    glBegin(GL_POLYGON);
		glColor3fv(&cubeColors[i][0]);
		glVertex4fv(&cubeVertexes[i][0][0]);
		glVertex4fv(&cubeVertexes[i][1][0]);
		glVertex4fv(&cubeVertexes[i][2][0]);
		glVertex4fv(&cubeVertexes[i][3][0]);
    glEnd();
  }
}

void sceneTransformation(){
  glLoadIdentity( );

  glTranslatef(-distanceX, distanceY, -distanceZ);
  glRotatef( anglex, 1.0, 0.0, 0.0 );
  glRotatef( angley, 0.0, 1.0, 0.0 );

  //gluLookAt(eyeX, eyeY, eyeZ, atX, atY, atZ, upX, upY, upZ);
}

void display(){
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Sets up transformation.
  sceneTransformation();
  // Draws the scene into the back buffer.
  drawScene();
  // Swaps the front buffer with the back buffer - assumes double buffering.
  glutSwapBuffers();
}