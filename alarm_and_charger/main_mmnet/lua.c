/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * Lua scripting.
 *
**/


#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include <sys/thread.h>

#include <lua/lua.h>

#include <lua/lauxlib.h>
#include <lua/lualib.h>


#if SERVANT_LUA

static THREAD(LuaThread, arg);


void lua_init(void)
{

    NutThreadCreate( "lua", LuaThread, NULL, 2048 );


}

char *lua_init_code = "";
char *lua_loop_code = "";

// Lua interpreter thread.
static THREAD(LuaThread, arg)
{
    lua_State *L = lua_open();

    luaL_dostring( L, lua_init_code);

    for (;;)
    {
        luaL_dostring( L, lua_loop_code);
    }


}








#endif // SERVANT_LUA
