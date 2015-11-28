#define _BSD_SOURCE
#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "panic_log.h"

#define LOG_MAX_LEN PIPE_BUF

struct timeval relTime(struct timeval timeAtStart)
{
   struct timeval toReturn;
   struct timeval timeNow;
   
   gettimeofday(&timeNow, NULL);
   
   timersub(&timeNow, &timeAtStart, &toReturn);
   
   return toReturn;
}

log_t* logInit(const char* fileName, const int logLevelMask,
               const int flagMask)
{
   log_t* const handle = malloc(sizeof(log_t));
   
   if (handle == NULL)
      return NULL;
   
   handle->logFileStr = malloc(sizeof(char) * PATH_MAX);
   
   if (handle->logFileStr == NULL)
      return NULL;
   
   gettimeofday(&handle->timeAtStart, NULL);
   
   handle->logFileStr[0] = 0;
   handle->logLevels = logLevelMask;
   handle->flags = flagMask;
   
   if ((fileName != NULL) && (fileName[0] != 0))
   {
      if ((handle->logFile = fopen(fileName, "w")) == NULL)
      {
         perror("fopen");
         return NULL;
      }
      
      strcpy(handle->logFileStr, fileName);
   }
   else
      if (LOG_PTR_IS_FLAG(LOG_FLAG_STDOUT))
         handle->logFile = stdout;
      else
         handle->logFile = stderr;
   
   setlinebuf(handle->logFile); /* Essential! */
   
   if (LOG_PTR_IS_FLAG(LOG_FLAG_CLOEXEC))
   {
      int logFD = fileno(handle->logFile);
      
      if (logFD == -1)
      {
         perror("fileno");
         return NULL;
      }
      
      if ((fcntl(logFD, F_SETFD, FD_CLOEXEC)) == -1)
      {
         perror("fcntl");
         return NULL;
      }
   }
   
   logPrint(handle, LOG_ARGS, LOG_INFO, "Log library started (v%d.%d.%d)",
            MAJOR_VER, MINOR_VER, BUILD_VER);
   logPrint(handle, LOG_ARGS, LOG_INFO, "loglevels mask = 0x%.08x",
            logLevelMask);
   
   return handle;
}

const char* logGetLogLevelStr(const int logLevel)
{
   const char* ret = logLevelUnknStr;
   
   switch (logLevel)
   {
      case LOG_INFO:
         ret = logLevelInfoStr;
         break;
      case LOG_WARN:
         ret = logLevelWarnStr;
         break;
      case LOG_ERR:
         ret = logLevelErrStr;
         break;
      case LOG_BUG:
         ret = logLevelBugStr;
         break;
      case LOG_L1:
         ret = logLevelL1Str;
         break;
      case LOG_L2:
         ret = logLevelL2Str;
         break;
      case LOG_L3:
         ret = logLevelL3Str;
         break;
      default:
         break;
   }
   
   return ret;
}

void logPrint(log_t* const handle, const char* file, const int line,
              const char* func, const int logLevel, const char* fmt, ...)
{
   if (LOG_PTR_IS_LL(logLevel))
   {
      va_list ap;
      char logLevelStr[8];
      char logEntry[LOG_MAX_LEN];
      struct timeval tv;
      
      strcpy(logLevelStr, logGetLogLevelStr(logLevel));
      
      va_start(ap, fmt);
      vsnprintf(logEntry, LOG_MAX_LEN, fmt, ap);
      va_end(ap);
      
      tv = relTime(handle->timeAtStart);
      
      if (LOG_PTR_IS_FLAG(LOG_FLAG_SRC_INFO))
      {
         if ((fprintf(handle->logFile,
                      "[%5ld.%.06ld] <%s:%d> \"%s\" %s: %s\n",
                      (long int) tv.tv_sec, (long int) tv.tv_usec, file,
                      line, func, logLevelStr, logEntry)) < 0)
            abort();
      }
      else
      {
         if ((fprintf(handle->logFile, "[%5ld.%.06ld] \"%s\" %s: %s\n",
                      (long int) tv.tv_sec, (long int) tv.tv_usec, func,
                      logLevelStr, logEntry)) < 0)
            abort();
      }
   }
}

