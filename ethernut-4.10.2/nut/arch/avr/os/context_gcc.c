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
 * Revision 1.13  2009/02/06 15:41:00  haraldkipp
 * Added stack checking code for AVR.
 *
 * Revision 1.12  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.11  2008/08/11 06:59:39  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2008/06/15 16:58:39  haraldkipp
 * Rolled back to version 1.8.
 *
 * Revision 1.8  2007/05/02 11:25:08  haraldkipp
 * Minor typo with big impact. Extended PC never set in context switch
 * frame.
 * Replaced __AVR_ATmega2561__ by __AVR_3_BYTE_PC__.
 * Removed unused local variable paddr.
 *
 * Revision 1.7  2007/04/12 09:19:00  haraldkipp
 * Added extended program counter byte for the ATmega2561. Code above 128k not
 * working, though.
 *
 * Revision 1.6  2006/09/29 12:27:31  haraldkipp
 * All code should use dedicated stack allocation routines. For targets
 * allocating stack from the normal heap the API calls are remapped by
 * preprocessor macros.
 *
 * Revision 1.5  2006/03/16 15:25:09  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.4  2005/10/04 05:17:15  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.3  2005/08/18 15:36:36  christianwelzel
 * Fixed bug in handling of NUTDEBUG.
 *
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:10:48  haraldkipp
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

/* Support for separate irq stack only for avr-gcc */
#include <dev/irqstack.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#endif

/*!
 * \addtogroup xgNutArchAvrOsContextGcc
 */
/*@{*/

#ifdef IRQSTACK_SIZE
/*!  * \brief Decrement value for thread's stack size, if separate irq stack is used.
 *
 * If separate irq stack is enabled (avr-gcc only), the initial parameter
 * 'stacksize' in function 'NutThreadCreate' is decremented by this value, if the
 * remaining stack size is 128 bytes or greater.
 *
 */
uint16_t _irqstackdec = 128;
#endif /* #ifdef IRQSTACK_SIZE */

/*!
 * \brief AVR GCC context switch frame layout.
 *
 * This is the layout of the stack after a thread's context has been
 * switched-out.
 */
typedef struct {
    uint8_t csf_r29;
    uint8_t csf_r28;
    uint8_t csf_r17;
    uint8_t csf_r16;
    uint8_t csf_r15;
    uint8_t csf_r14;
    uint8_t csf_r13;
    uint8_t csf_r12;
    uint8_t csf_r11;
    uint8_t csf_r10;
    uint8_t csf_r9;
    uint8_t csf_r8;
    uint8_t csf_r7;
    uint8_t csf_r6;
    uint8_t csf_r5;
    uint8_t csf_r4;
    uint8_t csf_r3;
    uint8_t csf_r2;
#ifdef __AVR_3_BYTE_PC__
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
    uint8_t cef_rampz;
    uint8_t cef_sreg;
    uint8_t cef_r1;
#ifdef __AVR_3_BYTE_PC__
    uint8_t cef_pcex;
#endif
    uint8_t cef_pchi;
    uint8_t cef_pclo;
} ENTERFRAME;

#define LONG_PTR_P(lp, mem_p) \
    __asm__ __volatile__("ldi %A0, lo8("#mem_p ")" "\n\t" \
                         "ldi %B0, hi8("#mem_p ")" "\n\t" \
                         "ldi %C0, hh8("#mem_p ")" "\n\t" \
                         "clr %D0" \
                         :"=d" (lp))

/*
 * This code is executed when entering a thread.
 */
