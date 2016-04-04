#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <panic_log.h>

void luaStackDump(const log_t* handle, const char* file, const int line,
                  const char* func, lua_State* lua)
{
   char logLevelStr[8];
   int i, type, top;
   struct timeval tv;
   
   top = lua_gettop(lua);
   strncpy(logLevelStr, logLevelInfoStr, 8);
   logLevelStr[7] = 0;
   tv = _relTime(handle->timeAtStart);

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
