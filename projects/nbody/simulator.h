#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#define EXITF exit(EXIT_FAILURE)

#define SIM_STATUS_INIT			0
#define SIM_STATUS_RUN			1
#define SIM_STATUS_PAUSED		2
#define SIM_STATUS_CLEANUP		3

#define SIM_LINE_SIZE 128

extern log_t logHandle;

extern part_t* parts;
extern pthread_t* threads;
extern int noOfThreads;
extern int partsPerThread;
extern FILE* file;

extern int iteration;

#endif
