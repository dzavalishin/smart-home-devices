/*
 * Copyright (C) 2008 by Thermotemp GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THERMOTEMP GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THERMOTEMP
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <sys/mutex.h>
#include "default.h"

#define LEVEL_NEVER  0
#define LEVEL_ERROR  1
#define LEVEL_WARN   2
#define LEVEL_INFO   3
#define LEVEL_ALWAYS 4

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL LEVEL_ALWAYS
#endif

#define DEBUG(_LEVEL, _FORMAT, ...)                                            \
	do {                                                                       \
		if((_LEVEL) <= (DEBUG_LEVEL)) {                                        \
			fprintf(stderr, _FORMAT, ## __VA_ARGS__);                          \
            fflush(stderr);                                                    \
		}                                                                      \
	} while(0)

#define ERROR(_FORMAT, ...) DEBUG(LEVEL_ERROR, _FORMAT, ## __VA_ARGS__)
#define WARN(_FORMAT, ...)  DEBUG(LEVEL_WARN,  _FORMAT, ## __VA_ARGS__)
#define INFO(_FORMAT, ...)  DEBUG(LEVEL_INFO,  _FORMAT, ## __VA_ARGS__)

#endif
