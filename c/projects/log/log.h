#ifndef LOG_H
#define LOG_H

#define LOG_MAX_LEN 1000

#define LOG_INFO	0
#define LOG_WARN	1
#define LOG_ERR	2

#define LOG_FLAG_NONE		0
#define LOG_FLAG_CLOEXEC	1
#define LOG_FLAG_SRC_INFO	2 /* Log also prints out file and line at call! */

#define LOG_IS_FLAG(x) ((flags & x) == x)

extern char logEntry[];

#define logPrint(x,y)		logPrint2(x, y, __FILE__, __LINE__)
extern int logPrint2(const int, const char*, const char*, const int);

extern int logInit(const char*, const int);
extern int logCleanup();

#endif
