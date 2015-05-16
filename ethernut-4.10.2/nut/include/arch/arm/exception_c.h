#ifndef ARCH_ARM_EXCEPTION_C_H
#define ARCH_ARM_EXCEPTION_C_H

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
 * \file include/arch/arm/exception_c.h
 *
 * \verbatim
 * $Id: exception_c.h 2400 2009-01-16 19:44:05Z haraldkipp $
 * \endverbatim
 */

// Not implemented void ARM_NOTUSED_Handler( struct pt_regs *p );
void ARM_COMMON_Handler_crash( struct pt_regs *p, const char *name );
void ARM_DA_Handler( struct pt_regs *p );
// Not implemented void ARM_IRQ_Handler( struct pt_regs *p );
// Not implemented void ARM_FIQ_Handler( struct pt_regs *p );
void ARM_PFA_Handler( struct pt_regs *p );
void ARM_SWI_Handler( struct pt_regs *p );
void ARM_UDF_Handler( struct pt_regs *p );

#endif

