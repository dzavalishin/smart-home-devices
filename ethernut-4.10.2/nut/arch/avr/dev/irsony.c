/*
 * Copyright (C) 2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2005/01/28 12:13:14  freckle
 * changed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.2  2004/03/18 18:41:18  haraldkipp
 * Bugfix. Now works with ICCAVR
 *
 * Revision 1.1  2003/07/21 18:07:54  haraldkipp
 * Sony infrared remote control driver added
 *
 */

#include <cfg/medianut.h>
#include <sys/atom.h>
#include <sys/event.h>

#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT4:iv_INT4
#pragma interrupt_handler SIG_OVERFLOW2:iv_TIMER2_OVF
#endif

/*!
 * \addtogroup xgIrSony
 */
/*@{*/

#include <dev/irsony.h>

#define IRTIMER_START   0xF0
#define IRTIMER_SCALE   0x01

/*!
 * \brief Last valid infrared code received.
 */
long nut_ircode;

/*!
 * \brief Queue of threads waiting for infrared input.
 */
HANDLE nut_irqueue;

/*
 * Tick counter, incremented on each timer overflow and
 * cleared on each edge detection on the infrared signal.
 */
static uint16_t irticks;

/*
 * Number of bits received from infrared decoder.
 */
static uint8_t irbitnum;

/*! \fn SIG_OVERFLOW2(void)
 * \brief Timer 2 overflow handler.
 */
SIGNAL(SIG_OVERFLOW2)
{
    /* Set the timer value. */
    outb(TCNT2, IRTIMER_START);

    /*
     * Increment our tick counter. If it overflows, then wait
     * for the next start bit and disable the timer.
     */
    if (++irticks == 0) {
        irbitnum = 0;
        cbi(TIMSK, 6);
    }
}

/*! \fn SIG_INTERRUPT4(void)
 * \brief Infrared decoder signal edge handler.
 */
SIGNAL(SIG_INTERRUPT4)
{
    static uint16_t minset;      /* Min. length of bit value 1, calculated from start bit. */
    static uint16_t ccode;       /* Current code. */
    static uint16_t lcode;       /* Last code. */
    static uint8_t ncode;        /* Number of equal codes. */
    uint16_t bitlen = irticks;   /* Length of the current bit. */

    irticks = 0;

    /* Rising egde marks end of bit. */
    if (inb(IR_SIGNAL_PIN) & _BV(IR_SIGNAL_BIT)) {
        /* Start bit. */
        if (irbitnum++ == 0) {
            ccode = 0;
            minset = bitlen / 3;
        }
        /* Data bits. */
        else {
            /* If its length is greater than a third of the start bit,
               then we assume this bit set. */
            if (bitlen >= minset)
                ccode |= _BV(12);
            ccode >>= 1;
            /* All data bits collected? */
            if (irbitnum > 12) {
                cbi(TIMSK, 6);
                irbitnum = 0;
                if (ncode++) {
                    if (lcode != ccode)
                        ncode = 0;
                    /* If we have two equal codes, pass it to the 
                       application. */
                    else if (ncode > 1) {
                        ncode = 0;
                        nut_ircode = ccode;
                        NutEventPostFromIrq(&nut_irqueue);
                    }
                } else
                    lcode = ccode;
            }
        }
    }
    /* Falling edges mark the start of a bit. Enable timer
       overflow interrupts. */
    else if (irbitnum == 0) {
        sbi(TIMSK, 6);
    }

}

/*!
 * \brief Enable Sony infrared remote control.
 */
int NutIrInitSony(void)
{
    NutEnterCritical();
    /* 
     * Initialize timer 2 and enable overflow interrupts.
     */
    outb(TCNT2, IRTIMER_START);
    outb(TCCR2, IRTIMER_SCALE);
    sbi(TIMSK, 6);

    /* 
     * Enable infrared decoder interrupts on both edges. 
     */
    cbi(IR_SIGNAL_DDR, IR_SIGNAL_BIT);
    sbi(EICR, 0);
    cbi(EICR, 1);
    sbi(EIMSK, IR_SIGNAL_BIT);

    NutExitCritical();

    return 0;
}

/*@}*/
