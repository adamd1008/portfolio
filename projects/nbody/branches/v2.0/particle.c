#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "simulator.h"
#include "particle.h"

double getRandom(double min, double max)
{
	double ret = (double) rand() / (double) RAND_MAX;
	double range = max - min;
	
	ret *= range;
	ret += min;
	
	logPrint(logHandle, LOG_ARGS, "RND", LOG_L3, "getRandom() min = %f, "
				"max = %f, range = %f, ret = %f", min, max, range, ret);
	
	return ret;
}

int initParticles()
{
	int i, j, k;
	
	if (parts != NULL)
		free(parts);
	
	parts = malloc(sizeof(part_t) * totalParts);
	
	if (parts == NULL)
	{
		sprintf(logEntry, "malloc: Out of memory");
		logPrint(LOG_ERR, logEntry);
		
		return -1;
	}
	
	k = 0;
	
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < totalPartTypes[i]; j++)
		{
			parts[j + k].id = j + k;
			
			parts[j + k].coords[0] = getRandom(minDist[i], maxDist[i]);
			parts[j + k].coords[1] = getRandom(minDist[i], maxDist[i]);
			parts[j + k].coords[2] = getRandom(minDist[i], maxDist[i]);
			
			parts[j + k].vel[0] = getRandom(minSpeed[i], maxSpeed[i]);
			parts[j + k].vel[1] = getRandom(minSpeed[i], maxSpeed[i]);
			parts[j + k].vel[2] = getRandom(minSpeed[i], maxSpeed[i]);
			
			parts[j + k].accel[0] = 0.0;
			parts[j + k].accel[1] = 0.0;
			parts[j + k].accel[2] = 0.0;
			
			parts[j + k].mass = getRandom(minMass[i], maxMass[i]);
			
			/*parts[j + k].radius = cbrt((3.0 * parts[j + k].mass) /
												(4 * pi * 5514.0));*/
			
			parts[j + k].type = i;
		}
		
		k = j;
	}
	
	return 0;
}

void freeParticles()
{
	free(parts);
	parts = NULL;
}

void processParts()
{
	int i, j;
	double dist, dist2, fg, fi, fj;
	double d[3];
	double angle[3];
	double compi[3];
	double compj[3];
	
	for (i = 0; i < noOfParts; i++)
	{
		parts[i].accel[0] = 0.0;
		parts[i].accel[1] = 0.0;
		parts[i].accel[2] = 0.0;
	}
	
	for (i = 0; i < noOfParts; i++)
		for (j = i; j < noOfParts; j++)
			if (i != j)
			{
				/* First, calculate the distance between the points, and therefore,
					the force of attraction between them.
					
					The gravity calculations below are a slight optimisation.
					Consider:
					
					F = (G * m_1 * m_2) / r^2
					F = ma
					
					Thus:
					
					a_1 = (G * m_2) / r^2
					a_2 = (G * m_1) / r^2
					
					We only need to calculate (G / r^2) ONCE and store in `fg`
					(instead of twice)
					
					=>
					
					a_1 = fg * m_2
					a_2 = fg * m_1
				*/
				
				d[0] = parts[j].coords[0] - parts[i].coords[0];
				d[1] = parts[j].coords[1] - parts[i].coords[1];
				d[2] = parts[j].coords[2] - parts[i].coords[2];
				
				dist = sqrt((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2]));
				dist2 = dist * dist;
				fg = G / dist2;
				fi = fg * parts[j].mass;
				fj = fg * parts[i].mass;
				
				angle[0] = acos(d[0] / dist);
				angle[1] = acos(d[1] / dist);
				angle[2] = acos(d[2] / dist);
				
				compi[0] = cos(angle[0]) * fi;
				compi[1] = cos(angle[1]) * fi;
				compi[2] = cos(angle[2]) * fi;
				
				compj[0] = cos(angle[0]) * fj;
				compj[1] = cos(angle[1]) * fj;
				compj[2] = cos(angle[2]) * fj;
				
				parts[i].accel[0] += compi[0];
				parts[i].accel[1] += compi[1];
				parts[i].accel[2] += compi[2];
				
				parts[j].accel[0] -= compj[0];
				parts[j].accel[1] -= compj[1];
				parts[j].accel[2] -= compj[2];
			}
	
	for (i = 0; i < noOfParts; i++)
	{
		parts[i].vel[0] += parts[i].accel[0];
		parts[i].coords[0] += parts[i].vel[0];
		
		parts[i].vel[1] += parts[i].accel[1];
		parts[i].coords[1] += parts[i].vel[1];
		
		parts[i].vel[2] += parts[i].accel[2];
		parts[i].coords[2] += parts[i].vel[2];
	}
}































