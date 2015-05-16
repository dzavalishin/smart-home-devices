/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.4  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2007/09/11 13:41:23  haraldkipp
 * NutEnter/ExitCritical destroyed R0 (ICCAVR).
 *
 * Revision 1.2  2005/07/26 15:47:06  haraldkipp
 * AtomicInc() and AtomicDec() are no longer required by Nut/Net.
 * Removed to simplify the porting job. Broken applications should
 * implement their own version.
 *
 * Revision 1.1  2005/06/06 10:49:35  haraldkipp
 * Building outside the source tree failed. All header files moved from
 * arch/cpu/include to include/arch/cpu.
 *
 * Revision 1.1  2005/05/27 17:41:52  drsung
 * Moved the file.
 *
 * Revision 1.1  2005/05/26 10:08:42  drsung
 * Moved the platform dependend code from include/sys/atom.h to this file.
 *
 *
 */
 
#ifndef _SYS_ATOM_H_
#error "Do not include this file directly. Use sys/atom.h instead!"
#endif

#ifdef __IMAGECRAFT__

/*!
 * \brief Enter critical section.
 *
 * Disables context switching until NutExitCritical() is called.
 * In Nut/OS only interrupt routines can preempt the currently running
 * thread. Thus, this routine simply disables CPU interrupts. However,
 * in order to allow nesting, the current interrupt enable status is
 * saved on the stack.
 *
 * Note, that after calling NutEnterCritical(), a corresponding 
 * NutExitCritical() must be called before returning from the
 * currently running subroutine.
 */
#define NutEnterCritical()  \
{                           \
    register uint8_t sreg = inb(SREG);   \
    asm("cli\n"             \
        "push %sreg\n");    \
}

/*!
 * \brief Exit critical section.
 *
 * Retrieves the interrupt enable status that has been previously
 * saved on the stack by NutEnterCritical().
 */
#define NutExitCritical()   \
{                           \
    /* Set to 0 to force register allocation. */ \
    register uint8_t sreg = 0;   \
    asm("pop %sreg\n");     \
    outb(SREG, sreg);       \
}

#else

#define NutEnterCritical_nt()               \
    asm volatile(                           \
        "in  __tmp_reg__, __SREG__" "\n\t"  \
        "cli"                       "\n\t"  \
        "push __tmp_reg__"          "\n\t"  \
    )

#define NutExitCritical_nt()                \
    asm volatile(                           \
        "pop __tmp_reg__"           "\n\t"  \
        "out __SREG__, __tmp_reg__" "\n\t"  \
    )

#ifdef NUTTRACER_CRITICAL
#define NutEnterCritical()                  \
    NutEnterCritical_nt();                  \
    TRACE_ADD_ITEM_PC(TRACE_TAG_CRITICAL_ENTER);

#define NutExitCritical()                   \
    TRACE_ADD_ITEM_PC(TRACE_TAG_CRITICAL_EXIT); \
    NutExitCritical_nt()
#else
#define NutEnterCritical()                  \
    NutEnterCritical_nt();

#define NutExitCritical()                   \
    NutExitCritical_nt()
#endif
#endif

#define NutJumpOutCritical() NutExitCritical()