static void NutThreadEntry(void) __attribute__ ((naked));
static void NutThreadEntry(void)
{
    __asm__ __volatile__("pop r25" "\n\t"       /* first parameter hi-byte */
                         "pop r24" "\n\t"       /* first parameter lo-byte */
                         "pop __tmp_reg__" "\n\t"       /* Get RAMPZ */
                         "out %0, __tmp_reg__" "\n\t"   /* Restore RAMPZ */
                         "pop __tmp_reg__" "\n\t"       /* Get SREG */
                         "out %1, __tmp_reg__" "\n\t"   /* Restore SREG */
                         "pop __zero_reg__" "\n\t"      /* Zero register */
                         "reti" "\n\t"  /* enables interrupts */
                         ::"I" _SFR_IO_ADDR(RAMPZ), "I" _SFR_IO_ADDR(SREG)
        );
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
void NutThreadSwitch(void) __attribute__ ((noinline)) __attribute__ ((naked));
void NutThreadSwitch(void)
{
    /*
     * Save all CPU registers.
     */
    asm volatile ("push r2" "\n\t"              /* */
              "push r3" "\n\t"              /* */
              "push r4" "\n\t"              /* */
              "push r5" "\n\t"              /* */
              "push r6" "\n\t"              /* */
              "push r7" "\n\t"              /* */
              "push r8" "\n\t"              /* */
              "push r9" "\n\t"              /* */
              "push r10" "\n\t"             /* */
              "push r11" "\n\t"             /* */
              "push r12" "\n\t"             /* */
              "push r13" "\n\t"             /* */
              "push r14" "\n\t"             /* */
              "push r15" "\n\t"             /* */
              "push r16" "\n\t"             /* */
              "push r17" "\n\t"             /* */
              "push r28" "\n\t"             /* */
              "push r29" "\n\t"             /* */
              "in %A0, %1" "\n\t"           /* */
              "in %B0, %2" "\n\t"           /* */
              :"=r" (runningThread->td_sp)  /* */
              :"I" _SFR_IO_ADDR(SPL),       /* */
               "I" _SFR_IO_ADDR(SPH)        /* */
    );

    /*
     * This defines a global label, which may be called
     * as an entry point into this function.
     */
    asm volatile (".global thread_start\n"  /* */
                  "thread_start:\n\t"::);

    /*
     * Reload CPU registers from the thread on top of the run queue.
     */
    runningThread = runQueue;
    runningThread->td_state = TDS_RUNNING;
    asm volatile ("out %1, %A0" "\n\t"          /* */
              "out %2, %B0" "\n\t"          /* */
              "pop r29" "\n\t"              /* */
              "pop r28" "\n\t"              /* */
              "pop r17" "\n\t"              /* */
              "pop r16" "\n\t"              /* */
              "pop r15" "\n\t"              /* */
              "pop r14" "\n\t"              /* */
              "pop r13" "\n\t"              /* */
              "pop r12" "\n\t"              /* */
              "pop r11" "\n\t"              /* */
              "pop r10" "\n\t"              /* */
              "pop r9" "\n\t"               /* */
              "pop r8" "\n\t"               /* */
              "pop r7" "\n\t"               /* */
              "pop r6" "\n\t"               /* */
              "pop r5" "\n\t"               /* */
              "pop r4" "\n\t"               /* */
              "pop r3" "\n\t"               /* */
              "pop r2" "\n\t"               /* */
              "ret" "\n\t"                  /* */
              ::"r" (runningThread->td_sp), /* */
                "I" _SFR_IO_ADDR(SPL),      /* */
                "I" _SFR_IO_ADDR(SPH)       /* */
    );
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
HANDLE NutThreadCreate(char * name, void (*fn) (void *), void *arg, size_t stackSize)
{
    uint8_t *threadMem;
    SWITCHFRAME *sf;
    ENTERFRAME *ef;
    NUTTHREADINFO *td;
#ifdef IRQSTACK_SIZE
    if (stackSize > _irqstackdec + 128) stackSize -= _irqstackdec;
#endif

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

    /* 
     * Set predefined values at the stack bottom. May be used to detect
     * stack overflows.
     */
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
#ifdef __AVR_3_BYTE_PC__
    ef->cef_pcex = 0;
#endif
    ef->cef_pchi = (uint8_t) (((uint16_t) fn) >> 8);
    ef->cef_pclo = (uint8_t) (((uint16_t) fn) & 0xff);
    ef->cef_sreg = 0x80;
    ef->cef_rampz = 0;
    ef->cef_r1 = 0;

    ef->cef_arglo = (uint8_t) (((uint16_t) arg) & 0xff);
    ef->cef_arghi = (uint8_t) (((uint16_t) arg) >> 8);

#ifdef __AVR_3_BYTE_PC__
    sf->csf_pcex = 0;
#endif
    sf->csf_pchi = (uint8_t) (((uint16_t) NutThreadEntry) >> 8);
    sf->csf_pclo = (uint8_t) (((uint16_t) NutThreadEntry) & 0xff);

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
        asm volatile ("rjmp thread_start\n\t"::);
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
