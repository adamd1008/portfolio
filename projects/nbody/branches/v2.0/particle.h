#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#define PART_TYPE_STAR		0
#define PART_TYPE_PLANET	1
#define PART_TYPE_ASTEROID	2
#define PART_TYPE_COMET		3

#define STAR_MIN_DIST

double minDist[4] = {0.0, 40000000000.0, 10000.0, 10000000.0};
double maxDist[4] = {0.0, 10000000000000.0, 10000000000000.0,
							10000000000000.0};

/*double minSpeed[4] = {0.0, 4000.0, 1000.0, 1000.0};
double maxSpeed[4] = {0.0, 50000.0, 25000.0, 50000.0};*/

double minSpeed[4] = {0.0, 18000.0, 8000.0, 15000.0};
double maxSpeed[4] = {0.0, 35000.0, 16000.0, 35000.0};

double minMass[4] = {1000000000000000000000000000000.0,
							10000000000000000000000.0,
							100000000000000.0,
							1000000000.0};
double maxMass[4] = {1000000000000000000000000000000.0,
							100000000000000000000000000.0,
							100000000000000000000.0,
							10000000000000.0};

double colours[4][4] = {{1.0, 0.0, 0.0, 1.0}, /* Stars are RED */
								{0.0, 1.0, 0.0, 1.0}, /* Planets are GREEN */
								{1.0, 1.0, 1.0, 1.0}, /* Asteroids are WHITE */
								{0.0, 1.0, 1.0, 1.0}}; /* Comets are CYAN */
/* Aren't colours redundant for the simulator? */

int totalPartTypes[4] = {1, 50, 500, 1000};
int totalParts;

typedef struct _part_t
{
	int id;
	double coords[3];
	double vel[3];
	double accel[3];
	double mass;
	/*double radius;
	double colour[3];*/
	int type;
	int collided;
	
	struct _part_t* next;
	
	/*float3_t colour[3]; // Derived from mass and mass variance */
} part_t;

int initParticles();
void freeParticles();
void processParts();

#endif
