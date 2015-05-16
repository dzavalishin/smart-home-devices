/*
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.17  2009/02/06 15:37:40  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.16  2009/01/16 17:02:19  haraldkipp
 * No longer save any default OS configuration in non-volatile RAM.
 * All platforms will now call NutLoadConfig().
 *
 * Revision 1.15  2008/08/27 06:40:41  thornen
 * Added MMnet03..04 and MMnet102..104 CPLD initialization.
 *
 * Revision 1.14  2008/08/26 17:36:45  haraldkipp
 * Revoked changes 2008/08/26 by thornen.
 *
 * Revision 1.12  2008/08/11 11:51:19  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.11  2008/08/11 06:59:39  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2007/06/14 07:24:38  freckle
 * Disable ADC and buskeeper during idle thread sleep, if IDLE_THREAD_ADC_OFF and IDLE_THREAD_BUSKEEPER_OFF are defined
 *
 * Revision 1.9  2006/10/07 00:24:14  hwmaier
 * Include of sys/heap.h added.
 *
 * Revision 1.8  2006/09/29 12:39:23  haraldkipp
 * Added support for ATmega2561.
 *
 * Revision 1.7  2006/07/04 03:38:56  hwmaier
 * Changed binary constants to hex constants in XNUT board
 * initialization code.
 *
 * Revision 1.6  2006/05/15 11:46:49  haraldkipp
 * Added heartbeat port bit, which is regularly toggled by the idle thread.
 * Helps to develop on boards with external watchdog hardware that can't be
 * disabled.
 *
 * Revision 1.5  2005/10/17 08:24:55  hwmaier
 * All platform specific initialisation (CPLD, IO pins etc.) has been consolidated using the new PLATFORM macro into a new function called NutCustomInit()
 *
 * Revision 1.4  2005/10/04 06:11:11  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.3  2005/09/07 16:22:45  christianwelzel
 * Added MMnet02 CPLD initialization
 *
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/05/27 17:17:31  drsung
 * Moved the file
 *
 * Revision 1.18  2005/05/16 08:49:37  haraldkipp
 * Arthernet requires different wait state settings.
 *
 * Revision 1.17  2005/02/28 08:44:54  drsung
 * Fixed missing return type of NutInitSP
 * Removed inlcude file avrpio.h
 *
 * Revision 1.16  2005/02/26 12:09:28  drsung
 * Moved heap initialization to section .init5 to support c++ constructors for static objects.
 *
 * Revision 1.15  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.12  2005/01/22 19:30:56  haraldkipp
 * Fixes Ethernut 1.3G memory bug.
 *
 * Revision 1.11  2004/11/08 18:58:59  haraldkipp
 * Configurable stack sizes
 *
 * Revision 1.10  2004/09/01 14:27:03  haraldkipp
 * Using configuration values from cfg/memory.h.
 * Added configurable reserved memory area.
 * Automatic check for external memory removed.
 *
 * Revision 1.9  2004/07/28 13:43:25  drsung
 * Corrected a misplaced #endif after last change.
 *
 * Revision 1.8  2004/07/09 19:51:17  freckle
 * Added new function NutThreadSetSleepMode to tell nut/os to set the MCU
 * into sleep mode when idle (avr-gcc && avr128 only)
 *
 * Revision 1.7  2004/07/09 14:40:43  freckle
 * Moved ((volatile u_char *) NUTRAMEND) cast into NUTRAMENDPTR define
 *
 * Revision 1.6  2004/07/09 14:23:13  freckle
 * Allow setting of NUTRAMEND by giving it as a compiler flag
 *
 * Revision 1.5  2004/05/25 17:13:48  drsung
 * Bit name SRW10 is not defined for atmega103, so added some defines to make it compatible.. :-X
 *
 * Revision 1.4  2004/05/25 12:03:37  olereinhardt
 * Sorry, fixed typing bug
 *
 * Revision 1.3  2004/05/25 12:00:37  olereinhardt
 * Newly added 3Waitstate support now needs to be enabled by
 * defining NUT_3WAITSTATES. By default this behaves like normal
 *
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 *
 */

