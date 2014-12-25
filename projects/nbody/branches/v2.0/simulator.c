#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <linux/limits.h>
#include <pthread.h>
#include "simulator.h"

log_t logHandle;

part_t* parts;
pthread_t* threads;
int noOfThreads;
int partsPerThread;
FILE* file;

double resolution;
const double G = 0.00000000006673; /* Gravitational const G = 6.673 x 10^-11 */

int iteration = 0;

struct option longOptions[] = {{"resolution", required_argument, NULL, 'r'},
										 {"threads", required_argument, NULL, 't'},
										 {NULL, 0, NULL, 0}};

log_t logHandle;

/* Functions to call during the simulation */

void (*step)();
/* The function to `step' each particle each iteration */

void (*regularise)();
/* The regularisation function
	
	The singularity in a discrete inverse-square simulation presents a serious
	problem. An N-body simulator is basically useless and totally inaccurate if
	something is not done about it. My first idea is to make particles collide
	and coalesce if they come too close. */

void* mainLoop(void* _tID)
{
	int tID = *((int*) _tID);
	int offset = 
	
}

void init()
{
	int i, res;
	
	threads = malloc(sizeof(pthread_t) * noOfThreads);
	
	if (threads == NULL)
	{
		logPrint(logHandle, LOG_ARGS, NULL, LOG_ERR, "malloc: Out of memory");
		logCleanup(logHandle);
		
		EXITF;
	}
	
	for (i = 0; i < noOfThreads; i++)
		if ((res = pthread_create(&threads[i], NULL, mainLoop, (void*) &i)) != 0)
		{
			logPrint(logHandle, LOG_ARGS, NULL, LOG_ERR, "pthread_create: "
						"returned %d", res);
			logCleanup(logHandle);
			
			EXITF;
		}
}

int main(int argc, char** argv)
{
	int opt;
	int optIndex = 0;
	
	logInit(&logHandle, NULL, LOG_ALL, LOG_FLAG_CLOEXEC | LOG_FLAG_SRC_INFO);
	
	srand((unsigned int) time(NULL));
	
	while ((opt == getopt_long(argc, argv, "r:t:", longOptions, &optIndex))
			 != -1)
	{
		switch (opt)
		{
			case 'r':
				resolution = atof(optarg);
				break;
			case 't':
				noOfThreads = atoi(optarg);
				break;
		}
	}
	
	totalParts = totalPartTypes[0] + totalPartTypes[1] + totalPartTypes[2] +
					 totalPartTypes[3];
	
	if ((totalParts % noOfThreads) != 0)
	{
		logPrint(logHandle, LOG_ARGS, NULL, LOG_ERR, "totalParts %% noOfThreads "
					"= %d", totalParts % noOfThreads);
		logCleanup(logHandle);
		
		EXITF;
	}
	
	init();
	initParticles();
	
	mainLoop();
	
	cleanup(EXIT_SUCCESS);
}
