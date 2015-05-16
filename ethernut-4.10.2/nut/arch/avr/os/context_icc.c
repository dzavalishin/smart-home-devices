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
 * Revision 1.11  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.10  2008/08/11 11:51:19  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.9  2008/08/11 06:59:39  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2008/06/15 17:00:21  haraldkipp
 * Rolled back to version 1.6.
 *
 * Revision 1.6  2006/09/29 12:27:31  haraldkipp
 * All code should use dedicated stack allocation routines. For targets
 * allocating stack from the normal heap the API calls are remapped by
 * preprocessor macros.
 *
 * Revision 1.5  2006/07/10 08:46:52  haraldkipp
 * Properly set 3 byte return address for extended AVR.
 *
 * Revision 1.4  2006/02/08 15:20:21  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.3  2005/10/04 05:17:15  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:10:49  haraldkipp
 * Moved from os/thread.c
 *
 * Revision 1.2  2005/07/14 08:55:57  freckle
 * Rewrote CS in NutThreadCreate
 *
 * Revision 1.1  2005/05/27 17:17:31  drsung
 * Moved the file
 *
 * Revision 1.6  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.5  2005/02/16 19:55:18  haraldkipp
 * Ready-to-run queue handling removed from interrupt context.
 * Avoid AVRGCC prologue and epilogue code. Thanks to Pete Allinson.
 *
 * Revision 1.4  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.3  2004/09/22 08:15:56  haraldkipp
 * Speparate IRQ stack configurable
 *
 * Revision 1.2  2004/04/25 17:06:17  drsung
 * Separate IRQ stack now compatible with nested interrupts.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2004/02/18 16:32:48  drsung
 * Bugfix in NutThreadCreate. Thanks to Mike Cornelius.
 *
 * Revision 1.1  2004/02/01 18:49:48  haraldkipp
 * Added CPU family support
 *
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>

/*!
 * \addtogroup xgNutArchAvrOsContextIcc
 */
/*@{*/

/*!
 * \brief ICC AVR context switch frame layout.
 *
 * This is the layout of the stack after a thread's context has been
 * switched-out.
 */
typedef struct {
    uint8_t csf_r29;
    uint8_t csf_r28;
    uint8_t csf_r23;
    uint8_t csf_r22;
    uint8_t csf_r21;
    uint8_t csf_r20;
    uint8_t csf_r15;
    uint8_t csf_r14;
    uint8_t csf_r13;
    uint8_t csf_r12;
    uint8_t csf_r11;
    uint8_t csf_r10;
#ifdef __AVR_ATmega2561__
    uint8_t csf_pcex;
#endif
    uint8_t csf_pchi;
    uint8_t csf_pclo;
} SWITCHFRAME;

/*!
 * \brief Thread entry frame layout.
 *
 * This is the stack layout being build to enter a new thread.
 */
typedef struct {
    uint8_t cef_arghi;
    uint8_t cef_arglo;
    /*!
     * \brief ImageCraft software stack.
     */
    uint8_t cef_yhi;
    uint8_t cef_ylo;
    uint8_t cef_rampz;
    uint8_t cef_sreg;
    uint8_t cef_r1;
#ifdef __AVR_3_BYTE_PC__
    uint8_t cef_pcex;
#endif
    uint8_t cef_pchi;
    uint8_t cef_pclo;
} ENTERFRAME;


/*
 * This code is executed when entering a thread.
 */
static void NutThreadEntry(void)
{
    asm("pop r17");             // first parameter for ICC
    asm("pop r16");
    asm("pop r29");             // SW-Stack; Y-Register
    asm("pop r28");
    asm("pop r0");              // r0 = _tmp_reg_
    asm("out 0x3B, r0");        // RAMPZ; replace with define later
    asm("pop r0");
    asm("pop r1");              // r1 = _zero_reg_
    asm("out 0x3F, r0");        // SREG; replace with define later
    asm("reti");
}

/*!
 * \brief Switch to another thread.
 *
 * Stop the current thread, saving its context. Then start the
 * one with the highest priority, which is ready to run.
 *
 * Application programs typically do not call this function.
 *
 * \note CPU interrupts must be disabled before calling this function.
 *
 */
void NutThreadSwitch(void)
{
    /*
     * Save all CPU registers.
     */
    register uint8_t i = 0;
    register uint8_t j = 0;

    asm("push r10");
    asm("push r11");
    asm("push r12");
    asm("push r13");
    asm("push r14");
    asm("push r15");
    asm("push r20");
    asm("push r21");
    asm("push r22");
    asm("push r23");
    asm("push r28");
    asm("push r29");
    asm("in %i, $3D");          // SPL
    asm("in %j, $3E");          // SPH

    runningThread->td_sp = (((uint16_t) j) << 8) & 0xFF00 | (i & 0xFF);

    /*
     * This defines a global label, which may be called
     * as an entry point into this function.
     */
    asm(".globl thread_start");
    asm("thread_start:");

    /*
     * Reload CPU registers from the thread in front
     * of the run queue.
     */
    runningThread = runQueue;
    runningThread->td_state = TDS_RUNNING;

    i = (uint8_t) (runningThread->td_sp & 0xFF);
    j = (uint8_t) ((runningThread->td_sp >> 8) & 0xFF);

    asm("out $3D, %i");         // SPL
    asm("out $3E, %j");         // SPH
    asm("pop r29");
    asm("pop r28");
    asm("pop r23");
    asm("pop r22");
    asm("pop r21");
    asm("pop r20");
    asm("pop r15");
    asm("pop r14");
    asm("pop r13");
    asm("pop r12");
    asm("pop r11");
    asm("pop r10");
}

