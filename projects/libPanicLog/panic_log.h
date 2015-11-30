#ifndef _PANIC_LOG_H_
#define _PANIC_LOG_H_

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

/* So I spent a while trying to standardise how much detail there should be
   based on levels 1, 2 and 3 when you factor in messages produced by LOG_INFO.
   
   L1:
      * Print significant events
      * i.e. creation/deletion of a new message, src/dest info
      * But not sending/receiving of messages
   
   L2:
      * Print all actions undertaken
      * i.e. message field adding, sending, querying...
   
   L3:
      * Method level verbose output
      * Print method and arguments given
*/

const char* const uuid = LOG_UUID;

#define LOG_INFO              0x0000
#define LOG_WARN              0x0001
#define LOG_ERR               0x0002
#define LOG_BUG               0x0004
#define LOG_L1                0x0010
#define LOG_L2                0x0020
#define LOG_L3                0x0040
#define LOG_ALL               0xffff

static const char* const logLevelInfoStr = "INFO";
static const char* const logLevelWarnStr = "WARN";
static const char* const logLevelErrStr = "ERR";
static const char* const logLevelBugStr = "BUG";
static const char* const logLevelL1Str = "L1";
static const char* const logLevelL2Str = "L2";
static const char* const logLevelL3Str = "L3";
static const char* const logLevelUnknStr = "UNKN";

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

/* Shortcut macros */
#define LOG_PTR_IS_LL(x)   ((handle->logLevels & x) == x)
#define LOG_IS_LL(x)       ((handle.logLevels & x) == x)
#define LOG_PTR_IS_FLAG(x) ((handle->flags & x) == x)
#define LOG_IS_FLAG(x)     ((handle.flags & x) == x)

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
log_t* logInit(const char* fileName, const int logLevelMask,
               const int flagMask);

const char* logGetLogLevelStr(const int logLevel);

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
void logPrint(log_t* const handle, const char* file, const int line,
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
void logHexdump(log_t* const handle, const char* file, const int line,
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
void logHexdumpz(log_t* const handle, const char* file, const int line,
                 const char* func, const int logLevel, const char* str);

/**
 * Gracefully close a log instance
 * 
 * @param handle Log handle pointer
 * @return       Zero on success; -1 otherwise
 */
int logCleanup(log_t* const handle);

/**
 * Determine whether a particular log level is enabled for the given instance
 * 
 * @param handle   Log handle pointer
 * @param logLevel Log level to test
 * @return         Zero if false; otherwise true
 */
int logGetLogLevel(log_t* const handle, int logLevel);

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
void logBacktrace(log_t* const handle, const char* file, const int line,
                  const char* func);

/**
 * Abort the application and print source location of failure
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 */
void logAbort(log_t* const handle, const char* file, const int line,
              const char* func);

#define logAssert(handle, cond) (void) ((cond) || \
        (__logAssert(handle, __FILE__, __LINE__, __func__, #cond),0))

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
void __logAssert(log_t* const handle, const char* file, const int line,
                 const char* func, const char* cond);

/**
 * Allocate memory, or abort and print log message if out of memory
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 * @param size   Bytes to allocate
 * @return       Pointer to start of  allocated region
 */
void* logMalloc(log_t* const handle, const char* file, const int line,
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
void logLuaStackDump(log_t* const handle, const char* file, const int line,
                     const char* func, lua_State* lua);

#ifdef __cplusplus
}
#endif

#endif
