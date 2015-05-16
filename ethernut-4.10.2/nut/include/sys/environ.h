#ifndef SYS_ENVIRON_H_
#define SYS_ENVIRON_H_

/*
 * Copyright 2008 by egnite Software GmbH
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

#include <cfg/eeprom.h>
#include <sys/types.h>

#ifndef ENVIRON_EE_OFFSET
#define ENVIRON_EE_OFFSET       256
#endif

#ifndef MAX_ENVIRON_ITEM_SIZE
#define MAX_ENVIRON_ITEM_SIZE   64
#endif

#define ENVIRON_MAGIC   0x00564E45

typedef struct _NUTENVIRONMENT {
    struct _NUTENVIRONMENT *env_next;
    struct _NUTENVIRONMENT *env_prev;
    char *env_name;
    char *env_value;
} NUTENVIRONMENT;

extern NUTENVIRONMENT *nut_environ;

extern NUTENVIRONMENT *findenv(CONST char *name);

extern char *nextenv(CONST char *name);

extern char *getenv(CONST char *name);
#if defined (__AVR__)
extern int setenv(CONST char *name, CONST char *value, int force);
extern void unsetenv(CONST char *name);
extern int putenv(CONST char *string);
#endif

#if defined (__CROSSWORKS_ARM)

#include <cfg/crt.h>

extern int setenv(CONST char *name, CONST char *value, int force);
extern int putenv(char *string);

#ifdef CRT_UNSETENV_POSIX
extern int unsetenv(CONST char *name);
#else
extern void unsetenv(CONST char *name);
#endif

#endif /* (__CROSSWORKS_ARM) */

#endif