#include <sys/thread.h>
#include <sys/heap.h>
#include <cfg/memory.h>
#include <cfg/os.h>
#include <cfg/arch/avr.h>
#include <cfg/arch.h>

#include <dev/board.h>

/*!
 * \addtogroup xgNutArchAvrInit
 */
/*@{*/

#ifdef NUTXMEM_SIZE
/*!
 * \brief Last memory address using external SRAM.
 */
#define NUTMEM_END (uint16_t)(NUTXMEM_START + (uint16_t)NUTXMEM_SIZE - 1U)

#else
/*!
 * \brief Last memory address without using external SRAM.
 *
 * \todo Shall we support NUTRAMEND for backward compatibility? If, then
 *       let's do it in cfg/memory.h.
 */
#define NUTMEM_END (uint16_t)(NUTMEM_START + (uint16_t)NUTMEM_SIZE - 1U)

#endif

#ifndef NUT_THREAD_MAINSTACK
#define NUT_THREAD_MAINSTACK    1024
#endif

#ifndef NUT_THREAD_IDLESTACK
#if defined(__GNUC__)
/* avr-gcc optimized code used 36 bytes. */
#define NUT_THREAD_IDLESTACK    128
#else
/* icc-avr v7.19 used 132 bytes. */
#define NUT_THREAD_IDLESTACK    256
#endif
#endif

#ifdef NUTMEM_RESERVED
/*!
 * \brief Number of bytes reserved in on-chip memory.
 *
 * AVR offers the option to temporarily use address and data bus
 * lines as general purpose I/O. If such drivers need data memory,
 * this must be located at internal memory addresses.
 *
 * \todo Not a nice implementation but works as long as this module
 *       is linked first. Should be made a linker option.
 */
uint8_t nutmem_onchip[NUTMEM_RESERVED];
#endif

/* sleep mode to put avr in idle thread, SLEEP_MODE_NONE is used for for non sleeping */
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
uint8_t idle_sleep_mode = SLEEP_MODE_NONE;

/* AT90CAN128 uses a different register to enter sleep mode */
#if defined(SMCR)
#define AVR_SLEEP_CTRL_REG    SMCR
#else
#define AVR_SLEEP_CTRL_REG    MCUCR
#endif

#endif

#ifdef __GNUC__
/*
 * Some special declarations for AVRGCC. The runtime library
 * executes section .init8 before finally jumping to main().
 * We never let it do that jump, but start main() as a
 * separate thread. This introduces new problems:
 * 1. The compiler reinitializes the stack pointer when
 *    entering main, at least version 3.3 does it.
 * 2. The compiler doesn't allow to redeclare main to make
 *    it fit for NutThreadCreate().
 * 3. The runtime library requires, that main is defined
 *    somewhere.
 * Solutions:
 * 1. We do not use main at all, but let the preprocessor
 *    redefine it to NutAppMain() in the application code.
 *    See compiler.h. Note, that the declaration of NutAppMain
 *    in this module differs from the one in the application
 *    code. Fortunately the linker doesn't detect this hack.
 * 2. We use a linker option to set the symbol main to zero.
 *
 * Thanks to Joerg Wunsch, who helped to solve this.
 */
void NutInit(void) __attribute__ ((naked)) __attribute__ ((section(".init8")));
extern void NutAppMain(void *arg) __attribute__ ((noreturn));
#else
extern void main(void *);
#endif

/*
 * External memory interface for GCC.
 */
#if defined(__GNUC__) && defined(NUTXMEM_SIZE)

/*
 * At the very beginning enable extended memory interface.
 */
