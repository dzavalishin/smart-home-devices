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
 * Revision 1.7  2009/02/06 15:37:39  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.6  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/06/27 01:42:56  hwmaier
 * Fixed bug related to edge triggered interrupt mode (RTL_IRQ_RISING_EDGE) in ISR.
 *
 * Revision 1.3  2006/03/02 23:57:12  hwmaier
 * Include cfg/dev.h added
 *
 * Revision 1.2  2006/01/11 08:33:30  hwmaier
 * Changes to make receiver thread's stack size configurable and honour
 * the NUT_THREAD_NICRXSTACK configuration setting
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.15  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.14  2005/02/10 07:06:18  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.13  2005/02/05 20:42:38  haraldkipp
 * Force compiler error for leftover debug prints.
 *
 * Revision 1.12  2005/01/24 21:11:50  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.11  2005/01/22 19:24:46  haraldkipp
 * Changed AVR port configuration names from PORTx to AVRPORTx.
 *
 * Revision 1.10  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.9  2004/12/17 15:31:28  haraldkipp
 * Support of rising edge interrupts for hardware w/o inverter gate.
 * Fixed compilation issue for hardware with RTL reset port.
 * Thanks to FOCUS Software Engineering Pty Ltd.
 *
 * Revision 1.8  2004/09/10 10:36:01  haraldkipp
 * ICCAVR compile problems fixed
 *
 * Revision 1.7  2004/08/25 10:41:00  haraldkipp
 * Hardware dependent definitions are configurable. For performance reasons the
 * base address is not kept in a variable any longer. It is now a preprocessor
 * macro and the parameters during device registration are ignored. The speed
 * improvements provided by Kolja Waschk for the LAN91C111 had been implemented
 * here too. The driver will not touch a port anymore unless a reset port bit
 * had been configured. For Ethernut 1.1 bit 4 PORTE must be specified in the
 * configuration. Finally, an EEPROM emulation had been added, which can use
 * address bus bits instead of wasting additional port pins. The required
 * hardware has been implemented on Rev.-G Ethernut 1.3 boards. This fixes the
 * Realtek full duplex problem.
 *
 * Revision 1.6  2004/05/17 19:14:53  haraldkipp
 * Added Bengt Florin's RTL8019 driver mods
 *
 * Revision 1.5  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.4  2003/08/05 20:05:59  haraldkipp
 * Bugfix. Empty MAC address is FF, not 00
 *
 * Revision 1.3  2003/07/17 09:39:56  haraldkipp
 * Optimized controller feeding.
 * Ignore LSB of packet status.
 *
 * Revision 1.2  2003/05/15 14:09:16  haraldkipp
 * Much better performance under heavy traffic.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:48  haraldkipp
 * Initial using 3.2.1
 *
 */

#include <cfg/os.h>
#include <cfg/arch/avr.h>
#include <cfg/dev.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <net/ether.h>
#include <net/if_var.h>

#include <dev/irqreg.h>
#include <dev/nicrtl.h>
#include "rtlregs.h"

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

#ifndef NUT_THREAD_NICRXSTACK
#define NUT_THREAD_NICRXSTACK   640
#endif

/*
 * Determine ports, which had not been explicitely configured.
 */
#if (RTL_EESK_AVRPORT == AVRPORTB)
#define RTL_EESK_PIN    PINB
#define RTL_EESK_DDR    DDRB

#elif (RTL_EESK_AVRPORT == AVRPORTC)
#define RTL_EE_MEMBUS
#define RTL_EESK_PIN    PINC
#define RTL_EESK_DDR    DDRC

#elif (RTL_EESK_AVRPORT == AVRPORTD)
#define RTL_EESK_PIN    PIND
#define RTL_EESK_DDR    DDRD

#elif (RTL_EESK_AVRPORT == AVRPORTE)
#define RTL_EESK_PIN    PINE
#define RTL_EESK_DDR    DDRE

#elif (RTL_EESK_AVRPORT == AVRPORTF)
#define RTL_EESK_PIN    PINF
#define RTL_EESK_DDR    DDRF

#endif /* RTL_EESK_AVRPORT */

#if (RTL_EEDO_AVRPORT == AVRPORTB)
#define RTL_EEDO_PORT   PORTB
#define RTL_EEDO_DDR    DDRB

#elif (RTL_EEDO_AVRPORT == AVRPORTC)
#define RTL_EE_MEMBUS
#define RTL_EEDO_PORT   PORTC
#define RTL_EEDO_DDR    DDRC

#elif (RTL_EEDO_AVRPORT == AVRPORTD)
#define RTL_EEDO_PORT   PORTD
#define RTL_EEDO_DDR    DDRD

#elif (RTL_EEDO_AVRPORT == AVRPORTE)
#define RTL_EEDO_PORT   PORTE
#define RTL_EEDO_DDR    DDRE

#elif (RTL_EEDO_AVRPORT == AVRPORTF)
#define RTL_EEDO_PORT   PORTF
#define RTL_EEDO_DDR    DDRF

#endif /* RTL_EEDO_AVRPORT */

#if (RTL_EEMU_AVRPORT == AVRPORTB)
#define RTL_EEMU_PORT   PORTB
#define RTL_EEMU_DDR    DDRB

