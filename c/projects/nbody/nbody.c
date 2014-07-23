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

#define DEG_TO_RAD 0.017453293
#define SIND(x) (sin(DEG_TO_RAD * (x)))
#define COSD(x) (cos(DEG_TO_RAD * (x)))

// Display list for coordinate axis
GLuint axisList;

int AXIS_SIZE = 100;
int axisEnabled = 1;

double G = 0.25; /* The gravitational constant! */

int w = 1920;
int h = 1080;

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
double3_t* lineColours = NULL;

int pointsToFollow = 2;

int head = 0, occupied = 0;

int randomMass = 0;
int lineColourMode = 1;

double volumeSize = 500.0;
double initVelocity = 0.5;

double3_t* accel = NULL;

double threshold = 3.0;

int spacePressed = 0;

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
	
	if (lineColours != NULL)
		free(lineColours);
	
	if (accel != NULL)
		free(accel);
	
	exit(EXIT_SUCCESS);
}

void initPoints(int new1)
{
	int i;
	
	if (points != NULL)
		free(points);
	
	points = (point_t*) malloc(sizeof(point_t) * new1);
	assert(points);
	
	noOfPoints = new1;
	
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
	assert(accel);
}

void initLines(int thisPoints, int lineCount)
{
	int i;
	
	if (lines != NULL)
	{
		for (i = 0; i < pointsToFollow; i++)
			free(lines[i]);
		
		free(lines);
		lines = NULL;
	}
	
	if (lineColours != NULL)
	{
		free(lineColours);
		lineColours = NULL;
	}
	
	noOfLines = lineCount;
	
	pointsToFollow = thisPoints;
	
	if (pointsToFollow > 0)
	{
		lines = (double3_t**) malloc(sizeof(double3_t*) * pointsToFollow);
		assert(lines);
		
		lineColours = (double3_t*) malloc(sizeof(double3_t) * pointsToFollow);
		assert(lineColours);
		
		for (i = 0; i < pointsToFollow; i++)
		{
			lines[i] = (double3_t*) malloc(sizeof(double3_t) * lineCount);
			assert(lines[i]);
		}
		
		if (lineColourMode == 1)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 1.0;
				lineColours[i].y = 0.0;
				lineColours[i].z = 0.0;
			}
		}
		else if (lineColourMode == 2)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 0.0;
				lineColours[i].y = 1.0;
				lineColours[i].z = 0.0;
			}
		}
		else if (lineColourMode == 3)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 0.0;
				lineColours[i].y = 0.0;
				lineColours[i].z = 1.0;
			}
		}
		else if (lineColourMode == 4)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 0.0;
				lineColours[i].y = 1.0;
				lineColours[i].z = 1.0;
			}
		}
		else if (lineColourMode == 5)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 1.0;
				lineColours[i].y = 0.0;
				lineColours[i].z = 1.0;
			}
		}
		else if (lineColourMode == 6)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = 1.0;
				lineColours[i].y = 1.0;
				lineColours[i].z = 0.0;
			}
		}
		else if (lineColourMode == 7)
		{
			for (i = 0; i < pointsToFollow; i++)
			{
				lineColours[i].x = myRandom();
				lineColours[i].y = myRandom();
				lineColours[i].z = myRandom();
			}
		}
		else if (lineColourMode == 8)
		{
			if (randomMass == 0)
			{
				for (i = 0; i < pointsToFollow; i++)
				{
					lineColours[i].x = 1.0;
					lineColours[i].y = 0.0;
					lineColours[i].z = 0.0;
				}
			}
			else
			{
				for (i = 0; i < pointsToFollow; i++)
				{
					lineColours[i].x = points[i].mass / randomMass;
					lineColours[i].y = 0.0;
					lineColours[i].z = 1.0 - (points[i].mass / randomMass);
				}
			}
		}
	}
}

void reset()
{
	lat = 45.0;
	lon = 135.0;
	r = 500.0;
	
	initPoints(noOfPoints);
	initLines(pointsToFollow, noOfLines);
	head = 0;
	occupied = 0;
	ticks = 0;
}

