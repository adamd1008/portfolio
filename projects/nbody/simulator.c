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
#include "panic_log.h"
#include "simulator.h"
#include "particle.h"

log_t logHandle;

part_t* parts;
pthread_t* threads;
int noOfThreads;
int partsPerThread;

double resolution;
const double G = 0.00000000006673; /* Gravitational const G = 6.673 x 10^-11 */

int iteration = 0;

struct option longOptions[] = {{"resolution", required_argument, NULL, 'r'},
										 {"threads", required_argument, NULL, 't'},
										 {NULL, 0, NULL, 0}};

log_t logHandle;

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

void* mainLoop(void* _tID)
{
	int tID = *((int*) _tID);
	int offset = 
	
}

int main(int argc, char** argv)
{
	int opt;
	
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
	initParticles(noOfParts, volumeSize, initVelocity, mass);
	
	mainLoop();
	
	cleanup(EXIT_SUCCESS);
}