#elif (RTL_EEMU_AVRPORT == AVRPORTC)
#define RTL_EE_MEMBUS
#define RTL_EEMU_PORT   PORTC
#define RTL_EEMU_DDR    DDRC

#elif (RTL_EEMU_AVRPORT == AVRPORTD)
#define RTL_EEMU_PORT   PORTD
#define RTL_EEMU_DDR    DDRD

#elif (RTL_EEMU_AVRPORT == AVRPORTE)
#define RTL_EEMU_PORT   PORTE
#define RTL_EEMU_DDR    DDRE

#elif (RTL_EEMU_AVRPORT == AVRPORTF)
#define RTL_EEMU_PORT   PORTF
#define RTL_EEMU_DDR    DDRF

#endif /* RTL_EEMU_AVRPORT */

#if (RTL_RESET_AVRPORT == AVRPORTB)
#define RTL_RESET_PORT   PORTB
#define RTL_RESET_DDR    DDRB

#elif (RTL_RESET_AVRPORT == AVRPORTD)
#define RTL_RESET_PORT   PORTD
#define RTL_RESET_DDR    DDRD

#elif (RTL_RESET_AVRPORT == AVRPORTE)
#define RTL_RESET_PORT   PORTE
#define RTL_RESET_DDR    DDRE

#elif (RTL_RESET_AVRPORT == AVRPORTF)
#define RTL_RESET_PORT   PORTF
#define RTL_RESET_DDR    DDRF

#endif /* RTL_RESET_AVRPORT */


/*!
 * \brief Interrupt used.
 */
#if (RTL_SIGNAL_IRQ == INT0)
#define RTL_SIGNAL sig_INTERRUPT0
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRA, ISC00); sbi(EICRA, ISC01)
#endif

#elif (RTL_SIGNAL_IRQ == INT1)
#define RTL_SIGNAL sig_INTERRUPT1
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRA, ISC10); sbi(EICRA, ISC11)
#endif

#elif (RTL_SIGNAL_IRQ == INT2)
#define RTL_SIGNAL sig_INTERRUPT2
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRA, ISC20); sbi(EICRA, ISC21)
#endif

#elif (RTL_SIGNAL_IRQ == INT3)
#define RTL_SIGNAL sig_INTERRUPT3
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRA, ISC30); sbi(EICRA, ISC31)
#endif

#elif (RTL_SIGNAL_IRQ == INT4)
#define RTL_SIGNAL sig_INTERRUPT4
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRB, ISC40); sbi(EICRB, ISC41)
#endif

#elif (RTL_SIGNAL_IRQ == INT6)
#define RTL_SIGNAL sig_INTERRUPT6
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRB, ISC60); sbi(EICRB, ISC61)
#endif

#elif (RTL_SIGNAL_IRQ == INT7)
#define RTL_SIGNAL sig_INTERRUPT7
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRB, ISC70); sbi(EICRB, ISC71)
#endif

#else
#define RTL_SIGNAL sig_INTERRUPT5
#ifdef __AVR_ENHANCED__
#define RTL_RISING_EDGE_MODE()   sbi(EICRB, ISC50); sbi(EICRB, ISC51)
#endif

#endif /* RTL_SIGNAL_IRQ */


/*!
 * \brief Size of a single ring buffer page.
 */
#define NIC_PAGE_SIZE   0x100

/*!
 * \brief First ring buffer page address.
 */
#define NIC_START_PAGE  0x40

/*!
 * \brief Last ring buffer page address plus 1.
 */
#define NIC_STOP_PAGE   0x60

/*!
 * \brief Number of pages in a single transmit buffer.
 *
 * This should be at least the MTU size.
 */
#define NIC_TX_PAGES    6

/*!
 * \brief Number of transmit buffers.
 *
 * The initial value had been 2. The idea was to use two alternating
 * buffers. However, this had never been implemented and we took over
 * Bengt Florin's change, defining 1 transmit buffer only and give
 * more buffer space to the receiver.
 *
 * The controller memory layout is now
 *
 * - 0x4000..0x45ff  1.5K bytes transmit buffer
 * - 0x4600..0x5fff  6.5K bytes receive buffer
 *
 * The routines are still not using the buffers in an optimal way,
 * as transmission is limited to 1 packet at a time. In fact several
 * smaller packets would fit into the 1.5 kByte buffer. On the other
 * hand, filling the buffer with more than one transmission packet
 * may result in other bad effects, like pulling the IORDY line more
 * often.
 */
#define NIC_TX_BUFFERS      1

#define NIC_FIRST_TX_PAGE   NIC_START_PAGE
#define NIC_FIRST_RX_PAGE   (NIC_FIRST_TX_PAGE + NIC_TX_PAGES * NIC_TX_BUFFERS)

#define NIC_CR_PAGE0 (0)
#define NIC_CR_PAGE1 (NIC_CR_PS0)
#define NIC_CR_PAGE2 (NIC_CR_PS1)
#define NIC_CR_PAGE3 (NIC_CR_PS1 | NIC_CR_PS0)

/*
 * This delay has been added by Bengt Florin and is used to minimize
 * the effect of the IORDY line during reads. Bengt contributed a
 * more versatile loop, which unfortunately wasn't portable to the
 * ImageCraft compiler.
 *
 * Both versions depend on the CPU clock and had been tested with
 * 14.7456 MHz.
 */
