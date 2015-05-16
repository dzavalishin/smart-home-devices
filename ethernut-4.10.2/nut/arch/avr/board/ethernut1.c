/*
 * Copyright 2010 by egnite GmbH
 *
 * All rights reserved.
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
 * For additional information see http://www.ethernut.de/
 */

/*
 * \file arch/avr/board/ethernut1.c
 * \brief Ethernut 1 board initialization.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/memory.h>
#include <cfg/arch/avr.h>

#include <arch/avr.h>

#if defined(RTL_EESK_BIT) && defined(__GNUC__) && defined(NUTXMEM_SIZE)

/*
 * Macros used for RTL8019AS EEPROM Emulation.
 * See FakeNicEeprom().
 */
#ifndef RTL_BASE_ADDR
#define RTL_BASE_ADDR 0x8300
#endif
#define NIC_CR  _MMIO_BYTE(RTL_BASE_ADDR)
#define NIC_EE  _MMIO_BYTE(RTL_BASE_ADDR + 1)

#if (RTL_EEMU_AVRPORT == AVRPORTB)
#define RTL_EEMU_PORT   PORTB
#define RTL_EEMU_DDR    DDRB

#elif (RTL_EEMU_AVRPORT == AVRPORTD)
#define RTL_EEMU_PORT   PORTD
#define RTL_EEMU_DDR    DDRD

#elif (RTL_EEMU_AVRPORT == AVRPORTE)
#define RTL_EEMU_PORT   PORTE
#define RTL_EEMU_DDR    DDRE

#elif (RTL_EEMU_AVRPORT == AVRPORTF)
#define RTL_EEMU_PORT   PORTF
#define RTL_EEMU_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EEMU_PORT   PORTC
#define RTL_EEMU_DDR    DDRC

#endif /* RTL_EEMU_AVRPORT */

#if (RTL_EEDO_AVRPORT == AVRPORTB)
#define RTL_EEDO_PORT   PORTB
#define RTL_EEDO_DDR    DDRB

#elif (RTL_EEDO_AVRPORT == AVRPORTD)
#define RTL_EEDO_PORT   PORTD
#define RTL_EEDO_DDR    DDRD

#elif (RTL_EEDO_AVRPORT == AVRPORTE)
#define RTL_EEDO_PORT   PORTE
#define RTL_EEDO_DDR    DDRE

#elif (RTL_EEDO_AVRPORT == AVRPORTF)
#define RTL_EEDO_PORT   PORTF
#define RTL_EEDO_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EEDO_PORT   PORTC
#define RTL_EEDO_DDR    DDRC

#endif /* RTL_EEDO_AVRPORT */

#if (RTL_EESK_AVRPORT == AVRPORTB)
#define RTL_EESK_PIN    PINB
#define RTL_EESK_DDR    DDRB

#elif (RTL_EESK_AVRPORT == AVRPORTD)
#define RTL_EESK_PIN    PIND
#define RTL_EESK_DDR    DDRD

#elif (RTL_EESK_AVRPORT == AVRPORTE)
#define RTL_EESK_PIN    PINE
#define RTL_EESK_DDR    DDRE

#elif (RTL_EESK_AVRPORT == AVRPORTF)
#define RTL_EESK_PIN    PINF
#define RTL_EESK_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EESK_PIN    PINC
#define RTL_EESK_DDR    DDRC

#endif /* RTL_EESK_AVRPORT */


/*
 * Before using extended memory, we need to run the RTL8019AS EEPROM emulation.
 * Not doing this may put this controller in a bad state, where it interferes
 * the data/address bus.
 *
 * This function has to be called as early as possible but after the external
 * memory interface has been enabled.
 *
 * The following part is used by the GCC environment only. For ICCAVR it has
 * been included in the C startup file.
 */
static void FakeNicEeprom(void) __attribute__ ((naked, section(".init1"), used));
void FakeNicEeprom(void)
{
    /*
     * Prepare the EEPROM emulation port bits. Configure the EEDO
     * and the EEMU lines as outputs and set both lines to high.
     */
#ifdef RTL_EEMU_BIT
    sbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    sbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    sbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    sbi(RTL_EEDO_DDR, RTL_EEDO_BIT);

    /* Force the chip to re-read the EEPROM contents. */
    NIC_CR = 0xE1;
    NIC_EE = 0x40;

    /* No external memory access beyond this point. */
#ifdef RTL_EE_MEMBUS
    cbi(MCUCR, SRE);
#endif

    /*
     * Loop until the chip stops toggling our EESK input. We do it in
     * assembly language to make sure, that no external RAM is used
     * for the counter variable.
     */
    __asm__ __volatile__("\n"   /* */
                         "EmuLoop:               " "\n" /* */
                         "        ldi  r24, 0    " "\n" /* Clear counter. */
                         "        ldi  r25, 0    " "\n" /* */
                         "        sbis %0, %1    " "\n" /* Check if EESK set. */
                         "        rjmp EmuClkClr " "\n" /* */
                         "EmuClkSet:             " "\n" /* */
                         "        adiw r24, 1    " "\n" /* Count loops with EESK set. */
                         "        breq EmuDone   " "\n" /* Exit loop on counter overflow. */
                         "        sbis %0, %1    " "\n" /* Test if EESK is still set. */
                         "        rjmp EmuLoop   " "\n" /* EESK changed, do another loop. */
                         "        rjmp EmuClkSet " "\n" /* Continue waiting. */
                         "EmuClkClr:             " "\n" /* */
                         "        adiw r24, 1    " "\n" /* Count loops with EESK clear. */
                         "        breq EmuDone   " "\n" /* Exit loop on counter overflow. */
                         "        sbic %0, %1    " "\n" /* Test if EESK is still clear. */
                         "        rjmp EmuLoop   " "\n" /* EESK changed, do another loop. */
                         "        rjmp EmuClkClr " "\n" /* Continue waiting. */
                         "EmuDone:               \n\t"  /* */
                         :      /* No outputs. */
                         :"I"(_SFR_IO_ADDR(RTL_EESK_PIN)), /* Emulation port. */
                          "I"(RTL_EESK_BIT)                 /* EESK bit. */
                         :"r24", "r25");

    /* Enable memory interface. */
#ifdef RTL_EE_MEMBUS
    sbi(MCUCR, SRE);
#endif

    /* Reset port outputs to default. */
#ifdef RTL_EEMU_BIT
    cbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    cbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    cbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
}

#endif /* RTL_EESK_BIT && __GNUC__ && NUTXMEM_SIZE */

/*!
 * \brief Early Ethernut 1 hardware initialization.
 */
void NutBoardInit(void)
{
}

