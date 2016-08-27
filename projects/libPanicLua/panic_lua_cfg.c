#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panic_core.h"
#include "panic_log.h"
#include "panic_lua_cfg.h"

cfg_t* cfgInit(log_t* log, const char* fileName)
{
   cfg_t* ret = malloc(sizeof(cfg_t));
   
   if (ret == NULL)
   {
      if (log != NULL)
      {
         logPrint(log, LOG_ARGS, LOG_ERR, "Out of memory");
         logAbort(log, LOG_ARGS);
      }
      else
      {
         fprintf(stderr, "Out of memory\n");
         abort();
      }
   }
   
   ret->fileName = malloc(PATH_MAX);
   
   if (ret->fileName == NULL)
   {
      if (log != NULL)
      {
         logPrint(log, LOG_ARGS, LOG_ERR, "Out of memory");
         logAbort(log, LOG_ARGS);
      }
      else
      {
         fprintf(stderr, "Out of memory\n");
         abort();
      }
   }
   
   strncpy(ret->fileName, fileName, PATH_MAX);
   ret->fileName[PATH_MAX - 1] = 0;
   
   ret->lua = luaL_newstate();
   luaL_openlibs(ret->lua);
   
   return ret;
}


