/*
 * Copyright 2009 by egnite GmbH
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*
** Based on
** Id: lua.c,v 1.160.1.2 2007/12/28 15:32:23 roberto Exp
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/

#include <dev/board.h>
#include <sys/version.h>
#include <sys/heap.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <sys/thread.h>

#include <gorp/edline.h>

#include <arpa/inet.h>
#include <pro/dhcp.h>

#include <stdio.h>
#include <io.h>
#include <string.h>

#define lua_c

#include <lua/lua.h>

#include <lua/lauxlib.h>
#include <lua/lualib.h>

#ifndef NUTLUA_PARSER_EXCLUDED

#ifdef __AVR__
#define THREAD_LUASTACK     2048
#else
#define THREAD_LUASTACK     8192
#endif

static char lua_line_buffer[LUA_MAXINPUT];
static EDLINE *lua_line_edit;

/*
 * Report bad state.
 */
static void report(lua_State * L)
{
    if (!lua_isnil(L, -1)) {
        const char *msg = lua_tostring(L, -1);

        if (msg == NULL)
            msg = "(error object is not a string)";
        fputs(msg, stderr);
        fputc('\n', stderr);
        fflush(stderr);
        lua_pop(L, 1);
    }
}

/*
 * Perform Lua back trace.
 */
static int traceback(lua_State * L)
{
    if (!lua_isstring(L, 1))    /* 'message' not a string? */
        return 1;               /* keep it intact */
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);        /* pass error message */
    lua_pushinteger(L, 2);      /* skip this function and traceback */
    lua_call(L, 2, 1);          /* call debug.traceback */
    return 1;
}

/*
 * Execute the chunk on top of the Lua stack.
 */
static int docall(lua_State * L)
{
    int status;
    int base;    /* function index */

    /* Retrieve the function index and push the traceback 
    ** function below the chunk and its arguments. */
    base = lua_gettop(L);
    lua_pushcfunction(L, traceback);
    lua_insert(L, base);

    status = lua_pcall(L, 0, LUA_MULTRET, base);

    /* Remove the traceback function. */
    lua_remove(L, base);

    /* Force a complete garbage collection in case of errors. */
    if (status)
        lua_gc(L, LUA_GCCOLLECT, 0);

    return status;
}

/*
 * Retrieve the currently valid prompt.
 */
static const char *get_prompt(lua_State * L, int firstline)
{
    const char *p;

    /* Try global settings first. */
    lua_getfield(L, LUA_GLOBALSINDEX, firstline ? "_PROMPT" : "_PROMPT2");
    p = lua_tostring(L, -1);
    if (p == NULL)
        p = (firstline ? LUA_PROMPT : LUA_PROMPT2);
    lua_pop(L, 1);

    return p;
}

/*
 * Check if Lua chunk is complete.
 */
static int is_complete(lua_State * L, int status)
{
    if (status == LUA_ERRSYNTAX) {
        size_t lmsg;
        const char *msg = lua_tolstring(L, -1, &lmsg);
        const char *tp = msg + lmsg - (sizeof(LUA_QL("<eof>")) - 1);

        if (strstr(msg, LUA_QL("<eof>")) == tp) {
            lua_pop(L, 1);
            return 0;
        }
    }
    return 1;
}

/*
 * Get next line of a Lua chunk.
 */
static int pushline(lua_State * L, int firstline)
{
    int l;
    const char *prmt;

    /* Display a prompt. */
    prmt = get_prompt(L, firstline);
    fputs(prmt, stdout);
    fflush(stdout);

    /* Get a new line from the line editor. */
    lua_line_buffer[0] = '\0';
    l = EdLineRead(lua_line_edit, lua_line_buffer, LUA_MAXINPUT);

    if (l >= 0) {
        /* Replace '=' at the beginning with 'return'. */
        if (firstline && lua_line_buffer[0] == '=')
            lua_pushfstring(L, "return %s", &lua_line_buffer[1]);
        else
            lua_pushstring(L, lua_line_buffer);
    }
    return l;
}

/*
 * Load complete Lua chunk.
 */
static int loadchunk(lua_State * L)
{
    int status = -1;

    /* Clear Lua stack. */
    lua_settop(L, 0);

    if (pushline(L, 1) >= 0) {
        /* Loop until we have a complete chunk. */
        for (;;) {
            status = luaL_loadbuffer(L, lua_tostring(L, 1), lua_strlen(L, 1), "=stdin");
            /* If the line is complete, we are done. */
            if (is_complete(L, status))
                break;
            /* Otherwise try to get another line. */
            if (pushline(L, 0) < 0)
                return -1;
            /* Add a linefeed between the two lines and join them. */
            lua_pushliteral(L, "\n");
            lua_insert(L, -2);
            lua_concat(L, 3);
        }
        lua_saveline(L, 1);
        lua_remove(L, 1);
    }
    return status;
}

