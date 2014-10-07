#ifndef LOG_H
#define LOG_H

#define _BSD_SOURCE
#include <sys/time.h>

#define LOG_INFO					0x0000
#define LOG_WARN					0x0001
#define LOG_ERR					0x0002
#define LOG_BUG					0x0004
#define LOG_TRACE_NET			0x0010
#define LOG_TRACE_DB				0x0020
/*#define LOG_TRACE_METHODS		0x1000 */
/* Surely this is too anal? Use a debugger to get this info! */
#define LOG_ALL					0xffff

#define LOG_FLAG_NONE		0
#define LOG_FLAG_CLOEXEC	1
#define LOG_FLAG_SRC_INFO	2 /* Log also prints out file and line at call! */

#define LOG_PTR_IS_LL(x)	((handle->logLevels & x) == x)
#define LOG_IS_LL(x)			((handle.logLevels & x) == x)

#define LOG_PTR_IS_FLAG(x)	((handle->flags & x) == x)
#define LOG_IS_FLAG(x)		((handle.flags & x) == x)

#define LOG_ARGS				__FILE__, __LINE__

#define EXITF				exit(EXIT_FAILURE)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	single,
	pthread,
	mpi
} logType_t;

typedef struct
{
	char* logFileStr;
	FILE* logFile;
	struct timeval timeAtStart;
	logType_t type;
	int* threadID;
	int logLevels;
	int flags;
} log_t;

/*#define logPrint(x, y, z)			logPrint2(x, __FILE__, __LINE__, y, z)
extern int logPrint2(log_t, const char*, const int, const int, char*);
#define logPrint(x,y,z,...)	logPrint2v(x, __FILE__, __LINE__, y, z, __VA_ARGS__) */

/* XXX
	Note: variadic macros don't work in conjunction with variadic functions!
*/

extern int logPrint(log_t, const char*, const int, const int, const char*, ...);

extern int logInit(log_t*, const char*, const int, const int, const logType_t,
						 int*);
extern int logHexdump(log_t, const char*, const int, const char*, const int);
extern int logHexdumpz(log_t, const char*, const int, const char*);
extern int logCleanup();

#ifdef __cplusplus
}
#endif

#endif