static INLINE void Delay16Cycles(void)
{
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
}

/*!
 * \addtogroup xgNicRtl
 */
/*@{*/

/*!
 * Realtek packet header.
 */
struct nic_pkt_header {
    uint8_t ph_status;           /*!< \brief Status, contents of RSR register */
    uint8_t ph_nextpg;           /*!< \brief Page for next packet */
    uint16_t ph_size;            /*!< \brief Size of header and packet in octets */
};

#define NICINB(reg)         (*((volatile uint8_t *)RTL_BASE_ADDR + reg))
#define NICOUTB(reg, val)   (*((volatile uint8_t *)RTL_BASE_ADDR + reg) = val)

/*!
 * \brief Reset the Ethernet controller.
 *
 */
static int NicReset(void)
{
    uint8_t i;
    uint8_t j;

/*
 * Toggle the hardware reset line. Since Ethernut version 1.3 the
 * hardware reset pin of the nic is no longer connected to bit 4
 * on port E, but wired to the board reset line.
 */
#ifdef RTL_RESET_BIT
    sbi(RTL_RESET_DDR, RTL_RESET_BIT);
    sbi(RTL_RESET_PORT, RTL_RESET_BIT);
    NutDelay(WAIT100);
    cbi(RTL_RESET_PORT, RTL_RESET_BIT);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
#endif

    /*
     * Do the software reset by reading from the reset register followed
     * by writing to the reset register. Wait until the controller enters
     * the reset state.
     */
    for (j = 0; j < 20; j++) {
        i = NICINB(NIC_RESET);
        NutDelay(WAIT5);
        NICOUTB(NIC_RESET, i);
        for (i = 0; i < 20; i++) {
            NutDelay(WAIT50);
            /*
             * We got the reset bit. However, Ethernut 1.1 may
             * still fail because the NIC hasn't got it's hardware
             * reset and the data lines remain in tristate. So we
             * read noise instead of the register. To solve this
             * problem, we will verify the NIC's id.
             */
            if ((NICINB(NIC_PG0_ISR) & NIC_ISR_RST) != 0 &&     /* */
                NICINB(NIC_PG0_RBCR0) == 0x50 &&        /* */
                NICINB(NIC_PG0_RBCR1) == 0x70)
                return 0;
        }
    }
    return -1;
}

static int DetectNicEeprom(void)
{
#ifdef RTL_EESK_BIT
    register unsigned int cnt = 0;

    NutEnterCritical();

    /*
     * Prepare the EEPROM emulation port bits. Configure the EEDO
     * and the EEMU lines as outputs and set both lines to high.
     */
    sbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    sbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
#ifdef RTL_EEMU_BIT
    sbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    sbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    NutDelay(20);

    /*
     * Force the chip to re-read the EEPROM contents.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1);
    NICOUTB(NIC_PG3_EECR, NIC_EECR_EEM0);

    /*
     * No external memory access beyond this point.
     */
#ifdef RTL_EE_MEMBUS
    /*
     * No external memory access beyond this point.
     */
#ifdef __AVR_ENHANCED__
    /* On the ATmega 128 we release bits 5-7 as normal port pins. */
    outb(XMCRB, inb(XMCRB) | _BV(XMM0) | _BV(XMM1));
#else
    /* On the ATmega 103 we have to disable the external memory interface. */
    cbi(MCUCR, SRE);
#endif
#endif

    /*
     * Check, if the chip toggles our EESK input. If not, we do not
     * have EEPROM emulation hardware.
     */
    if (bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT)) {
        while (++cnt && bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT));
    } else {
        while (++cnt && bit_is_clear(RTL_EESK_PIN, RTL_EESK_BIT));
    }

#ifdef RTL_EE_MEMBUS
    /*
     * Enable memory interface.
     */
#ifdef __AVR_ENHANCED__
    /* On the ATmega 128 we release bits 5-7 as normal port pins. */
    outb(XMCRB, inb(XMCRB) & ~(_BV(XMM0) | _BV(XMM1)));
#else
    /* On the ATmega 103 we have to disable the external memory interface. */
    sbi(MCUCR, SRE);
#endif
#endif

    /* Reset port outputs to default. */
    cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    cbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
#ifdef RTL_EEMU_BIT
    cbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    cbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif

    /* Restore previous interrupt enable state. */
    NutExitCritical();

    /* Wait until controller ready. */
    while (NICINB(NIC_CR) != (NIC_CR_STP | NIC_CR_RD2));

    return cnt ? 0 : -1;
#else
    return -1;
#endif
}

#ifdef RTL_EESK_BIT
/*
 * Emulated EEPROM contents.
 *
 * In jumper mode our influence is quite limited, only CONFIG3 and CONFIG4
 * can be modified.
 */
static prog_char nic_eeprom[18] = {
    0xFF,                       /* CONFIG2: jPL1 jPL0   0      jBS4   jBS3   jBS2  jBS1  jBS0  */
    0xFF,                       /* CONFIG1: 1    jIRQS2 jIRQS1 jIRQS0 jIOS3  jIOS2 jIOS1 jIOS0 */

    0xFF,                       /* CONFIG4: -    -      -      -      -      -     -     IOMS  */
    0x30,                       /* CONFIG3  PNP  FUDUP  LEDS1  LEDS0  -      0     PWRDN ACTB  */

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* MAC */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF      /* ID */
};
#endif

