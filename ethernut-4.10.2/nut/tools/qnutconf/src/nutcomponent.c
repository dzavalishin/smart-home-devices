/*
 * Copyright (C) 2004-2007 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.39  2009/01/09 18:50:50  haraldkipp
 * Re-introduced support for Lua 5.0.
 *
 * Revision 1.38  2009/01/04 04:52:39  thiagocorrea
 * Add .svn to ignore list when copying files and reduce some duplicated code.
 *
 * Revision 1.37  2009/01/04 04:35:45  thiagocorrea
 * Update lua initialization to conform to lua 5.x API change.
 * This change should still be compatible with 4.x.
 *
 * Revision 1.36  2008/09/18 09:52:42  haraldkipp
 * Version 2.0.6 fixes a few memory holes and avoids empty macro definitions
 * for items with "integer" flavor.
 *
 * Revision 1.35  2008/08/29 15:03:04  haraldkipp
 * Fixed Configurator bug #2082123. Options now correctly enabled.
 *
 * Revision 1.34  2008/08/28 11:09:29  haraldkipp
 * Added Lua extension to query specific provisions.
 *
 * Revision 1.33  2008/08/20 07:00:33  haraldkipp
 * Incremented build number because of bad package. No code change.
 *
 * Revision 1.32  2008/07/29 07:30:30  haraldkipp
 * Make sure that children of a disabled parents are disabled too.
 *
 * Revision 1.31  2008/07/28 08:41:32  haraldkipp
 * Configurator accepts empty install path for using the default.
 *
 * Revision 1.30  2008/07/24 15:41:41  haraldkipp
 * Dynamic configuration.
 *
 * Revision 1.29  2008/03/17 10:22:49  haraldkipp
 * Added more comments.
 *
 * Revision 1.28  2007/09/11 14:19:50  haraldkipp
 * Use avr-dude as default.
 *
 * Revision 1.27  2007/04/25 15:59:17  haraldkipp
 * Version 1.4.2 had been tested with wxWidgets 2.8.3.
 *
 * Revision 1.26  2007/02/15 19:33:45  haraldkipp
 * Version 1.4.1 works with wxWidgets 2.8.0.
 * Several wide character issues fixed.
 *
 * Revision 1.25  2006/10/06 17:49:57  haraldkipp
 * MAX_PATH is unknown to Linux.
 *
 * Revision 1.24  2006/10/05 17:04:46  haraldkipp
 * Heavily revised and updated version 1.3
 *
 * Revision 1.23  2005/11/22 09:20:22  haraldkipp
 * Removed modification of a relative top_blddir path.
 *
 * Revision 1.22  2005/10/07 22:36:00  hwmaier
 * Removed generation of -DETHERNUT2 entry for UserConf.mk as this can now accomplished by the PLATFORM macro.
 *
 * Revision 1.21  2005/10/07 22:11:59  hwmaier
 * Changed LoadComponents to parse options in root level.
 * Added bld_dir parameter to CreateSampleDirectory.
 *
 * Revision 1.20  2005/09/07 16:24:23  christianwelzel
 * Changed handling of default parameters. Nutconf now creates all default
 * parameter defines within header files.
 *
 * Revision 1.19  2005/07/26 15:55:36  haraldkipp
 * Version 1.2.3.
 * Added new keyword "default" to specify default values. They will no
 * longer appear in the conf files and we can remove the booldata flavor
 * from most options.
 * Bugfix: Options will now be recognized in all sublevels.
 *
 * Revision 1.18  2005/07/20 09:21:10  haraldkipp
 * Allow subdivided modules
 *
 * Revision 1.17  2005/05/25 09:59:53  haraldkipp
 * Bugfix: Absolute application path for top_appdir pointed to source directory.
 *
 * Revision 1.16  2005/04/28 16:18:22  haraldkipp
 * Autoconfiscated
 *
 * Revision 1.15  2005/04/22 15:16:01  haraldkipp
 * Can now run without GUI.
 *
 * Revision 1.14  2005/02/26 12:13:40  drsung
 * Added support for relative paths for sample application directory.
 *
 * Revision 1.13  2005/02/06 16:39:52  haraldkipp
 * GBA linker script entry in NutConf.mk fixed
 *
 * Revision 1.12  2004/11/24 15:36:53  haraldkipp
 * Release 1.1.1.
 * Do not store empty options.
 * Remove include files from the build tree, if they are no longer used.
 * Command line parameter 's' allows different settings.
 * Minor compiler warning fixed.
 *
 * Revision 1.11  2004/09/26 13:25:00  drsung
 * Fixed call to strdup.
 *
 * Revision 1.10  2004/09/26 12:04:07  drsung
 * Fixed several hundred memory leaks :-).
 * Relative pathes can now be used for source, build and install directory.
 *
 * Revision 1.9  2004/09/19 15:13:09  haraldkipp
 * Only one target per OBJx entry
 *
 * Revision 1.8  2004/09/17 13:02:18  haraldkipp
 * First and last directory added to sample dir
 *
 * Revision 1.7  2004/09/07 19:18:11  haraldkipp
 * Trying to get additional .S and .c targets assembled/compiled.
 * ETHERNUT2 default for newly created UserConf.mk.
 *
 * Revision 1.6  2004/08/18 16:06:03  haraldkipp
 * Use consistent directory structure
 *
 * Revision 1.5  2004/08/18 15:36:25  haraldkipp
 * Phony target clean divided
 *
 * Revision 1.4  2004/08/18 14:05:22  haraldkipp
 * Fill error text if script not found
 *
 * Revision 1.3  2004/08/18 13:34:20  haraldkipp
 * Now working on Linux
 *
 * Revision 1.2  2004/08/03 15:03:25  haraldkipp
 * Another change of everything
 *
 * Revision 1.1  2004/06/07 16:11:22  haraldkipp
 * Complete redesign based on eCos' configtool
 *
 */

/*
 * This module had been intentionally written in C, not C++,
 * to make its use in simple non-GUI applications as easy as
 * possible.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#define NUT_CONFIGURE_VERSION   "2.0.10"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#if defined(_MSC_VER) || defined( __MINGW32__ )

#include <io.h>
#include <direct.h>
#define mkdir(P, M) _mkdir(P)
#define access _access
#define strcasecmp stricmp
#define strncasecmp strnicmp

#else

#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#endif

#include <fcntl.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef NUT_CONFIGURE_EXEC
#include "getopt.h"
#endif

#include "nutcomponent.h"

/*! \brief Internal component name.
 * Must be unique.
 */
#define TKN_NAME    "name"

/*! \brief Name of a macro.
 * Identifier used in configuration include files.
 */
#define TKN_MACRO   "macro"

/*! \brief String containing a Lua script.
 */
#define TKN_SCRIPT  "script"

/*! \brief Subdirectory of a component
 */
#define TKN_SUBDIR  "subdir"

/*! \brief Short component description.
 */
#define TKN_BRIEF   "brief"

/*! \brief Detailed component description.
 */
#define TKN_DESC    "description"

/*! \brief Option's default value.
 */
#define TKN_DEFAULT "default"

#define TKN_REQUIRES "requires"
#define TKN_PROVIDES "provides"

/*! \brief Component option list.
 */
#define TKN_OPTIONS "options"

/*! \brief Condition for automatic activation.
 */
#define TKN_ACTIF   "active_if"

/*! \brief Component's option flavor.
 * Possible values are
 * - boolean: Option may be disabled or enabled
 * - booldata: Option may be disabled or enabled and contains a value if enabled.
 */
#define TKN_FLAVOR  "flavor"

/*! \brief Option's value type.
 * - integer
 * - bool
 * - enumerated
 */
#define TKN_TYPE    "type"

/*! \brief C language type.
 * Not yet supported.
 */
#define TKN_CTYPE   "ctype"

/*! \brief Relative path of the configuration include file.
 */
#define TKN_FILE    "file"

/*! \brief List of source files.
 */
#define TKN_SOURCES "sources"

/*! \brief List of explicit target files.
 */
#define TKN_TARGETS "targets"

/*! \brief List of choices of an enumerated option.
 */
#define TKN_CHOICES "choices"

/*! \brief Optional Makefile definitions.
 */
#define TKN_MAKEDEFS "makedefs"

/*! \brief List of options which are mutually exclusive.
 */
#define TKN_EXCLUSIVITY "exclusivity"

static int errsts;
static char errtxt[1024];

#define LRK_NUTREPOSITORY       "nutconf.NUTREPOSITORY"
#define LRK_NUTROOTCOMPONENT    "nutconf.NUTROOTCOMPONENT"
#define LRK_NUTSOURCEPATH       "nutconf.NUTSOURCEPATH"
#define LRK_NUTLIBPATH          "nutconf.NUTLIBPATH"
#define LRK_NUTBUILDPATH        "nutconf.NUTSBUILDPATH"
#define LRK_NUTSAMPLEPATH       "nutconf.NUTSAMPLEPATH"
#define LRK_NUTCOMPILERPLATFORM "nutconf.NUTCOMPILERPLATFORM"

static int IsProvided(NUTREPOSITORY *repo, NUTCOMPONENT *compo, const char *requirement);

#if 0

/* Quite helpful during debugging. */
static void DumpStack(FILE *fp, char * pos, lua_State * ls)
{
    int top;
    int i;
    int type;
    FILE *fpl = fp;

    if (fpl == NULL) {
        fpl = fopen("dump.log", "a");
    }

    fprintf(fpl, "Stack dump at %s\n", pos);
    top = lua_gettop(ls);
    for (i = top; i > 0; i--) {
        type = lua_type(ls, i);
        fprintf(fpl, "%d %s", i - top - 1, lua_typename(ls, type));
        switch (type) {
        case LUA_TNONE:
            fprintf(fpl, "\n");
            break;
        case LUA_TNIL:
            fprintf(fpl, "\n");
            break;
        case LUA_TBOOLEAN:
            fprintf(fpl, "\n");
            break;
        case LUA_TLIGHTUSERDATA:
            fprintf(fpl, "\n");
            break;
        case LUA_TNUMBER:
            fprintf(fpl, "\n");
            break;
        case LUA_TSTRING:
            fprintf(fpl, "=%s\n", lua_tostring(ls, i));
            break;
        case LUA_TTABLE:
            fprintf(fpl, "=\n");
            lua_pushnil(ls);
            while (lua_next(ls, i)) {
                if (lua_type(ls, -2) == LUA_TSTRING) {
                    fprintf(fpl, "  [%s] %s\n", lua_tostring(ls, -2), lua_typename(ls, lua_type(ls, -1)));
                } else if (lua_type(ls, -2) == LUA_TNUMBER) {
                    fprintf(fpl, "  [%.0lf] %s\n", lua_tonumber(ls, -2), lua_typename(ls, lua_type(ls, -1)));
                } else {
                    fprintf(fpl, "  [%s] %s\n", lua_typename(ls, lua_type(ls, -2)), lua_typename(ls, lua_type(ls, -1)));
                }
                lua_pop(ls, 1);
            }
            break;
        case LUA_TFUNCTION:
            fprintf(fpl, "\n");
            break;
        case LUA_TUSERDATA:
            fprintf(fpl, "\n");
            break;
        case LUA_TTHREAD:
            fprintf(fpl, "\n");
            break;
        }
    }
    if (fp == NULL) {
        fprintf(fpl, "--------------------------------------------------\n");
        fclose(fpl);
    }
}

static void DumpCompoTree(FILE *fp, NUTCOMPONENT * compo, int level)
{
    NUTCOMPONENTOPTION *opts;
    int i;
    FILE *fpl = fp;

    if (fpl == NULL) {
        fpl = fopen("dump.log", "a");
    }

    while (compo) {
        for (i = 0; i < level; i++) {
            fprintf(fpl, "  ");
        }
        fprintf(fpl, "%s\n", compo->nc_name);
        opts = compo->nc_opts;
        while (opts) {
            if (opts->nco_value) {
                for (i = 0; i <= level; i++) {
                    fprintf(fpl, "  ");
                }
                fprintf(fpl, "%s = %s\n", opts->nco_name, opts->nco_value);
            }
            opts = opts->nco_nxt;
        }
        if (compo->nc_child) {
            DumpCompoTree(fpl, compo->nc_child, level + 1);
        }
        compo = compo->nc_nxt;
    }
    if (fp == NULL) {
        fprintf(fpl, "--------------------------------------------------\n");
        fclose(fpl);
    }
}

// #else
static void DumpStack(FILE *fp, char * pos, lua_State * ls)
{
    if (fp && pos && ls);
}
static void DumpCompoTree(FILE *fp, NUTCOMPONENT * compo, int level)
{
    if (fp && compo && level);
}
#endif

/*! 
 * \brief Get the current script status.
 */
const int GetScriptStatus(void)
{
    return errsts;
}

/*! 
 * \brief Get the script's error message.
 */
const char *GetScriptErrorString(void)
{
    errsts = 0;
    return errtxt;
}

/*!
 * \brief Store string value in the Lua registry.
 *
 * \todo Same as RegisterStringValue(). Remove this one?
 *
 * \param ls Lua state.
 */
