/*
** $Id: lapi.h 2345 2008-10-10 11:52:25Z haraldkipp $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include <lua/lobject.h>


LUAI_FUNC void luaA_pushobject (lua_State *L, const TValue *o);

#endif