/*!
 * \brief EEPROM emulator.
 *
 * Forces the chip to re-read the EEPROM contents and emulates a serial
 * EEPROM.
 *
 * If the hardware does not support this feature, then this call will
 * never return. Thus, make sure to have the driver properly configured.
 */
static void EmulateNicEeprom(void)
{
#ifdef RTL_EESK_BIT
    register uint8_t clk;
    register uint8_t cnt;
    register uint8_t val;

    /*
     * Disable all interrupts. This routine requires critical timing
     * and optionally may disable the memory interface.
     */
    NutEnterCritical();

    /*
     * Prepare the EEPROM emulation port bits. Configure the EEDO and
     * the EEMU lines as outputs and set EEDO to low and EEMU to high.
     */
    cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    sbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
#ifdef RTL_EEMU_BIT
    sbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    sbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    NutDelay(20);

    /*
     * Start EEPROM configuration. Stop/abort any activity and select
     * configuration page 3. Setting bit EEM0 will force the controller
     * to read the EEPROM contents.
     */

    /* Select page 3, stop and abort/complete. */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1);
    NICOUTB(NIC_PG3_EECR, NIC_EECR_EEM0);

    /*
     * We can avoid wasting port pins for EEPROM emulation by using the
     * upper bits of the address bus.
     */
#ifdef RTL_EE_MEMBUS
    /*
     * No external memory access beyond this point.
     */
#ifdef __AVR_ENHANCED__
    /* On the ATmega 128 we release bits 5-7 as normal port pins. */
    outb(XMCRB, inb(XMCRB) | _BV(XMM0) | _BV(XMM1));
#else
    /* On the ATmega 103 we have to disable the external memory interface. */
    cbi(MCUCR, SRE);
#endif
#endif

    /*
     * Loop for all EEPROM words.
     */
    for (cnt = 0; cnt < sizeof(nic_eeprom);) {

        /*
         *
         * 1 start bit, always high
         * 2 op-code bits
         * 7 address bits
         * 1 dir change bit, always low
         */
        for (clk = 0; clk < 11; clk++) {
            while (bit_is_clear(RTL_EESK_PIN, RTL_EESK_BIT));
            while (bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT));
        }

        /*
         * Shift out the high byte, MSB first. Our data changes at the EESK
         * rising edge. Data is sampled by the Realtek at the falling edge.
         */
        val = PRG_RDB(nic_eeprom + cnt);
        cnt++;
        for (clk = 0x80; clk; clk >>= 1) {
            while (bit_is_clear(RTL_EESK_PIN, RTL_EESK_BIT));
            if (val & clk)
                sbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
            while (bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT));
            cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
        }

        /*
         * Shift out the low byte.
         */
        val = PRG_RDB(nic_eeprom + cnt);
        cnt++;
        for (clk = 0x80; clk; clk >>= 1) {
            while (bit_is_clear(RTL_EESK_PIN, RTL_EESK_BIT));
            if (val & clk)
                sbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
            while (bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT));
            cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
        }


        /* 5 remaining clock cycles. */
        for (clk = 0; clk < 5; clk++) {
            while (bit_is_clear(RTL_EESK_PIN, RTL_EESK_BIT));
            while (bit_is_set(RTL_EESK_PIN, RTL_EESK_BIT));
        }
    }

#ifdef RTL_EE_MEMBUS
    /*
     * Enable memory interface.
     */
#ifdef __AVR_ENHANCED__
    /* On the ATmega 128 we release bits 5-7 as normal port pins. */
    outb(XMCRB, inb(XMCRB) & ~(_BV(XMM0) | _BV(XMM1)));
#else
    /* On the ATmega 103 we have to disable the external memory interface. */
    sbi(MCUCR, SRE);
#endif
#endif

    /* Reset port outputs to default. */
    cbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
#ifdef RTL_EEMU_BIT
    cbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    cbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif

    /* Restore previous interrupt enable state. */
    NutExitCritical();
#endif
}

/*
 * Fires up the network interface. NIC interrupts
 * should have been disabled when calling this
 * function.
 */
