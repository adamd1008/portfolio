////////////////////////////////////////////////////////////////
// School of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
//
// Skeleton code for COMP37111 coursework, 2013-14
//
// Authors: Arturs Bekasovs, Toby Howard and Adam Dodd (7341193)
//
/////////////////////////////////////////////////////////////////

#define _BSD_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#ifdef __linux
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#endif
#include <GL/glut.h>
#include "mpi.h"

#define DEG_TO_RAD 0.017453293
#define SIND(x) (sin(DEG_TO_RAD * (x)))
#define COSD(x) (cos(DEG_TO_RAD * (x)))

// Display list for coordinate axis
GLuint axisList;

int AXIS_SIZE = 100;
int axisEnabled = 1;

double G = 0.25; /* The gravitational constant! */

int w = 1280;
int h = 720;

long ticks = 0;

int mouseLeftPressed = 0;
int mouseMiddlePressed = 0;

double lat = 45.0;
double lon = 135.0;
double r = 500.0;
int prevX, prevY;

typedef struct
{
   double x, y, z;
} double3_t;

typedef struct
{
   double3_t coords;
   double3_t vel;
   double mass;
   double3_t colour;
} point_t;

int noOfPoints = 500;
point_t* points = NULL;

int noOfLines = 500;
double3_t** lines = NULL; /* [noOfPoints][noOfLines] */

int pointsToFollow = 2;

int head = 0, occupied = 0;

int randomMass = 0;
int lineColourMode = 1;

double volumeSize = 500.0;
double initVelocity = 0.5;

double3_t* accel = NULL;

double threshold = 3.0;

int spacePressed = 0;

int rank;
int commSize;
int particleIndex;
int particlesPerProc;

struct timeval lastTime, thisTime, diffTime;

int benchmark = 0;
float fpsArray[10];

#ifdef __linux
struct option longOptions[] = {{"particles", required_argument, NULL, 'n'},
                               {"volume", required_argument, NULL, 'd'},
                               {"mass", required_argument, NULL, 'm'},
                               {"lines", required_argument, NULL, 'l'},
                               {"velocity", required_argument, NULL, 'v'},
                               {"follow", required_argument, NULL, 'f'},
                               {"gravity", required_argument, NULL, 'g'},
                               {"colour", required_argument, NULL, 'c'},
                               {"threshold", required_argument, NULL, 'h'},
                               {"radius", required_argument, NULL, 'r'},
                               {"benchmark", no_argument, NULL, 'b'},
                               {NULL, 0, NULL, 0}};
#endif

double myRandom()
//Return random double within range [0,1]
{
  return (rand() / (double) RAND_MAX);
}

void quit()
{
   int i;
   
   if (points != NULL)
      free(points);
   
   if (lines != NULL)
   {
      for (i = 0; i < pointsToFollow; i++)
         free(lines[i]);
   
      free(lines);
   }
   
   if (accel != NULL)
      free(accel);
   
   exit(EXIT_SUCCESS);
}