static void NutInitXRAM(void) __attribute__ ((naked, section(".init1"), used));
void NutInitXRAM(void)
{
#if defined(__AVR_AT90CAN128__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
/*
 * Note: Register structure of ATCAN128 differs from ATMEGA128 in regards
 * to wait states.
 */
#ifdef NUT_3WAITSTATES /* One wait state 1 for low, 3 for high memory range */
    XMCRA = _BV(SRE) | _BV(SRL2) | _BV(SRW00) | _BV(SRW10) | _BV(SRW11);
#else
    XMCRA = _BV(SRE) | _BV(SRW10); /* One wait state for the whole memory range */
#endif

#elif defined(__AVR_ATmega128__)

    MCUCR = _BV(SRE) | _BV(SRW10);

/* Configure two sectors, lower sector = 0x1100 - 0x7FFF,
 * Upper sector = 0x8000 - 0xFFFF and run 3 wait states for the
 * upper sector (NIC), 1 wait state for lower sector (XRAM).
 */
#ifdef NUT_3WAITSTATES
    XMCRA |= _BV(SRL2) | _BV(SRW00) | _BV(SRW11); /* SRW10 is set in MCUCR */
    XMCRB = 0;
#endif

#else  /* ATmega103 */
    MCUCR = _BV(SRE) | _BV(SRW);
#endif
}

#endif

/*! \fn NutThreadSetSleepMode(uint8_t mode)
 * \brief Sets the sleep mode to enter in Idle thread.
 *
 * If the idle thread is running, no other thread is active
 * so we can safely put the mcu to sleep.
 *
 * \param mode one of the sleep modes defined in avr/sleep.h or
 *             sleep_mode_none (don't enter sleep mode)
 *
 * \return previous sleep mode
 */
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
uint8_t NutThreadSetSleepMode(uint8_t mode)
{
    uint8_t old_mode = idle_sleep_mode;
    idle_sleep_mode = mode;
    return old_mode;
}
#endif

/*!
 * \brief AVR Idle thread.
 *
 * Running at priority 254 in an endless loop.
 */
