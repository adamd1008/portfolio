#ifndef _PANIC_LUA_CFG_H_
#define _PANIC_LUA_CFG_H_

#include <linux/limits.h>
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TYPE_CHAR         0
#define CFG_TYPE_I16          1
#define CFG_TYPE_U16          2
#define CFG_TYPE_I32          3
#define CFG_TYPE_U32          4
#define CFG_TYPE_I64          5
#define CFG_TYPE_U64          6
#define CFG_TYPE_STR          7
/*#define CFG_TYPE_LUA_FUNC   8 /Future use*/

/*typedef enum
{
   i8,
   u8,
   i16,
   u16,
   i32,
   u32,
   i64,
   u64,
   f32,
   f64,
   str
} cfgType_t;*/

typedef struct
{
   char* name;
   
   union
   {
      uint8_t c;
      int16_t i16;
      uint16_t u16;
      int32_t i32;
      uint32_t u32;
      int64_t i64;
      uint64_t u64;
      float f32;
      double f64;
      char* str;
   } value;
   
   int type;
} cfgItem_t;

typedef struct
{
   log_t* log;
   lua_State* lua;
   char* fileName;
} cfg_t;

#ifdef __cplusplus
}
#endif

#endif