static int NicStart(CONST uint8_t * mac)
{
    uint8_t i;

    if (NicReset()) {
        return -1;
    }
    if (DetectNicEeprom() == 0) {
        EmulateNicEeprom();
    }

    /*
     * Mask all interrupts and clear any interrupt status flag to set the
     * INT pin back to low.
     */
    NICOUTB(NIC_PG0_IMR, 0);
    NICOUTB(NIC_PG0_ISR, 0xff);

    /*
     * During reset the nic loaded its initial configuration from an
     * external eeprom. On the ethernut board we do not have any
     * configuration eeprom, but simply tied the eeprom data line to
     * high level. So we have to clear some bits in the configuration
     * register. Switch to register page 3.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1);

    /*
     * The nic configuration registers are write protected unless both
     * EEM bits are set to 1.
     */
    NICOUTB(NIC_PG3_EECR, NIC_EECR_EEM0 | NIC_EECR_EEM1);

    /*
     * Network media had been set to 10Base2 by the virtual EEPROM and
     * will be set now to auto detect. This will initiate a link test.
     * We don't force 10BaseT, because this would disable the link test.
     */
    NICOUTB(NIC_PG3_CONFIG2, NIC_CONFIG2_BSELB);

    /*
     * Disable sleep and power down.
     *
     * The virtual EEPROM (resistor tight to VCC) will set all bits of
     * CONFIG3 to 1. Unfortunately we are not able to modify the full
     * duplex bit. The only solution is to use a real EEPROM or emulate
     * one.
     */
    NICOUTB(NIC_PG3_CONFIG3, NIC_CONFIG3_LEDS1 | NIC_CONFIG3_LEDS1);

    /*
     * Reenable write protection of the nic configuration registers
     * and wait for link test to complete.
     */
    NICOUTB(NIC_PG3_EECR, 0);
    NutDelay(255);

    /*
     * Switch to register page 0 and set data configuration register
     * to byte-wide DMA transfers, normal operation (no loopback),
     * send command not executed and 8 byte fifo threshold.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2);
    NICOUTB(NIC_PG0_DCR, NIC_DCR_LS | NIC_DCR_FT1);

    /*
     * Clear remote dma byte count register.
     */
    NICOUTB(NIC_PG0_RBCR0, 0);
    NICOUTB(NIC_PG0_RBCR1, 0);

    /*
     * Temporarily set receiver to monitor mode and transmitter to
     * internal loopback mode. Incoming packets will not be stored
     * in the nic ring buffer and no data will be send to the network.
     */
    NICOUTB(NIC_PG0_RCR, NIC_RCR_MON);
    NICOUTB(NIC_PG0_TCR, NIC_TCR_LB0);

    /*
     * Configure the nic's ring buffer page layout.
     * NIC_PG0_BNRY: Last page read.
     * NIC_PG0_PSTART: First page of receiver buffer.
     * NIC_PG0_PSTOP: Last page of receiver buffer.
     */
    NICOUTB(NIC_PG0_TPSR, NIC_FIRST_TX_PAGE);
    NICOUTB(NIC_PG0_BNRY, NIC_STOP_PAGE - 1);
    NICOUTB(NIC_PG0_PSTART, NIC_FIRST_RX_PAGE);
    NICOUTB(NIC_PG0_PSTOP, NIC_STOP_PAGE);

    /*
     * Once again clear interrupt status register.
     */
    NICOUTB(NIC_PG0_ISR, 0xff);

    /*
     * Switch to register page 1 and copy our MAC address into the nic.
     * We are still in stop mode.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0);
    for (i = 0; i < 6; i++)
        NICOUTB(NIC_PG1_PAR0 + i, mac[i]);

    /*
     * Clear multicast filter bits to disable all packets.
     */
    for (i = 0; i < 8; i++)
        NICOUTB(NIC_PG1_MAR0 + i, 0);

    /*
     * Set current page pointer to one page after the boundary pointer.
     */
    NICOUTB(NIC_PG1_CURR, NIC_FIRST_RX_PAGE);

    /*
     * Switch back to register page 0, remaining in stop mode.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2);

    /*
     * Take receiver out of monitor mode and enable it for accepting
     * broadcasts.
     */
    NICOUTB(NIC_PG0_RCR, NIC_RCR_AB);

    /*
     * Clear all interrupt status flags and enable interrupts.
     */
    NICOUTB(NIC_PG0_ISR, 0xff);
    /* Note: transmitter if polled, thus no NIC_IMR_PTXE */
    NICOUTB(NIC_PG0_IMR, NIC_IMR_PRXE | NIC_IMR_RXEE | NIC_IMR_TXEE | NIC_IMR_OVWE);

    /*
     * Fire up the nic by clearing the stop bit and setting the start bit.
     * To activate the local receive dma we must also take the nic out of
     * the local loopback mode.
     */
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD2);
    NICOUTB(NIC_PG0_TCR, 0);

    NutDelay(255);

    return 0;
}

/*!
 * Complete remote DMA.
 */
static void NicCompleteDma(void)
{
    uint8_t i;

    /*
     * Complete remote dma.
     */
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Check that we have a DMA complete flag.
     */
    for (i = 0; i <= 20; i++)
        if (NICINB(NIC_PG0_ISR) & NIC_ISR_RDC)
            break;

    /*
     * Reset remote dma complete flag.
     */
    NICOUTB(NIC_PG0_ISR, NIC_ISR_RDC);
}

/*
 * Write data block to the NIC.
 */
static void NicWrite(uint8_t * buf, uint16_t len)
{
    register uint16_t l = len - 1;
    register uint8_t ih = (uint16_t) l >> 8;
    register uint8_t il = (uint8_t) l;

    if (!len)
        return;

    do {
        do {
            NICOUTB(NIC_IOPORT, *buf++);
        } while (il-- != 0);
    } while (ih-- != 0);
}

/*
 * Read data block from the NIC.
 */
static void NicRead(uint8_t * buf, uint16_t len)
{
    register uint16_t l = len - 1;
    register uint8_t ih = (uint16_t) l >> 8;
    register uint8_t il = (uint8_t) l;

    if (!len)
        return;

    do {
        do {
            *buf++ = NICINB(NIC_IOPORT);
        } while (il-- != 0);
    } while (ih-- != 0);
}


/*!
 * \brief Load a packet into the nic's transmit ring buffer.
 *
 *
 * \param base NIC hardware base address.
 * \param nb Network buffer structure containing the packet to be sent.
 *           The structure must have been allocated by a previous
 *           call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors. Errors
 *         will automatically release the network buffer
 *         structure.
 */