void initPoints()
{
   int i;
   
   if (points != NULL)
      free(points);
   
   points = (point_t*) malloc(sizeof(point_t) * noOfPoints);
   
   if (points == NULL)
   {
      fprintf(stderr, "Out of memory!\n");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   for (i = 0; i < noOfPoints; i++)
   {
      points[i].coords.x = ((myRandom()) * volumeSize) - (volumeSize / 2.0);
      points[i].coords.y = ((myRandom()) * volumeSize) - (volumeSize / 2.0);
      points[i].coords.z = ((myRandom()) * volumeSize) - (volumeSize / 2.0);
      points[i].vel.x = ((myRandom()) * initVelocity) - (initVelocity / 2.0);
      points[i].vel.y = ((myRandom()) * initVelocity) - (initVelocity / 2.0);
      points[i].vel.z = ((myRandom()) * initVelocity) - (initVelocity / 2.0);
      
      if (randomMass <= 0)
      {
         points[i].mass = 1.0;
         points[i].colour.x = 1.0;
         points[i].colour.y = 1.0;
         points[i].colour.z = 1.0;
      }
      else
      {
         points[i].mass = myRandom() * randomMass;
         points[i].colour.x = points[i].mass / randomMass;
         points[i].colour.y = 0.0;
         points[i].colour.z = 1.0 - (points[i].mass / randomMass);
      }
   }
   
   if (accel != NULL)
      free(accel);
   
   accel = (double3_t*) malloc(sizeof(double3_t) * noOfPoints);
   
   if (accel == NULL)
   {
      fprintf(stderr, "Out of memory!\n");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
}

void initLines()
{
   int i;
   
   if (pointsToFollow > 0)
   {
      lines = (double3_t**) malloc(sizeof(double3_t*) * pointsToFollow);
      
      if (lines == NULL)
      {
         fprintf(stderr, "Out of memory!\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
      
      for (i = 0; i < pointsToFollow; i++)
      {
         lines[i] = (double3_t*) malloc(sizeof(double3_t) * noOfLines);
         
         if (lines[i] == NULL)
         {
            fprintf(stderr, "Out of memory!\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
         }
      }
      
      if (lineColourMode == 1)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 1.0;
            points[i].colour.y = 0.0;
            points[i].colour.z = 0.0;
         }
      }
      else if (lineColourMode == 2)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 0.0;
            points[i].colour.y = 1.0;
            points[i].colour.z = 0.0;
         }
      }
      else if (lineColourMode == 3)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 0.0;
            points[i].colour.y = 0.0;
            points[i].colour.z = 1.0;
         }
      }
      else if (lineColourMode == 4)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 0.0;
            points[i].colour.y = 1.0;
            points[i].colour.z = 1.0;
         }
      }
      else if (lineColourMode == 5)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 1.0;
            points[i].colour.y = 0.0;
            points[i].colour.z = 1.0;
         }
      }
      else if (lineColourMode == 6)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = 1.0;
            points[i].colour.y = 1.0;
            points[i].colour.z = 0.0;
         }
      }
      else if (lineColourMode == 7)
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            points[i].colour.x = myRandom();
            points[i].colour.y = myRandom();
            points[i].colour.z = myRandom();
         }
      }
      else if (lineColourMode == 8)
      {
         if (randomMass == 0)
         {
            for (i = 0; i < pointsToFollow; i++)
            {
               points[i].colour.x = 1.0;
               points[i].colour.y = 0.0;
               points[i].colour.z = 0.0;
            }
         }
         else
         {
            for (i = 0; i < pointsToFollow; i++)
            {
               points[i].colour.x = points[i].mass / randomMass;
               points[i].colour.y = 0.0;
               points[i].colour.z = 1.0 - (points[i].mass / randomMass);
            }
         }
      }
   }
}