/*
 * Lua line processing loop.
 */
static int pmain(lua_State * L)
{
    int status;

    /* Open pre-configured libraries. Garbage collector must be disabled. */
    lua_gc(L, LUA_GCSTOP, 0);
    luaL_openlibs(L);
    lua_gc(L, LUA_GCRESTART, 0);

    /* This loop loads Lua chunks and processes them until we get an 
    ** input error. We may later use Ctrl-C or break to exit the 
    ** interpreter. Right now this is not supported by our line editor. */
    while ((status = loadchunk(L)) != -1) {
        /* If everything is OK, execute the chunk. */
        if (status == 0)
            status = docall(L);
        /* Report bad state... */
        if (status) {
            report(L);
        }
        /* ...or print result, if any. */
        else if (lua_gettop(L) > 0) {
            lua_getglobal(L, "print");
            lua_insert(L, 1);
            if (lua_pcall(L, lua_gettop(L) - 1, 0, 0) != 0) {
                fprintf(stderr, lua_pushfstring(L, "error calling " 
                    LUA_QL("print") " (%s)", lua_tostring(L, -1)));
                fputc('\n', stderr);
                fflush(stderr);
            }
        }
    }
    /* Clear Lua stack. */
    lua_settop(L, 0);

    fputc('\n', stdout);
    fflush(stdout);

    return 0;
}

/*
 * Lua interpreter thread.
 */
THREAD(LuaThread, arg)
{
    lua_State *L;

    for (;;) {
        /* Display second banner line here, so we know 
        ** that everything is up and running. */
        printf("Running on Nut/OS %s - %ld bytes free\n", NutVersionString(), (long)NutHeapAvailable());

        /* Open the line editor. */
        lua_line_edit = EdLineOpen(EDIT_MODE_ECHO | EDIT_MODE_HISTORY);
        if (lua_line_edit) {
            /* Register VT100 key mapping. This makes the arrow keys 
            ** work when using a VT100 terminal emulator for input. */
            EdLineRegisterKeymap(lua_line_edit, EdLineKeyMapVt100);

            /* Create a Lua state. */
            L = lua_open();
            if (L) {
                if (lua_cpcall(L, &pmain, NULL)) {
                    report(L);
                }
                /* Release the Lua state. */
                lua_close(L);
            }
            /* Release the line editor. */
            EdLineClose(lua_line_edit);
        }
    }
}

#endif /* NUTLUA_PARSER_EXCLUDED */

/*
 * Lua interpreter entry.
 */
int main(void)
{
    unsigned long baud = 115200;

    /* Initialize the console. */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    freopen(DEV_CONSOLE_NAME, "w", stderr);
    freopen(DEV_CONSOLE_NAME, "r", stdin);
    _ioctl(_fileno(stdin), UART_SETSPEED, &baud);

    /* Display banner. */
    puts("\n" LUA_RELEASE "  " LUA_COPYRIGHT);

#ifdef NUTLUA_PARSER_EXCLUDED
    puts("Error: Stand-alone interpreter requires parser");
#else
#ifdef NUTLUA_IOLIB_TCP
    NutRegisterDevice(&DEV_ETHER, 0, 0);
    if (NutDhcpIfConfig(DEV_ETHER_NAME, 0, 60000)) {
        uint8_t mac[6] = { 0x00, 0x06, 0x98, 0x00, 0x00, 0x00 };
        if (NutDhcpIfConfig("eth0", mac, 60000)) {
            uint32_t ip_addr = inet_addr("192.168.192.100");
            uint32_t ip_mask = inet_addr("255.255.255.0");
            NutNetIfConfig("eth0", mac, ip_addr, ip_mask);
        }
    }
    printf("Network interface %s\n", inet_ntoa(confnet.cdn_ip_addr));
#endif /* NUTLUA_IOLIB_TCP */

    /* Lua is stack hungry and requires to adjust the stack size of our 
    ** main thread. To get it run on a standard system we create a new
    ** thread with sufficient stack space. */
    NutThreadCreate("lua", LuaThread, NULL, THREAD_LUASTACK * NUT_THREAD_STACK_MULT + NUT_THREAD_STACK_ADD);
#endif /* NUTLUA_PARSER_EXCLUDED */
    /* Nothing left to do here. */
    for(;;) {
        NutSleep(1000);
    }
    return 0;
}
