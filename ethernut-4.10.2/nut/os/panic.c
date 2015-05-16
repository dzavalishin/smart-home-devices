/*
 * Copyright (C) 2009 by egnite GmbH
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

/*!
 * \file os/panic.c
 * \brief Panic handler.
 *
 * \verbatim
 * $Id: panic.c 2458 2009-02-13 14:45:20Z haraldkipp $
 * \endverbatim
 */

#include <stdio.h>
#include <sys/atom.h>
#include <sys/nutdebug.h>

/*!
 * \brief This function is called on fatal errors.
 *
 * The function will enter a critical section and print a description
 * of the problem to stdout. It is assumed, that stdout is available 
 * and has been assigned to a debug output device, such as devDebug or
 * similar. Then the function will enter an endless loop, which
 * freezes the system.
 *
 * If this is not, what the application is expected to do on fatal
 * errors or if no debug device has been assigned to stdout, the
 * application must define its own version of this routine.
 *
 * \param fmt Format string containing conversion specifications.
 */
void NUTPANIC(CONST char *fmt, ...)
{
    va_list ap;

    NutEnterCritical();
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    for(;;);
}