static int NicPutPacket(NETBUF * nb)
{
    uint16_t sz;
    uint16_t i;
    uint8_t padding = 0;

    /*
     * Calculate the number of bytes to be send. Do not
     * send packets larger than 1514 bytes.
     *
     * The previous version was wrong by specifying a maximum
     * of 1518, because it didn't take the CRC into account,
     * which is generated by the hardware and automatically
     * appended. Thanks to Bengt Florin, who discovered this.
     */
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (sz > 1514)
        return -1;

    /*
     * The controller will not append pad bytes,
     * so we have to do this.
     */
    if (sz < 60) {
        padding = (uint8_t) (60 - sz);
        sz = 60;
    }

    /*
     * Bengt Florin introduces polling mode for the transmitter. Be
     * aware, that this may introduce other problems. If a high
     * priority thread is waiting for the transmitter, it may hold
     * the CPU for more than 1.2 milliseconds in worst cases.
     */
    while (NICINB(NIC_CR) & NIC_CR_TXP)
        NutThreadYield();

    /* we don't want to be interrupted by NIC owerflow */
    cbi(EIMSK, RTL_SIGNAL_IRQ);

    /*
     * Set remote dma byte count
     * and start address.
     */
    NICOUTB(NIC_PG0_RBCR0, sz);
    NICOUTB(NIC_PG0_RBCR1, sz >> 8);
    NICOUTB(NIC_PG0_RSAR0, 0);
    NICOUTB(NIC_PG0_RSAR1, NIC_FIRST_TX_PAGE);

    /*
     * Peform the write.
     */
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD1);

    /*
     * Transfer the Ethernet frame.
     */
    NicWrite(nb->nb_dl.vp, nb->nb_dl.sz);
    NicWrite(nb->nb_nw.vp, nb->nb_nw.sz);
    NicWrite(nb->nb_tp.vp, nb->nb_tp.sz);
    NicWrite(nb->nb_ap.vp, nb->nb_ap.sz);

    /*
     * Add pad bytes.
     */
    for (i = 0; i < padding; i++)
        NICOUTB(NIC_IOPORT, 0);

    /*
     * Complete remote dma.
     */
    NicCompleteDma();

    /*
     * Number of bytes to be transmitted.
     */
    NICOUTB(NIC_PG0_TBCR0, (sz & 0xff));
    NICOUTB(NIC_PG0_TBCR1, ((sz >> 8) & 0xff));

    /*
     * First page of packet to be transmitted.
     */
    NICOUTB(NIC_PG0_TPSR, NIC_FIRST_TX_PAGE);

    /*
     * Start transmission.
     */
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2);

    sbi(EIMSK, RTL_SIGNAL_IRQ);

    return 0;
}

/*!
 * \brief Fetch the next packet out of the receive ring buffer.
 *
 * Nic interrupts must be disabled when calling this funtion.
 *
 * \return Pointer to an allocated ::NETBUF. If there is no
 *         no data available, then the function returns a
 *         null pointer. If the NIC's buffer seems to be
 *         corrupted, a pointer to 0xFFFF is returned.
 */
