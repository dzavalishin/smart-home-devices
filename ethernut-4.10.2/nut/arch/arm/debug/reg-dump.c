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
 * \file arch/arm/debug/reg-dump.c
 *
 * \verbatim
 * $Id: reg-dump.c 2399 2009-01-16 19:39:07Z haraldkipp $
 * \endverbatim
 */

#include <stdio.h>
#include <sys/ptrace.h>
#include <arch/arm/ptrace.h>

/*
 * For Nut/OS we can use DEV_DEBUG with stdio.
 */

void
ptrace_dump_regs( struct pt_regs *p )
{
  int x;
  char *mode;

  for (x = PTRACE_R0_idx; x <= PTRACE_R7_idx; x++) {
    printf("R%d : 0x%08lx   ", x, p->uregs[x]);
    x += PTRACE_R8_idx;
    printf("R%d%s: 0x%08lx\n", x, x < 10 ? " " : "", p->uregs[x]);
    x -= PTRACE_R8_idx;
  }
  printf("PSW: 0x%08lx ", p->uregs[ PTRACE_CPSR_idx ] );
  /* now decode the flags */
  x = p->uregs[ PTRACE_CPSR_idx ];
  putchar( ( x & ARM_CPSR_N_BIT ) ? 'N' : 'n' );
  putchar( ( x & ARM_CPSR_Z_BIT ) ? 'Z' : 'z' );
  putchar( ( x & ARM_CPSR_C_BIT ) ? 'C' : 'c' );
  putchar( ( x & ARM_CPSR_V_BIT ) ? 'V' : 'v' );
  printf("...");
  putchar( ( x & ARM_CPSR_F_BIT ) ? 'F' : 'f' );
  putchar( ( x & ARM_CPSR_I_BIT ) ? 'I' : 'i' );
  putchar( ( x & ARM_CPSR_T_BIT ) ? 'T' : 't' );
  putchar(' ');
  switch( ARM_MODE_MASK & x ){
  case ARM_USR_MODE:
    mode = "user";
    break;
  case ARM_FIQ_MODE:
    mode = "fiq";
    break;
  case ARM_IRQ_MODE:
    mode = "irq";
    break;
  case ARM_SVC_MODE:
    mode = "svc";
    break;
  case ARM_ABT_MODE:
    if( p->uregs[ PTRACE_FRAMETYPE_idx ] == PTRACE_FRAMETYPE_pfa ){
      mode = "pfa";
    } else if( p->uregs[ PTRACE_FRAMETYPE_idx ] == PTRACE_FRAMETYPE_da  ){
      mode = "da";
    } else {
      mode = "unknown-abort";
    }
    break;
  case ARM_UND_MODE:
    mode = "und";
    break;
  case ARM_SYS_MODE:
    mode = "sys";
    break;
  default:
    mode = NULL;
    break;
  }
  if (mode)
    printf("%s-mode\n", mode);
}