void processPoints()
{
   int i, j;
   double dX, dY, dZ, dist, f, fi, fj;
   double angleX, angleY, angleZ,
          compXi, compYi, compZi,
          compXj, compYj, compZj;
   
   if ((MPI_Allgather(&points[particleIndex],
                      particlesPerProc * sizeof(point_t), MPI_BYTE, points,
                      particlesPerProc * sizeof(point_t), MPI_BYTE,
                      MPI_COMM_WORLD)) != MPI_SUCCESS)
   {
      fprintf(stderr, "Error in MPI_Allgather!\n");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   for (i = particleIndex; i < (particleIndex + particlesPerProc); i++)
      accel[i].x = accel[i].y = accel[i].z = 0.0;
   
   /* First, calculate _just_ acceleration */
   
   for (i = particleIndex; i < (particleIndex + particlesPerProc); i++)
         for (j = 0; j < noOfPoints; j++)
            if (i != j)
            {
               /* First, calculate the distance between the points, and therefore,
                  the force of attraction between them */
            
               dX = points[j].coords.x - points[i].coords.x;
               dY = points[j].coords.y - points[i].coords.y;
               dZ = points[j].coords.z - points[i].coords.z;
            
               dist = sqrt(pow(dX, 2.0) + pow(dY, 2.0) + pow(dZ, 2.0));
            
               f = points[i].mass * points[j].mass;
               //f /= pow(dist, 2.0);
               f /= dist * dist;
               f *= G;
               fi = f / points[i].mass;
               fj = f / points[j].mass;
               /* As per F = ma, i.e. the heavier an object, the less acceleration
                  it experiences for a given force. */
            
               /* OK, now we've done that, calculate the x, y, z components of the
                  force. */
            
               /* Determine the angles according to the ORIGINAL distance, as they
                  are the same as the angles of the new force vector; i.e. only
                  the amplitude has changed; the force is still 'felt' from the
                  same direction. */
            
               angleX = acos(dX / dist);
               angleY = acos(dY / dist);
               angleZ = acos(dZ / dist);
            
               compXi = cos(angleX) * fi;
               compYi = cos(angleY) * fi;
               compZi = cos(angleZ) * fi;

               compXj = cos(angleX) * fj;
               compYj = cos(angleY) * fj;
               compZj = cos(angleZ) * fj;
            
               accel[i].x += compXi;
               accel[i].y += compYi;
               accel[i].z += compZi;
            
               accel[j].x -= compXj;
               accel[j].y -= compYj;
               accel[j].z -= compZj;
            }
   
   /* Second, calculate their new velocity */
   
   for (i = particleIndex; i < (particleIndex + particlesPerProc); i++)
   {
      if ((accel[i].x > threshold) || (accel[i].y > threshold) ||
          (accel[i].z > threshold))
         printf("Acceleration threshold exceeded {%f, %f, %f}\n",
                accel[i].x, accel[i].y, accel[i].z);
      
      points[i].vel.x += accel[i].x;
      points[i].vel.y += accel[i].y;
      points[i].vel.z += accel[i].z;
   }
   
   /* Third, calculate their new positions */
   
   for (i = particleIndex; i < (particleIndex + particlesPerProc); i++)
   {
      points[i].coords.x += points[i].vel.x;
      points[i].coords.y += points[i].vel.y;
      points[i].coords.z += points[i].vel.z;
   }
   
   if ((rank != 0) && (benchmark == 1))
   {
      ticks++;
      
      if (ticks == 100)
      {
         MPI_Finalize();
         exit(EXIT_SUCCESS);
      }
   }
}

void display(void)
{
   int i, j, k;
   
   glLoadIdentity();
   
   gluLookAt(COSD(lat) * SIND(lon) * r,
             SIND(lon - 90.0) * r,
             SIND(lat) * SIND(lon) * r,
             0.0, 0.0, 0.0,
             0.0, 1.0, 0.0);
   
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT);
   // If enabled, draw coordinate axis
   if(axisEnabled)
      glCallList(axisList);
   
   processPoints();
   
   glBegin(GL_POINTS);
   
   for (i = 0; i < noOfPoints; i++)
   {
      glColor3d(points[i].colour.x, points[i].colour.y, points[i].colour.z);
      glVertex3d(points[i].coords.x, points[i].coords.y, points[i].coords.z);
      
      /*
      glPushMatrix();
      glTranslatef(points[i].coords.x, points[i].coords.y, points[i].coords.z);
      //glutSolidSphere(points[i].mass, 10, 10);
      glutSolidSphere(1.0, 10, 10);
      glPopMatrix();
      */
   }
   
   glEnd();
   
   /* Add new line to array! */
   
   if (pointsToFollow > 0)
   {
      if (occupied == noOfLines)
      {
         if (head == noOfLines)
            head = 0;
         
         for (i = 0; i < pointsToFollow; i++)
         {
            lines[i][head].x = points[i].coords.x;
            lines[i][head].y = points[i].coords.y;
            lines[i][head].z = points[i].coords.z;
         }
         
         head++;
      }
      else
      {
         for (i = 0; i < pointsToFollow; i++)
         {
            lines[i][occupied].x = points[i].coords.x;
            lines[i][occupied].y = points[i].coords.y;
            lines[i][occupied].z = points[i].coords.z;
         }
         
         occupied++;
         head++;
      }
      
      if (ticks >= 1)
      {
         glBegin(GL_LINES);
         
         j = head - 1;
         
         for (k = 0; k < (occupied - 1); k++)
         {
            if (j == 0)
            {
               for (i = 0; i < pointsToFollow; i++)
               {
                  glColor3d((((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.x,
                            (((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.y,
                            (((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.z);
                  
                  glVertex3d(lines[i][noOfLines - 1].x,
                             lines[i][noOfLines - 1].y,
                             lines[i][noOfLines - 1].z);
                  
                  glVertex3d(lines[i][0].x, lines[i][0].y, lines[i][0].z);
               }
               
               j = noOfLines - 1;
            }
            else
            {
               for (i = 0; i < pointsToFollow; i++)
               {
                  glColor3d((((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.x,
                            (((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.y,
                            (((double) (noOfLines - k)) /
                            ((double) noOfLines)) *
                            points[i].colour.z);
                  
                  glVertex3d(lines[i][j - 1].x,
                             lines[i][j - 1].y,
                             lines[i][j - 1].z);
                  
                  glVertex3d(lines[i][j].x, lines[i][j].y, lines[i][j].z);
               }
            
               j--;
            }
         }
         
         glEnd();
      }
   }
   
   ticks++;
   
   gettimeofday(&thisTime, NULL);
   timersub(&thisTime, &lastTime, &diffTime);
   
   char diffStr[32];
   
   sprintf(diffStr, "%d.%.06d", diffTime.tv_sec, diffTime.tv_usec);
   
   float diff = atof(diffStr);
   
   //printf("thisTime = {%d.%d}, lastTime = {%d.%d}, diff = %f\n",
   //     thisTime.tv_sec, thisTime.tv_usec, lastTime.tv_sec, lastTime.tv_usec,
   //     diff);
   
   if (diff == 0.0f)
      diff = 1.0f;
   
   diff = 1.0f / diff;
   
   if (benchmark == 1)
   {
      if (ticks > 90)
         fpsArray[ticks - 91] = diff;
      
      if (ticks == 100)
      {
         float fpsAvg = 0.0f;
         
         for (i = 0; i < 10; i++)
            fpsAvg += fpsArray[i];
         
         fpsAvg /= 10.0f;
         
         fprintf(stderr, "%f\n", fpsAvg);
         
         MPI_Finalize();
         exit(EXIT_SUCCESS);
      }
   }
   
   /* XXX The following FPS counter code is code written by Toby Howard */
   
   char *ch; 
   GLint matrixMode;
   GLboolean lightingOn;
   
   sprintf(diffStr, "fps %f", diff);
   
   lightingOn = glIsEnabled(GL_LIGHTING);
   
   if (lightingOn)
      glDisable(GL_LIGHTING);
   
   glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */
   
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, 1.0, 0.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
   glColor3f(1.0f, 1.0f, 1.0f);
   
   glRasterPos3f(0.0f, 0.005f, 0.0f);
   
   for(ch = diffStr; *ch; ch++)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int) *ch);
   
   glPopAttrib();
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(matrixMode);
   if (lightingOn)
      glEnable(GL_LIGHTING);
   
   /* XXX End */
   
   memcpy(&lastTime, &thisTime, sizeof(struct timeval));
   
#ifdef NBODY_SINGLE_BUF
   glFlush();
#else
   glutSwapBuffers();
#endif
   glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
   if (key == 27)
      quit();
   else if (key == ' ')
      spacePressed = 1;
}

void keyboardUp(unsigned char key, int x, int y)
{
   if (key == ' ')
      spacePressed = 0;
}

void reshape(int width, int height)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90, (GLfloat)width / (GLfloat)height, 1.0, 100000000.0);
  glMatrixMode(GL_MODELVIEW);
}

void mouseFunc(int button, int state, int x, int y)
{
   if (button == GLUT_LEFT_BUTTON)
   {
      if (state == GLUT_UP)
         mouseLeftPressed = 0;
      else if (state == GLUT_DOWN)
      {
         mouseLeftPressed = 1;
         prevX = x;
         prevY = y;
      }
   }
   else if (button == GLUT_MIDDLE_BUTTON)
   {
      if (state == GLUT_UP)
         mouseMiddlePressed = 0;
      else if (state == GLUT_DOWN)
      {
         mouseMiddlePressed = 1;
         prevX = x;
         prevY = y;
      }
   }
}

void motionFunc(int x, int y)
{
   if (mouseLeftPressed == 1 && mouseMiddlePressed == 1)
   {
      // Do nothing
   }
   else if (mouseLeftPressed == 1)
   {
      double scaleX;
      double scaleY;
      
      if (spacePressed == 1)
      {
         scaleX = 0.5;
         scaleY = 0.5;
      }
      else
      {
         scaleX = 0.1;
         scaleY = 0.1;
      }
   
      lat += (x - prevX) * scaleX;
      lon += (y - prevY) * scaleY;
   
      if (lat < 0.0)
         lat = 360.0 - lat;
      else if (lat >= 360.0)
         lat = lat - 360.0;
   
      if (lon > 179.9)
         lon = 179.9;
      else if (lon < 0.1)
         lon = 0.1;
      
      prevX = x;
      prevY = y;
   }
   else if (mouseMiddlePressed == 1)
   {
      double scaleX;
      double scaleY;
      
      if (spacePressed == 1)
      {
         scaleX = 0.5;
         scaleY = 100.0;
      }
      else
      {
         scaleX = 0.1;
         scaleY = 2.0;
      }
      
      lat += (x - prevX) * scaleX;
      
      if (lat < 0.0)
         lat = 360.0 - lat;
      else if (lat >= 360.0)
         lat = lat - 360.0;
      
      r -= (y - prevY) * scaleY;
      
      if (r < 10.0)
         r = 10.0;
      
      prevX = x;
      prevY = y;
   }
}

void initGraphics(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(w, h);
   glutInitWindowPosition(0, 0);
#ifdef NBODY_SINGLE_BUF
   glutInitDisplayMode(GLUT_SINGLE);
#else
   glutInitDisplayMode(GLUT_DOUBLE);
#endif
   glutCreateWindow("n-body simulation");
   //glutFullScreen();
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboardUp);
   glutMouseFunc(mouseFunc);
   glutMotionFunc(motionFunc);
   glutReshapeFunc(reshape);
   
   glLineWidth(1.0f);
}

void initMPI()
{
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &commSize);
   
   MPI_Bcast(&noOfPoints, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&volumeSize, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(&randomMass, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&noOfLines, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&initVelocity, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(&pointsToFollow, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&G, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(&lineColourMode, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&threshold, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(&r, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   
   if ((noOfPoints % commSize) != 0)
   {
      fprintf(stderr, "Bad noOfPoints %% commSize!");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   particlesPerProc = noOfPoints / commSize;
   particleIndex = rank * particlesPerProc;
   
   initPoints();
   
   if ((MPI_Allgather(&points[particleIndex],
                      particlesPerProc * sizeof(point_t), MPI_BYTE, points,
                      particlesPerProc * sizeof(point_t), MPI_BYTE,
                      MPI_COMM_WORLD)) != MPI_SUCCESS)
   {
      fprintf(stderr, "Error in MPI_Allgather!\n");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   if (rank == 0)
      initLines();
}

int main(int argc, char** argv)
{
   if ((MPI_Init(&argc, &argv)) != MPI_SUCCESS)
   {
      fprintf(stderr, "Couldn't init MPI!\n");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   if (rank == 0)
   {
#ifdef __linux
      int opt;
      int optIndex = 0;
      
      while ((opt = getopt_long(argc, argv, "n:d:m:l:v:f:g:c:t:r:b",
                                longOptions, &optIndex)) != -1)
      {
         switch (opt)
         {
            case 'n':
               noOfPoints = atoi(optarg);
               break;
            case 'd':
               volumeSize = atof(optarg);
               break;
            case 'm':
               randomMass = atoi(optarg);
               break;
            case 'l':
               noOfLines = atoi(optarg);
               break;
            case 'v':
               initVelocity = atof(optarg);
               break;
            case 'f':
               pointsToFollow = atoi(optarg);
               break;
            case 'g':
               G = atof(optarg);
               break;
            case 'c':
               lineColourMode = atoi(optarg);
               break;
            case 'h':
               threshold = atof(optarg);
               break;
            case 'r':
               r = atof(optarg);
               break;
            case 'b':
               benchmark = 1;
               break;
         }
      }
#endif
   }
   
   srand((unsigned int) time(NULL));
   
   initMPI();
   
   if (rank == 0)
   {
      initGraphics(argc, argv);
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
      gettimeofday(&lastTime, NULL);
      //memcpy(&lastTime, &thisTime, sizeof(struct timeval));
      glutMainLoop();
   }
   else
   {
      while (1)
         processPoints();
   }
   
   return EXIT_SUCCESS;
}