/*!
 * \brief Create a new thread.
 *
 * If the current thread's priority is lower or equal than the default
 * priority (64), then the current thread is stopped and the new one
 * is started.
 *
 * \param name      String containing the symbolic name of the new thread,
 *                  up to 8 characters long.
 * \param fn        The thread's entry point, typically created by the
 *                  THREAD macro.
 * \param arg       Argument pointer passed to the new thread.
 * \param stackSize Number of bytes of the stack space allocated for
 *                  the new thread.
 *
 * \return Pointer to the NUTTHREADINFO structure or 0 to indicate an
 *         error.
 */
HANDLE NutThreadCreate(uint8_t * name, void (*fn) (void *), void *arg, size_t stackSize)
{
    uint8_t *threadMem;
    SWITCHFRAME *sf;
    ENTERFRAME *ef;
    NUTTHREADINFO *td;
    uint16_t yreg;
    const uint8_t *paddr;

    /*
     * Allocate stack and thread info structure in one block.
     */
    if ((threadMem = NutStackAlloc(stackSize + sizeof(NUTTHREADINFO))) == 0) {
        return 0;
    }

    td = (NUTTHREADINFO *) (threadMem + stackSize);
    ef = (ENTERFRAME *) ((uint16_t) td - sizeof(ENTERFRAME));
    sf = (SWITCHFRAME *) ((uint16_t) ef - sizeof(SWITCHFRAME));


    memcpy(td->td_name, name, sizeof(td->td_name) - 1);
    td->td_name[sizeof(td->td_name) - 1] = 0;
    td->td_sp = (uint16_t) sf - 1;
    td->td_memory = threadMem;
#if defined(NUTDEBUG_CHECK_STACKMIN) || defined(NUTDEBUG_CHECK_STACK)
    {
        uint32_t *fip = (uint32_t *)threadMem;
        while (fip < (uint32_t *)sf) {
            *fip++ = DEADBEEF;
        }
    }
#else
    *((uint32_t *) threadMem) = DEADBEEF;
    *((uint32_t *) (threadMem + 4)) = DEADBEEF;
    *((uint32_t *) (threadMem + 8)) = DEADBEEF;
    *((uint32_t *) (threadMem + 12)) = DEADBEEF;
#endif
    td->td_priority = 64;

    /*
     * Setup entry frame to simulate C function entry.
     */
    paddr = (const uint8_t *) fn;
    ef->cef_pclo = *paddr;
    ef->cef_pchi = *(paddr + 1);
#ifdef __AVR_3_BYTE_PC__
    ef->cef_pcex = *(paddr + 2);
#endif
    ef->cef_sreg = 0x80;
    ef->cef_rampz = 0;
    ef->cef_r1 = 0;

    ef->cef_arglo = (uint8_t) (((uint16_t) arg) & 0xff);
    ef->cef_arghi = (uint8_t) (((uint16_t) arg) >> 8);

    yreg = td->td_sp - 40;
    ef->cef_yhi = (uint8_t) (yreg >> 8);
    ef->cef_ylo = (uint8_t) (yreg & 0xFF);

    paddr = (const uint8_t *) NutThreadEntry;
    sf->csf_pclo = *paddr;
    sf->csf_pchi = *(paddr + 1);
#ifdef __AVR_3_BYTE_PC__
    sf->csf_pcex = *(paddr + 2);
#endif

    /*
     * Insert into the thread list and the run queue.
     */

    td->td_next = nutThreadList;
    nutThreadList = td;
    td->td_state = TDS_READY;
    td->td_timer = 0;
    td->td_queue = 0;
#ifdef NUTDEBUG
    if (__os_trf)
        fprintf(__os_trs, "Cre<%04x>", (uintptr_t) td);
#endif

    NutThreadAddPriQueue(td, (NUTTHREADINFO **) & runQueue);

#ifdef NUTDEBUG
    if (__os_trf) {
        NutDumpThreadList(__os_trs);
        //NutDumpThreadQueue(__os_trs, runQueue);
    }
#endif

    /*
     * If no thread is active, switch to new thread.
     */
    if (runningThread == 0) {
        NutEnterCritical();
        asm("rjmp thread_start");
        /* we will never come back here .. */
    }

    /*
     * If current context is not in front of
     * the run queue (highest priority), then
     * switch to the thread in front.
     */
    if (runningThread != runQueue) {
        runningThread->td_state = TDS_READY;
#ifdef NUTDEBUG
        if (__os_trf)
            fprintf(__os_trs, "New<%04x %04x>", (uintptr_t) runningThread, (uintptr_t) runQueue);
#endif
        NutEnterCritical();
        NutThreadSwitch();
        NutExitCritical();
    }

    return td;
}

/*@}*/
