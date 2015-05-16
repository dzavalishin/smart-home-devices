#ifndef _NUTCPP_H_
#define _NUTCPP_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2006/07/10 14:28:06  haraldkipp
 * Defined functions for new and delete operator.
 *
 * Revision 1.1  2005/01/22 19:10:54  haraldkipp
 * Added C++ support contributed by Oliver Schulz (MPI).
 *
 *
 */



#if defined(__cplusplus)

// ====================================================================

#include <compiler.h>
#include <stdlib.h>

// == Adapt NutOS main() hack for gcc to C++ ==================================

#if defined(__GNUC__) && defined(main)

#undef main
#define main    _cxx_main_
#define _cxx_pre_main_    NutAppMain

int main(void);

#endif // defined(__cplusplus) && defined(__GNUC__) && defined(main)


// == Define new and delete if needed =================================

#if defined(__GNUC__) && ((defined(__AVR) || defined(__arm__)))
//inline void * operator new(unsigned int s) {
inline void * operator new(size_t s) {
    return malloc(s);
}

inline void operator delete(void *m) {
    free(m);
}
#endif // defined(__GNUC__) && defined(__AVR)


// ====================================================================

#endif // defined(__cplusplus)
#endif // _NUTCPP_H_
