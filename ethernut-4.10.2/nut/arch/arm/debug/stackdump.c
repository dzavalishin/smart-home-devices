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
 * \file arch/arm/debug/stackdump.c
 *
 * \verbatim
 * $Id: stackdump.c 2399 2009-01-16 19:39:07Z haraldkipp $
 * \endverbatim
 */

#include <stdio.h>
#include <sys/ptrace.h>

#include <arch/arm/ptrace.h>

/*
 * The original code doesn't work for Nut/OS. Without understanding
 * the underlying frame layout, I found the solution by trial and
 * error. I assume, that the difference between Nut/OS and LostARM
 * is caused by compile options.
 *
 * This early release had been tested with an AT91SAM7SE512 (EIR 1.0C)
 * and data abort exceptions only.
 *
 * For Nut/OS we are save to use DEV_DEBUG with stdio.
 */
void
ptrace_stackdump_from( const int *fp )
{
  int depth;
  const int *newfp;
  int  pc;

  puts("Backtrace:");
  /* Added for Nut/OS. No idea why this is required. */
  fp = (int *)fp[-4];
  for(depth = 0; fp != NULL; depth++){
    /* LostARM references fp[-1]. Why this differs? */
    pc = fp[0];
    printf(" %d) 0x%08x\n", depth, pc);

    newfp = (const int *)fp[-3];
    /* is new frame pointer some what valid? */
    if( (newfp <= fp) || (newfp > &(fp[1024])) ){
      /* no, then stop */
      break;
    }
    fp = newfp;
  }
}

void
ptrace_stackdump_regs( struct pt_regs *p )
{
  ptrace_dump_regs( p );
  ptrace_stackdump_from( (int *)(p->uregs[ PTRACE_R13_idx ]) );
}

void
ptrace_stackdump_here( void )
{
  const int *fp;
  asm volatile ("mov %0,fp\n" : "=r" (fp) );

  ptrace_stackdump_from( fp );
}
