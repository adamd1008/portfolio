#ifndef _PANIC_LOG_H_
#define _PANIC_LOG_H_

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define LOG_INFO              0x0000
#define LOG_WARN              0x0001
#define LOG_ERR               0x0002
#define LOG_BUG               0x0004
#define LOG_L1                0x0010
#define LOG_L2                0x0020
#define LOG_L3                0x0040
#define LOG_ALL               0xffff

#define LOG_FLAG_NONE            0x00
#define LOG_FLAG_CLOEXEC         0x01
/* Log sets the CLOEXEC flag on its out file */
#define LOG_FLAG_SRC_INFO        0x02
/* Log also prints out file and line at call! */
#define LOG_FLAG_STDOUT          0x04
/* When no log file is specified, output to stdout instead */
#define LOG_FLAG_ENABLE_ASSERTS  0x08
/* Asserts are active with this flag set */
#define LOG_FLAG_ALL             0xff

/* Can't use a variadic macro in conjunction with a variadic function, so sadly
   you need to explicitly type __FILE__ and __LINE__, so use this shortcut! */
#define LOG_ARGS           __FILE__, __LINE__, __func__

#define EXITF           exit(EXIT_FAILURE)

typedef struct
{
   char* logFileStr;
   FILE* logFile;
   struct timeval timeAtStart;
   int logLevels;
   int flags;
} log_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise a new logging instance
 * 
 * @param fileName     File path of output log file (can be NULL to indicate
 *                     output to stderr/stdout
 * @param logLevelMask Mask of log levels that this instance will output
 * @param flagMask     Mask of flags to set parameters
 * @return             Const pointer to the instance (or NULL on failure)
 */
log_t* logInit(const char* fileName,  const char* file, const int line,
               const char* func, const int logLevelMask, const int flagMask);

/**
 * Print a variadic message to the log file
 * 
 * @param handle   Log handle pointer
 * @param file     Source file name (__FILE__ from LOG_ARGS)
 * @param line     Source line number (__LINE__ from LOG_ARGS)
 * @param func     Current function (__func__ from LOG_ARGS)
 * @param logLevel Log level of this message
 * @param fmt      Variadic message format
 * @param ...      Variadic arguments
 */
void logPrint(const log_t* handle, const char* file, const int line,
              const char* func, const int logLevel, const char* fmt, ...);

/**
 * Print a hexdump of the given string to the log file
 * 
 * Note: this function is for strings that are not null-terminated, and/or
 * contains nulls. For null-terminated strings, use logHexdumpz().
 * 
 * @param handle   Log handle pointer
 * @param file     Source file name (__FILE__ from LOG_ARGS)
 * @param line     Source line number (__LINE__ from LOG_ARGS)
 * @param func     Current function (__func__ from LOG_ARGS)
 * @param logLevel Log level of this message
 * @param str      String to output
 * @param len      Length of string
 */
void logHexdump(const log_t* handle, const char* file, const int line,
                const char* func, const int logLevel, const char* str,
                const int len);

/**
 * Wrapper for logHexdump() for null-terminated strings
 * 
 * @param handle   Log handle pointer
 * @param file     Source file name (__FILE__ from LOG_ARGS)
 * @param line     Source line number (__LINE__ from LOG_ARGS)
 * @param func     Current function (__func__ from LOG_ARGS)
 * @param logLevel Log level of this message
 * @param str      Null-terminated string to output
 */
void logHexdumpz(const log_t* handle, const char* file, const int line,
                 const char* func, const int logLevel, const char* str);

/**
 * Gracefully exit the application. No need to call logCleanup(), this method
 * does that.
 * 
 * Never returns - because it exits the application!
 * 
 * @param handle   Log handle pointer
 * @param file     Source file name (__FILE__ from LOG_ARGS)
 * @param line     Source line number (__LINE__ from LOG_ARGS)
 * @param func     Current function (__func__ from LOG_ARGS)
 * @param exitCode Code to exit with
 */
void logExit(const log_t* handle, const char* file, const int line,
             const char* func, int exitCode);

/**
 * Gracefully close a log instance
 * 
 * @param handle Log handle pointer
 */
void logCleanup(const log_t* handle, const char* file, const int line,
                const char* func);

/**
 * Determine whether a particular log level is enabled for the given instance
 * 
 * @param handle   Log handle pointer
 * @param logLevel Log level to test
 * @return         Zero if false; otherwise true
 */
int logGetLogLevel(const log_t* handle, int logLevel);

/**
 * Print a backtrace to the log file
 * 
 * Note that applications should be compiled with the `-rdynamic` switch for the
 * information printed by this function to be meaningful.
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 */
void logBacktrace(const log_t* handle, const char* file, const int line,
                  const char* func);

/**
 * Abort the application and print source location of failure
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 */
void logAbort(const log_t* handle, const char* file, const int line,
              const char* func);

/**
 * Perform an assertion and output to log and abort in the event of failure
 * 
 * Note: use logAssert(handle ptr, assertion)
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 * @param cond   String representation of assertion failure
 */
void __logAssert(const log_t* handle, const char* file, const int line,
                 const char* func, const char* cond);

#define logAssert(handle, cond)                              \
   ((cond)                                                                     \
    ? (void) (0)                                                               \
    : __logAssert(handle, __FILE__, __LINE__, __func__, #cond))

/**
 * Allocate memory, or abort and print log message if out of memory
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 * @param size   Bytes to allocate
 * @return       Pointer to start of allocated region
 */
void* logMalloc(const log_t* handle, const char* file, const int line,
                const char* func, size_t size);

/**
 * Dump the stack of a Lua instance to the log file
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 * @param lua    Lua state instance
 */
void logLuaStackDump(const log_t* handle, const char* file, const int line,
                     const char* func, lua_State* lua);

#ifdef __cplusplus
}
#endif

#endif
