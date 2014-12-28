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

/* POSIX threads variables and structures */

pthread_t* threads;
int noOfThreads = -1;
int partsPerThread;

#ifdef NBODY_USE_SPINLOCKS
pthread_spinlock_t

/* Other vars */

part_t* parts;

FILE* file;

double resolution;
const double G = 0.00000000006673; /* Gravitational const G = 6.673 x 10^-11 */

int iteration = 0;

struct option longOptions[] = {{"resolution", required_argument, NULL, 'r'},
										 {"threads", required_argument, NULL, 't'},
										 {NULL, 0, NULL, 0}};

log_t logHandle;

/* Functions to call during the simulation */

void (*step)(int);
/* The function to `step' each particle each iteration */

void (*regulariseCheck)(int);
void (*regularise)();
/* The regularisation functions
	
	The singularity in a discrete inverse-square simulation presents a serious
	problem. An N-body simulator is basically useless and totally inaccurate if
	something is not done about it. My first idea is to make particles collide
	and coalesce if they come too close. */

void* mainLoop(void* _tID)
{
	int tID = *((int*) _tID);
	/* So, the thread has an ID such that 0 <= tID < noOfThreads. The offset into
		the particles that this thread is responsible for is calculated using
		(totalParts / noOfThreads) * tID. We already know that the noOfThreads
		divides absolutely into totalParts. */
	
	int offset = (totalParts / noOfThreads) * tID;
	
}

void initThreads()
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

void waitForEnd()
{
	int i;
	
	for (i = 0; i < noOfThreads; i++)
		if ((pthread_join(&threads[i], NULL)) != 0)
		{
			logPrint(logHandle, LOG_ARGS, NULL, LOG_ERR, "Error in pthread_join");
			log
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
	
	if (noOfThreads <= 0)
	{
		logPrint(logHandle, LOG_ARGS, NULL, LOG_ERR, "Must specify a valid "
					"number of threads (-t)");
		logCleanup(logHandle);
		
		EXITF;
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
	
	partsPerThread = totalParts / noOfThreads;
	
	initParticles();
	initThreads();
	
	waitForEnd();
	
	return EXIT_SUCCESS;
}
