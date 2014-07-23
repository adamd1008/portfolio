#define _BSD_SOURCE
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
//#include <pthread.h>

//#define MAIN_LOG_SRC
#include "log.h"

#define LOG_MAX_LEN 1000
// FIXME temporary!!

#define PATH_MAX 4096

int flags;
char logFileStr[PATH_MAX];
FILE* logFile;
char logEntry[LOG_MAX_LEN];
struct timeval timeAtStart;
//pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

struct timeval relTime()
{
	struct timeval timeNow;
	struct timeval toReturn;
	
	gettimeofday(&timeNow, NULL);
	
	timersub(&timeNow, &timeAtStart, &toReturn);
	
	return toReturn;
}

int logInit(const char* fileName, const int req_flags)
{
	gettimeofday(&timeAtStart, NULL);
	
	logFileStr[0] = 0;
	
	if ((fileName != NULL) && (fileName[0] != 0))
	{
		if ((logFile = fopen(fileName, "w")) == NULL)
		{
			perror("fopen");
			return -1;
		}
		
		strcpy(logFileStr, fileName);
	}
	else
		logFile = stderr;
	
	setlinebuf(logFile); // Essential!
	
	flags = req_flags;
	
	if (LOG_IS_FLAG(LOG_FLAG_CLOEXEC))
	{
		int logFD = fileno(logFile);
		
		if (logFD == -1)
		{
			perror("fileno");
			return -1;
		}
		
		if ((fcntl(logFD, F_SETFD, FD_CLOEXEC)) == -1)
		{
			perror("fcntl");
			return -1;
		}
	}
	
	return 0;
}

int logPrint2(const int logLevel, const char* str, const char* file,
				  const int line)
{
	int res;
	char logLevelStr[8];
	
	/*
	if ((res = pthread_mutex_lock(&logMutex)) != 0)
	{
		fprintf(stderr, "pthread_mutex_lock returned %d\n", res);
		return -1;
	}
	*/
	
	if (logLevel == 0)
		strcpy(logLevelStr, "INFO:");
	else if (logLevel == 1)
		strcpy(logLevelStr, "WARN:");
	else
		strcpy(logLevelStr, "ERR:");
	
	struct timeval tv = relTime();
	
	if (LOG_IS_FLAG(LOG_FLAG_SRC_INFO))
		fprintf(logFile, "[%5d.%.06d] <%s:%d> %s %s\n",
				  tv.tv_sec, tv.tv_usec, file, line, logLevelStr, str);
	else
		fprintf(logFile, "[%5d.%.06d] %s %s\n",
				  tv.tv_sec, tv.tv_usec, logLevelStr, str);
	
	/*
	if ((res = pthread_mutex_unlock(&logMutex)) != 0)
	{
		fprintf(stderr, "pthread_mutex_unlock returned %d\n", res);
		return -1;
	}
	*/
	
	return 0;
}

int logCleanup()
{
	logPrint(LOG_INFO, "Logging stopped");
		
	if (logFileStr[0] != 0)
		if ((fclose(logFile)) != 0)
		{
			perror("fclose");
			return -1;
		}
	
	return 0;
}
