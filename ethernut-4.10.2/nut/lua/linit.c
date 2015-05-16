/*
** $Id: linit.c 2831 2009-12-08 14:19:44Z haraldkipp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include <lua/lua.h>

#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/lrotable.h>


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
#ifdef NUTLUA_LOADLIB_IS_STANDARD
  {LUA_LOADLIBNAME, luaopen_package},
#endif
#ifdef NUTLUA_IOLIB_IS_STANDARD
  {LUA_IOLIBNAME, luaopen_io},
#endif
#ifdef NUTLUA_STRLIB_IS_STANDARD
  {LUA_STRLIBNAME, luaopen_string},
#endif
#if NUTLUA_OPTIMIZE_MEMORY == 0
#ifdef NUTLUA_MATHLIB_IS_STANDARD
  {LUA_MATHLIBNAME, luaopen_math},
#endif
#ifdef NUTLUA_OSLIB_IS_STANDARD
  {LUA_OSLIBNAME, luaopen_os},
#endif
#ifdef NUTLUA_TABLIB_IS_STANDARD
  {LUA_TABLIBNAME, luaopen_table},
#endif
#ifdef NUTLUA_DEBUGLIB_IS_STANDARD
  {LUA_DBLIBNAME, luaopen_debug},
#endif
#endif
  {NULL, NULL}
};

/* The read-only tables are defined here */
extern const luaL_Reg mathlib[];
extern const luaR_value_entry mathlib_vals[];
extern const luaL_Reg syslib[];
extern const luaL_Reg tab_funcs[];
extern const luaL_Reg dblib[];
extern const luaL_Reg co_funcs[];
const luaR_table lua_rotable[] =
{
#if NUTLUA_OPTIMIZE_MEMORY > 0
#ifndef LAU_MATHLIB_NOT_IMPLEMENTED
  {LUA_MATHLIBNAME, mathlib, mathlib_vals},
#endif
#ifndef LAU_OSLIB_NOT_IMPLEMENTED
  {LUA_OSLIBNAME, syslib, NULL},
#endif
#ifndef LAU_TABLIB_NOT_IMPLEMENTED
  {LUA_TABLIBNAME, tab_funcs, NULL},
#endif
#ifndef LAU_DEBUGLIB_NOT_IMPLEMENTED
  {LUA_DBLIBNAME, dblib, NULL},
#endif
  {LUA_COLIBNAME, co_funcs, NULL},
#endif
  {NULL, NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}
