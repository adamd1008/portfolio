/* 
 * File:   panic_lua.h
 * Author: ADAM
 *
 * Created on 04 January 2016, 18:33
 */

#ifndef _PANIC_LUA_H_
#define _PANIC_LUA_H_

#include <panic_log.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Dump the stack of a Lua instance to the log file
 * 
 * @param handle Log handle pointer
 * @param file   Source file name (__FILE__ from LOG_ARGS)
 * @param line   Source line number (__LINE__ from LOG_ARGS)
 * @param func   Current function (__func__ from LOG_ARGS)
 * @param lua    Lua state instance
 */
void luaStackDump(const log_t* handle, const char* file, const int line,
                  const char* func, lua_State* lua);

#ifdef __cplusplus
}
#endif

#endif /* PANIC_LUA_H */