int SetLuaRegString(lua_State *ls, char *key, char *value)
{
    lua_pushstring(ls, key);
    lua_pushstring(ls, value);
    lua_settable(ls, LUA_REGISTRYINDEX);

    return 0;
}

/*!
 * \brief Retrieve string value from the Lua registry.
 *
 * \param ls Lua state.
 */
const char * GetLuaRegString(lua_State *ls, char *key)
{
    const char *value;

    lua_pushstring(ls, key);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    value = lua_tostring(ls, -1);
    lua_pop(ls, 1);

    return value;
}

/*!
 * \brief Process c_repo_path call from Lua.
 *
 * \param ls Lua state.
 */
static int l_repo_path(lua_State *ls) 
{
    NUTREPOSITORY *repo;

    /* Retrieve pointer to repository path from Lua registry. */
    lua_pushstring(ls, LRK_NUTREPOSITORY);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    repo = (NUTREPOSITORY *)lua_topointer(ls, -1);

    /* push result */
    if (repo) {
        lua_pushstring(ls, repo->nr_dir);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

/*!
 * \brief Process c_nut_source_path call from Lua.
 *
 * \param ls Lua state.
 */
static int l_nut_source_path(lua_State *ls) 
{
    const char *str;

    lua_pushstring(ls, LRK_NUTSOURCEPATH);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    str = lua_tostring(ls, -1);

    /* push result */
    if (str) {
        lua_pushstring(ls, str);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

/*!
 * \brief Process c_nut_build_path call from Lua.
 *
 * \param ls Lua state.
 */
static int l_nut_build_path(lua_State *ls) 
{
    const char *str;

    lua_pushstring(ls, LRK_NUTBUILDPATH);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    str = lua_tostring(ls, -1);

    /* push result */
    if (str) {
        lua_pushstring(ls, str);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

/*!
 * \brief Process c_nut_lib_path call from Lua.
 *
 * \param ls Lua state.
 */
static int l_nut_lib_path(lua_State *ls) 
{
    const char *str;

    lua_pushstring(ls, LRK_NUTLIBPATH);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    str = lua_tostring(ls, -1);

    /* push result */
    if (str) {
        lua_pushstring(ls, str);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

/*!
 * \brief Process c_nut_sample_path call from Lua.
 *
 * \param ls Lua state.
 */
static int l_nut_sample_path(lua_State *ls) 
{
    const char *str;

    lua_pushstring(ls, LRK_NUTSAMPLEPATH);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    str = lua_tostring(ls, -1);

    /* push result */
    if (str) {
        lua_pushstring(ls, str);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

/*!
 * \brief Process c_compiler_platform call from Lua.
 *
 * \param ls Lua state.
 */
static int l_compiler_platform(lua_State *ls) 
{
    const char *str;

    lua_pushstring(ls, LRK_NUTCOMPILERPLATFORM);
    lua_gettable(ls, LUA_REGISTRYINDEX);
    str = lua_tostring(ls, -1);

    /* push result */
    if (str) {
        lua_pushstring(ls, str);
    }
    else {
        lua_pushnil(ls);
        lua_pushstring(ls, "Internal registry error");
    }
    /* number of results */
    return 1;
}

static char * GetMacroEdit(NUTREPOSITORY *repo, NUTCOMPONENT *compo, const char *macro)
{
    char *rc = NULL;
    NUTCOMPONENTOPTION *opts;

    while (compo && rc == NULL) {
        opts = compo->nc_opts;
        while (opts && rc == NULL) {
            if (strcmp(opts->nco_name, macro) == 0) {
                if (opts->nco_value) {
                    rc = strdup(opts->nco_value);
                } else {
                    rc = GetConfigValueOrDefault(repo, compo, opts->nco_name);
                }
            }
            opts = opts->nco_nxt;
        }
        if (rc == NULL) {
            rc = GetMacroEdit(repo, compo->nc_child, macro);
        }
        compo = compo->nc_nxt;
    }
    return rc;
}

static int GetActiveState(NUTCOMPONENT *compo, const char *name)
{
    int rc = 0;
    int found = 0;
    NUTCOMPONENTOPTION *opts;

    while (!found && compo) {
        opts = compo->nc_opts;
        while (!found && opts) {
            if (strcmp(opts->nco_name, name) == 0) {
                found = 1;
                rc = opts->nco_active;
            }
            opts = opts->nco_nxt;
        }
        if (!found) {
            rc = GetActiveState(compo->nc_child, name);
        }
        compo = compo->nc_nxt;
    }
    return rc;
}

static int GetEnableState(NUTCOMPONENT *compo, const char *name)
{
    int rc = 0;
    int found = 0;
    NUTCOMPONENTOPTION *opts;

    while (!found && compo) {
        if (strcmp(compo->nc_name, name) == 0) {
            found = 1;
            rc = compo->nc_enabled;
        }
        opts = compo->nc_opts;
        while (!found && opts) {
            if (strcmp(opts->nco_name, name) == 0) {
                found = 1;
                rc = opts->nco_enabled;
            }
            opts = opts->nco_nxt;
        }
        if (!found) {
            rc = GetEnableState(compo->nc_child, name);
        }
        compo = compo->nc_nxt;
    }
    return rc;
}

/*!
 * \brief Process c_macro_edit call from Lua.
 *
 * \param ls Lua state.
 */
static int l_macro_edit(lua_State *ls) 
{
    char *value = NULL;
    const char *macro = luaL_checkstring(ls, 1);

    if (macro) {
        NUTREPOSITORY *repo;
        NUTCOMPONENT *root;

        /* Retrieve pointer to repository from Lua registry. */
        lua_pushstring(ls, LRK_NUTREPOSITORY);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        repo = (NUTREPOSITORY *)lua_topointer(ls, -1);

        /* Retrieve pointer to root component from Lua registry. */
        lua_pushstring(ls, LRK_NUTROOTCOMPONENT);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        root = (NUTCOMPONENT *)lua_topointer(ls, -1);

        value = GetMacroEdit(repo, root, macro);
    }
    if (value) {
        lua_pushstring(ls, value);
    } else {
        lua_pushstring(ls, "");
    }
    return 1;
}

/*!
 * \brief Process c_is_enabled call from Lua.
 *
 * \param ls Lua state.
 */
static int l_is_enabled(lua_State *ls) 
{
    int status = 0;
    const char *name = luaL_checkstring(ls, 1);

    if (name) {
        NUTCOMPONENT *root;

        /* Retrieve pointer to root component from Lua registry. */
        lua_pushstring(ls, LRK_NUTROOTCOMPONENT);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        root = (NUTCOMPONENT *)lua_topointer(ls, -1);
        status = GetEnableState(root, name);
    }
    lua_pushboolean(ls, status);

    return 1;
}

/*!
 * \brief Process c_is_active call from Lua.
 *
 * \param ls Lua state.
 */
static int l_is_active(lua_State *ls)
{
    int status = 0;
    const char *name = luaL_checkstring(ls, 1);

    if (name) {
        NUTCOMPONENT *root;

        /* Retrieve pointer to root component from Lua registry. */
        lua_pushstring(ls, LRK_NUTROOTCOMPONENT);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        root = (NUTCOMPONENT *)lua_topointer(ls, -1);
        status = GetActiveState(root, name);
    }
    lua_pushboolean(ls, status);

    return 1;
}

/*!
 * \brief Process c_is_provided call from Lua.
 *
 * \param ls Lua state.
 */
static int l_is_provided(lua_State *ls)
{
    int status = 0;
    const char *name = luaL_checkstring(ls, 1);

    if (name) {
        NUTREPOSITORY *repo;
        NUTCOMPONENT *root;

        /* Retrieve pointer to repository from Lua registry. */
        lua_pushstring(ls, LRK_NUTREPOSITORY);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        repo = (NUTREPOSITORY *)lua_topointer(ls, -1);

        /* Retrieve pointer to root component from Lua registry. */
        lua_pushstring(ls, LRK_NUTROOTCOMPONENT);
        lua_gettable(ls, LUA_REGISTRYINDEX);
        root = (NUTCOMPONENT *)lua_topointer(ls, -1);
        status = IsProvided(repo, root, name);
    }
    lua_pushboolean(ls, status);

    return 1;
}

static const struct luaL_reg nutcomp_lib[] = {
    { "c_repo_path", l_repo_path },
    { "c_nut_source_path", l_nut_source_path },
    { "c_nut_build_path", l_nut_build_path },
    { "c_nut_lib_path", l_nut_lib_path },
    { "c_nut_sample_path", l_nut_sample_path },
    { "c_compiler_platform", l_compiler_platform },
    { "c_macro_edit", l_macro_edit },
    { "c_is_enabled", l_is_enabled },
    { "c_is_active", l_is_active },
    { "c_is_provided", l_is_provided },
    { NULL, NULL }
};

int luaopen_nutcomp_lib(lua_State *ls) 
{
    luaL_openlib(ls, "nutcomp_lib", nutcomp_lib, 0);

    return 1;
}

void RegisterLuaExtension(lua_State *ls, const luaL_reg *reg) 
{
    for (; reg->name; reg++) {
        lua_pushcfunction(ls, reg->func);
        lua_setglobal(ls, reg->name);
   }
}

/*!
 * \brief Store string value in the Lua registry.
 *
 * \param ls   Lua state.
 * \param name Item name.
 * \param name Item value.
 *
 * \return Always 0.
 */
static int RegisterStringValue(lua_State *ls, const char *name, const char *str)
{
    lua_pushstring(ls, name);
    lua_pushstring(ls, str);
    lua_settable(ls, LUA_REGISTRYINDEX);

    return 0;
}

int CountLuaChunkResults(lua_State * ls, char *chunk)
{
    int ec;
    int rc = lua_gettop(ls);

    /* Compile the chunk and push it on top of the stack. */
    ec = luaL_loadbuffer(ls, chunk, strlen(chunk), "name");
    if (ec) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        lua_pop(ls, 1);
        return 0;
    }
    /* Execute the function on top of the stack and remove it. */
    ec = lua_pcall(ls, 0, LUA_MULTRET, 0);
    if (ec) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        lua_pop(ls, 1);
        return 0;
    }
    rc = lua_gettop(ls) - rc;

    /* Re-balance the stack. */
    lua_pop(ls, rc);

    return rc;
}

char **ExecuteLuaChunk(lua_State * ls, char *chunk)
{
    int ec;
    char **result = NULL;
    int n = lua_gettop(ls);
    int i;

    /* Compile the chunk and push it on top of the stack. */
    ec = luaL_loadbuffer(ls, chunk, strlen(chunk), "name");
    if (ec) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        lua_pop(ls, 1);
        return NULL;
    }
    /* Execute the function on top of the stack and remove it. */
    ec = lua_pcall(ls, 0, LUA_MULTRET, 0);
    if (ec) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        lua_pop(ls, 1);
        return NULL;
    }
    n = lua_gettop(ls) - n;

    result = calloc(n + 1, sizeof(char *));
    for (i = 0; n--; ) {
        if (lua_isstring(ls, -1)) {
            result[i++] = strdup(lua_tostring(ls, -1));
        }
        lua_pop(ls, 1);
    }
    return result;
}

/*!
 * \brief Get a string value from a named item of a table.
 *
 * The table is expected at a specified position of the Lua stack.
 *
 * \param ls   Lua state.
 * \param idx  Stack position of the table.
 * \param name Name of the table item.
 * \param dst  Pointer to buffer that receives the string. If NULL, the
 *             buffer will be allocated from heap.
 * \param siz  Size of the buffer. Ignored if dst is NULL.
 *
 * \return Pointer to the buffer containing the retrieved value or NULL,
 *         if there is no table item with the specified name.
 */
static char *GetStringByNameFromTable(lua_State * ls, int idx, char *name, char *dst, size_t siz)
{
    int i;
    char *str = NULL;

    /* Lua expects the named key into the table on top of the stack. */
    lua_pushstring(ls, name);
    /* This puts the value of the named table entry on top of the stack.
       Note, that we have to adjust the index because we pushed the key
       on top. */
    lua_gettable(ls, idx < 0 ? idx - 1 : idx + 1);
    /* Handle functions first. */
    if (lua_isfunction(ls, -1)) {
        if (lua_pcall(ls, 0, 1, 0)) {
            strcpy(errtxt, lua_tostring(ls, -1));
            errsts++;
            lua_pop(ls, 1);
            return NULL;
        }
    }
    /* Handle string or number. */
    if (lua_isstring(ls, -1)) {
        str = strdup(lua_tostring(ls, -1));
        /* Is this a script? If yes, execute it and add the results. */
        if (strlen(str) > 3 && str[0] == '-' && str[1] == '-'&& str[2] == '\n') {
            char **script_result = ExecuteLuaChunk(ls, str);
            free(str);
            str = NULL;
            /* Add chunk results to array. */
            if (script_result) {
                /* Retrieve the result that had been pushed last. */
                str = script_result[0];
                /* Discard any remaining results. */
                for (i = 1; script_result[i]; i++) {
                    free(script_result[i]);
                }
                free(script_result);
            }
        }
    } else {
        dst = NULL;
    }
    /* Remove the value to keep the stack balanced. */
    lua_pop(ls, 1);

    if (dst == NULL) {
        dst = str;
    } else if (str && strlen(str) < siz) {
        strcpy(dst, str);
        free(str);
    } else {
        dst = NULL;
    }
    return dst;
}

/*!
 * \brief Get an array of string values from a named item of a table.
 *
 * The table is expected at a specified position of the Lua stack.
 *
 * \param ls   Lua state.
 * \param idx  Stack position of the table.
 * \param name Name of the table item.
 *
 * \return Pointer to an array of string pointers to the retrieved values
 *         or NULL, if there is no table item with the specified name.
 *         The string buffers as well as the array itself are allocated
 *         from heap.
 */
char **GetStringArrayByNameFromTable(lua_State * ls, int idx, char *name)
{
    int cnt = 0;
    char **result = NULL;
    char *str;
    char **script_result;
    int i;

    /* Lua expects the named key into the table on top of the stack. */
    lua_pushstring(ls, name);
    /* Replaces the key by its value. */
    lua_gettable(ls, idx < 0 ? idx - 1 : idx + 1);

    /* Handle functions first. */
    if (lua_isfunction(ls, -1)) {
        if (lua_pcall(ls, 0, 1, 0)) {
            strcpy(errtxt, lua_tostring(ls, -1));
            errsts++;
            lua_pop(ls, 1);
            return NULL;
        }
    }
    /* Make sure we have a table on top of the stack and traverse the
       table, counting the number of strings in this table. */
    if (lua_istable(ls, -1)) {
        lua_pushnil(ls);
        while (lua_next(ls, -2)) {
            if (lua_isfunction(ls, -1)) {
                if (lua_pcall(ls, 0, 1, 0)) {
                    strcpy(errtxt, lua_tostring(ls, -1));
                    errsts++;
                }
            }
            else if (lua_isstring(ls, -1)) {
                str = strdup(lua_tostring(ls, -1));
                /* Is this a script? If yes, count the number of results. */
                if (strlen(str) > 3 && str[0] == '-' && str[1] == '-'&& str[2] == '\n') {
                    cnt += CountLuaChunkResults(ls, str);
                }
                else {
                    cnt++;
                }
                free(str);
            }
            lua_pop(ls, 1);
        }
    }
    /* Keep the stack unmodified. */
    lua_pop(ls, 1);

    /* If we found a table with one or more strings, then allocate a
       pointer array to store these strings. */
    if (cnt) {
        lua_pushstring(ls, name);
        lua_gettable(ls, idx < 0 ? idx - 1 : idx + 1);
        if (lua_isfunction(ls, -1)) {
            if (lua_pcall(ls, 0, 1, 0)) {
                strcpy(errtxt, lua_tostring(ls, -1));
                errsts++;
                lua_pop(ls, 1);
                return NULL;
            }
        }
        if (lua_istable(ls, -1)) {
            result = calloc(cnt + 1, sizeof(char *));
            cnt = 0;
            /* Traverse the table. */
            lua_pushnil(ls);
            while (lua_next(ls, -2)) {
                if (lua_isfunction(ls, -1)) {
                    if (lua_pcall(ls, 0, 1, 0)) {
                        strcpy(errtxt, lua_tostring(ls, -1));
                        errsts++;
                    }
                }
                else if (lua_isstring(ls, -1)) {
                    str = strdup(lua_tostring(ls, -1));
                    /* Is this a script? If yes, execute it and add the results. */
                    if (strlen(str) > 3 && str[0] == '-' && str[1] == '-'&& str[2] == '\n') {
                        script_result = ExecuteLuaChunk(ls, str);
                        /* Add chunk results to array. */
                        if (script_result) {
                            for (i = 0; script_result[i]; i++) {
                                result[cnt++] = script_result[i];
                            }
                            free(script_result);
                        }
                        free(str);
                    }
                    else {
                        result[cnt++] = str;
                    }
                }
                lua_pop(ls, 1);
            }
        }
        result[cnt] = NULL;
        /* Keep the stack unmodified. */
        lua_pop(ls, 1);
    }
    return result;
}

NUTCOMPONENT *FindComponentByName(NUTCOMPONENT * root, char *name)
{
    NUTCOMPONENT *walk;
    NUTCOMPONENT *compo = NULL;

    walk = root->nc_child;
    while (walk && compo == NULL) {
        if (strcmp(walk->nc_name, name) == 0) {
            compo = walk;
            break;
        } else if (walk->nc_child) {
            compo = FindComponentByName(walk, name);
        }
        walk = walk->nc_nxt;
    }
    return compo;
}

/*
 * The component's table is expected on top of the LUA stack.
 */
void LoadComponentOptions(lua_State * ls, NUTCOMPONENT * compo)
{
    char *name;
    NUTCOMPONENTOPTION *opts;

    /* Push the option array on the stack. */
    lua_pushstring(ls, TKN_OPTIONS);
    lua_gettable(ls, -2);
    if (lua_istable(ls, -1)) {
        /* Move the structure pointer to the end of the linked list. */
        opts = compo->nc_opts;
        while (opts && opts->nco_nxt) {
            opts = opts->nco_nxt;
        }
        /* Traverse the option table array. */
        lua_pushnil(ls);
        while (lua_next(ls, -2)) {
            /* Next option table is on top of the stack, retrieve its name. */
            name = GetStringByNameFromTable(ls, -1, TKN_MACRO, NULL, 0);
            if (name) {
                /* Create a new entry in the linked structure list. */
                if (opts) {
                    opts->nco_nxt = calloc(1, sizeof(NUTCOMPONENTOPTION));
                    opts = opts->nco_nxt;
                } else {
                    opts = calloc(1, sizeof(NUTCOMPONENTOPTION));
                    compo->nc_opts = opts;
                }
                /* Set the option's name, which is the name of the C macro. */
                opts->nco_name = name;
                /* Set a component reference. */
                opts->nco_compo = compo;
                /* Retrieve the option's activation requirements. */
                opts->nco_active_if = GetStringByNameFromTable(ls, -1, TKN_ACTIF, NULL, 0);
                /*
                 * If we do not have automatic activation and if this options
                 * has a default value without boolean flavor, then activate it.
                 */
                if (opts->nco_active_if == NULL) {
                    char *flavor = GetStringByNameFromTable(ls, -1, TKN_FLAVOR, NULL, 0);
                    if (flavor == NULL || strncasecmp(flavor, "bool", 4)) {
                        char *defval = GetStringByNameFromTable(ls, -1, TKN_DEFAULT, NULL, 0);
                        if (defval) {
                            opts->nco_active = 1;
                            free(defval);
                        }
                    }
                    if (flavor) {
                        free(flavor);
                    }
                }
                /* Retrieve exclusivity of the option's value. */
                opts->nco_exclusivity = GetStringArrayByNameFromTable(ls, -1, TKN_EXCLUSIVITY);
            }
            /* Remove the option table. */
            lua_pop(ls, 1);
        }
    }
    lua_pop(ls, 1);
}


/*!
 * \brief Recursively load Nut/OS component options.
 */
void LoadOptions(lua_State * ls, NUTCOMPONENT * root, NUTCOMPONENT * compo)
{
    NUTCOMPONENT *subc;
    char *name;


    while (compo) {
        /*
         * Push the component array with the given name on top of the
         * Lua stack and make sure we got a valid result.
         */
        lua_getglobal(ls, compo->nc_name);
        if (lua_istable(ls, -1)) {
            LoadComponentOptions(ls, compo);

            /*
             * Enumerate the array. Start with nil for the first key.
             */
            lua_pushnil(ls);

            /*
             * Each loop will take the key from the top of the stack and
             * push the next key followed by the corresponding value back
             * to the stack.
             */
            while (lua_next(ls, -2)) {
                /*
                 * Now the next value is on top and its key (array index) is below. Components
                 * are specified without a named index. Thus, they have a numeric index.
                 */
                if (lua_isnumber(ls, -2)) {
                    name = GetStringByNameFromTable(ls, -1, TKN_NAME, NULL, 0);
                    if (name) {
                        subc = FindComponentByName(root, name);
                        if (subc) {
                            LoadComponentOptions(ls, subc);
                        }
				    	free(name);
                    }
                }
                lua_pop(ls, 1);
            }
        }
        lua_pop(ls, 1);

        /*
         * Process subcomponents.
         */
        if (compo->nc_child) {
            LoadOptions(ls, root, compo->nc_child);
        }

        compo = compo->nc_nxt;
    }
}

/*!
 * \brief Recursively load Nut/OS components.
 *
 * Components may have siblings and children. Right now, Nut/OS has two levels
 * only. The root component is the repository itself and all Nut/OS components
 * like os, crt, net etc. are children of this root. Anyway, we recursively
 * call this routine to be prepared for future subcomponents.
 *
 * \param ls     Lua state.
 * \param parent Parent component.
 * \param path   Top directory of the repository.
 * \param file   Relative pathname of the script file.
 *
 * \return 0 on success, -1 otherwise.
 *
 * Option name is a global LUA variable.
 */
int LoadComponentTree(lua_State * ls, NUTCOMPONENT * parent, const char *path, const char *file)
{
    int rc = 0;
    char script[255];
    char *name;
    NUTCOMPONENT *compo = NULL;

    /* Build the pathname and check if the script file exists. */
    strcpy(script, path);
    strcat(script, "/");
    strcat(script, file);
    if (access(script, 0)) {
        sprintf(errtxt, "%s: Script not found", file);
        errsts++;
        return -1;
    }

    /*
     * Let the interpreter load and parse the script file. In case of
     * an error, the error text is available on top of the Lua stack.
     */
    if ((rc = luaL_loadfile(ls, script)) != 0) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        return -1;
    }

    /*
     * Run the script. Actually the script doesn't do much except
     * initializing a bunch of global Lua variables.
     */
    if(lua_pcall(ls, 0, 0, 0)) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        return -1;
    }

    /*
     * The component is defined by a Lua array, which name is the
     * name of the parent component. Push this array on top of the
     * Lua stack and make sure we got an array.
     */
    lua_getglobal(ls, parent->nc_name);
    if (!lua_istable(ls, -1)) {
        sprintf(errtxt, "%s: '%s' missing or not an array", file, parent->nc_name);
        errsts++;
        lua_pop(ls, 1);
        return -1;
    }

    /*
     * Enumerate the array. Start with nil for the first key.
     */
    lua_pushnil(ls);

    /*
     * Each loop will take the key from the top of the stack and
     * push the next key followed by the corresponding value back
     * to the stack.
     */
    while (lua_next(ls, -2)) {
        /*
         * Now the next value is on top and its key (array index) is below. Components
         * are specified without a named index. Thus, they have a numeric index.
         */
        if (lua_isnumber(ls, -2)) {

            /*
             * This is a numeric index. Now let's check the value, which is
             * expected to be an array containing a named index 'name'.
             */
            name = GetStringByNameFromTable(ls, -1, TKN_NAME, NULL, 0);
            if (name) {
                /*
                 * The value on top of the stack is an array, which contains
                 * a named item 'name'. We probably found a new component.
                 */
                if (compo == NULL) {
                    /* This is the first child component of our parent. */
                    compo = calloc(1, sizeof(NUTCOMPONENT));
                    parent->nc_child = compo;
                } else {
                    /* Siblings exist already. */
                    compo->nc_nxt = calloc(1, sizeof(NUTCOMPONENT));
                    compo->nc_nxt->nc_prv = compo;
                    compo = compo->nc_nxt;
                }

                /*
                 * Transfer the component's items from Lua to a C structure.
                 */
                compo->nc_parent = parent;
                compo->nc_name = name;
                /* Retrieve the component's activation requirements. */
                compo->nc_active_if = GetStringByNameFromTable(ls, -1, TKN_ACTIF, NULL, 0);
                /* Retrieve exclusivity list for this component. */
                compo->nc_exclusivity = GetStringArrayByNameFromTable(ls, -1, TKN_EXCLUSIVITY);

                /* If this component got any subcomponent, then load it now. */
                if (GetStringByNameFromTable(ls, -1, TKN_SCRIPT, script, sizeof(script))) {
                    if(LoadComponentTree(ls, compo, path, script)) {
                        return -1;
                    }
                }
            }
        }

        /*
         * Remove the value from stack, so the next lua_next will find the
         * key (array index) on top.
         */
        lua_pop(ls, 1);
    }

    /*
     * Remove the key in order to leave the stack like we found it.
     */
    lua_pop(ls, 1);

    return rc;
}

int CompareTableString(lua_State * ls, char *idx, char *str)
{
    int rc = -1;

    if (lua_istable(ls, -1)) {
        char *val = GetStringByNameFromTable(ls, -1, idx, NULL, 0);
        rc = strcmp(str, val);
        free(val);
    }
    return rc;
}

char * GetComponentValue(lua_State * ls, NUTCOMPONENT * compo, char * item)
{
    char *rc = NULL;
    int found_comp = 0;

    lua_getglobal(ls, compo->nc_name);
    if (lua_istable(ls, -1)) {
        rc = GetStringByNameFromTable(ls, -1, item, NULL, 0);
    } 
    if (rc == NULL && compo->nc_parent) {
        lua_pop(ls, 1);
        /* Get table of our parent component, which is hopefully global. */
        lua_getglobal(ls, compo->nc_parent->nc_name);
        if (lua_istable(ls, -1)) {
            /* Traverse component array on top of stack. */
            lua_pushnil(ls);
            while (lua_next(ls, -2)) {
                /* Check the name of this component. */
                if (CompareTableString(ls, TKN_NAME, compo->nc_name) == 0) {
                    found_comp = 1;
                    /* Retrieve the wanted item. */
                    rc = GetStringByNameFromTable(ls, -1, item, NULL, 0);
                }
                /* Remove the component array element. */
                lua_pop(ls, 1);
                if (found_comp) {
                    /* Stop traversion the component array. */
                    lua_pop(ls, 1);
                    break;
                }
            }
        }
    }
    /* Remove the table of the component's parent. */
    lua_pop(ls, 1);

    return rc;
}

static char ** GetComponentTableValues(lua_State * ls, NUTCOMPONENT * compo, char * item)
{
    char **rc = NULL;
    int found_comp = 0;

    lua_getglobal(ls, compo->nc_name);
    if (lua_istable(ls, -1)) {
        rc = GetStringArrayByNameFromTable(ls, -1, item);
    } 
    if (rc == NULL && compo->nc_parent) {
        lua_pop(ls, 1);
        /* Get table of our parent component, which is hopefully global. */
        lua_getglobal(ls, compo->nc_parent->nc_name);
        if (lua_istable(ls, -1)) {
            /* Traverse component array on top of stack. */
            lua_pushnil(ls);
            while (lua_next(ls, -2)) {
                /* Check the name of this component. */
                if (CompareTableString(ls, TKN_NAME, compo->nc_name) == 0) {
                    found_comp = 1;
                    /* Retrieve the wanted item. */
                    rc = GetStringArrayByNameFromTable(ls, -1, item);
                }
                /* Remove the component array element. */
                lua_pop(ls, 1);
                if (found_comp) {
                    /* Stop traversion the component array. */
                    lua_pop(ls, 1);
                    break;
                }
            }
        }
    }
    /* Remove the table of the component's parent. */
    lua_pop(ls, 1);

    return rc;
}

static char * GetOptionString(lua_State * ls, char * macro, char *item)
{
    int found = 0;
    char *rc = NULL;

    /* Get table of component options. */
    lua_pushstring(ls, TKN_OPTIONS);
    lua_gettable(ls, -2);
    if (lua_istable(ls, -1)) {
        /* Traverse option array on top of stack. */
        lua_pushnil(ls);
        while (lua_next(ls, -2)) {
            /* Check the name of this option. */
            if (CompareTableString(ls, TKN_MACRO, macro) == 0) {
                found = 1;
                /* Retrieve the wanted option item. */
                rc = GetStringByNameFromTable(ls, -1, item, NULL, 0);
            }
            /* Remove the option array element. */
            lua_pop(ls, 1);
            if (found) {
                /* Stop traversing the option array. */
                lua_pop(ls, 1);
                break;
            }
        }
    }
    /* Remove the component option table. */
    lua_pop(ls, 1);

    return rc;
}

/*!
 * \brief Retrieve table from specified option item.
 *
 * The component table entry is expected at the Lua stack's top.
 *
 * \param ls    Lua state.
 * \param macro Name of the macro option.
 * \param item  Name of option's item to retrieve.
 *
 * \return Pointer to an array of strings, allocated from heap.
 */
static char ** GetOptionTableStrings(lua_State * ls, char * macro, char *item)
{
    int found = 0;
    char ** rc = NULL;

    /* Get table of component options. */
    lua_pushstring(ls, TKN_OPTIONS);
    lua_gettable(ls, -2);
    if (lua_istable(ls, -1)) {
        /* Traverse option array on top of stack. */
        lua_pushnil(ls);
        while (lua_next(ls, -2)) {
            /* Check the name of this option. */
            if (CompareTableString(ls, TKN_MACRO, macro) == 0) {
                int cnt = 0;
                found = 1;
                /* Retrieve the wanted option item. */
                lua_pushstring(ls, item);
                lua_gettable(ls, -2);
                if (lua_isfunction(ls, -1)) {
                    if (lua_pcall(ls, 0, 1, 0)) {
                        strcpy(errtxt, lua_tostring(ls, -1));
                        errsts++;
                    }
                }
                if (lua_istable(ls, -1)) {
                    lua_pushnil(ls);
                    while (lua_next(ls, -2)) {
                        if (lua_isfunction(ls, -1) || lua_isstring(ls, -1)) {
                            cnt++;
                        }
                        lua_pop(ls, 1);
                    }
                }

                /* Re-balance the stack. */
                lua_pop(ls, 1);

                if (cnt) {
                    rc = calloc(cnt + 1, sizeof(char *));
                    cnt = 0;
                    lua_pushstring(ls, item);
                    lua_gettable(ls, -2);
                    if (lua_isfunction(ls, -1)) {
                        if (lua_pcall(ls, 0, 1, 0)) {
                            strcpy(errtxt, lua_tostring(ls, -1));
                            errsts++;
                        }
                    }
                    if (lua_istable(ls, -1)) {
                        /* Traverse the table. */
                        lua_pushnil(ls);
                        while (lua_next(ls, -2)) {
                            if (lua_isstring(ls, -1)) {
                                rc[cnt++] = strdup(lua_tostring(ls, -1));
                            }
                            lua_pop(ls, 1);
                        }
                    }
                    rc[cnt] = NULL;
                    /* Re-balance the stack. */
                    lua_pop(ls, 1);
                }
            }
            /* Remove the option array element. */
            lua_pop(ls, 1);
            if (found) {
                /* Stop traversing the option array. */
                lua_pop(ls, 1);
                break;
            }
        }
    }
    /* Remove the component option table. */
    lua_pop(ls, 1);

    return rc;
}

static char * GetOptionStringValue(lua_State * ls, NUTCOMPONENT * compo, char * macro, char * item)
{
    char *rc = NULL;
    int found = 0;

    lua_getglobal(ls, compo->nc_name);
    if (lua_istable(ls, -1)) {
        rc = GetOptionString(ls, macro, item);
    } 
    if (rc == NULL && compo->nc_parent) {
        lua_pop(ls, 1);
        /* Get table of our parent component, which is hopefully global. */
        lua_getglobal(ls, compo->nc_parent->nc_name);
        if (lua_istable(ls, -1)) {
            /* Traverse component array on top of stack. */
            lua_pushnil(ls);
            while (lua_next(ls, -2)) {
                /* Check the name of this component. */
                if (CompareTableString(ls, TKN_NAME, compo->nc_name) == 0) {
                    found = 1;
                    rc = GetOptionString(ls, macro, item);
                }
                /* Remove the component array element. */
                lua_pop(ls, 1);
                if (found) {
                    /* Stop traversion the component array. */
                    lua_pop(ls, 1);
                    break;
                }
            }
        }
    }
    /* Remove the table of the component's parent. */
    lua_pop(ls, 1);

    return rc;
}

/*!
 * \brief Retrieve table from specified component option.
 *
 * The component table entry is expected at the Lua stack's top.
 *
 * \param ls    Lua state.
 * \param compo Pointer to a library component.
 * \param macro 
 * \param item
 *
 * \return Pointer to an array of strings, allocated from heap.
 */
static char ** GetOptionTableValues(lua_State * ls, NUTCOMPONENT * compo, char * macro, char * item)
{
    char **rc = NULL;
    int found_comp = 0;

    lua_getglobal(ls, compo->nc_name);
    if (lua_istable(ls, -1)) {
        rc = GetOptionTableStrings(ls, macro, item);
    } 
    if (rc == NULL && compo->nc_parent) {
        lua_pop(ls, 1);
        /* Get table of our parent component, which is hopefully global. */
        lua_getglobal(ls, compo->nc_parent->nc_name);
        if (lua_istable(ls, -1)) {
            /* Traverse component array on top of stack. */
            lua_pushnil(ls);
            while (lua_next(ls, -2)) {
                /* Check the name of this component. */
                if (CompareTableString(ls, TKN_NAME, compo->nc_name) == 0) {
                    found_comp = 1;
                    rc = GetOptionTableStrings(ls, macro, item);
                }
                /* Remove the component array element. */
                lua_pop(ls, 1);
                if (found_comp) {
                    /* Stop traversion the component array. */
                    lua_pop(ls, 1);
                    break;
                }
            }
        }
    }
    /* Remove the table of the component's parent. */
    lua_pop(ls, 1);

    return rc;
}

char * GetGlobalValue(lua_State * ls, char * name)
{
    char *str = NULL;

    lua_getglobal(ls, name);
    if (lua_isfunction(ls, -1)) {
        if (lua_pcall(ls, 0, 1, 0)) {
            strcpy(errtxt, lua_tostring(ls, -1));
            errsts++;
            lua_pop(ls, 1);
            return NULL;
        }
    }
    if (lua_isstring(ls, -1)) {
        str = strdup(lua_tostring(ls, -1));
    }
    else if (lua_isboolean(ls, -1) && lua_toboolean(ls, -1)) {
        str = strdup("");
    }
    lua_pop(ls, 1);

    return str;
}

static int LoadConfigValues(lua_State * ls, NUTCOMPONENT * compo)
{
    NUTCOMPONENTOPTION *opts;

    while (compo) {
        opts = compo->nc_opts;
        while (opts) {
            lua_getglobal(ls, opts->nco_name);
            if (lua_isfunction(ls, -1)) {
                if (lua_pcall(ls, 0, 1, 0)) {
                    strcpy(errtxt, lua_tostring(ls, -1));
                    errsts++;
                    lua_pop(ls, 1);
                    return -1;
                }
            }
            if (lua_isstring(ls, -1)) {

                char *str = strdup(lua_tostring(ls, -1));
                /* Is this a script? If yes, execute it and use the result. */
                if (strlen(str) > 3 && str[0] == '-' && str[1] == '-'&& str[2] == '\n') {
                    char **script_result = ExecuteLuaChunk(ls, str);
                    free(str);
                    str = NULL;
                    /* Add chunk results to array. */
                    if (script_result) {
                        int i;
                        /* Retrieve the result that had been pushed last. */
                        str = script_result[0];
                        /* Discard any remaining results. */
                        for (i = 1; script_result[i]; i++) {
                            free(script_result[i]);
                        }
                        free(script_result);
                    }
                }
                /* 
                 * For backward compatibility. Drop down boxes of enumerated
                 * values didn't accept empty strings. To allow empty values,
                 * we used a space instead, which was a bad hack. Sigh!
                 */
                if (str && (str[0] == '\0' || str[0] == ' ')) {
                    free(str);
                    str = NULL;
                }
                /* Configured values are always active. */
                opts->nco_active = 1;
            }
            else if (lua_isboolean(ls, -1)) {
                /*
                 * Starting from version 2 we should use booleans instead
                 * of empty strings.
                 */
                opts->nco_active = lua_toboolean(ls, -1);
            }
            lua_pop(ls, 1);
            opts = opts->nco_nxt;
        }
        if (compo->nc_child) {
            if (LoadConfigValues(ls, compo->nc_child)) {
                return -1;
            }
        }
        compo = compo->nc_nxt;
    }
    return 0;
}

#if 0
int LuaPanic(lua_State *ls)
{
    int i = 1;
    return 0;
}

int LuaError(lua_State *ls)
{
    return -1;
    //exit(0);
}
#endif

/*!
 * \brief Open a Nut/OS component repository.
 *
 * \param pathname Pathname of the repository file. Use slashes, not backslashes.
 *
 * \return Pointer to a NUTREPOSITORY structure on success or NULL otherwise.
 */
NUTREPOSITORY *OpenRepository(const char *pathname)
{
    char *cp;
    NUTREPOSITORY *repo;
    lua_State * ls;

    if(pathname == NULL || access(pathname, 0)) {
        return NULL;
    }
    if((repo = calloc(1, sizeof(NUTREPOSITORY))) != NULL) {
        /*
         * Cut off the directory path of the repository script. This
         * directory is our root directory. All component scripts will
         * be below this point.
         */
        if((repo->nr_dir = strdup(pathname)) == NULL) {
            free(repo);
            return NULL;
        }
        if ((cp = strrchr(repo->nr_dir, '/')) != NULL) {
            *cp++ = 0;
            repo->nr_name = cp;
        } else {
            repo->nr_dir[0] = 0;
        }

        /*
         * Create a LUA state.
         */
        ls = lua_open();
        if (ls) {
            repo->nr_ls = (void *)ls;
            //lua_atpanic(ls, LuaPanic);
            //lua_cpcall(ls, LuaError, NULL);

#if LUA_VERSION_NUM >= 501
            luaL_openlibs(ls);
#else
            luaopen_base(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_table(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_io(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_string(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_math(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_debug(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
            luaopen_loadlib(repo->nr_ls);
            lua_settop(repo->nr_ls, 0);
#endif
            /* Store pointer to C repository structure in Lua registry. */
            lua_pushstring(ls, LRK_NUTREPOSITORY);
            lua_pushlightuserdata(ls, (void *)repo);
            lua_settable(ls, LUA_REGISTRYINDEX);

            RegisterLuaExtension(ls, nutcomp_lib) ;
        }

    }
    return repo;
}

/*!
 * \brief Close a Nut/OS component repository.
 *
 * \param repo Pointer to a NUTREPOSITORY structure.
 */
void CloseRepository(NUTREPOSITORY *repo)
{
    if(repo) {
        if(repo->nr_dir) {
            free(repo->nr_dir);
        }
        if(repo->nr_ls) {
            lua_close((lua_State *)(repo->nr_ls));
        }
		free(repo);
    }
}

/*!
 * \brief Store source tree path in the Lua registry.
 *
 * \param repo Pointer to the repository information.
 * \param path Pointer to the path name.
 */
int RegisterSourcePath(NUTREPOSITORY *repo, const char *path) 
{
    return RegisterStringValue((lua_State *)(repo->nr_ls), LRK_NUTSOURCEPATH, path);
}

/*!
 * \brief Store build tree path in the Lua registry.
 *
 * \param repo Pointer to the repository information.
 * \param path Pointer to the path name.
 */
int RegisterBuildPath(NUTREPOSITORY *repo, const char *path) 
{
    return RegisterStringValue((lua_State *)(repo->nr_ls), LRK_NUTBUILDPATH, path);
}

/*!
 * \brief Store library path in the Lua registry.
 *
 * \param repo Pointer to the repository information.
 * \param path Pointer to the path name.
 */
int RegisterLibPath(NUTREPOSITORY *repo, const char *path) 
{
    return RegisterStringValue((lua_State *)(repo->nr_ls), LRK_NUTLIBPATH, path);
}

/*!
 * \brief Store sample tree path in the Lua registry.
 *
 * \param repo Pointer to the repository information.
 * \param path Pointer to the path name.
 */
int RegisterSamplePath(NUTREPOSITORY *repo, const char *path) 
{
    return RegisterStringValue((lua_State *)(repo->nr_ls), LRK_NUTSAMPLEPATH, path);
}

/*!
 * \brief Store target compiler platform in the Lua registry.
 *
 * \param repo     Pointer to the repository information.
 * \param platform Pointer to the platform name.
 */
int RegisterCompilerPlatform(NUTREPOSITORY *repo, const char *platform) 
{
    return RegisterStringValue((lua_State *)(repo->nr_ls), LRK_NUTCOMPILERPLATFORM, platform);
}

void ReleaseStringArray(char **stringarray)
{
	int cnt = 0;
	while (stringarray[cnt])
		free(stringarray[cnt++]);
	free(stringarray);
}

void ReleaseComponentOptions(NUTCOMPONENTOPTION *opts)
{
	NUTCOMPONENTOPTION *c;
	while (opts)
	{
		c = opts->nco_nxt;

		if (opts->nco_name) 
            free(opts->nco_name);
        if (opts->nco_active_if) 
            free(opts->nco_active_if);
        if (opts->nco_value) 
            free(opts->nco_value);
        if (opts->nco_exclusivity) 
            ReleaseStringArray(opts->nco_exclusivity);

		free(opts);
		opts = c;
	}
}


void ReleaseComponents(NUTCOMPONENT *comp)
{
	NUTCOMPONENT *child = comp->nc_child, *c;
	while (child)
	{
		c = child->nc_nxt;
		ReleaseComponents(child);
		child = c;
	}

    if (comp->nc_name) 
        free (comp->nc_name);
    if (comp->nc_active_if) 
        free(comp->nc_active_if);
    if (comp->nc_exclusivity) 
        ReleaseStringArray(comp->nc_exclusivity);
	if (comp->nc_opts) 
        ReleaseComponentOptions(comp->nc_opts);
	free (comp);
}

/*!
 * \brief Load Nut/OS repository components.
 *
 * \param repo Pointer to a NUTREPOSITORY structure.
 *
 * \return Root pointer to a linked tree of NUTCOMPONENT structures.
 */
NUTCOMPONENT *LoadComponents(NUTREPOSITORY *repo)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);
    NUTCOMPONENT *root;

    /* Create a repository root component. */
    root = calloc(1, sizeof(NUTCOMPONENT));
    root->nc_name = strdup("repository");

    /*
     * Collect the components first. As a result we will have a tree
     * structure of all components.
     */
    if(LoadComponentTree(ls, root, repo->nr_dir, repo->nr_name)) {
        free(root->nc_name);
        free(root);
        root = NULL;
    }

    /*
     * Now walk along the component tree and collect the options of
     * all components incl. root itself.
     */
    if(root) {
        LoadOptions(ls, root, root);
        /* Store pointer to C repository structure in Lua registry. */
        lua_pushstring(ls, LRK_NUTROOTCOMPONENT);
        lua_pushlightuserdata(ls, (void *)root);
        lua_settable(ls, LUA_REGISTRYINDEX);
    }
    return root;
}

/*!
 * \brief Read Nut/OS component configuration values.
 *
 * \param repo Pointer to a NUTREPOSITORY structure.
 * \param root Pointer to a linked tree of NUTCOMPONENT structures.
 * \param pathname Pathname of the repository file. Use slashes, not backslashes.
 *
 * \return 0 on success or -1 otherwise.
 */
int ConfigureComponents(NUTREPOSITORY *repo, NUTCOMPONENT *root, const char *pathname)
{
    int rc;
    lua_State *ls = (lua_State *)(repo->nr_ls);

    if(ls == NULL || root == NULL || pathname == NULL) {
        return -1;
    }
    if(access(pathname, 0)) {
        sprintf(errtxt, "Failed to access %s", pathname);
        errsts++;
        return -1;
    }

    /* Let the interpreter load the script file. */
    if ((rc = luaL_loadfile(ls, pathname)) != 0) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        return -1;
    }
    if(lua_pcall(ls, 0, 0, 0)) {
        strcpy(errtxt, lua_tostring(ls, -1));
        errsts++;
        return -1;
    }
    return LoadConfigValues(ls, root);
}

/*!
 * \brief Get the brief description of a component.
 *
 * \param repo Pointer to the repository information.
 */
char * GetComponentBrief(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentValue(ls, comp, TKN_BRIEF);
}

/*!
 * \brief Get the detailed description of a component.
 *
 * \param repo Pointer to the repository information.
 */
char * GetComponentDescription(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentValue(ls, comp, TKN_DESC);
}

/*!
 * \brief Get the subdirectory name of a component.
 *
 * \param repo Pointer to the repository information.
 */
char * GetComponentSubdir(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentValue(ls, comp, TKN_SUBDIR);
}

/*!
 * \brief Get the requirements of a component.
 *
 * \param repo Pointer to the repository information.
 */
char **GetComponentRequirements(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentTableValues(ls, comp, TKN_REQUIRES);
}

/*!
 * \brief Get the provisions of a component.
 *
 * \param repo Pointer to the repository information.
 */
char **GetComponentProvisions(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentTableValues(ls, comp, TKN_PROVIDES);
}

/*!
 * \brief Get the source files of a component.
 *
 * \param repo Pointer to the repository information.
 */
char **GetComponentSources(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentTableValues(ls, comp, TKN_SOURCES);
}

/*!
 * \brief Get the target files of a component.
 *
 * \param repo Pointer to the repository information.
 */
char **GetComponentTargets(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentTableValues(ls, comp, TKN_TARGETS);
}

/*!
 * \brief Get the special make definitions of a component.
 *
 * \param repo Pointer to the repository information.
 */
char **GetComponentMakedefs(NUTREPOSITORY *repo, NUTCOMPONENT *comp)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetComponentTableValues(ls, comp, TKN_MAKEDEFS);
}

/*!
 * \brief Get the brief descripton of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionBrief(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_BRIEF);
}

/*!
 * \brief Get the detailed descripton of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionDescription(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_DESC);
}

/*!
 * \brief Get the default value of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionDefault(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_DEFAULT);
}

/*!
 * \brief Get the header file name of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionFile(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_FILE);
}

/*!
 * \brief Get the flavour of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionFlavour(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_FLAVOR);
}

/*!
 * \brief Get the type of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetOptionTypeString(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionStringValue(ls, comp, name, TKN_TYPE);
}

/*!
 * \brief Get the requirements of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char **GetOptionRequirements(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionTableValues(ls, comp, name, TKN_REQUIRES);
}

/*!
 * \brief Get the provisions of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char **GetOptionProvisions(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionTableValues(ls, comp, name, TKN_PROVIDES);
}

/*!
 * \brief Get the available choices of an enumerated component option.
 *
 * \param repo Pointer to the repository information.
 */
char **GetOptionChoices(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionTableValues(ls, comp, name, TKN_CHOICES);
}

/*!
 * \brief Get the special make definitions of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char **GetOptionMakedefs(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetOptionTableValues(ls, comp, name, TKN_MAKEDEFS);
}

/*!
 * \brief Get the current value of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetConfigValue(NUTREPOSITORY *repo, char * name)
{
    lua_State *ls = (lua_State *)(repo->nr_ls);

    return GetGlobalValue(ls, name);
}

/*!
 * \brief Get the current or the default value of a component option.
 *
 * \param repo Pointer to the repository information.
 */
char * GetConfigValueOrDefault(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name)
{
    char *val;

    lua_State *ls = (lua_State *)(repo->nr_ls);

    /* Try configured value, which is always global. */
    val = GetGlobalValue(ls, name);
    if (val == NULL) {
        /* Not configured, retrieve any default. */
        val = GetOptionStringValue(ls, comp, name, TKN_DEFAULT);
    }
    return val;
}

/*!
 * \brief Check if a specific requirement is provided.
 *
 * \param repo Pointer to the repository information.
 */
static int IsProvided(NUTREPOSITORY *repo, NUTCOMPONENT *compo, const char *requirement)
{
    NUTCOMPONENTOPTION *opts;
    int i;

    while (compo) {
        if(compo->nc_enabled) {
            char **cprovides = GetComponentProvisions(repo, compo);
            if(cprovides) {
                for (i = 0; cprovides[i]; i++) {
                    if(strcmp(cprovides[i], requirement) == 0) {
                        ReleaseStringArray(cprovides);
                        return 1;
                    }
                }
                ReleaseStringArray(cprovides);
            }
            opts = compo->nc_opts;
            while (opts) {
                if(opts->nco_enabled && opts->nco_active) {
                    char **oprovides = GetOptionProvisions(repo, compo, opts->nco_name);
                    if (oprovides) {
                        for (i = 0; oprovides[i]; i++) {
                            if(strcmp(oprovides[i], requirement) == 0) {
                                ReleaseStringArray(oprovides);
                                return 1;
                            }
                        }
                        ReleaseStringArray(oprovides);
                    }
                }
                opts = opts->nco_nxt;
            }
            if (IsProvided(repo, compo->nc_child, requirement)) {
                return 1;
            }
        }
        compo = compo->nc_nxt;
    }
    return 0;
}

void EnableSubComponents(NUTCOMPONENT *compo, int enable)
{
    NUTCOMPONENTOPTION *opts;

    while (compo) {
        compo->nc_enabled = enable;
        opts = compo->nc_opts;
        while (opts) {
            opts->nco_enabled = enable;
            opts = opts->nco_nxt;
        }
        EnableSubComponents(compo->nc_child, enable);
        compo = compo->nc_nxt;
    }
}

void EnableComponentTree(NUTCOMPONENT *compo, int enable)
{
    NUTCOMPONENTOPTION *opts;

    compo->nc_enabled = enable;
    opts = compo->nc_opts;
    while (opts) {
        opts->nco_enabled = enable;
        opts = opts->nco_nxt;
    }
    EnableSubComponents(compo->nc_child, enable);
}

typedef struct {
    char **st_array;
    int st_index;
    int st_size;
} STRINGTABLE;

int StringTableSearch(STRINGTABLE *tab, char *str)
{
    int i;

    for (i = 0; i < tab->st_index; i++) {
        if (strcmp(tab->st_array[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

void StringTableAdd(STRINGTABLE *tab, char *str)
{
    if (StringTableSearch(tab, str) < 0) {
        if (tab->st_index >= tab->st_size) {
            tab->st_size += 100;
            tab->st_array = realloc(tab->st_array, tab->st_size * sizeof(char *));
        }
        tab->st_array[tab->st_index++] = strdup(str);
    }
}

STRINGTABLE *StringTableCreate(void)
{
    return calloc(1, sizeof(STRINGTABLE));
}

void StringTableDestroy(STRINGTABLE *tab)
{
    int i;

    if (tab) {
        for (i = 0; i < tab->st_index; i++) {
            free(tab->st_array[i]);
        }
        free(tab);
    }
}

/*!
 * \brief Refresh the component tree.
 *
 * \param repo Pointer to the repository information.
 */
static int RefreshComponentTree(NUTREPOSITORY *repo, NUTCOMPONENT *root, NUTCOMPONENT *compo, STRINGTABLE *provisions)
{
    int rc = 0;
    int i;
    NUTCOMPONENTOPTION *opts;

    while (compo) {
        int cprovided = 1;
        char **crequires = GetComponentRequirements(repo, compo);

        if(crequires) {
            for (i = 0; crequires[i]; i++) {
                if ((cprovided = (StringTableSearch(provisions, crequires[i])) >= 0) == 0) {
                    break;
                }
            }
            for (i = 0; crequires[i]; i++) {
                free(crequires[i]);
            }
            free(crequires);
        }
        if(cprovided != compo->nc_enabled) {
            /* Update this component branch. */
            EnableComponentTree(compo, cprovided);
            rc++;
        }
        if (cprovided) {
            opts = compo->nc_opts;
            while (opts) {
                int oprovided = 1;

                char **orequires = GetOptionRequirements(repo, compo, opts->nco_name);
                if (orequires) {
                    for (i = 0; orequires[i]; i++) {
                        if ((oprovided = (StringTableSearch(provisions, orequires[i])) >= 0) == 0) {
                            break;
                        }
                    }
                    for (i = 0; orequires[i]; i++) {
                        free(orequires[i]);
                    }
                    free(orequires);
                }
                if(oprovided != opts->nco_enabled) {
                    opts->nco_enabled = oprovided;
                    rc++;
                }
                opts = opts->nco_nxt;
            }
            rc += RefreshComponentTree(repo, root, compo->nc_child, provisions);
        }
        compo = compo->nc_nxt;
    }
    return rc;
}

void CollectProvisions(NUTREPOSITORY *repo, NUTCOMPONENT *compo, STRINGTABLE *tab)
{
    NUTCOMPONENTOPTION *opts;
    int i;

    while (compo) {
        if(compo->nc_enabled) {
            char **cprovides = GetComponentProvisions(repo, compo);
            if(cprovides) {
                /* Add component provisions. */
                for (i = 0; cprovides[i]; i++) {
                    StringTableAdd(tab, cprovides[i]);
                }
                ReleaseStringArray(cprovides);
            }
            opts = compo->nc_opts;
            while (opts) {
                if(opts->nco_enabled && opts->nco_active) {
                    char **oprovides = GetOptionProvisions(repo, compo, opts->nco_name);
                    if (oprovides) {
                        /* Add option provisions. */
                        for (i = 0; oprovides[i]; i++) {
                            StringTableAdd(tab, oprovides[i]);
                        }
                        ReleaseStringArray(oprovides);
                    }
                }
                opts = opts->nco_nxt;
            }
            CollectProvisions(repo, compo->nc_child, tab);
        }
        compo = compo->nc_nxt;
    }
}

/*!
 * \brief Refresh the component tree.
 *
 * \param repo Pointer to the repository information.
 *
 * \return 0 on success or -1 on cyclic dependencies.
 */
int RefreshComponents(NUTREPOSITORY *repo, NUTCOMPONENT *root)
{
    int i;

    /* Start with all components enabled. */
    EnableComponentTree(root, 1);

    /* Loop until no new updates appear. Limit the number of loops to
    ** detect cyclic dependencies. */
    for(i = 0; i < 10; i++) {
        STRINGTABLE *provisions = StringTableCreate();
        CollectProvisions(repo, root, provisions);
        if(RefreshComponentTree(repo, root, root, provisions) == 0) {
            StringTableDestroy(provisions);
            return 0;
        }
        StringTableDestroy(provisions);
    }
    return -1;
}

/*
 * \brief Creates the complete directory path to a given pathname of a file.
 *
 * \param path The file's pathname.
 *
 * \return 0 on success, -1 if we failed to create any non existing subdirectoy.
 */
static int CreateDirectoryPath(const char *path)
{
    char subpath[255];
    char *cp;

    if(*path) {
        /*
         * Copy any optional WIN32 device/drive information.
         */
        if((cp = strchr(path, ':')) != 0) {
            for(cp = subpath; *path != ':'; path++, cp++) {
                *cp = *path;
            }
        }
        else {
            cp = subpath;
        }

        /*
         * Copy the first character unchecked. This way we avoid to check the
         * root directory.
         */
        if(*path) {
            *cp++ = *path++;
        }

        /*
         * Create the directory tree, processing path component by path component.
         */
        while(*path) {
            if(*path == '/') {
                *cp = 0;
                if(access(subpath, 0)) {
                    if(mkdir(subpath, S_IRWXU)) {
                        sprintf(errtxt, "Failed to make %s", subpath);
                        errsts++;
                        return -1;
                    }
                }
            }
            *cp++ = *path++;
        }
    }
    return 0;
}

static const char *MakeTargetPath(const char *dir, const char *path)
{
    static char result[255];

    /* Immediately return absolute paths. */
    if (dir[0] == '/' || dir[1] == ':') {
        const char *sp = dir;
        char *dp = result;

        /* I generally hate to have spaces in directories, but in case
           of some weird Win32 paths. */
        while (*sp) {
            if (*sp == ' ') {
                *dp++ = '\\';
            }
            *dp++ = *sp++;
        }
        *dp = 0;

        return result;
    }

    /* Relative path. */
    strcpy(result, path);
    if (*dir) {
        strcat(result, "/");
        strcat(result, dir);
    }
    return result;
}

/*!
 * \brief Add the list of source files to a Makefile.
 *
 * \param repo Pointer to the repository information.
 */
int AddMakeSources(FILE * fp, NUTREPOSITORY *repo, NUTCOMPONENT * compo, const char *sub_dir, int *lpos)
{
    int rc = 0;
    int i;
    NUTCOMPONENT *cop = compo;

    while (cop) {
        if(cop->nc_enabled) {
            char **sources = GetComponentSources(repo, cop);
            char **targets = GetComponentTargets(repo, cop);
            if(sources) {
                for (i = 0; sources[i]; i++) {

                    /*
                     * If sources are located in a subdirectory, make sure
                     * the same directory exists in the build tree.
                     */
                    if(strchr(sources[i], '/')) {
                        char path[255];
                        sprintf(path, "%s/%s", sub_dir, sources[i]);
                        CreateDirectoryPath(path);
                    }

                    /* Check if this source results in an explicit target. */
                    if(targets && targets[i]) {
                        rc++;
                    }
                    else {
                        *lpos += strlen(sources[i]);
                        if (*lpos > 72) {
                            fprintf(fp, " \\\n\t");
                            *lpos = 8;
                        }
                        fprintf(fp, " %s", sources[i]);
                    }
                }
            }
            if (cop->nc_child) {
                rc += AddMakeSources(fp, repo, cop->nc_child, sub_dir, lpos);
            }
        }
        cop = cop->nc_nxt;
    }
    return rc;
}

/*!
 * \brief Add the list of target files to a Makefile.
 *
 * \param repo Pointer to the repository information.
 */
int AddMakeTargets(FILE * fp, NUTREPOSITORY *repo, NUTCOMPONENT * compo, int cnt)
{
    int i;
    int rc = 0;
    NUTCOMPONENT *cop = compo;

    /*
     * If explicit targets are specified, we list the objects and
     * hope that Makerules will do it right.
     */
    while (rc < cnt && cop) {
        if(cop->nc_enabled) {
            char **sources = GetComponentSources(repo, cop);
            char **targets = GetComponentTargets(repo, cop);
            if(sources) {
                for (i = 0; sources[i]; i++) {
                    if(targets && targets[i]) {
                        rc++;
                        fprintf(fp, "OBJ%d = %s\n", rc, targets[i]);
                    }
                }
            }
            if (cop->nc_child) {
                rc += AddMakeTargets(fp, repo, cop->nc_child, cnt);
            }
        }
        cop = cop->nc_nxt;
    }
    return rc;
}

/*!
 * \brief Add the source file list to the Makefile.
 *
 * \param fp      Pointer to an opened file.
 * \param repo    Pointer to the repository information.
 * \param compo   Pointer to a library component.
 * \param sub_dir Component's subdirectory in the build tree.
 *
 * \return Number of explicit target files.
 */
int WriteMakeSources(FILE * fp, NUTREPOSITORY *repo, NUTCOMPONENT * compo, const char *sub_dir)
{
    int rc;
    int lpos;

    fprintf(fp, "SRCS =\t");
    lpos = 8;
    rc = AddMakeSources(fp, repo, compo, sub_dir, &lpos);
    fputc('\n', fp);

    AddMakeTargets(fp, repo, compo, rc);
    fputc('\n', fp);

    return rc;
}

/*!
 * \brief Add the configured lines to the Makefile.
 *
 * \param fp Pointer to an opened file.
 * \param repo Pointer to the repository information.
 * \param compo Pointer to a library component.
 *
 * \todo This is not yet finished. All 'name=value' pairs should
 *       be collected and combined.
 */
static void WriteMakedefLines(FILE * fp, NUTREPOSITORY *repo, NUTCOMPONENT * compo)
{
    NUTCOMPONENTOPTION *opts;
    int i;
    char *value;

    /*
     * Loop through all components.
     */
    while (compo) {
        /* If this component is enabled and contains Makefile macros,
           then simply print them to the file. */
        if(compo->nc_enabled) {
            char **cmakedefs = GetComponentMakedefs(repo, compo);
            if (cmakedefs) {
                for (i = 0; cmakedefs[i]; i++) {
                    fprintf(fp, "%s\n", cmakedefs[i]);
                }
                free(cmakedefs[i]);
            }
            free(cmakedefs);
        }

        /*
         * Loop through all options of this component.
         */
        opts = compo->nc_opts;
        while (opts) {
            if (opts->nco_enabled && opts->nco_active) {
                char **makedefs = GetOptionMakedefs(repo, compo, opts->nco_name);
                if (makedefs) {
                    for (i = 0; makedefs[i]; i++) {
                        fprintf(fp, "%s", makedefs[i]);
                        /* Get edited, configured or default value, in this priority. */
                        if (opts->nco_value) {
                            value = strdup(opts->nco_value);
                        } else {
                            value = GetConfigValueOrDefault(repo, compo, opts->nco_name);
                        }
                        if(strchr(makedefs[i], '=') || value == NULL) {
                            fputc('\n', fp);
                        }
                        else {
                            fprintf(fp, "=%s\n", value);
                        }
                        if (value) {
                            free(value);
                        }
                        free(makedefs[i]);
                    }
                    free(makedefs);
                }
            }
            opts = opts->nco_nxt;
        }
        /* Recursively process the subcomponents. */
        if (compo->nc_child) {
            WriteMakedefLines(fp, repo, compo->nc_child);
        }
        compo = compo->nc_nxt;
    }
}

/*!
 * \brief Add target to the Makefile in the top build directory.
 *
 * \param fp     Pointer to an opened file.
 * \param repo   Pointer to the repository information.
 * \param compo  Pointer to the first child of the root component.
 * \param target Makefile target, set to NULL for 'all'.
 */
void WriteMakeRootLines(FILE * fp, NUTREPOSITORY *repo, NUTCOMPONENT * compo, char *target)
{
    if (target) {
        fprintf(fp, "%s:\n", target);
    } else {
        fprintf(fp, "all:\n");
    }
    while (compo) {
        char *subdir = GetComponentSubdir(repo, compo);
        if (subdir) {
            fprintf(fp, "\t$(MAKE) -C %s", subdir);
            if (target) {
                fprintf(fp, " %s", target);
            }
            fputc('\n', fp);
        }
        compo = compo->nc_nxt;
    }
    fprintf(fp, "\n");
}

/*!
 * \brief Create makefiles from a specified NUTCOMPONENT tree.
 *
 * This routine creates all required Makefiles, one in the top build directory
 * and one in each library's subdirectory. It will also create the NutConf.mk
 * and UserConf.mk. Except for UserConf.mk, any existing file will be replaced.
 *
 * \param repo       Pointer to the repository information.
 * \param root       Pointer to the root component.
 * \param bld_dir    Pathname of the top build directory.
 * \param src_dir    Pathname of the top source directory.
 * \param mak_ext    Filename extension of the Makedefs/Makerules to be used, e.g. avr-gcc.
 * \param ifirst_dir Optional include directory. Header files will be included first
 *                   and thus may replace standard Nut/OS headers with the same name.
 * \param ilast_dir  Optional include directory. Header files will be included last.
 *                   This parameter is typically used to specify the compilers runtime
 *                   library. Header files with the same name as Nut/OS standard headers
 *                   are ignored.
 * \param ins_dir    Final target directory of the Nut/OS libraries. Will be used with
 *                   'make install'.
 *
 * \return 0 on success, otherwise return -1.
 *
 * \todo This function's parameter list is a bit overloaded. Either split the function
 *       or use a parameter structure.
 */
int CreateMakeFiles(NUTREPOSITORY *repo, NUTCOMPONENT *root, const char *bld_dir, const char *src_dir, 
                    const char *mak_ext, const char *ifirst_dir, const char *ilast_dir, const char *ins_dir)
{
    FILE *fp;
    char path[255];
    NUTCOMPONENT *compo;
    int targets;
    int i;
    struct tm *ltime;
    time_t now;

    time(&now);
    ltime = localtime(&now);

    /* Create the Makedefs file */
    sprintf(path, "%s/NutConf.mk", bld_dir);
    if(CreateDirectoryPath(path) == 0) {
        fp = fopen(path, "w");
        if (fp) {
            fprintf(fp, "# Automatically generated on %s", asctime(ltime));
            fprintf(fp, "#\n# Do not edit, modify UserConf.mk instead!\n#\n\n");
            WriteMakedefLines(fp, repo, root->nc_child);
		    fprintf(fp, "\n\ninclude $(top_blddir)/UserConf.mk\n");
            fclose(fp);
        }
    }

    /* Create the user's Makedefs file, if it doesn't exist */
    sprintf(path, "%s/UserConf.mk", bld_dir);
    if(access(path, 0)) {
        fp = fopen(path, "w");
        if (fp) {
            fprintf(fp, "# Automatically created on %s", asctime(ltime));
            fprintf(fp, "#\n# You can use this file to modify values in NutConf.mk\n#\n\n");
            fclose(fp);
        }
    }

    /* Create the root Makefile */
    sprintf(path, "%s/Makefile", bld_dir);
    if(CreateDirectoryPath(path) == 0) {
        fp = fopen(path, "w");
        if (fp) {
            fprintf(fp, "# Do not edit! Automatically generated on %s\n", asctime(ltime));
            WriteMakeRootLines(fp, repo, root->nc_child, NULL);
            if(ins_dir && *ins_dir) {
                WriteMakeRootLines(fp, repo, root->nc_child, "install");
            }
            WriteMakeRootLines(fp, repo, root->nc_child, "clean");
            fclose(fp);
        }
    }

    /* Create library Makefiles */
    compo = root->nc_child;
    while (compo) {
        char *subdir = GetComponentSubdir(repo, compo);
        if (subdir) {
            sprintf(path, "%s/%s", bld_dir, subdir);
            strcat(path, "/Makefile");
            if(CreateDirectoryPath(path) == 0) {
                fp = fopen(path, "w");
                if (fp) {
                    fprintf(fp, "# Do not edit! Automatically generated on %s\n", asctime(ltime));
                    fprintf(fp, "PROJ =\tlib%s\n\n", compo->nc_name);
					fprintf(fp, "top_srcdir = %s\n", MakeTargetPath(src_dir, "../.."));
					fprintf(fp, "top_blddir = %s\n\n", MakeTargetPath(bld_dir, "../.."));

                    fprintf(fp, "VPATH = $(top_srcdir)/%s\n\n", subdir);

                    sprintf(path, "%s/%s", bld_dir, subdir);
                    targets = WriteMakeSources(fp, repo, compo->nc_child, path);

                    fprintf(fp, "OBJS = $(SRCS:.c=.o)\n");
                    //for(i = 0; i < targets; i++) {
                    //    fprintf(fp, "OBJ%d = $(SRC%d:.c=.o)\n", i + 1, i + 1);
                    //}
                    fprintf(fp, "include $(top_blddir)/NutConf.mk\n\n");
                    fprintf(fp, "include $(top_srcdir)/Makedefs.%s\n\n", mak_ext);

                    fprintf(fp, "INCFIRST=$(INCPRE)$(top_blddir)/include ");
                    if(ifirst_dir && *ifirst_dir) {
                        fprintf(fp, " $(INCPRE)%s", ifirst_dir);
                    }
                    fputc('\n', fp);
                    if(ilast_dir && *ilast_dir) {
                        fprintf(fp, "INCLAST = $(INCPRE)%s\n", ilast_dir);
                    }

                    fprintf(fp, "\nall: $(PROJ).a $(OBJS)");
                    for(i = 0; i < targets; i++) {
                        fprintf(fp, " $(OBJ%d)", i + 1);
                    }
                    fprintf(fp, "\n\n");

                    if(ins_dir && *ins_dir) {
                        /* Sigh! What a crap! But the bloody create requires a file. */
                        sprintf(path, "%s/read.me", ins_dir);
                        CreateDirectoryPath(path);

                        fprintf(fp, "install: $(PROJ).a");
                        for(i = 0; i < targets; i++) {
                            fprintf(fp, " $(OBJ%d)", i + 1);
                        }

						strcpy (path, MakeTargetPath(ins_dir, "../.."));

                        fprintf(fp, "\n\t$(CP) $(PROJ).a %s/$(PROJ).a\n", path);
                        for(i = 0; i < targets; i++) {
                            fprintf(fp, "\t$(CP) $(OBJ%d) %s/$(notdir $(OBJ%d))\n", i + 1, path, i + 1);
                        }
                    }
                    fprintf(fp, "\ninclude $(top_srcdir)/Makerules.%s\n\n", mak_ext);

                    fprintf(fp, ".PHONY: clean\n");
                    fprintf(fp, "clean: cleancc cleanedit\n");
                    fprintf(fp, "\t-rm -f $(PROJ).a\n");
                    for(i = 0; i < targets; i++) {
                        fprintf(fp, "\t-rm -f $(OBJ%d)\n", i + 1);
                    }

                    fclose(fp);
                }
            }
        }
        compo = compo->nc_nxt;
    }
    return 0;
}

typedef struct _NUTHEADERMACRO NUTHEADERMACRO;

/*!
 * \brief Linked list of header file macros.
 */
struct _NUTHEADERMACRO {
    NUTHEADERMACRO *nhm_nxt;
    char *nhm_name;
    char *nhm_value;
};

typedef struct _NUTHEADERFILE NUTHEADERFILE;

/*!
 * \brief Linked list of header files.
 */
struct _NUTHEADERFILE {
    NUTHEADERFILE *nhf_nxt;
    char *nhf_path;
    NUTHEADERMACRO *nhf_macros;
};

/*!
 * \brief Get entry to the header file list.
 */
NUTHEADERFILE *GetHeaderFileEntry(NUTHEADERFILE **nh_root, char *filename)
{
    NUTHEADERFILE *nhf;

    /* Add to existing list. */
    if (*nh_root) {
        nhf = *nh_root;
        while (strcasecmp(nhf->nhf_path, filename)) {
            if (nhf->nhf_nxt) {
                nhf = nhf->nhf_nxt;
            } else {
                nhf->nhf_nxt = calloc(1, sizeof(NUTHEADERFILE));
                nhf = nhf->nhf_nxt;
                nhf->nhf_path = strdup(filename);
            }
        }
    }
    /* First entry, create list root. */
    else {
        nhf = calloc(1, sizeof(NUTHEADERFILE));
        nhf->nhf_path = strdup(filename);
        *nh_root = nhf;
    }
    return nhf;
}

/*!
 * \brief Add header file and option to the header file list.
 */
static NUTHEADERFILE *AddHeaderFileMacro(NUTHEADERFILE *nh_root, char *fname, char *macro, char *value)
{
    NUTHEADERFILE *nhf;
    NUTHEADERMACRO *nhm;

    /* Get the linked list entry of this header file. */
    nhf = GetHeaderFileEntry(&nh_root, fname);

    /* Add macro to existing header file entry. */
    if (nhf->nhf_macros) {
        nhm = nhf->nhf_macros;
        while (strcasecmp(nhm->nhm_name, macro)) {
            if (nhm->nhm_nxt) {
                nhm = nhm->nhm_nxt;
            } else {
                nhm->nhm_nxt = calloc(1, sizeof(NUTHEADERMACRO));
                nhm = nhm->nhm_nxt;
                nhm->nhm_name = strdup(macro);
                nhm->nhm_value = strdup(value);
            }
        }
    }

    /* First entry of this header file. */
    else {
        nhm = calloc(1, sizeof(NUTHEADERMACRO));
        nhm->nhm_name = strdup(macro);
        nhm->nhm_value = strdup(value);
        nhf->nhf_macros = nhm;
    }
    return nh_root;
}

/*!
 * \brief Create a linked list of header files and associated macros.
 *
 * \param repo Pointer to the repository information.
 */
NUTHEADERFILE *CreateHeaderList(NUTREPOSITORY *repo, NUTCOMPONENT * compo, NUTHEADERFILE *nh_root)
{
    NUTCOMPONENTOPTION *opts;

    while (compo) {
        opts = compo->nc_opts;
        while (opts) {
            char *fname = GetOptionFile(repo, compo, opts->nco_name);
            /* A file entry is required. */
            if (fname) {
                /* Do not store disabled or inactive items. */
                if(opts->nco_enabled && opts->nco_active) {
                    char *flavor;
                    /* Get edited, configured or default value, in this priority. */
                    char *value = NULL;
                    if (opts->nco_value) {
                        value = strdup(opts->nco_value);
                    } else {
                        value = GetConfigValueOrDefault(repo, compo, opts->nco_name);
                    }
                    if (value) {
                        flavor = GetOptionFlavour(repo, compo, opts->nco_name);
                        if (flavor) {
                            char *cp;

                            if (strcasecmp(flavor, "integer") == 0) {
                                for (cp = value; *cp == ' '; cp++);
                                if (*cp == '\0') {
                                    free(value);
                                    value = NULL;
                                }
                            }
                            free(flavor);
                        }
                    }
                    if (value) {
                        nh_root = AddHeaderFileMacro(nh_root, fname, opts->nco_name, value);
                        free(value);
                    }
                    else {
                        GetHeaderFileEntry(&nh_root, fname);
                    }
                }
                else {
                    GetHeaderFileEntry(&nh_root, fname);
                }
                free(fname);
            }
            opts = opts->nco_nxt;
        }
        if (compo->nc_child) {
            nh_root = CreateHeaderList(repo, compo->nc_child, nh_root);
        }
        compo = compo->nc_nxt;
    }
    return nh_root;
}

/*!
 * \brief Deletes a linked list of header files and associated macros.
 */
void ReleaseHeaderList(NUTHEADERFILE *nh_root)
{
	NUTHEADERFILE *nhf;
	NUTHEADERMACRO *nhm, *c;
	while (nh_root)
	{
		nhm = nh_root->nhf_macros;
        while (nhm) {
            if (nhm->nhm_name) {
                free(nhm->nhm_name);
            }
            if (nhm->nhm_value) {
                free(nhm->nhm_value);
            }
			c = nhm->nhm_nxt;
			free (nhm);
			nhm = c;
		}
		nhf = nh_root->nhf_nxt;
        if (nh_root->nhf_path) {
            free(nh_root->nhf_path);
        }
		free (nh_root);
		nh_root = nhf;
	}
}

/*!
 * \brief Create header files from a specified NUTCOMPONENT tree.
 *
 * This routine creates all build specific header files in the build
 * directory. Existing files will be replaced.
 *
 * \param repo    Pointer to the repository information.
 * \param root    Pointer to the root component.
 * \param bld_dir Pathname of the top build directory.
 *
 * \return 0 on success, otherwise return -1.
 *
 * \todo Release allocated heap space.
 */
int CreateHeaderFiles(NUTREPOSITORY *repo, NUTCOMPONENT * root, const char *bld_dir)
{
    NUTHEADERFILE *nh_root = NULL;
    NUTHEADERFILE *nhf;
    NUTHEADERMACRO *nhm;
    FILE *fp;
    char path[255];
    char exname[255];
    char *cp;
    struct tm *ltime;
    time_t now;

    time(&now);
    ltime = localtime(&now);

    /* Create a linked list of header files with active component options. */
    nh_root = CreateHeaderList(repo, root->nc_child, nh_root);

    for (nhf = nh_root; nhf; nhf = nhf->nhf_nxt) {
        strcpy(path, bld_dir);
        strcat(path, "/");
        strcat(path, nhf->nhf_path);

        strcpy(exname, "_");
        strcat(exname, nhf->nhf_path);
        strcat(exname, "_");
        for(cp = exname; *cp; cp++) {
            if(*cp < '0') {
                *cp = '_';
            }
            else {
                *cp = (char)toupper(*cp);
            }
        }

        /* If no macros are specified, then remove the file. */
        if(nhf->nhf_macros == NULL) {
            unlink(path);
        }
        else if(CreateDirectoryPath(path) == 0) {
            if ((fp = fopen(path, "w")) != 0) {
                fprintf(fp, "#ifndef %s\n", exname);
                fprintf(fp, "#define %s\n\n", exname);
                fprintf(fp, "/*\n * Do not edit! Automatically generated on %s */\n\n", asctime(ltime));
                for (nhm = nhf->nhf_macros; nhm; nhm = nhm->nhm_nxt) {
                    fprintf(fp, "#ifndef %s\n", nhm->nhm_name);
                    fprintf(fp, "#define %s", nhm->nhm_name);
                    if (nhm->nhm_value) {
                        fprintf(fp, " %s", nhm->nhm_value);
                    }
                    fprintf(fp, "\n#endif\n\n");
                }
                fprintf(fp, "\n#endif\n");
                fclose(fp);
            }
            else {
                sprintf(errtxt, "Failed to create %s", path);
                errsts++;
                return -1;
            }
        }
	}
	ReleaseHeaderList (nh_root);
    return 0;
}

/*!
 * \brief Create build directory for Nut/OS applications.
 *
 * This routine creates Makedefs and Makerules in the specified directory.
 * It will also create the NutConf.mk and UserConf.mk. Except for UserConf.mk,
 * any existing file will be replaced.
 *
 * \param repo       Pointer to the repository information.
 * \param root       Pointer to the root component.
 * \param bld_dir    Pathname of the top build directory.
 * \param app_dir    Pathname of the application build directory.
 * \param src_dir    Pathname of the top source directory.
 * \param lib_dir    Pathname of the directory containing the libraries.
 * \param mak_ext    Filename extension of the platform specific Makedefs/Makerules, e.g. avr-gcc.
 * \param prg_ext    Filename extension of the programmer specific Makedefs/Makerules, e.g. uisp-avr.
 * \param ifirst_dir Optional include directory. Header files will be included first
 *                   and thus may replace standard Nut/OS headers with the same name.
 * \param ilast_dir  Optional include directory. Header files will be included last.
 *                   This parameter is typically used to specify the compilers runtime
 *                   library. Header files with the same name as Nut/OS standard headers
 *                   are ignored.
 *
 * \return 0 on success, otherwise return -1.
 */
int CreateSampleDirectory(NUTREPOSITORY *repo, NUTCOMPONENT * root, const char *bld_dir, const char *app_dir, 
                          const char *src_dir, const char *lib_dir, const char *mak_ext, const char *prg_ext,
                          const char *ifirst_dir, const char *ilast_dir)
{
    FILE *fp;
    char path[255];
    struct tm *ltime;
    time_t now;

    time(&now);
    ltime = localtime(&now);

    sprintf(path, "%s/NutConf.mk", app_dir);
    if(CreateDirectoryPath(path) == 0) {
        /* Create the configuration Makedefs file */
        fp = fopen(path, "w");
        if (fp) {
            fprintf(fp, "# Automatically generated on %s", asctime(ltime));
            fprintf(fp, "#\n# Do not edit, modify UserConf.mk instead!\n#\n\n");
            WriteMakedefLines(fp, repo, root->nc_child);
            fprintf(fp, "\n\ninclude $(top_appdir)/UserConf.mk\n");
            fclose(fp);
        }

        /* Create the user's Makedefs file, if it doesn't exist */
        sprintf(path, "%s/UserConf.mk", app_dir);
        if(access(path, 0)) {
            fp = fopen(path, "w");
            if (fp) {
                fprintf(fp, "# Automatically created on %s", asctime(ltime));
                fprintf(fp, "#\n# You can use this file to modify values in NutConf.mk\n#\n\n");
                fclose(fp);
            }
        }

        /* Create the application Makedefs. */
        sprintf(path, "%s/Makedefs", app_dir);
        if ((fp = fopen(path, "w")) != 0) {
            fprintf(fp, "# Do not edit! Automatically generated on %s\n", asctime(ltime));

            fprintf(fp, "top_srcdir = %s\n", MakeTargetPath(src_dir, "../.."));
            fprintf(fp, "top_blddir = %s\n", MakeTargetPath(bld_dir, "../.."));


			//fprintf(fp, "top_appdir = %s\n", app_dir);
            fprintf(fp, "top_appdir = %s\n\n", MakeTargetPath(app_dir, "../.."));

			//fprintf(fp, "LIBDIR = %s\n\n", lib_dir);
            fprintf(fp, "LIBDIR = %s\n", MakeTargetPath(lib_dir, "../.."));

            fprintf(fp, "INCFIRST=$(INCPRE)$(top_blddir)/include ");
            if(ifirst_dir && *ifirst_dir) {
                fprintf(fp, " $(INCPRE)%s", ifirst_dir);
            }
            fputc('\n', fp);
            if(ilast_dir && *ilast_dir) {
                fprintf(fp, "INCLAST = $(INCPRE)%s\n", ilast_dir);
            }
            fprintf(fp, "include $(top_appdir)/NutConf.mk\n");
            fprintf(fp, "include $(top_srcdir)/app/Makedefs.%s\n", mak_ext);
            fprintf(fp, "include $(top_srcdir)/app/Makeburn.%s\n\n", prg_ext);
            fclose(fp);
        }
        else {
            sprintf(errtxt, "Failed to create %s", path);
            errsts++;
            return -1;
        }

        /* Create the application Makerules. */
        sprintf(path, "%s/Makerules", app_dir);
        if ((fp = fopen(path, "w")) != 0) {
            fprintf(fp, "# Do not edit! Automatically generated on %s\n\n", asctime(ltime));
            fprintf(fp, "include $(top_srcdir)/app/Makerules.%s\n", mak_ext);
            fclose(fp);
        }
        else {
            sprintf(errtxt, "Failed to create %s", path);
            errsts++;
            return -1;
        }
    }
    else {
        sprintf(errtxt, "Failed to create directory for %s", path);
        errsts++;
        return -1;
    }
    return 0;
}

#ifdef NUT_CONFIGURE_EXEC

/*!
 * \brief Display program usage.
 */
void usage(void)
{
    fputs("Usage: nutconfigure OPTIONS ACTIONS\n"
      "OPTIONS:\n"
      "-a<dir>  application directory (./nutapp)\n"
      "-b<dir>  build directory (./nutbld)\n"
      "-c<file> configuration file (./nut/conf/ethernut21b.conf)\n"
      "-i<dir>  first include path ()\n"
      "-j<dir>  last include path ()\n"
      "-l<dir>  library directory ()\n"
      "-m<type> target platform (avr-gcc)\n"
      "-p<type> programming adapter (avr-dude)\n"
      "-q       quiet (verbose)\n"
      "-s<dir>  source directory (./nut)\n"
      "-r<file> repository (./nut/conf/repository.nut)\n"
      "ACTIONS:\n"
      "create-buildtree\n"
      "create-apptree\n"
    , stderr);
}

/*!
 * \brief Copy a file.
 *
 * \param src_path Source pathname.
 * \param dst_path Destination pathname.
 * \param quiet   If not 0, then progress and error output will be suppressed.
 *
 * \return 0 on success, -1 otherwise.
 */
int copy_file(char *src_path, char *dst_path, int quiet)
{
    int rc = -1;
    int fs;
    int fd;
    char buf[512];
    int cnt;

    if(CreateDirectoryPath(dst_path)) {
        return -1;
    }
    if((fs = open(src_path, O_BINARY)) != -1) {
        if((fd = open(dst_path, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE )) != -1) {
            for(;;) {
                if((cnt = read(fs, buf, sizeof(buf))) < 0) {
                    if (!quiet) {
                        printf("Error %d reading %s\n", errno, src_path);
                    }
                    break;
                }
                if (cnt == 0) {
                    rc = 0;
                    break;
                }
                if(write(fd, buf, cnt) != cnt) {
                    if (!quiet) {
                        printf("Error %d writing %s\n", errno, dst_path);
                    }
                    break;
                }
            }
            close(fd);
        }
        else {
            if (!quiet) {
                printf("Error %d opening %s\n", errno, dst_path);
            }
        }
        close(fs);
    }
    else {
        if (!quiet) {
            printf("Error %d opening %s\n", errno, src_path);
        }
    }
    return rc;
}

/*!
 * \brief Copy directory with application samples.
 *
 * \param src_dir Source directory.
 * \param dst_dir Destination directory.
 * \param quiet   If not 0, then progress and error output will be suppressed.
 *
 * \return 0 on success, -1 otherwise.
 */
int copy_appdir(char *src_dir, char *dst_dir, int quiet)
{
    int rc = 0;
    char src_path[256];
    char dst_path[256];
    DIR *dir;
    struct dirent *dire;
    struct stat statbuf;

    if((dir = opendir(src_dir)) == NULL) {
        if (!quiet) {
            printf("Failed to scan directory %s\n", src_dir);
        }
        return -1;
    }
    if(!quiet) {
        printf("Copying %s\n", src_dir);
    }
    while((dire = readdir(dir)) != NULL && rc == 0) {
        /* Do not copy dot files or Makefiles for source tree builds. */
        if(dire->d_name[0] == '.' ||
           strcasecmp(dire->d_name, "cvs") == 0 ||
		   strcasecmp(dire->d_name, ".svn") == 0 ||
           strncasecmp(dire->d_name, "Makeburn", 8) == 0 ||
           strncasecmp(dire->d_name, "Makedefs", 8) == 0 ||
           strncasecmp(dire->d_name, "Makerules", 9) == 0) {
            continue;
        }

        strcpy(dst_path, dst_dir);
        strcat(dst_path, "/");
        strcat(dst_path, dire->d_name);

        strcpy(src_path, src_dir);
        strcat(src_path, "/");
        strcat(src_path, dire->d_name);
        stat(src_path, &statbuf);

        if(statbuf.st_mode & S_IFDIR) {
            rc = copy_appdir(src_path, dst_path, quiet);
        }
        /* Regular file. Do not overwrite any existing source file. */
        else if((statbuf.st_mode & S_IFREG) == S_IFREG && access(dst_path, 0) != 0) {
            copy_file(src_path, dst_path, quiet);
        }
    }
    closedir(dir);
    return rc;
}

/*!
 * \brief Running without GUI.
 *
 * All settings are passed as command line options.
 */
int main(int argc, char **argv)
{
    int rc = 3;
    int option;
    int quiet = 0;
    char *app_dir = strdup("./nutapp");
    char *bld_dir = strdup("./nutbld");
    char *conf_name = strdup("./nut/conf/ethernut21b.conf");
    char *ifirst_dir = strdup("");
    char *ilast_dir = strdup("");
    char *lib_dir = strdup("");
    char *mak_ext = strdup("avr-gcc");
    char *prg_ext = strdup("avr-dude");
    char *src_dir = strdup("./nut");
    char *repo_name = strdup("./nut/conf/repository.nut");
    NUTREPOSITORY *repo;
    NUTCOMPONENT *root;

    while((option = getopt(argc, argv, "a:b:c:i:j:l:m:p:s:r:v?")) != EOF) {
        switch(option) {
        case 'a':
            free(app_dir);
            app_dir = strdup(optarg);
            break;
        case 'b':
            free(bld_dir);
            bld_dir = strdup(optarg);
            break;
        case 'c':
            free(conf_name);
            conf_name = strdup(optarg);
            break;
        case 'i':
            free(ifirst_dir);
            ifirst_dir = strdup(optarg);
            break;
        case 'j':
            free(ilast_dir);
            ilast_dir = strdup(optarg);
            break;
        case 'l':
            free(lib_dir);
            lib_dir = strdup(optarg);
            break;
        case 'm':
            free(mak_ext);
            mak_ext = strdup(optarg);
            break;
        case 'p':
            free(prg_ext);
            prg_ext = strdup(optarg);
            break;
        case 'q':
            quiet = 1;
            break;
        case 's':
            free(src_dir);
            src_dir = strdup(optarg);
            break;
        case 'r':
            free(repo_name);
            repo_name = strdup(optarg);
            break;
        default:
            usage();
            return 1;
        }
    }
    argc -= optind;
    argv += optind;

    if (*lib_dir == '\0') {
        free(lib_dir);
        lib_dir = malloc(strlen(bld_dir) + 5);
        strcpy(lib_dir, bld_dir);
        strcat(lib_dir, "/lib");
    }

    if(!quiet) {
        printf("nutconfigure %s\n", NUT_CONFIGURE_VERSION);
        printf("Loading %s...", repo_name);
    }

    /*
     * The repository is the top Lua script.
     */
    if ((repo = OpenRepository(repo_name)) != NULL) {
        /* Load the component tree. */
        if ((root = LoadComponents(repo)) != NULL) {
            if(!quiet) {
                printf("OK\nLoading %s...", conf_name);
            }
            /* Load the hardware specific configuration file. */
            if (ConfigureComponents(repo, root, conf_name)) {
                if(!quiet) {
                    printf("%s\n", GetScriptErrorString());
                }
            }
            else {
                if(!quiet) {
                    printf("OK\n");
                }
                /* Store options in the Lua registry. */
                RegisterSourcePath(repo, src_dir);
                RegisterBuildPath(repo, bld_dir);
                RegisterLibPath(repo, lib_dir);
                RegisterSamplePath(repo, app_dir);
                RegisterCompilerPlatform(repo, mak_ext);
                RefreshComponents(repo, root);
                if(argc == 0) {
                    if(!quiet) {
                        printf("Nothing to do\n");
                    }
                }
                else if(strcmp(argv[0], "create-buildtree") == 0) {
                    if(!quiet) {
                        printf("Creating Makefiles for %s in %s...", mak_ext, bld_dir);
                    }
                    if (CreateMakeFiles(repo, root, bld_dir, src_dir, mak_ext, ifirst_dir, ilast_dir, lib_dir)) {
                        if(!quiet) {
                            printf("failed\n");
                        }
                    }
                    else {
                        if(!quiet) {
                            printf("OK\nCreating header files in %s...", bld_dir);
                        }
                        if (CreateHeaderFiles(repo, root, bld_dir)) {
                            if(!quiet) {
                                printf("failed\n");
                            }
                        }
                        else {
                            if(!quiet) {
                                printf("OK\n");
                            }
                            rc = 0;
                        }
                    }
                }
                else if(strcmp(argv[0], "create-apptree") == 0) {
                    if (CreateSampleDirectory(repo, root, bld_dir, app_dir, src_dir, lib_dir, mak_ext, prg_ext, ifirst_dir, ilast_dir)) {
                        if(!quiet) {
                            printf("failed\n");
                        }
                    }
                    else {
                        if(!quiet) {
                            printf("OK\n");
                        }
                        src_dir = realloc(src_dir, strlen(src_dir) + 5);
                        strcat(src_dir, "/app");
                        rc = copy_appdir(src_dir, app_dir, quiet);
                    }
                }
                else if(!quiet) {
                    usage();
                }
            }
        }
        else if(!quiet) {
            printf("%s\n", GetScriptErrorString());
        }
        CloseRepository(repo);
    }
    else if(!quiet) {
        printf("failed\n");
    }
    return rc;
}

#endif
