#define _BSD_SOURCE
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>
#include "log.h"

#define LOG_MAX_LEN PIPE_BUF

struct timeval relTime(struct timeval timeAtStart)
{
	struct timeval toReturn;
	struct timeval timeNow;
	
	gettimeofday(&timeNow, NULL);
	
	timersub(&timeNow, &timeAtStart, &toReturn);
	
	return toReturn;
}

int logInit(log_t* handle, const char* fileName, const int reqLogLevels,
				const int reqFlags, const logType_t type, int* threadID)
{
	handle->logFileStr = malloc(sizeof(char) * PATH_MAX);
	
	if (handle->logFileStr == NULL)
		return -1;
	
	gettimeofday(&handle->timeAtStart, NULL);
	
	handle->logFileStr[0] = 0;
	
	if ((fileName != NULL) && (fileName[0] != 0))
	{
		if ((handle->logFile = fopen(fileName, "w")) == NULL)
		{
			perror("fopen");
			return -1;
		}
		
		strcpy(handle->logFileStr, fileName);
	}
	else
		handle->logFile = stderr;
	
	setlinebuf(handle->logFile); /* Essential! */
	
	handle->logLevels = reqLogLevels;
	handle->flags = reqFlags;
	
	if (LOG_PTR_IS_FLAG(LOG_FLAG_CLOEXEC))
	{
		int logFD = fileno(handle->logFile);
		
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
	
	handle->type = type;
	handle->threadID = threadID;
	
	if ((type != single) && (threadID == NULL))
	{
		fprintf(stderr, "threadID cannot be null when multi-threaded!\n");
		EXITF;
	}
	
	char typeStr[16];
	
	if (type == single)
		strcpy(typeStr, "single");
	else if (type == pthread)
		strcpy(typeStr, "pthread");
	else if (type == mpi)
		strcpy(typeStr, "mpi");
	else
		strcpy(typeStr, "unknown");
	
	logPrint(*handle, LOG_ARGS, LOG_INFO, "Log library started (v%d.%d.%d)",
				MAJOR_VER, MINOR_VER, BUILD_VER);
	
	if (threadID != NULL)
		logPrint(*handle, LOG_ARGS, LOG_INFO, "threadID = %d, type = %s",
					*threadID, typeStr);
	else
		logPrint(*handle, LOG_ARGS, LOG_INFO, "type = %s", typeStr);
	
	logPrint(*handle, LOG_ARGS, LOG_INFO, "loglevels mask = 0x%.08x",
				reqLogLevels);
	
	return 0;
}

int logPrint(log_t handle, const char* file, const int line,
				  const int logLevel, const char* fmt, ...)
{
	if (LOG_IS_LL(logLevel))
	{
		va_list ap;
		char logLevelStr[8];
		char logEntry[LOG_MAX_LEN];
		
		switch (logLevel)
		{
			case LOG_INFO:
				strcpy(logLevelStr, "INFO");
				break;
			case LOG_WARN:
				strcpy(logLevelStr, "WARN");
				break;
			case LOG_ERR:
				strcpy(logLevelStr, "ERR");
				break;
			case LOG_BUG:
				strcpy(logLevelStr, "BUG");
				break;
			case LOG_TRACE_NET:
				strcpy(logLevelStr, "NET*");
				break;
			case LOG_TRACE_DB:
				strcpy(logLevelStr, "DB*");
				break;
			default:
				strcpy(logLevelStr, "UNKN");
				break;
		}
		
		va_start(ap, fmt);
		vsnprintf(logEntry, LOG_MAX_LEN, fmt, ap);
		va_end(ap);
		
		struct timeval tv = relTime(handle.timeAtStart);
		
		if (LOG_IS_FLAG(LOG_FLAG_SRC_INFO))
		{
			if (handle.type == single)
				fprintf(handle.logFile, "[%5d.%.06d] <%s:%d> %s: %s\n",
						  tv.tv_sec, tv.tv_usec, file, line, logLevelStr, logEntry);
			else
				fprintf(handle.logFile, "[%5d.%.06d] <%s:%d> #%d %s: %s\n",
						  tv.tv_sec, tv.tv_usec, file, line, *handle.threadID,
						  logLevelStr, logEntry);
		}
		else
		{
			if (handle.type == single)
				fprintf(handle.logFile, "[%5d.%.06d] %s: %s\n",
						  tv.tv_sec, tv.tv_usec, logLevelStr, logEntry);
			else
				fprintf(handle.logFile, "[%5d.%.06d] #%d %s: %s\n",
						  tv.tv_sec, tv.tv_usec, *handle.threadID, logLevelStr,
						  logEntry);
		}
	}
	
	return 0;
}

int logHexdump(log_t handle, const char* file, const int line, const char* str,
					const int len)
{
	int i, j;
	struct timeval tv = relTime(handle.timeAtStart);
	
	if (LOG_IS_FLAG(LOG_FLAG_SRC_INFO))
	{
		if (handle.type == single)
			fprintf(handle.logFile, "[%5d.%.06d] <%s:%d> HEX:\n", tv.tv_sec,
					  tv.tv_usec, file, line);
		else
			fprintf(handle.logFile, "[%5d.%.06d] <%s:%d> #%d HEX:\n", tv.tv_sec,
					  tv.tv_usec, file, line, *handle.threadID);
	}
	else
	{
		if (handle.type == single)
			fprintf(handle.logFile, "[%5d.%.06d] HEX:\n", tv.tv_sec, tv.tv_usec);
		else
			fprintf(handle.logFile, "[%5d.%.06d] #%d HEX:\n", tv.tv_sec,
					  tv.tv_usec, *handle.threadID);
	}
	
	fprintf(handle.logFile, "---BEGIN_HEX---");
	
	for (i = 0; i < len; i++)
	{
		if ((i % 16) == 0)
			fprintf(handle.logFile, "\n%.08x  ", i);
		
		fprintf(handle.logFile, "%.02hhx ", str[i]);
		
		if (i > 0)
		{
			if ((i % 8) == 7)
				fputc(' ', handle.logFile);
		
			if ((i % 16) == 15)
			{
				/* Use isprint() to print the chars, then print offset after new
					line */
				
				fputc('|', handle.logFile);
				
				for (j = i - 15; j <= i; j++)
					if (isprint(str[j]))
						fputc(str[j], handle.logFile);
					else
						fputc('.', handle.logFile);
				
				fputc('|', handle.logFile);
			}
		}
	}
	
	/* What if the hexdump didn't finish perfectly at (i % 16) == 0? */
	
	if ((i % 16) != 0)
	{
		/* Calculate the number of spaces to add to the ASCII area */
		int spaces = (16 - (i % 16)) * 3 + 1;
		
		if ((i % 16) < 8)
			spaces++;
			/* Don't forget the space in the middle! */
		
		for (j = 0; j < spaces; j++)
			fputc(' ', handle.logFile);
		
		fputc('|', handle.logFile);
		
		for (j = i - (i % 16); j < i; j++)
			if (isprint(str[j]))
				fputc(str[j], handle.logFile);
			else
				fputc('.', handle.logFile);
		
		fputc('|', handle.logFile);
	}
	
	fprintf(handle.logFile, "\n%.08x\n---END_HEX---\n", i);
	return 0;
}

int logHexdumpz(log_t handle, const char* file, const int line, const char* str)
{
	return logHexdump(handle, file, line, str, strlen(str));
}

int logCleanup(log_t handle)
{
	logPrint(handle, LOG_ARGS, LOG_INFO, "Logging stopped");
	
	free(handle.logFileStr);
	
	if (handle.logFileStr[0] != 0)
		if ((fclose(handle.logFile)) != 0)
		{
			perror("fclose");
			return -1;
		}
	
	return 0;
}
