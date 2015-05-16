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
 * Revision 1.7  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.6  2008/07/07 11:04:27  haraldkipp
 * Configurable ways of handling critical sections for ARM targets.
 *
 * Revision 1.5  2008/03/17 10:15:36  haraldkipp
 * Added memory and cc clobbers to NutEnter/ExitCritical. This keeps
 * compiler optimizations inside and outside of critical sections
 * seperated.
 *
 * Revision 1.4  2007/08/17 10:47:03  haraldkipp
 * Bug #1757410 fixed. NutEnter/ExitCritical destroyed ARM register R0.
 *
 * Revision 1.3  2006/03/02 20:03:39  haraldkipp
 * Added ICCARM inline assembly for NutEnter/ExitCritical(). Also fixed
 * SF 1440949 (FIQ never enabled).
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

#ifdef __GNUC__

#if defined(NUT_CRITICAL_NESTING)

#if defined(NUT_CRITICAL_NESTING_STACK)

#define NutEnterCritical() \
{ \
    int temp_; \
    asm volatile (             \
            "@ NutEnterCritical"    "\n\t" \
            "mrs     %0, cpsr"      "\n\t" \
            "stmfd   sp!, {%0}"     "\n\t" \
            "orr     %0, %0, #0xC0" "\n\t" \
            "msr     cpsr, %0"      "\n\t" \
            : "=r" (temp_) : : "memory", "cc"); \
}

#define NutExitCritical() \
{ \
    int temp_; \
    asm volatile (             \
            "@ NutExitCritical" "\n\t" \
            "ldmfd   sp!, {%0}" "\n\t" \
            "msr     cpsr, %0"  "\n\t" \
            : "=r" (temp_) : : "memory", "cc"); \
}

#else /* NUT_CRITICAL_NESTING_STACK */

extern unsigned int critical_nesting_level;

#define NutEnterCritical() \
if (critical_nesting_level++ == 0) { \
    int temp_; \
    asm volatile (             \
            "@ NutEnterCritical"    "\n\t" \
            "mrs     %0, cpsr"      "\n\t" \
            "orr     %0, %0, #0xC0" "\n\t" \
            "msr     cpsr, %0"      "\n\t" \
            : "=r" (temp_) : : "cc"); \
}

#define NutExitCritical() \
if (critical_nesting_level) { \
    int temp_; \
    asm volatile (             \
            "@ NutExitCritical"     "\n\t" \
            "mrs     %0, cpsr"      "\n\t" \
            "bic     %0, %0, #0xC0" "\n\t" \
            "msr     cpsr, %0"      "\n\t" \
            : "=r" (temp_) : : "cc"); \
    critical_nesting_level--; \
}

#endif /* NUT_CRITICAL_NESTING_STACK */

#else /* NUT_CRITICAL_NESTING */

#define NutEnterCritical() \
{ \
    int temp_; \
    asm volatile (             \
            "@ NutEnterCritical"    "\n\t" \
            "mrs     %0, cpsr"      "\n\t" \
            "orr     %0, %0, #0xC0" "\n\t" \
            "msr     cpsr, %0"      "\n\t" \
            : "=r" (temp_) : : "cc"); \
}

#define NutExitCritical() \
{ \
    int temp_; \
    asm volatile (             \
            "@ NutExitCritical"     "\n\t" \
            "mrs     %0, cpsr"      "\n\t" \
            "bic     %0, %0, #0xC0" "\n\t" \
            "msr     cpsr, %0"      "\n\t" \
            : "=r" (temp_) : : "cc"); \
}

#endif /* NUT_CRITICAL_NESTING */

#define NutJumpOutCritical()    NutExitCritical()

#else /* __IMAGECRAFT__ */

#define NutEnterCritical() \
        asm("; NutEnterCritical\n" \
            "mrs r12, cpsr\n" \
            "orr r12, r12, #0xC0\n" \
            "msr cpsr_c, r12")

#define NutExitCritical() \
        asm("; NutExitCritical\n" \
            "mrs r12, cpsr\n" \
            "bic r12, r12, #0xC0\n" \
            "msr cpsr_c, r12")

#endif
