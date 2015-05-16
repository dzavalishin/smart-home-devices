/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/05/27 17:18:41  drsung
 * Moved the file.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <h83068f.h>

/*!
 * \brief Loop for a specified number of milliseconds.
 *
 * This call will not release the CPU and will
 * not switch to another thread. However, because
 * of absent thread switching, this delay time is
 * very exact.
 *
 * Use NutSleep() to avoid blocking the CPU, if no
 * exact timing is needed.
 *
 * \param ms Delay time in milliseconds, maximum is 255.
 */
void NutDelay(u_char ms)
{
    u_short delay_cnt = 1843;   //*KU* for 22.118400 MHz Clock
    u_short delay_cnt_buffer;

    while (ms--) {
        delay_cnt_buffer = delay_cnt;
        while (delay_cnt_buffer--);
    }
}

/*
 * Timer 0 interrupt handler.
 */
static void NutTimer0Intr(void *arg)
{
    NUTTIMERINFO *tnp;

    /* Clear timer 0, channnel A compare/match bit */
    ITU.TISRA.BIT.IMFA0 = 0;

    /*
     * Increment the tick counter used by Michael Fischer's
     * NutGetTickCount() routine.
     */
    milli_ticks++;

#ifdef NUT_CPU_FREQ

    millis++;
    if (ms1++ >= 999) {
        ms1 = 0;
        seconds++;
    }
#else                           /* NUT_CPU_FREQ */

    if (ms62_5 & 1) {
        millis += 62;
    } else {
        millis += 63;
    }
    /*
     * Update RTC. We do a post increment here, because
     * of an ImageCraft bug with volatiles. Thanks to
     * Michael Fischer.
     */
    if (ms62_5++ >= 15) {
        ms62_5 = 0;
        seconds++;
    }
#endif                          /* NUT_CPU_FREQ */

    if (nutTimerList) {
        if (nutTimerList->tn_ticks_left)
            nutTimerList->tn_ticks_left--;

        /*
         * Process all elapsed timers.
         * Bugfix by KU: Avoid crash on empty timer list.
         */
        while ((nutTimerList != 0) && (nutTimerList->tn_ticks_left == 0)) {
            /*
             * Execute the callback.
             */
            if (nutTimerList->tn_callback)
                (*nutTimerList->tn_callback) (nutTimerList, (void *) nutTimerList->tn_arg);

            /*
             * Remove the timer from the list.
             */
            tnp = nutTimerList;
            //fprintf(__os_trs, "RemTmr<%04X>\r\n", tnp);
            nutTimerList = nutTimerList->tn_next;

            /*
             * Periodic timers are re-inserted.
             */
            if ((tnp->tn_ticks_left = tnp->tn_ticks) != 0)
                NutTimerInsert(tnp);

            /*
             * We can't touch the heap while running in
             * interrupt context. Oneshot timers are added 
             * to a pool of available timers.
             */
            else {
                tnp->tn_next = nutTimerPool;
                nutTimerPool = tnp;
                //NutKDumpTimerList();
            }
        }
    }
}

#ifndef NUT_CPU_FREQ

/*!
 * \brief Compute CPU clock in Hertz.
 *
 * This function determines the CPU clock by running
 * a counter loop between two timer interrupts.
 *
 * \return CPU clock in Hertz.
 *
 */
static u_long NutComputeCpuClock(void)
{
    return NUT_CPU_FREQ;
}

#endif                          /* #ifndef NUT_CPU_FREQ */

/*!
 * \brief Initialize system timer.
 *
 * This function is automatically called by Nut/OS
 * during system initialization.
 *
 * Nut/OS uses on-chip timer 0 for its timer services.
 * Applications should not modify any registers of this
 * timer, but make use of the Nut/OS timer API. Timer 1
 * and timer 2 are available to applications.
 */
void NutTimerInit(void)
{
#ifdef NUT_CPU_FREQ
    /* 16 bit timer ch. 0 high priority */
    INTC.IPRA.BIT._ITU0 = 1;
    /* auto clear TCNT, clock is phi/4 */
    ITU0.TCR.BYTE = 0x22;
    /* no output on GRA */
    ITU0.TIOR.BYTE = 0x00;
    /* generate interrupt every 10ms */
    /*    ITU0.GRA = 0xd7ff; */
    /* generate interrupt every 1ms */
    ITU0.GRA = 0x159a;
    /* clear counter register */
    ITU0.TCNT = 0x0000;
    /* enable IMIA0 interrupt */
    ITU.TISRA.BIT.IMIEA0 = 1;

    NutRegisterIrqHandler(&sig_IMIA0, NutTimer0Intr, 0);

    /* start timer 0 */
    ITU.TSTR.BIT.STR0 = 1;

#else                           /* #ifdef NUT_CPU_FREQ */
/*        TODO !!! */
#error "NUT_CPU_FREQ must be defined"
#endif                          /* #ifdef NUT_CPU_FREQ */
}