static NETBUF *NicGetPacket(void)
{
    NETBUF *nb = 0;
    struct nic_pkt_header hdr;
    uint16_t count;
    uint8_t *buf;
    uint8_t nextpg;
    uint8_t bnry;
    uint8_t curr;
    uint16_t i;
    uint8_t drop = 0;

    /* we don't want to be interrupted by NIC owerflow */
    cbi(EIMSK, RTL_SIGNAL_IRQ);
    /*
     * Get the current page pointer. It points to the page where the NIC
     * will start saving the next incoming packet.
     */
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD2 | NIC_CR_PS0);
    Delay16Cycles();
    curr = NICINB(NIC_PG1_CURR);
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Get the pointer to the last page we read from. The following page
     * is the one where we start reading. If it's equal to the current
     * page pointer, then there's nothing to read. In this case we return
     * a null pointer.
     */
    if ((bnry = NICINB(NIC_PG0_BNRY) + 1) >= NIC_STOP_PAGE)
        bnry = NIC_FIRST_RX_PAGE;

    if (bnry == curr) {
        sbi(EIMSK, RTL_SIGNAL_IRQ);
        return 0;
    }

    /*
     * Read the NIC specific packet header.
     */
    NICOUTB(NIC_PG0_RBCR0, sizeof(struct nic_pkt_header));
    NICOUTB(NIC_PG0_RBCR1, 0);
    NICOUTB(NIC_PG0_RSAR0, 0);
    NICOUTB(NIC_PG0_RSAR1, bnry);
    buf = (uint8_t *) & hdr;
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD0);
    Delay16Cycles();
    for (i = 0; i < sizeof(struct nic_pkt_header); i++)
        *buf++ = NICINB(NIC_IOPORT);
    NicCompleteDma();

    /*
     *  Check packet length. Silently discard packets of illegal size.
     */
    if (hdr.ph_size < 60 + sizeof(struct nic_pkt_header) ||     /* */
        hdr.ph_size > 1514 + sizeof(struct nic_pkt_header)) {
        drop = 1;
    }

    /*
     * Calculate the page of the next packet. If it differs from the
     * pointer in the packet header, we return with errorcode.
     */
    nextpg = bnry + (hdr.ph_size >> 8) + ((hdr.ph_size & 0xFF) != 0);
    if (nextpg >= NIC_STOP_PAGE) {
        nextpg -= NIC_STOP_PAGE;
        nextpg += NIC_FIRST_RX_PAGE;
    }
    if (nextpg != hdr.ph_nextpg) {
        uint8_t nextpg1 = nextpg + 1;
        if (nextpg1 >= NIC_STOP_PAGE) {
            nextpg1 -= NIC_STOP_PAGE;
            nextpg1 += NIC_FIRST_RX_PAGE;
        }
        if (nextpg1 != hdr.ph_nextpg) {
            sbi(EIMSK, RTL_SIGNAL_IRQ);
            return (NETBUF *) 0xFFFF;
        }
        nextpg = nextpg1;
    }

    /*
     * Check packet status. It should have set bit 0, but
     * even without this bit packets seem to be OK.
     */
    if (!drop && ((hdr.ph_status & 0x0E) == 0)) {
        /*
         * Allocate a NETBUF.
         * Omit the fcs.
         */
        count = hdr.ph_size - 4;
        if ((nb = NutNetBufAlloc(0, NBAF_DATALINK, count))) {
            /*
             * Set remote dma byte count and
             * start address. Don't read the
             * header again.
             */
            NICOUTB(NIC_PG0_RBCR0, count);
            NICOUTB(NIC_PG0_RBCR1, count >> 8);
            NICOUTB(NIC_PG0_RSAR0, sizeof(struct nic_pkt_header));
            NICOUTB(NIC_PG0_RSAR1, bnry);

            /*
             * Perform the read.
             */
            NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD0);
            Delay16Cycles();
            NicRead(nb->nb_dl.vp, count);
            NicCompleteDma();
        }
    }

    /*
     * Set boundary register to the last page we read.
     * This also drops packets with errors
     */
    if (--nextpg < NIC_FIRST_RX_PAGE)
        nextpg = NIC_STOP_PAGE - 1;
    NICOUTB(NIC_PG0_BNRY, nextpg);

    sbi(EIMSK, RTL_SIGNAL_IRQ);
    return nb;
}

/*
 * \brief Handle NIC overflows.
 *
 * When a receiver buffer overflow occurs, the NIC will defer any subsequent
 * action until properly restarted.
 *
 * This routine is called within interrupt context, which introduces a big
 * problem. It waits for the last transmission to finish, which may take
 * several milliseconds. Since Nut/OS 3.5, we do not support nested interrupts
 * on AVR systems anymore. So this routine may now increase interrupt
 * latency in an unacceptable way. The solution might be to handle overflows
 * in the receiver thread.
 *
 * In any case, this routines needs a major redesign. But it has been
 * tested in its current form to gracefully withstand ping floods. Thanks
 * to Bengt Florin for contributing his code, which provides much more
 * stability than its predecessor.
 */
static uint8_t NicOverflow(void)
{
    uint8_t cr;
    uint8_t resend = 0;
    uint8_t curr;

    /*
     * Wait for any transmission in progress. Save the command register,
     * so we can later determine, if NIC transmitter has been interrupted.
     * or reception in progress.
     */
    while (NICINB(NIC_CR) & NIC_CR_TXP);
    cr = NICINB(NIC_CR);

    /*
     * Get the current page pointer. It points to the page where the NIC
     * will start saving the next incoming packet.
     */
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0);
    curr = NICINB(NIC_PG1_CURR);
    NICOUTB(NIC_CR, NIC_CR_STP | NIC_CR_RD2);

    /* Clear remote byte count register. */
    NICOUTB(NIC_PG0_RBCR0, 0);
    NICOUTB(NIC_PG0_RBCR1, 0);

    /* Check for any incomplete transmission. */
    if ((cr & NIC_CR_TXP) && ((NICINB(NIC_PG0_ISR) & (NIC_ISR_PTX | NIC_ISR_TXE)) == 0)) {
        resend = 1;
    }

    /* Enter loopback mode and restart the NIC. */
    NICOUTB(NIC_PG0_TCR, NIC_TCR_LB0);
    NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Discard all packets from the receiver buffer. Set boundary
     * register to the last page we read.
     */
    if (--curr < NIC_FIRST_RX_PAGE) {
        curr = NIC_STOP_PAGE - 1;
    }
    NICOUTB(NIC_PG0_BNRY, curr);

    /* Switch from loopback to normal mode mode. */
    NICOUTB(NIC_PG0_TCR, 0);

    /* Re-invoke any interrupted transmission. */
    if (resend) {
        NICOUTB(NIC_CR, NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2);
    }

    /* Finally clear the overflow flag */
    NICOUTB(NIC_PG0_ISR, NIC_ISR_OVW);
    return resend;
}


