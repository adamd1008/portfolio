#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static int logLuaInit(lua_State* lua)
{
   log_t* log;
   const char* fileName;
   
   if (luaL_checkany(lua, 1))
   {
      if (lua_isnil(lua, 1))
      {
         
      }
      else
      {
         
      }
   }
   else
   {
      return luaL_error(lua, "logLuaInit: Bad arguments");
   }
}

static const struct luaL_Reg libPanicLog[] = {
   {"logInit", logLuaInit},
   {"logPrint", logLuaPrint},
   {"logHexdump", logLuaHexdump},
   /*{"logHexdumpz", logLuaHexdumpz},*/
   {"logCleanup", logLuaCleanup},
   {"logGetLogLevel", logLuaGetLogLevel},
   {"logBacktrace", logLuaBacktrace},
   {"logAbort", logLuaAbort},
   {NULL, NULL}
};

int luaopen_libPanicLog(lua_State* lua)
{
   luaL_newlib(lua, libPanicLog);
   return 1;
}