void processPoints()
{
	int i, j;
	double dX, dY, dZ, dist, f, fi, fj;
	double angleX, angleY, angleZ,
			 compXi, compYi, compZi,
			 compXj, compYj, compZj;
	
	for (i = 0; i < noOfPoints; i++)
		accel[i].x = accel[i].y = accel[i].z = 0.0;
	
	/* First, calculate _just_ acceleration */
	
	for (i = 0; i < noOfPoints; i++)
		for (j = i; j < noOfPoints; j++)
			if (i != j)
			{
				/* First, calculate the distance between the points, and therefore,
					the force of attraction between them */
				
				dX = points[j].coords.x - points[i].coords.x;
				dY = points[j].coords.y - points[i].coords.y;
				dZ = points[j].coords.z - points[i].coords.z;
				
				dist = sqrt(pow(dX, 2.0) + pow(dY, 2.0) + pow(dZ, 2.0));
				
				f = points[i].mass * points[j].mass;
				f /= pow(dist, 2.0);
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
	
	for (i = 0; i < noOfPoints; i++)
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
	
	for (i = 0; i < noOfPoints; i++)
	{
		points[i].coords.x += points[i].vel.x;
		points[i].coords.y += points[i].vel.y;
		points[i].coords.z += points[i].vel.z;
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
									 lineColours[i].x,
									 (((double) (noOfLines - k)) /
									 ((double) noOfLines)) *
									 lineColours[i].y,
									 (((double) (noOfLines - k)) /
									 ((double) noOfLines)) *
									 lineColours[i].z);
						
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
									 lineColours[i].x,
									 (((double) (noOfLines - k)) /
									 ((double) noOfLines)) *
									 lineColours[i].y,
									 (((double) (noOfLines - k)) /
									 ((double) noOfLines)) *
									 lineColours[i].z);
						
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
	//		 thisTime.tv_sec, thisTime.tv_usec, lastTime.tv_sec, lastTime.tv_usec,
	//		 diff);
	
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
	
	glutSwapBuffers();
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

void makeAxes() {
// Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      //glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f((float) AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, (float) AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, (float) AXIS_SIZE);
    glEnd();
  glEndList();
}

void menu(int menuEntry)
{
	switch (menuEntry)
	{
		case 1:
			if (axisEnabled == 1)
				axisEnabled = 0;
			else
				axisEnabled = 1;
			
			break;
		case 7:
			reset();
			break;
		case 8:
			quit();
			break;
	}
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

void pointCountMenu(int value)
{
	int thisNoOfPoints = 0;
	
	switch (value)
	{
		case 1:
			thisNoOfPoints = 100;
			break;
		case 2:
			thisNoOfPoints = 200;
			break;
		case 3:
			thisNoOfPoints = 300;
			break;
		case 4:
			thisNoOfPoints = 400;
			break;
		case 5:
			thisNoOfPoints = 500;
			break;
		case 6:
			thisNoOfPoints = 600;
			break;
		case 7:
			thisNoOfPoints = 750;
			break;
		case 8:
			thisNoOfPoints = 1000;
			break;
		case 9:
			thisNoOfPoints = 1500;
			break;
		case 10:
			thisNoOfPoints = 3000;
			break;
	}
	
	lat = 45.0;
	lon = 135.0;
	r = 500.0;
	
	initPoints(thisNoOfPoints);
	
	if (thisNoOfPoints < pointsToFollow)
		initLines(thisNoOfPoints, noOfLines);
	else
		initLines(pointsToFollow, noOfLines);
	
	head = 0;
	occupied = 0;
	ticks = 0;
}

void volumeSizeMenu(int value)
{
	switch (value)
	{
		case 1:
			volumeSize = 100.0;
			break;
		case 2:
			volumeSize = 300.0;
			break;
		case 3:
			volumeSize = 500.0;
			break;
		case 4:
			volumeSize = 700.0;
			break;
		case 5:
			volumeSize = 1000.0;
			break;
		case 6:
			volumeSize = 1500.0;
			break;
		case 7:
			volumeSize = 2500.0;
			break;
		case 8:
			volumeSize = 5000.0;
			break;
	}
	
	reset();
}

void massMenu(int value)
{
	switch (value)
	{
		case 1:
			randomMass = 0;
			break;
		case 2:
			randomMass = 1;
			break;
		case 3:
			randomMass = 2;
			break;
		case 4:
			randomMass = 3;
			break;
		case 5:
			randomMass = 5;
			break;
		case 6:
			randomMass = 8;
			break;
		case 7:
			randomMass = 12;
			break;
		case 8:
			randomMass = 20;
			break;
	}
	
	reset();
}

void gMenu(int value)
{
	switch (value)
	{
		case 1:
			G = 0.0625;
			break;
		case 2:
			G = 0.125;
			break;
		case 3:
			G = 0.25;
			break;
		case 4:
			G = 0.5;
			break;
		case 5:
			G = 1.0;
			break;
		case 6:
			G = 2.0;
			break;
	}
}

void lineLengthMenu(int value)
{
	int thisNoOfLines = 0;
	
	switch (value)
	{
		case 1:
			thisNoOfLines = 50;
			break;
		case 2:
			thisNoOfLines = 125;
			break;
		case 3:
			thisNoOfLines = 250;
			break;
		case 4:
			thisNoOfLines = 500;
			break;
		case 5:
			thisNoOfLines = 750;
			break;
		case 6:
			thisNoOfLines = 1000;
			break;
		case 7:
			thisNoOfLines = 1500;
			break;
		case 8:
			thisNoOfLines = 2000;
			break;
		case 9:
			thisNoOfLines = 4000;
			break;
	}
	
	initLines(pointsToFollow, thisNoOfLines);
	
	head = 0;
	occupied = 0;
	ticks = 0;
}

void followCountMenu(int value)
{
	int thisPointsToFollow = 0;
	
	switch (value)
	{
		case 1:
			thisPointsToFollow = 0;
			break;
		case 2:
			thisPointsToFollow = 1;
			break;
		case 3:
			thisPointsToFollow = 2;
			break;
		case 4:
			thisPointsToFollow = 5;
			break;
		case 5:
			thisPointsToFollow = 10;
			break;
		case 6:
			thisPointsToFollow = 25;
			break;
		case 7:
			thisPointsToFollow = 100;
			break;
		case 8:
			thisPointsToFollow = noOfPoints;
			break;
	}
	
	initLines(thisPointsToFollow, noOfLines);
	
	head = 0;
	occupied = 0;
	ticks = 0;
}

void initVelocityMenu(int value)
{
	switch (value)
	{
		case 1:
			initVelocity = 0.0;
			break;
		case 2:
			initVelocity = 0.5;
			break;
		case 3:
			initVelocity = 1.0;
			break;
		case 4:
			initVelocity = 2.0;
			break;
		case 5:
			initVelocity = 4.0;
			break;
		case 6:
			initVelocity = 10.0;
			break;
	}
	
	reset();
}

void trailColourMenu(int value)
{
	lineColourMode = value;
	
	initLines(pointsToFollow, noOfLines);
	
	head = 0;
	occupied = 0;
	ticks = 0;
}

void initGraphics(int argc, char *argv[])
{
	int pointCountMenuID, volumeSizeMenuID, massMenuID, gMenuID;
	int lineLengthMenuID, followCountMenuID, trailColourMenuID;
	int initVelocityMenuID;

	glutInit(&argc, argv);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutCreateWindow("N-body simulation");
	glutFullScreen();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutReshapeFunc(reshape);
	makeAxes();
	
	glLineWidth(1.0f);
	
	pointCountMenuID = glutCreateMenu(pointCountMenu);
	glutAddMenuEntry("100", 1);
	glutAddMenuEntry("200", 2);
	glutAddMenuEntry("300", 3);
	glutAddMenuEntry("400", 4);
	glutAddMenuEntry("500 (def)", 5);
	glutAddMenuEntry("600", 6);
	glutAddMenuEntry("750", 7);
	glutAddMenuEntry("1000", 8);
	glutAddMenuEntry("1500", 9);
	glutAddMenuEntry("3000", 10);
	
	
	volumeSizeMenuID = glutCreateMenu(volumeSizeMenu);
	glutAddMenuEntry("100", 1);
	glutAddMenuEntry("300", 2);
	glutAddMenuEntry("500 (def)", 3);
	glutAddMenuEntry("700", 4);
	glutAddMenuEntry("1000", 5);
	glutAddMenuEntry("1500", 6);
	glutAddMenuEntry("2500", 7);
	glutAddMenuEntry("5000", 8);
	
	massMenuID = glutCreateMenu(massMenu);
	glutAddMenuEntry("No variation (def)", 1);
	glutAddMenuEntry("1", 2);
	glutAddMenuEntry("2", 3);
	glutAddMenuEntry("3", 4);
	glutAddMenuEntry("5", 5);
	glutAddMenuEntry("8", 6);
	glutAddMenuEntry("12", 7);
	glutAddMenuEntry("20", 8);
	
	gMenuID = glutCreateMenu(gMenu);
	glutAddMenuEntry("0.0625", 1);
	glutAddMenuEntry("0.125", 2);
	glutAddMenuEntry("0.25 (def)", 3);
	glutAddMenuEntry("0.5", 4);
	glutAddMenuEntry("1.0", 5);
	glutAddMenuEntry("2.0", 6);
	
	lineLengthMenuID = glutCreateMenu(lineLengthMenu);
	glutAddMenuEntry("50", 1);
	glutAddMenuEntry("125", 2);
	glutAddMenuEntry("250", 3);
	glutAddMenuEntry("500 (def)", 4);
	glutAddMenuEntry("750", 5);
	glutAddMenuEntry("1000", 6);
	glutAddMenuEntry("1500", 7);
	glutAddMenuEntry("2000", 8);
	glutAddMenuEntry("4000", 9);
	
	followCountMenuID = glutCreateMenu(followCountMenu);
	glutAddMenuEntry("0", 1);
	glutAddMenuEntry("1", 2);
	glutAddMenuEntry("2 (def)", 3);
	glutAddMenuEntry("5", 4);
	glutAddMenuEntry("10", 5);
	glutAddMenuEntry("25", 6);
	glutAddMenuEntry("100", 7);
	glutAddMenuEntry("All", 8);
	
	trailColourMenuID = glutCreateMenu(trailColourMenu);
	glutAddMenuEntry("Red (def)", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("", lineColourMode);
	glutAddMenuEntry("FIREWORKS!", 7);
	glutAddMenuEntry("Mass", 8);
	
	initVelocityMenuID = glutCreateMenu(initVelocityMenu);
	glutAddMenuEntry("+/- 0.0", 1);
	glutAddMenuEntry("+/- 0.25 (def)", 2);
	glutAddMenuEntry("+/- 0.5", 3);
	glutAddMenuEntry("+/- 1.0", 4);
	glutAddMenuEntry("+/- 2.0", 5);
	glutAddMenuEntry("+/- 5.0", 6);
	
	glutCreateMenu(menu);
	glutAddSubMenu("Volume size", volumeSizeMenuID);
	glutAddSubMenu("Particle count", pointCountMenuID);
	glutAddSubMenu("Mass variation", massMenuID);
	glutAddSubMenu("Initial velocity", initVelocityMenuID);
	glutAddSubMenu("Trail length", lineLengthMenuID);
	glutAddSubMenu("Trail count", followCountMenuID);
	glutAddSubMenu("Trail colouring", trailColourMenuID);
	glutAddSubMenu("G", gMenuID);
	glutAddMenuEntry("", 999);
	glutAddMenuEntry("Toggle axes", 1);
	glutAddMenuEntry("Reset", 7);
	glutAddMenuEntry("Quit", 8);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	initPoints(noOfPoints);
	initLines(pointsToFollow, noOfLines);
}

int main(int argc, char** argv)
{
#ifdef __linux
	int opt;
	int optIndex = 0;
	
	while ((opt = getopt_long(argc, argv, "n:d:m:l:v:f:g:c:t:r:b", longOptions,
			  &optIndex)) != -1)
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
	
	srand((unsigned int) time(NULL));
	initGraphics(argc, argv);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	gettimeofday(&lastTime, NULL);
	glutMainLoop();
	
	return EXIT_SUCCESS;
}
