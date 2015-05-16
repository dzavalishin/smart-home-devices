#ifndef _INCLUDE_LUANUT_H_
#define _INCLUDE_LUANUT_H_

#include <cfg/lua.h>

#ifndef NUTLUA_FLOATING_POINT
/*! \brief Enable Lua integer mode. */
#define LUA_NUMBER_INT
#endif

/*! \name Unsupported Lua functions. */
/*@{*/
#define NUTLUA_IOLIB_TMPFILE_NOT_IMPLEMENTED
#define NUTLUA_IOLIB_SETVBUF_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_CLOCK_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_DATE_STRING_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_REMOVE_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_TMPNAME_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_DIFFTIME_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_EXIT_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_RENAME_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_EXECUTE_NOT_IMPLEMENTED
#define NUTLUA_OSLIB_SETLOCALE_NOT_IMPLEMENTED
/*@}*/

/* math lib fixes. */
#ifdef LUA_NUMBER_INT
#undef HUGE_VAL
#define HUGE_VAL    __INT_MAX__
#else
#ifndef HUGE_VAL
#define HUGE_VAL    __DBL_MAX__
#endif
#endif

#if defined(__AVR__)
#include <sys/environ.h>
/* Hack. */
#define strerror(errno) ("Error")
#endif /* __AVR__ */

#endif
