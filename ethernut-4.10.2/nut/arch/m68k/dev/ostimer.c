/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.1  2005/07/26 18:02:41  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/05/27 17:21:10  drsung
 * Moved the file.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/02/01 18:49:48  haraldkipp
 * Added CPU family support
 *
 */

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
    u_short delay_cnt = 2400;   //*KU* for 14.745600 MHz Clock
    u_short delay_cnt_buffer;

    while (ms--) {
        delay_cnt_buffer = delay_cnt;
        while (delay_cnt_buffer--);
    }
}

/*
 * Timer interrupt handler.
 */
static void NutTimerIntr(void *arg)
{
    NUTTIMERINFO *tnp;

    /*
     * Increment the tick counter used by Michael Fischer's
     * NutGetTickCount() routine.
     */
    milli_ticks++;

    millis++;
    if (ms1++ >= 999) {
        ms1 = 0;
        seconds++;
    }

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
}