void logHexdump(log_t* const handle, const char* file, const int line,
                const char* func, const int logLevel, const char* str,
                const int len)
{
   if (LOG_PTR_IS_LL(logLevel))
   {
      char logLevelStr[8];
      int i, j;
      struct timeval tv;
      
      strcpy(logLevelStr, logGetLogLevelStr(logLevel));
      tv = relTime(handle->timeAtStart);
      
      if (LOG_PTR_IS_FLAG(LOG_FLAG_SRC_INFO))
      {
         fprintf(handle->logFile, "[%5ld.%.06ld] <%s:%d> \"%s\" %s: HEX\n",
                 (long int) tv.tv_sec, (long int) tv.tv_usec, file, line,
                 func, logLevelStr);
      }
      else
      {
         fprintf(handle->logFile, "[%5ld.%.06ld] \"%s\" %s: HEX\n",
                 (long int) tv.tv_sec, (long int) tv.tv_usec, func,
                 logLevelStr);
      }
      
      fprintf(handle->logFile, "---BEGIN_HEX---");
      
      for (i = 0; i < len; i++)
      {
         if ((i % 16) == 0)
            fprintf(handle->logFile, "\n%.08x  ", i);
         
         fprintf(handle->logFile, "%.02hx ", str[i]);
         
         if (i > 0)
         {
            if ((i % 8) == 7)
               fputc(' ', handle->logFile);
            
            if ((i % 16) == 15)
            {
               /* Use isprint() to print the chars, then print offset after new
                  line */
               
               fputc('|', handle->logFile);
               
               for (j = i - 15; j <= i; j++)
                  if (isprint(str[j]))
                     fputc(str[j], handle->logFile);
                  else
                     fputc('.', handle->logFile);
               
               fputc('|', handle->logFile);
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
            fputc(' ', handle->logFile);
         
         fputc('|', handle->logFile);
         
         for (j = i - (i % 16); j < i; j++)
            if (isprint(str[j]))
               fputc(str[j], handle->logFile);
            else
               fputc('.', handle->logFile);
         
         fputc('|', handle->logFile);
      }
      
      fprintf(handle->logFile, "\n%.08x\n---END_HEX---\n", i);
   }
}

void logHexdumpz(log_t* const handle, const char* file, const int line,
                 const char* func, const int logLevel, const char* str)
{
   logHexdump(handle, file, line, func, logLevel, str, strlen(str));
}

int logCleanup(log_t* const handle)
{
   logPrint(handle, LOG_ARGS, LOG_INFO, "Logging stopped");
   
   if (handle->logFileStr[0] != 0)
      if ((fclose(handle->logFile)) != 0)
      {
         perror("fclose");
         return -1;
      }
   
   free(handle->logFileStr);
   free(handle);
   
   return 0;
}

int logGetLogLevel(log_t* const handle, int logLevel)
{
   return LOG_PTR_IS_LL(logLevel);
}

void logBacktrace(log_t* const handle, const char* file, const int line,
                  const char* func)
{
   void* buf[128];
   int nptrs = backtrace(buf, 128);
   struct timeval tv = relTime(handle->timeAtStart);
   
   char** strings = backtrace_symbols(buf, nptrs);
   int i;
   
   if (LOG_PTR_IS_FLAG(LOG_FLAG_SRC_INFO))
   {
      fprintf(handle->logFile,
              "[%5ld.%.06ld] \"%s\" <%s:%d> Stack backtrace:\n",
              (long int) tv.tv_sec, (long int) tv.tv_usec, func, file, line);
   }
   else
   {
      fprintf(handle->logFile, "[%5ld.%.06ld] \"%s\" Stack backtrace:\n",
              (long int) tv.tv_sec, (long int) tv.tv_usec, func);
   }
   
   fprintf(handle->logFile, "---BEGIN_BACKTRACE---\n");
   
   for (i = 0; i < nptrs; i++)
      fprintf(handle->logFile, "%s\n", strings[i]);
   
   fprintf(handle->logFile, "---END_BACKTRACE---\n");
   
   free(strings);
}

void logAbort(log_t* const handle, const char* file, const int line,
              const char* func)
{
   logPrint(handle, file, line, func, LOG_INFO, "Process is now aborting");
   
   logBacktrace(handle, file, line, func);
   logCleanup(handle);
   
   abort();
}

void __logAssert(log_t* const handle, const char* file, const int line,
                 const char* func, const char* cond)
{
   if (LOG_PTR_IS_FLAG(LOG_FLAG_ENABLE_ASSERTS))
   {
      logPrint(handle, file, line, func, LOG_ERR, "Assertion \"%s\" failed",
               cond);
      logAbort(handle, file, line, func);
   }
}

void* logMalloc(log_t* const handle, const char* file, const int line,
                const char* func, size_t size)
{
   char* ret = malloc(size);
   
   if (ret == NULL)
   {
      logPrint(handle, file, line, func, LOG_ERR, "malloc: Out of memory");
      logAbort(handle, file, line, func);
   }
   
   return (void*) ret;
}

void logLuaStackDump(log_t* const handle, const char* file, const int line,
                     const char* func, lua_State* lua)
{
   char logLevelStr[8];
   int i, type, top;
   struct timeval tv;
   
   top = lua_gettop(lua);
   strcpy(logLevelStr, logLevelInfoStr);
   tv = relTime(handle->timeAtStart);

   if (LOG_PTR_IS_FLAG(LOG_FLAG_SRC_INFO))
   {
      fprintf(handle->logFile, "[%5ld.%.06ld] <%s:%d> \"%s\" %s: LUA\n",
              (long int) tv.tv_sec, (long int) tv.tv_usec, file, line,
              func, logLevelStr);
   }
   else
   {
      fprintf(handle->logFile, "[%5ld.%.06ld] \"%s\" %s: LUA\n",
              (long int) tv.tv_sec, (long int) tv.tv_usec, func,
              logLevelStr);
   }

   fprintf(handle->logFile, "---BEGIN_STACK_DUMP---\n");
   
   for (i = 1; i <= top; i++)
   {
      type = lua_type(lua, i);
      
      switch (type)
      {
         case LUA_TSTRING:
            fprintf(handle->logFile, "[%4d] \"%s\"\n", i,
                    lua_tostring(lua, i));
            break;
         
         case LUA_TBOOLEAN:
            fprintf(handle->logFile, "[%4d] %s\n", i,
                    lua_toboolean(lua, i) ? "true" : "false");
         
         case LUA_TNUMBER:
            fprintf(handle->logFile, "[%4d] %g\n", i, lua_tonumber(lua, i));
            break;
         
         default:
            fprintf(handle->logFile, "[%4d] (%s)\n", i,
                    lua_typename(lua, type));
            break;
      }
   }
   
   fprintf(handle->logFile, "---END_STACK_DUMP---\n");
}