THREAD(NutIdle, arg)
{
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
    uint8_t sleep_mode;
#endif
#ifdef IDLE_HEARTBEAT_BIT
    uint8_t beat = 0;
#endif

#ifdef NUT_INIT_IDLE
    NutIdleInit();
#endif

    /* Initialize system timers. */
    NutTimerInit();

#ifdef NUT_INIT_MAIN
    NutMainInit();
#endif

    /* Create the main application thread. */
    NutThreadCreate("main", main, 0,
        (NUT_THREAD_MAINSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);

    /*
     * Run in an idle loop at the lowest priority. We can still
     * do something useful here, like killing terminated threads
     * or putting the CPU into sleep mode.
     */
    NutThreadSetPriority(254);
    for (;;) {
        NutThreadYield();
        NutThreadDestroy();

#ifdef IDLE_HEARTBEAT_BIT
        if ((beat = !beat) == 0) {
            //UDR = '*';
            cbi(IDLE_HEARTBEAT_PORT, IDLE_HEARTBEAT_BIT);
        }
        else {
            sbi(IDLE_HEARTBEAT_PORT, IDLE_HEARTBEAT_BIT);
        }
        sbi(IDLE_HEARTBEAT_DDR, IDLE_HEARTBEAT_BIT);
#endif

#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
        if (idle_sleep_mode != SLEEP_MODE_NONE) {
            sleep_mode = AVR_SLEEP_CTRL_REG & _SLEEP_MODE_MASK;
            set_sleep_mode(idle_sleep_mode);
#ifdef IDLE_THREAD_ADC_OFF
            uint8_t adc = bit_is_set(ADCSR, ADEN);
            cbi(ADCSR, ADEN); // disable ADC
#endif
#ifdef IDLE_THREAD_BUSKEEPER_OFF
            uint8_t bitkeeper = bit_is_set(XMCRB, XMBK);
            cbi(XMCRB, XMBK); // disable buskeeper
#endif
            /* Note:  avr-libc has a sleep_mode() function, but it's broken for
            AT90CAN128 with avr-libc version earlier than 1.2 */
            AVR_SLEEP_CTRL_REG |= _BV(SE);
            __asm__ __volatile__ ("sleep" "\n\t" :: );
            AVR_SLEEP_CTRL_REG &= ~_BV(SE);
#ifdef IDLE_THREAD_ADC_OFF
            if (bitkeeper) {
                sbi(XMCRB, XMBK); // re-enable buskeeper
            }
#endif
#ifdef IDLE_THREAD_BUSKEEPER_OFF
            if (adc) {
                sbi(ADCSR, ADEN); // re-enable ADC
            }
#endif
            set_sleep_mode(sleep_mode);
        }
#endif
    }
}

#if defined(__GNUC__)
static void NutInitSP(void) __attribute__ ((naked, section (".init5"), used));
void NutInitSP(void)
{
#if defined (NUTMEM_STACKHEAP)
    /* Stack must remain in internal RAM where avr-libc's runtime lib init placed it */
#else
   /* Initialize stack pointer to end of external RAM while starting up the system
    * to avoid overwriting .data and .bss section.
    */
    SP = (uint16_t)(NUTMEM_END);
#endif
}
#endif

#if defined(__GNUC__)
static void NutInitHeap(void) __attribute__ ((naked, section (".init5"), used));
#endif
void NutInitHeap()
{
#if defined (NUTMEM_STACKHEAP) /* Stack resides in internal memory */
    NutStackAdd((void *) NUTMEM_START, NUTMEM_STACKHEAP);
#endif
    /* Then add the remaining RAM to heap.
     *
     * 20.Aug.2004 haraldkipp: This had been messed up somehow. It's nice to have
     * one continuous heap area, but we lost the ability to have systems with
     * a gap between internal and external RAM.
     */
    if ((uint16_t)NUTMEM_END - (uint16_t) (&__heap_start) > 384) {
        NutHeapAdd(&__heap_start, (uint16_t) NUTMEM_END - 256 - (uint16_t) (&__heap_start));
    }
}

#if defined(__GNUC__)
static void NutCustomInit(void) __attribute__ ((naked, section (".init1"), used));
#endif
/*!
 * NutCustomInit is a container function for hardware specific init code.
 *
 * The hardware is selected with a PLATFORM macro definition.
 *
 * Typically this function configures CPLDs, enables chips,
 * overwrites NutInitXRAM's default wait state settings, sets the default
 * baudrate for NUDEBUG as they depend on the crystal frequency used, etc.
 */
void NutCustomInit(void)
/*
* MMnet02..04 and MMnet102..104 CPLD initialization.
*/
#if defined(MMNET02)  || defined(MMNET03)  || defined(MMNET04) ||\
	defined(MMNET102) || defined(MMNET103) || defined(MMNET104)
{
    volatile uint8_t *breg = (uint8_t *)((size_t)-1 & ~0xFF);

    *(breg + 1) = 0x01; // Memory Mode 1, Banked Memory

    /* Assume 14.745600 MHz crystal, set to 115200bps */
    outb(UBRR, 7);
    outb(UBRR1L, 7);
}
/*
 * Arthernet CPLD initialization.
 */
#elif defined(ARTHERNET1)
{
    /* Arthernet1 memory setup - mt - TODO: check this
    Note: This overwrites the default settings of NutInitXRAM()!
    0x1100-0x14FF  CLPD area  -> use 3 Waitstates for 0x1100-0x1FFF (no Limit at 0x1500 available)
    0x1500-0xFFFF  Heap/Stack -> use 1 Waitstate  for 0x2000-0xFFFF
    */
    MCUCR  = _BV(SRE); /* enable xmem-Interface */
    XMCRA |= _BV(SRL0) | _BV(SRW01) | _BV(SRW00); /* sep. at 0x2000, 3WS for lower Sector */
    XMCRB = 0;

    *((volatile uint8_t *)(ARTHERCPLDSTART)) = 0x10; // arthernet cpld init - Bank
    *((volatile uint8_t *)(ARTHERCPLDSPI)) = 0xFF; // arthernet cpld init - SPI

    /* Assume standard Arthernet1 with 16 MHz crystal, set to 38400 bps */
    outb(UBRR, 25);
    outb(UBRR1L, 25);
}
/*
* XNUT board initialization
*/
#elif defined(XNUT_100) || defined(XNUT_105)
{
    PORTB = 0x35;
    DDRB  = 0x3F;
    PORTD = 0xE8;
    DDRD  = 0xB0;
    PORTE = 0x0E;
    DDRE  = 0x02;
    PORTF = 0xF0;
    DDRF  = 0x0F;
    PORTG = 0x1F;
    DDRG  = 0x07;

    ACSR |= _BV(ACD); /* Switch off analog comparator to reduce power consumption */

    /* Init I2C bus w/ 100 kHz */
    TWSR = 0;
    TWBR = (NUT_CPU_FREQ / 100000UL - 16) / 2; /* 100 kHz I2C */

    /* Set default baudrate */
#if NUT_CPU_FREQ == 14745600
    UBRR0L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
#else
    sbi(UCSR0A, U2X0);
    sbi(UCSR1A, U2X1);
    UBRR0L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
#endif
}
/*
* HHOpen 63f board initialization
*/
#elif defined(HHOPEN_63F)
{
	PORTA = 0xF8; DDRA  = 0x08;
	PORTB = 0x01; DDRB  = 0xE7;
	PORTC = 0xFF; DDRC  = 0x01;
    PORTD = 0x18; DDRD  = 0xDB;
	PORTE = 0x5A; DDRE  = 0xEA;
    PORTF = 0x80; DDRF  = 0xFF;
    PORTG = 0x00; DDRG  = 0xFF;

    ACSR |= _BV(ACD); /* Switch off analog comparator to reduce power consumption */

    /* Init I2C bus w/ 100 kHz */
    TWSR = 0;
    TWBR = (NUT_CPU_FREQ / 100000UL - 16) / 2; /* 100 kHz I2C */

    /* Set default baudrate */
#if NUT_CPU_FREQ == 14745600
    UBRR0L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
#else
    sbi(UCSR0A, U2X0);
    sbi(UCSR1A, U2X1);
    UBRR0L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
#endif
}

/*
 * Rest of the world and standard ETHERNUT 1/2
 */
#else
{
    /* Assume standard Ethernut with 14.745600 MHz crystal, set to 115200bps */
    outb(UBRR, 7);
#ifdef __AVR_ENHANCED__
    outb(UBRR1L, 7);
#endif
}
#endif

/*!
 * \brief Nut/OS Initialization.
 *
 * Initializes the memory management and the thread system and starts
 * an idle thread, which in turn initializes the timer management.
 * Finally the application's main() function is called.
 *
 * Depending on the compiler, different methods are used to execute this
 * function before main() is called.
 *
 * For ICCAVR the default crtatmega.o startup file is replaced by
 * crtnut.o, which calls NutInit instead of main(). This is done
 * by adding the following compiler options in the project:
 * \code -ucrtnut.o nutinit.o \endcode
 *
 * crtnut.o should be replaced by crtnutram.o, if the application's
 * variable space exceeds 4kB. For boards with RTL8019AS and EEPROM
 * emulation (like Ethernut 1.3 Rev-G) use crtenut.o or crtenutram.o.
 *
 * For AVRGCC this function is located in section .init8, which is
 * called immediately before jumping to main(). NutInit is defined
 * as:
 * \code
 * void NutInit(void) __attribute__ ((naked)) __attribute__ ((section (".init8")));
 * \endcode
 *
 * \todo Make heap threshold configurable, currently hardcoded at 384.
 *
 * \todo Make wait states for external memory access configuratble.
 *
 * \todo Make early UART initialization for kernel debugging configurable.
 */
void NutInit(void)
{
    /*
     * We can't use local variables in naked functions.
     */

#ifdef NUTDEBUG
    /* Note: The platform's default baudrate will be set in NutCustomInit() */
    outb(UCR, BV(RXEN) | BV(TXEN));
#endif

#ifdef NUT_INIT_BOARD
    NutBoardInit();
#endif

#ifndef __GNUC__
    NutCustomInit();

    /* Initialize stack pointer to end of external RAM while starting up the system
     * to avoid overwriting .data and .bss section.
     */
    SP = (uint16_t)(NUTMEM_END);

    /* Initialize the heap memory
     */
    NutInitHeap();
#endif /* __GNUC__ */

    /* Read OS configuration from non-volatile memory. */
    NutLoadConfig();

    /* Create idle thread
     */
    NutThreadCreate("idle", NutIdle, 0,
        (NUT_THREAD_IDLESTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);
}

/*@}*/
