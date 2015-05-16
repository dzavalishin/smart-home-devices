/*
 * Copyright (C) 2008 by Duane Ellis
 *
 * All rights reserved.
 *
 * The original code had been released as part of the LoastARM Project 
 * under GPL Version 2 and is published here under the following license
 * with kind permission from the author:
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
 * For additional information see http://lostarm.sourceforge.net/
 */

/*!
 * \file arch/arm/debug/common_xxx_handler.c
 *
 * \verbatim
 * $Id: common_xxx_handler.c 2399 2009-01-16 19:39:07Z haraldkipp $
 * \endverbatim
 */

#include <stdio.h>
#include <sys/ptrace.h>
#include <arch/arm/exception_c.h>

static const char spinner[] = {
  '|',
  '/',
  '-',
  '\\',
  '|',
  '/',
  '-',
  '\\'
};

static char spinner_idx;

void
ARM_COMMON_Handler_crash( struct pt_regs *p, const char *name )
{
  int x;
  printf("\nUnexpected: %s\n", name );
  ptrace_stackdump_regs( p );
  x = 0;
  for(;;){
    /* these numbers are all bullshit */
    /* We cannot depend on any hardware */
    /* the system has PANICed */
    /* We have to do this in software */
    
    /* ASSUME 50mhz CPU, a 200mhz cpu is 4x faster */
    /* ASSUME goal is 20 ticks per second */
    /* ASSUME each loop = 20 instructions */
    /* assume each instruction about 3 clocks */
    /* thus - each loop = 60 clocks  */
    /* make the math easy - use 50 clocks */
    /* thus 50,000,000 / 50 => 100,000 = 1 second */
    /* we want 5 ticks per second, so 20,000 is good */
    /* power of 2 is a better thing - we'll use 32K instead */

    if( x == 0 ){
      putchar(8);
      x = spinner_idx;
      spinner_idx = (x+1) & 7;
      putchar(spinner[x]);
      x = (1<<16);
    }
    x--;
  }
}
