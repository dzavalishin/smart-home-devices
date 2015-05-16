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
 * Revision 1.8  2008/08/22 09:25:34  haraldkipp
 * Clock value caching and new functions NutArchClockGet, NutClockGet and
 * NutClockSet added.
 *
 * Revision 1.7  2008/08/11 11:51:19  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.6  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2008/07/08 08:25:04  haraldkipp
 * NutDelay is no more architecture specific.
 * Number of loops per millisecond is configurable or will be automatically
 * determined.
 * A new function NutMicroDelay provides shorter delays.
 *
 * Revision 1.4  2006/12/20 15:14:41  freckle
 * corrected millisecond to nut ticks . Same bug as fixed at 2006-09-05
 *
 * Revision 1.3  2006/10/05 17:13:12  haraldkipp
 * Fixes bug #1567730. The problem had been reported by several people.
 * Lars Andersson provided the most complete solution, IMHO.
 *
 * Revision 1.2  2006/02/08 15:18:49  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.2  2005/06/12 16:50:57  haraldkipp
 * Major redesign to provide better portability and reduce interrupt latency.
 *
 * Revision 1.1  2005/05/27 17:17:31  drsung
 * Moved the file
 *
 * Revision 1.7  2005/05/16 08:54:45  haraldkipp
 * Original routines did not work for Arthernet.
 *
 * Revision 1.6  2005/03/09 08:33:34  hwmaier
 * Finally implemented the correct timer routines and init for AT90CAN128. Timer2 is now used on AT90CAN128 rather Timer0 because Atmel (don't blame me) swapped the Timer designation.
 *
 * Revision 1.5  2005/02/21 12:38:00  phblum
 * Removed tabs and added semicolons after NUTTRACER macros
 *
 * Revision 1.4  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.3  2005/01/24 22:34:50  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.2  2004/12/16 08:40:35  haraldkipp
 * Late increment fixes ICCAVR bug.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.3  2004/03/05 20:38:18  drsung
 * Bugfix from bugfix. sorry!
 *
 * Revision 1.2  2004/03/05 20:19:45  drsung
 * Bugfix in NutTimerInit. ICCAVR failed to compile, if NUT_CPU_FREQ is defined.
 *
 * Revision 1.1  2004/02/01 18:49:48  haraldkipp
 * Added CPU family support
 *
 */

#include <cfg/os.h>
#include <sys/atom.h>
#include <dev/irqreg.h>
#include <arch/timer.h>

/*!
 * \brief Nominal number of system ticks per second.
 *
 * The actual frequency depends on the timer crystal.
 *
 * \note Since version 3.9.8, the default frequency had been changed 
 *       from 16 Hz to 1024 Hz, when the timer is running with an 
 *       external 32 kHz clock crystal.
 */
#ifndef NUT_TICK_NFREQ
#ifdef NUT_CPU_FREQ
#define NUT_TICK_NFREQ  1000L
#else
#define NUT_TICK_NFREQ  1024L
#endif
#endif

#ifdef NUT_CPU_FREQ             /* ----- NUT_CPU_FREQ */
#ifndef NUT_TIMER_CRYSTAL
#define NUT_TIMER_CRYSTAL   NUT_CPU_FREQ
#endif
#ifndef NUT_TIMER_PRESCALE
#define NUT_TIMER_PRESCALE  128
#endif
#else                           /* ----- !NUT_CPU_FREQ */
#ifndef NUT_TIMER_CRYSTAL
#define NUT_TIMER_CRYSTAL   32768L
#endif
#ifndef NUT_TIMER_PRESCALE
#define NUT_TIMER_PRESCALE  1
#endif
#endif                          /* ----- NUT_CPU_FREQ */

/* Output compare register value. */
#define OCR_VALUE       (((2 * NUT_TIMER_CRYSTAL / (NUT_TIMER_PRESCALE * NUT_TICK_NFREQ) + 1) / 2) - 1)

/* Calculated number of timer ticks per second. */
#define NUT_TICK_FREQ   ((2 * NUT_TIMER_CRYSTAL / (OCR_VALUE + 1) / NUT_TIMER_PRESCALE + 1) / 2)


#ifdef NUT_CPU_FREQ             /* ----- NUT_CPU_FREQ */
#if defined(MCU_AT90CAN128)
#define TCCR_FLAGS  (_BV(CS20) | _BV(CS22) | _BV(WGM21))
#elif defined(MCU_ATMEGA2560)
#define TCCR_FLAGS  (_BV(WGM21))
#define TCCR2B_FLAGS  (_BV(CS20) | _BV(CS22))
#elif defined(MCU_ATMEGA2561)
#define TCCR_FLAGS  (_BV(WGM21))
#define TCCR2B_FLAGS  (_BV(CS20) | _BV(CS22))
#elif defined(MCU_ATMEGA103)    /* MCU_ATMEGA103 */
#define TCCR_FLAGS  (_BV(CS00) | _BV(CS02) | _BV(CTC0))
#else                           /* MCU_ATMEGA128 */
#define TCCR_FLAGS  (_BV(CS00) | _BV(CS02) | _BV(WGM01))
#endif
#else                           /* ----- !NUT_CPU_FREQ */
#if defined(MCU_ATMEGA103)      /* MCU_ATMEGA103 */
#define TCCR_FLAGS  (_BV(CS00) | _BV(CTC0))
#define TCCR_AFLAGS _BV(CS01)
#define ASSR_BIT    AS0
#define ASSR_BUSY   (_BV(TCN0UB) | _BV(OCR0UB) | _BV(TCR0UB))
#elif defined(MCU_ATMEGA128)    /* MCU_ATMEGA128 */
#define TCCR_FLAGS  (_BV(CS00) | _BV(WGM01))
#define TCCR_AFLAGS _BV(CS01)
#define ASSR_BIT    AS0
#define ASSR_BUSY   (_BV(TCN0UB) | _BV(OCR0UB) | _BV(TCR0UB))
#else                           /* Other MCU */
#error Define NUT_CPU_FREQ to compile for this CPU.
#endif
#endif                          /* ----- NUT_CPU_FREQ */

#if defined(MCU_AT90CAN128) || defined(MCU_ATMEGA2560) || defined(MCU_ATMEGA2561)
#define TCCRx       TCCR2A
#define TCNTx       TCNT2
#define OCRx        OCR2A
#define TIFR_OCFx   _BV(OCF2A)
#define TIFR_TOVx  _BV(TOV2)
#define sig_TIMER   sig_OUTPUT_COMPARE2
#else
#define TCCRx       TCCR0
#define TCNTx       TCNT0
#define OCRx        OCR0
#define TIFR_OCFx   _BV(OCF0)
#define TIFR_TOVx  _BV(TOV0)
#define sig_TIMER   sig_OUTPUT_COMPARE0
#endif

static uint32_t cpu_clock;

/*! \fn CountCpuLoops(void)
 * \brief Count the number of code loops until timer overflows.
 *
 * \return The number of loops.
 */
#ifndef NUT_CPU_FREQ
static uint32_t CountCpuLoops(void)
{
#ifdef __GNUC__
    uint32_t rc = 1;

    __asm__ __volatile__("firstovf:              \n\t"  /* */
                         "in %D0,%1              \n\t"  /* */
                         "andi %D0,%2            \n\t"  /* */
                         "breq firstovf          \n\t"  /* */
                         "out %1,%D0             \n\n"  /* */
                         /* This loop has 8 cycles. */
                         "nextovf:               \n\t"  /* */
                         "sec                    \n\t"  /* */
                         "adc %A0,__zero_reg__   \n\t"  /* */
                         "adc %B0,__zero_reg__   \n\t"  /* */
                         "adc %C0,__zero_reg__   \n\t"  /* */
                         "in %D0,%1              \n\t"  /* */
                         "andi %D0,%2            \n\t"  /* */
                         "breq nextovf           \n\t"  /* */
                         "clr %D0                    "  /* */
                         :"=d"(rc)      /* Output %0 */
                         :"I"(_SFR_IO_ADDR(TIFR))       /* Input %1 */
                         , "I"(TIFR_TOVx)      /* Input %2 */
                         , "0"(rc)      /* Input %0 */
        );
    return rc;
#elif defined(__IMAGECRAFT__)
    uint32_t rc;

    asm("CLR  R0");
    asm("CLR  R16");
    asm("CLR  R17");
    asm("CLR  R18");
    asm("firstovf:");
    asm("IN   R19, 0x36");
    asm("ANDI R19,1");
    asm("BREQ firstovf");
    asm("OUT  0x36, R19");
    /* This loop has 8 cycles. */
    asm("nextovf:");
    asm("SEC");
    asm("ADC  R16, R0");
    asm("ADC  R17, R0");
    asm("ADC  R18, R0");
    asm("IN   R19, 0x36");
    asm("ANDI R19, 1");
    asm("BREQ nextovf");
    asm("CLR  R19");
    asm("STD  Y+0, R16");
    asm("STD  Y+1, R17");
    asm("STD  Y+2, R18");
    asm("STD  Y+3, R19");

    return rc;
#endif
}
#endif

/*!
 * \brief Compute CPU clock in Hertz.
 *
 * This function determines the CPU clock by running
 * a counter loop between two timer interrupts.
 *
 * \return CPU clock in Hertz.
 *
 */
#ifndef NUT_CPU_FREQ
static uint32_t NutComputeCpuClock(void)
{
    uint32_t rc;

    /* Disable timer interrupts. */
    NutDisableTimerIrq();

    /* Select asynchronous oscillator. */
    sbi(ASSR, ASSR_BIT);

    /* Reset counter register. */
    outb(TCNTx, 0);

    /* Set prescaler to 8. Overflow will occur every 62.5 ms. */
    outb(TCCRx, TCCR_AFLAGS);

    /* Wait for asynchronous busy clear. */
    while ((inb(ASSR) & ASSR_BUSY) != 0);

    /* Clear interrupt flag. */
    outb(TIFR, TIFR_TOVx);

    NutEnterCritical();
    rc = CountCpuLoops();
    NutExitCritical();

    return rc << 7;
}
#endif

/*!
 * \brief Initialize system timer hardware.
 *
 * This function is automatically called by Nut/OS during system 
 * initialization.
 *
 * \param handler System timer interrupt handler.
 */
void NutRegisterTimer(void (*handler) (void *))
{
#ifdef NUT_CPU_FREQ
    cpu_clock = NUT_CPU_FREQ;
#else
    cpu_clock = NutComputeCpuClock();
#endif

    /* Disable timer interrupts. */
    NutDisableTimerIrq();

#ifndef NUT_CPU_FREQ
    /* Select asynchronous oscillator. */
    sbi(ASSR, ASSR_BIT);
#endif

    /* Reset counter register. */
    outb(TCNTx, 0);

    /* Set CTC mode and prescaler. */
    outb(TCCRx, TCCR_FLAGS);
#ifdef TCCR2B_FLAGS
    outb(TCCR2B, TCCR2B_FLAGS);
#endif

    /* Set output compare register. */
    outb(OCRx, OCR_VALUE);

#ifndef NUT_CPU_FREQ
    /* Wait for asynchronous busy clear. */
    while ((inb(ASSR) & ASSR_BUSY) != 0);
#endif

    /* Clear interrupt flags. */
    outb(TIFR, TIFR_OCFx);

    /* Enable timer compare interrupts. */
    NutRegisterIrqHandler(&sig_TIMER, handler, 0);
}

/*!
 * \brief Return the CPU clock in Hertz.
 *
 * \return CPU clock frequency in Hertz.
 */
uint32_t NutArchClockGet(int idx)
{
    return cpu_clock;
}

/*!
 * \brief Return the number of system ticks per second.
 *
 * \return System tick frequency in Hertz.
 */
uint32_t NutGetTickClock(void)
{
    return NUT_TICK_FREQ;
}

/*!
 * \brief Calculate system ticks for a given number of milliseconds.
 */
uint32_t NutTimerMillisToTicks(uint32_t ms)
{
#if (NUT_TICK_FREQ % 1000)
    if (ms >= 0x3E8000UL)
        return (ms / 1000UL) * NUT_TICK_FREQ;
    return (ms * NUT_TICK_FREQ + 999UL) / 1000UL;
#else
    return ms * (NUT_TICK_FREQ / 1000UL);
#endif
}

