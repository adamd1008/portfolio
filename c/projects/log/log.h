#ifndef LOG_H
#define LOG_H

#define _BSD_SOURCE
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

#define LOG_INFO					0x0000
#define LOG_WARN					0x0001
#define LOG_ERR					0x0002
#define LOG_BUG					0x0004
#define LOG_TRACE_NET			0x0010
#define LOG_TRACE_DB				0x0020
#define LOG_ALL					0xffff

#define LOG_FLAG_NONE		0
#define LOG_FLAG_CLOEXEC	1
/* Log sets the CLOEXEC flag on its out file */
#define LOG_FLAG_SRC_INFO	2
/* Log also prints out file and line at call! */

/* Shortcut macros */
#define LOG_PTR_IS_LL(x)	((handle->logLevels & x) == x)
#define LOG_IS_LL(x)			((handle.logLevels & x) == x)
#define LOG_PTR_IS_FLAG(x)	((handle->flags & x) == x)
#define LOG_IS_FLAG(x)		((handle.flags & x) == x)

/* Can't use a variadic macro in conjunction with a variadic function, so sadly
	you need to explicitly type __FILE__ and __LINE__, so use this shortcut! */
#define LOG_ARGS				__FILE__, __LINE__

#define EXITF				exit(EXIT_FAILURE)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	single,
	pthread
} logType_t;
/* When the log type is single, the threadID argument to logPrint() is always
	ignored; when it's pthread, the file stream is wrapped in a mutex (although
	I'm not 100% sure it's necessary!) */

typedef struct
{
	char* logFileStr;
	FILE* logFile;
	struct timeval timeAtStart;
	logType_t type;
	int logLevels;
	int flags;
	pthread_mutex_t mutex;
} log_t;

/*
	logInit()
	
	Args:
		log_t*				handle pointer
		const char*			source file (always use __FILE__ macro)
		const int			line in file (always use __LINE__ macro)
		const int			log level bitmask
		const logType_t	type of logging required, i.e. single- or multi-threaded
*/
int logInit(log_t* handle, const char* fileName, const int reqLogLevels,
				const int reqFlags, const logType_t type);
/*
	logPrint()
	
	Args:
		log_t					handle
		const char*			source file (always use __FILE__ macro)
		const int			line in file (always use __LINE__ macro)
		const int			log level bitmask
*/
int logPrint(log_t handle, const char* file, const int line, const int logLevel,
				 const int threadID, const char* fmt, ...);

/*
	logHexdump()
	
	As logPrint(), except it produces hexdump output of string `str`
*/
int logHexdump(log_t handle, const char* file, const int line,
					const int threadID, const char* str, const int len);

/*
	logHexdumpz()
	
	WARNING: this function is only safe for null-terminated alphanumeric strings!
	It simply calls strlen() and passes to logHexdump()
*/
int logHexdumpz(log_t handle, const char* file, const int line,
					 const int threadID, const char* str);

/*
	logCleanup()
	
	Cleans up log handle
*/
int logCleanup(log_t handle);

#ifdef __cplusplus
}
#endif

#endif