/*
 * \brief NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    uint8_t isr;
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    ni->ni_interrupts++;

#ifdef RTL_IRQ_RISING_EDGE
    do
    {
#endif 
    isr = NICINB(NIC_PG0_ISR);
    NICOUTB(NIC_PG0_ISR, isr);

    /*
     * Recover from receive buffer overflow. This may take some
     * time, so we enable global interrupts but keep NIC
     * interrupts disabled.
     */
    if (isr & NIC_ISR_OVW) {
        /* The AVR platform uses a dedicated interrupt stack, which
         * forbids interrupt nesting. */
#if !defined(__AVR__)
        cbi(EIMSK, RTL_SIGNAL_IRQ);
        sei();
#endif
        NicOverflow();
#if !defined(__AVR__)
        cli();
        sbi(EIMSK, RTL_SIGNAL_IRQ);
#endif
        ni->ni_rx_overruns++;
    } else {
        /*
         * If this is a transmit interrupt, then a packet has been sent.
         * So we can clear the transmitter busy flag and wake up the
         * transmitter thread.
         */
        if (isr & NIC_ISR_TXE)
            ni->ni_tx_errors++;

        /*
         * If this is a receive interrupt, then wake up the receiver
         * thread.
         */
        if (isr & NIC_ISR_PRX)
            NutEventPostFromIrq(&ni->ni_rx_rdy);

        if (isr & NIC_ISR_RXE) {
            ni->ni_rx_frame_errors += NICINB(NIC_PG0_CNTR0);
            ni->ni_rx_crc_errors += NICINB(NIC_PG0_CNTR1);
            ni->ni_rx_missed_errors += NICINB(NIC_PG0_CNTR2);
        }
    }
#ifdef RTL_IRQ_RISING_EDGE
    /* Check that all unmasked interrupts are cleared before we
    * leave the ISR to assert the INT line goes back to low
    * and a new interrupt edge will be generated for following
    * interrupts.
    */
    }
    while (bit_is_set(PINE, RTL_SIGNAL_IRQ));
#endif
}

/*! \fn NicRx(void *arg)
 * \brief NIC receiver thread.
 *
 *
 * It runs with high priority.
 */
THREAD(NicRx, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

    NutThreadSetPriority(9);
    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until one has been
     * set.
     */
    if ((ifn->if_mac[0] & ifn->if_mac[1] & ifn->if_mac[2]) == 0xFF) {
        while ((ifn->if_mac[0] & ifn->if_mac[1] & ifn->if_mac[2]) == 0xFF)
            NutSleep(125);
        cbi(EIMSK, RTL_SIGNAL_IRQ);
        NicStart(ifn->if_mac);
        sbi(EIMSK, RTL_SIGNAL_IRQ);
    }

    while (1) {
        NutEventWait(&ni->ni_rx_rdy, 0);
        /*
         * Fetch all packets from the NIC's internal
         * buffer and pass them to the registered handler.
         */
        do {
            nb = NicGetPacket();

            /* The sanity check may fail because the controller is too busy.
               restart the NIC. */
            if ((uint16_t) nb == 0xFFFF) {
                NicStart(ifn->if_mac);
                ni->ni_rx_size_errors++;
            } else if (nb) {
                ni->ni_rx_packets++;
                (*ifn->if_recv) (dev, nb);
            }
        } while (nb);
    }
}

/*!
 * \brief Send Ethernet packet.
 *
 * \param dev   Identifies the device to use.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
int NicOutput(NUTDEVICE * dev, NETBUF * nb)
{
    int rc = -1;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

    if (NicPutPacket(nb) == 0) {
        ni->ni_tx_packets++;
        rc = 0;
    }
    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets RTL8019AS Ethernet controller, initializes all required
 * hardware registers and starts a background thread for incoming
 * Ethernet traffic.
 *
 * Applications should do not directly call this function. It is
 * automatically executed during during device registration by
 * NutRegisterDevice(). Note, that base address and interrupt number
 * passed to NutRegisterDevice() are silently ignored by this driver
 * for performance reasons. These values can be changed only by
 * using the Nut/OS Configurator to rebuild the system.
 *
 * If the network configuration hasn't been set by the application
 * before registering the specified device, this function will
 * call NutNetLoadConfig() to get the MAC address.
 *
 * \param dev Identifies the device to initialize.
 */
int NicInit(NUTDEVICE * dev)
{
    IFNET *ifn;
    NICINFO *ni;

    /*
     * We need to know our MAC address. If no configuration is
     * available, load it now.
     */
    if (confnet.cd_size == 0)
        NutNetLoadConfig(dev->dev_name);

    ifn = dev->dev_icb;
    memcpy(ifn->if_mac, confnet.cdn_mac, 6);
    ni = (NICINFO *) dev->dev_dcb;
    memset(ni, 0, sizeof(NICINFO));

    /*
     * Start the receiver thread.
     */
    NutThreadCreate("rxi5", NicRx, dev, 
        (NUT_THREAD_NICRXSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);
    NutSleep(WAIT500);

    /*
     * Register interrupt handler and enable interrupts.
     */
    if (NutRegisterIrqHandler(&RTL_SIGNAL, NicInterrupt, dev))
        return -1;

    cbi(EIMSK, RTL_SIGNAL_IRQ);
#ifdef RTL_IRQ_RISING_EDGE
    /* Support of rising edge interrupts for HW w/o inverter gate */
    RTL_RISING_EDGE_MODE();
#endif

    if (ifn->if_mac[0] | ifn->if_mac[1] | ifn->if_mac[2])
        if (NicStart(ifn->if_mac))
            return -1;

    sbi(EIMSK, RTL_SIGNAL_IRQ);

    return 0;
}


/*@}*/
