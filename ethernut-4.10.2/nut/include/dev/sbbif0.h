#ifndef _DEV_SBBIF0_H_
#define _DEV_SBBIF0_H_
/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \file dev/sbbif0.h
 * \brief Serial bit banged interface 0.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2009/01/18 16:44:56  haraldkipp
 * Introduced target independent port numbers.
 * Distinguish between PIO IDs and port numbers.
 *
 * Revision 1.4  2009/01/18 10:45:54  haraldkipp
 * Fixed bit banging SPI for ARM targets.
 *
 * Revision 1.3  2008/08/28 16:11:31  haraldkipp
 * Fixed bitbanging SPI for ARM targets.
 *
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 09:07:54  haraldkipp
 * Configurable SPI added.
 *
 *
 * \endverbatim
 */

#include <cfg/arch/gpio.h>
#include <stdint.h>

/*!
 * \brief Maximum number of devices (chip selects).
 */
#ifndef SBBI0_MAX_DEVICES
#define SBBI0_MAX_DEVICES   4
#endif

#if defined(__AVR__)            /* MCU */
/*
 * AVR implementation.
 * ======================================
 */

#ifdef SBBI0_CS0_BIT

#if (SBBI0_CS0_PORT == AVRPORTB)
#define SBBI0_CS0_SOD_REG PORTB
#define SBBI0_CS0_OE_REG  DDRB
#elif (SBBI0_CS0_PORT == AVRPORTD)
#define SBBI0_CS0_SOD_REG PORTD
#define SBBI0_CS0_OE_REG  DDRD
#elif (SBBI0_CS0_PORT == AVRPORTE)
#define SBBI0_CS0_SOD_REG PORTE
#define SBBI0_CS0_OE_REG  DDRE
#elif (SBBI0_CS0_PORT == AVRPORTF)
#define SBBI0_CS0_SOD_REG PORTF
#define SBBI0_CS0_OE_REG  DDRF
#elif (SBBI0_CS0_PORT == AVRPORTG)
#define SBBI0_CS0_SOD_REG PORTG
#define SBBI0_CS0_OE_REG  DDRG
#elif (SBBI0_CS0_PORT == AVRPORTH)
#define SBBI0_CS0_SOD_REG PORTH
#define SBBI0_CS0_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 chip select 0 output. */
#define SBBI0_CS0_ENA()      sbi(SBBI0_CS0_OE_REG, SBBI0_CS0_BIT)
/*! \brief Set SBBI0 chip select 0 output low. */
#define SBBI0_CS0_CLR()      cbi(SBBI0_CS0_SOD_REG, SBBI0_CS0_BIT)
/*! \brief Set SBBI0 chip select 0 output high. */
#define SBBI0_CS0_SET()      sbi(SBBI0_CS0_SOD_REG, SBBI0_CS0_BIT)

#else /* SBBI0_CS0_BIT */

#define SBBI0_CS0_ENA()
#define SBBI0_CS0_CLR()
#define SBBI0_CS0_SET()

#endif /* SBBI0_CS0_BIT */

#ifdef SBBI0_CS1_BIT

#if (SBBI0_CS1_PORT == AVRPORTB)
#define SBBI0_CS1_SOD_REG PORTB
#define SBBI0_CS1_OE_REG  DDRB
#elif (SBBI0_CS1_PORT == AVRPORTD)
#define SBBI0_CS1_SOD_REG PORTD
#define SBBI0_CS1_OE_REG  DDRD
#elif (SBBI0_CS1_PORT == AVRPORTE)
#define SBBI0_CS1_SOD_REG PORTE
#define SBBI0_CS1_OE_REG  DDRE
#elif (SBBI0_CS1_PORT == AVRPORTF)
#define SBBI0_CS1_SOD_REG PORTF
#define SBBI0_CS1_OE_REG  DDRF
#elif (SBBI0_CS1_PORT == AVRPORTG)
#define SBBI0_CS1_SOD_REG PORTG
#define SBBI0_CS1_OE_REG  DDRG
#elif (SBBI0_CS1_PORT == AVRPORTH)
#define SBBI0_CS1_SOD_REG PORTH
#define SBBI0_CS1_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 chip select 1 output. */
#define SBBI0_CS1_ENA()      sbi(SBBI0_CS1_OE_REG, SBBI0_CS1_BIT)
/*! \brief Set SBBI0 chip select 1 output low. */
#define SBBI0_CS1_CLR()      cbi(SBBI0_CS1_SOD_REG, SBBI0_CS1_BIT)
/*! \brief Set SBBI0 chip select 1 output high. */
#define SBBI0_CS1_SET()      sbi(SBBI0_CS1_SOD_REG, SBBI0_CS1_BIT)

#else /* SBBI0_CS1_BIT */

#define SBBI0_CS1_ENA()
#define SBBI0_CS1_CLR()
#define SBBI0_CS1_SET()

#endif /* SBBI0_CS1_BIT */

#ifdef SBBI0_CS2_BIT

#if (SBBI0_CS2_PORT == AVRPORTB)
#define SBBI0_CS2_SOD_REG PORTB
#define SBBI0_CS2_OE_REG  DDRB
#elif (SBBI0_CS2_PORT == AVRPORTD)
#define SBBI0_CS2_SOD_REG PORTD
#define SBBI0_CS2_OE_REG  DDRD
#elif (SBBI0_CS2_PORT == AVRPORTE)
#define SBBI0_CS2_SOD_REG PORTE
#define SBBI0_CS2_OE_REG  DDRE
#elif (SBBI0_CS2_PORT == AVRPORTF)
#define SBBI0_CS2_SOD_REG PORTF
#define SBBI0_CS2_OE_REG  DDRF
#elif (SBBI0_CS2_PORT == AVRPORTG)
#define SBBI0_CS2_SOD_REG PORTG
#define SBBI0_CS2_OE_REG  DDRG
#elif (SBBI0_CS2_PORT == AVRPORTH)
#define SBBI0_CS2_SOD_REG PORTH
#define SBBI0_CS2_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 chip select 2 output. */
#define SBBI0_CS2_ENA()      sbi(SBBI0_CS2_OE_REG, SBBI0_CS2_BIT)
/*! \brief Set SBBI0 chip select 2 output low. */
#define SBBI0_CS2_CLR()      cbi(SBBI0_CS2_SOD_REG, SBBI0_CS2_BIT)
/*! \brief Set SBBI0 chip select 2 output high. */
#define SBBI0_CS2_SET()      sbi(SBBI0_CS2_SOD_REG, SBBI0_CS2_BIT)

#else /* SBBI0_CS2_BIT */

#define SBBI0_CS2_ENA()
#define SBBI0_CS2_CLR()
#define SBBI0_CS2_SET()

#endif /* SBBI0_CS2_BIT */

#ifdef SBBI0_CS3_BIT

#if (SBBI0_CS3_PORT == AVRPORTB)
#define SBBI0_CS3_SOD_REG PORTB
#define SBBI0_CS3_OE_REG  DDRB
#elif (SBBI0_CS3_PORT == AVRPORTD)
#define SBBI0_CS3_SOD_REG PORTD
#define SBBI0_CS3_OE_REG  DDRD
#elif (SBBI0_CS3_PORT == AVRPORTE)
#define SBBI0_CS3_SOD_REG PORTE
#define SBBI0_CS3_OE_REG  DDRE
#elif (SBBI0_CS3_PORT == AVRPORTF)
#define SBBI0_CS3_SOD_REG PORTF
#define SBBI0_CS3_OE_REG  DDRF
#elif (SBBI0_CS3_PORT == AVRPORTG)
#define SBBI0_CS3_SOD_REG PORTG
#define SBBI0_CS3_OE_REG  DDRG
#elif (SBBI0_CS3_PORT == AVRPORTH)
#define SBBI0_CS3_SOD_REG PORTH
#define SBBI0_CS3_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 chip select 3 output. */
#define SBBI0_CS3_ENA()      sbi(SBBI0_CS3_OE_REG, SBBI0_CS3_BIT)
/*! \brief Set SBBI0 chip select 3 output low. */
#define SBBI0_CS3_CLR()      cbi(SBBI0_CS3_SOD_REG, SBBI0_CS3_BIT)
/*! \brief Set SBBI0 chip select 3 output high. */
#define SBBI0_CS3_SET()      sbi(SBBI0_CS3_SOD_REG, SBBI0_CS3_BIT)

#else /* SBBI0_CS3_BIT */

#define SBBI0_CS3_ENA()
#define SBBI0_CS3_CLR()
#define SBBI0_CS3_SET()

#endif /* SBBI0_CS3_BIT */

#ifdef SBBI0_RST0_BIT

#if (SBBI0_RST0_PORT == AVRPORTB)
#define SBBI0_RST0_SOD_REG PORTB
#define SBBI0_RST0_OE_REG  DDRB
#elif (SBBI0_RST0_PORT == AVRPORTD)
#define SBBI0_RST0_SOD_REG PORTD
#define SBBI0_RST0_OE_REG  DDRD
#elif (SBBI0_RST0_PORT == AVRPORTE)
#define SBBI0_RST0_SOD_REG PORTE
#define SBBI0_RST0_OE_REG  DDRE
#elif (SBBI0_RST0_PORT == AVRPORTF)
#define SBBI0_RST0_SOD_REG PORTF
#define SBBI0_RST0_OE_REG  DDRF
#elif (SBBI0_RST0_PORT == AVRPORTG)
#define SBBI0_RST0_SOD_REG PORTG
#define SBBI0_RST0_OE_REG  DDRG
#elif (SBBI0_RST0_PORT == AVRPORTH)
#define SBBI0_RST0_SOD_REG PORTH
#define SBBI0_RST0_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 reset 0 output. */
#define SBBI0_RST0_ENA()      sbi(SBBI0_RST0_OE_REG, SBBI0_RST0_BIT)
/*! \brief Set SBBI0 reset 0 output low. */
#define SBBI0_RST0_CLR()      cbi(SBBI0_RST0_SOD_REG, SBBI0_RST0_BIT)
/*! \brief Set SBBI0 reset 0 output high. */
#define SBBI0_RST0_SET()      sbi(SBBI0_RST0_SOD_REG, SBBI0_RST0_BIT)

#else /* SBBI0_RST0_BIT */

#define SBBI0_RST0_ENA()
#define SBBI0_RST0_CLR()
#define SBBI0_RST0_SET()

#endif /* SBBI0_RST0_BIT */

#ifdef SBBI0_RST1_BIT

#if (SBBI0_RST1_PORT == AVRPORTB)
#define SBBI0_RST1_SOD_REG PORTB
#define SBBI0_RST1_OE_REG  DDRB
#elif (SBBI0_RST1_PORT == AVRPORTD)
#define SBBI0_RST1_SOD_REG PORTD
#define SBBI0_RST1_OE_REG  DDRD
#elif (SBBI0_RST1_PORT == AVRPORTE)
#define SBBI0_RST1_SOD_REG PORTE
#define SBBI0_RST1_OE_REG  DDRE
#elif (SBBI0_RST1_PORT == AVRPORTF)
#define SBBI0_RST1_SOD_REG PORTF
#define SBBI0_RST1_OE_REG  DDRF
#elif (SBBI0_RST1_PORT == AVRPORTG)
#define SBBI0_RST1_SOD_REG PORTG
#define SBBI0_RST1_OE_REG  DDRG
#elif (SBBI0_RST1_PORT == AVRPORTH)
#define SBBI0_RST1_SOD_REG PORTH
#define SBBI0_RST1_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 reset 1 output. */
#define SBBI0_RST1_ENA()      sbi(SBBI0_RST1_OE_REG, SBBI0_RST1_BIT)
/*! \brief Set SBBI0 reset 1 output low. */
#define SBBI0_RST1_CLR()      cbi(SBBI0_RST1_SOD_REG, SBBI0_RST1_BIT)
/*! \brief Set SBBI0 reset 1 output high. */
#define SBBI0_RST1_SET()      sbi(SBBI0_RST1_SOD_REG, SBBI0_RST1_BIT)

#else /* SBBI0_RST1_BIT */

#define SBBI0_RST1_ENA()
#define SBBI0_RST1_CLR()
#define SBBI0_RST1_SET()

#endif /* SBBI0_RST1_BIT */

#ifdef SBBI0_RST2_BIT

#if (SBBI0_RST2_PORT == AVRPORTB)
#define SBBI0_RST2_SOD_REG PORTB
#define SBBI0_RST2_OE_REG  DDRB
#elif (SBBI0_RST2_PORT == AVRPORTD)
#define SBBI0_RST2_SOD_REG PORTD
#define SBBI0_RST2_OE_REG  DDRD
#elif (SBBI0_RST2_PORT == AVRPORTE)
#define SBBI0_RST2_SOD_REG PORTE
#define SBBI0_RST2_OE_REG  DDRE
#elif (SBBI0_RST2_PORT == AVRPORTF)
#define SBBI0_RST2_SOD_REG PORTF
#define SBBI0_RST2_OE_REG  DDRF
#elif (SBBI0_RST2_PORT == AVRPORTG)
#define SBBI0_RST2_SOD_REG PORTG
#define SBBI0_RST2_OE_REG  DDRG
#elif (SBBI0_RST2_PORT == AVRPORTH)
#define SBBI0_RST2_SOD_REG PORTH
#define SBBI0_RST2_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 reset 2 output. */
#define SBBI0_RST2_ENA()      sbi(SBBI0_RST2_OE_REG, SBBI0_RST2_BIT)
/*! \brief Set SBBI0 chip reset 2 output low. */
#define SBBI0_RST2_CLR()      cbi(SBBI0_RST2_SOD_REG, SBBI0_RST2_BIT)
/*! \brief Set SBBI0 chip reset 2 output high. */
#define SBBI0_RST2_SET()      sbi(SBBI0_RST2_SOD_REG, SBBI0_RST2_BIT)

#else /* SBBI0_RST2_BIT */

#define SBBI0_RST2_ENA()
#define SBBI0_RST2_CLR()
#define SBBI0_RST2_SET()

#endif /* SBBI0_RST2_BIT */

#ifdef SBBI0_RST3_BIT

#if (SBBI0_RST3_PORT == AVRPORTB)
#define SBBI0_RST3_SOD_REG PORTB
#define SBBI0_RST3_OE_REG  DDRB
#elif (SBBI0_RST3_PORT == AVRPORTD)
#define SBBI0_RST3_SOD_REG PORTD
#define SBBI0_RST3_OE_REG  DDRD
#elif (SBBI0_RST3_PORT == AVRPORTE)
#define SBBI0_RST3_SOD_REG PORTE
#define SBBI0_RST3_OE_REG  DDRE
#elif (SBBI0_RST3_PORT == AVRPORTF)
#define SBBI0_RST3_SOD_REG PORTF
#define SBBI0_RST3_OE_REG  DDRF
#elif (SBBI0_RST3_PORT == AVRPORTG)
#define SBBI0_RST3_SOD_REG PORTG
#define SBBI0_RST3_OE_REG  DDRG
#elif (SBBI0_RST3_PORT == AVRPORTH)
#define SBBI0_RST3_SOD_REG PORTH
#define SBBI0_RST3_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 reset 3 output. */
#define SBBI0_RST3_ENA()      sbi(SBBI0_RST3_OE_REG, SBBI0_RST3_BIT)
/*! \brief Set SBBI0 reset 3 output low. */
#define SBBI0_RST3_CLR()      cbi(SBBI0_RST3_SOD_REG, SBBI0_RST3_BIT)
/*! \brief Set SBBI0 reset 3 output high. */
#define SBBI0_RST3_SET()      sbi(SBBI0_RST3_SOD_REG, SBBI0_RST3_BIT)

#else /* SBBI0_RST3_BIT */

#define SBBI0_RST3_ENA()
#define SBBI0_RST3_CLR()
#define SBBI0_RST3_SET()

#endif /* SBBI0_RST3_BIT */

#ifdef SBBI0_SCK_BIT

#if (SBBI0_SCK_PORT == AVRPORTB)
#define SBBI0_SCK_SOD_REG PORTB
#define SBBI0_SCK_OE_REG  DDRB
#elif (SBBI0_SCK_PORT == AVRPORTD)
#define SBBI0_SCK_SOD_REG PORTD
#define SBBI0_SCK_OE_REG  DDRD
#elif (SBBI0_SCK_PORT == AVRPORTE)
#define SBBI0_SCK_SOD_REG PORTE
#define SBBI0_SCK_OE_REG  DDRE
#elif (SBBI0_SCK_PORT == AVRPORTF)
#define SBBI0_SCK_SOD_REG PORTF
#define SBBI0_SCK_OE_REG  DDRF
#elif (SBBI0_SCK_PORT == AVRPORTG)
#define SBBI0_SCK_SOD_REG PORTG
#define SBBI0_SCK_OE_REG  DDRG
#elif (SBBI0_SCK_PORT == AVRPORTH)
#define SBBI0_SCK_SOD_REG PORTH
#define SBBI0_SCK_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 clock output. */
#define SBBI0_SCK_ENA()      sbi(SBBI0_SCK_OE_REG, SBBI0_SCK_BIT)
/*! \brief Set SBBI0 clock output low. */
#define SBBI0_SCK_CLR()      cbi(SBBI0_SCK_SOD_REG, SBBI0_SCK_BIT)
/*! \brief Set SBBI0 clock output high. */
#define SBBI0_SCK_SET()      sbi(SBBI0_SCK_SOD_REG, SBBI0_SCK_BIT)

#if defined(SBBI0_MOSI_BIT)

#if (SBBI0_MOSI_PORT == AVRPORTB)
#define SBBI0_MOSI_SOD_REG PORTB
#define SBBI0_MOSI_OE_REG  DDRB
#elif (SBBI0_MOSI_PORT == AVRPORTD)
#define SBBI0_MOSI_SOD_REG PORTD
#define SBBI0_MOSI_OE_REG  DDRD
#elif (SBBI0_MOSI_PORT == AVRPORTE)
#define SBBI0_MOSI_SOD_REG PORTE
#define SBBI0_MOSI_OE_REG  DDRE
#elif (SBBI0_MOSI_PORT == AVRPORTF)
#define SBBI0_MOSI_SOD_REG PORTF
#define SBBI0_MOSI_OE_REG  DDRF
#elif (SBBI0_MOSI_PORT == AVRPORTG)
#define SBBI0_MOSI_SOD_REG PORTG
#define SBBI0_MOSI_OE_REG  DDRG
#elif (SBBI0_MOSI_PORT == AVRPORTH)
#define SBBI0_MOSI_SOD_REG PORTH
#define SBBI0_MOSI_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 data output. */
#define SBBI0_MOSI_ENA()      sbi(SBBI0_MOSI_OE_REG, SBBI0_MOSI_BIT)
/*! \brief Set SBBI0 data output low. */
#define SBBI0_MOSI_CLR()      cbi(SBBI0_MOSI_SOD_REG, SBBI0_MOSI_BIT)
/*! \brief Set SBBI0 data output high. */
#define SBBI0_MOSI_SET()      sbi(SBBI0_MOSI_SOD_REG, SBBI0_MOSI_BIT)

#else                           /* SBBI0_MOSI_BIT */

#define SBBI0_MOSI_ENA()
#define SBBI0_MOSI_CLR()
#define SBBI0_MOSI_SET()

#endif                          /* SBBI0_MOSI_BIT */

#if defined(SBBI0_MISO_BIT)

#if (SBBI0_MISO_PORT == AVRPORTB)
#define SBBI0_MISO_PDS_REG PINB
#define SBBI0_MISO_PUE_REG PORTB
#define SBBI0_MISO_OE_REG  DDRB
#elif (SBBI0_MISO_PORT == AVRPORTD)
#define SBBI0_MISO_PDS_REG PIND
#define SBBI0_MISO_PUE_REG PORTD
#define SBBI0_MISO_OE_REG  DDRD
#elif (SBBI0_MISO_PORT == AVRPORTE)
#define SBBI0_MISO_PDS_REG PINE
#define SBBI0_MISO_PUE_REG PORTE
#define SBBI0_MISO_OE_REG  DDRE
#elif (SBBI0_MISO_PORT == AVRPORTF)
#define SBBI0_MISO_PDS_REG PINF
#define SBBI0_MISO_PUE_REG PORTF
#define SBBI0_MISO_OE_REG  DDRF
#elif (SBBI0_MISO_PORT == AVRPORTG)
#define SBBI0_MISO_PDS_REG PING
#define SBBI0_MISO_PUE_REG PORTG
#define SBBI0_MISO_OE_REG  DDRG
#elif (SBBI0_MISO_PORT == AVRPORTH)
#define SBBI0_MISO_PDS_REG PINH
#define SBBI0_MISO_PUE_REG PORTH
#define SBBI0_MISO_OE_REG  DDRH
#endif

/*! \brief Enable SBBI0 data input. */
#define SBBI0_MISO_ENA() \
    cbi(SBBI0_MISO_OE_REG, SBBI0_MISO_BIT); \
    sbi(SBBI0_MISO_PUE_REG, SBBI0_MISO_BIT)
/*! \brief Query SBBI0 data input. */
#define SBBI0_MISO_TST()    ((inb(SBBI0_MISO_PDS_REG) & _BV(SBBI0_MISO_BIT)) == _BV(SBBI0_MISO_BIT))

#else                           /* SBBI0_MISO_BIT */

#define SBBI0_MISO_ENA()
#define SBBI0_MISO_TST()   0

#endif                          /* SBBI0_MISO_BIT */

#else                           /* SBBI0_SCK_BIT */

#define SBBI0_SCK_ENA()
#define SBBI0_SCK_CLR()
#define SBBI0_SCK_SET()

#define SBBI0_MOSI_ENA()
#define SBBI0_MOSI_CLR()
#define SBBI0_MOSI_SET()

#define SBBI0_MISO_ENA()
#define SBBI0_MISO_TST()   0

#endif                          /* SBBI0_SCK_BIT */

#else                           /* MCU */
/*
 * AT91 implementation.
 * ======================================
 */

#ifdef SBBI0_CS0_BIT

#ifndef SBBI0_CS0_PORT
#define SBBI0_CS0_PORT  NUTGPIO_PORT
#endif

#if SBBI0_CS0_PORT == NUTGPIO_PORT
#define SBBI0_CS0_PE_REG        PIO_PER
#define SBBI0_CS0_OE_REG        PIO_OER
#define SBBI0_CS0_COD_REG       PIO_CODR
#define SBBI0_CS0_SOD_REG       PIO_SODR
#elif SBBI0_CS0_PORT == NUTGPIO_PORTA
#define SBBI0_CS0_PE_REG        PIOA_PER
#define SBBI0_CS0_OE_REG        PIOA_OER
#define SBBI0_CS0_COD_REG       PIOA_CODR
#define SBBI0_CS0_SOD_REG       PIOA_SODR
#elif SBBI0_CS0_PORT == NUTGPIO_PORTB
#define SBBI0_CS0_PE_REG        PIOB_PER
#define SBBI0_CS0_OE_REG        PIOB_OER
#define SBBI0_CS0_COD_REG       PIOB_CODR
#define SBBI0_CS0_SOD_REG       PIOB_SODR
#elif SBBI0_CS0_PORT == NUTGPIO_PORTC
#define SBBI0_CS0_PE_REG        PIOC_PER
#define SBBI0_CS0_OE_REG        PIOC_OER
#define SBBI0_CS0_COD_REG       PIOC_CODR
#define SBBI0_CS0_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SBBI0 chip select 0 output. */
#define SBBI0_CS0_ENA() \
    outr(SBBI0_CS0_PE_REG, _BV(SBBI0_CS0_BIT)); \
    outr(SBBI0_CS0_OE_REG, _BV(SBBI0_CS0_BIT))
/*! \brief Set SBBI0 chip select 0 output low. */
#define SBBI0_CS0_CLR()   outr(SBBI0_CS0_COD_REG, _BV(SBBI0_CS0_BIT))
/*! \brief Set SBBI0 chip select 0 output high. */
#define SBBI0_CS0_SET()   outr(SBBI0_CS0_SOD_REG, _BV(SBBI0_CS0_BIT))

#else /* SBBI0_CS0_BIT */

#define SBBI0_CS0_ENA()
#define SBBI0_CS0_CLR()
#define SBBI0_CS0_SET()

#endif /* SBBI0_CS0_BIT */

#ifdef SBBI0_CS1_BIT

#ifndef SBBI0_CS1_PORT
#define SBBI0_CS1_PORT  NUTGPIO_PORT
#endif

#if SBBI0_CS1_PORT == NUTGPIO_PORT
#define SBBI0_CS1_PE_REG        PIO_PER
#define SBBI0_CS1_OE_REG        PIO_OER
#define SBBI0_CS1_COD_REG       PIO_CODR
#define SBBI0_CS1_SOD_REG       PIO_SODR
#elif SBBI0_CS1_PORT == NUTGPIO_PORTA
#define SBBI0_CS1_PE_REG        PIOA_PER
#define SBBI0_CS1_OE_REG        PIOA_OER
#define SBBI0_CS1_COD_REG       PIOA_CODR
#define SBBI0_CS1_SOD_REG       PIOA_SODR
#elif SBBI0_CS1_PORT == NUTGPIO_PORTB
#define SBBI0_CS1_PE_REG        PIOB_PER
#define SBBI0_CS1_OE_REG        PIOB_OER
#define SBBI0_CS1_COD_REG       PIOB_CODR
#define SBBI0_CS1_SOD_REG       PIOB_SODR
#elif SBBI0_CS1_PORT == NUTGPIO_PORTC
#define SBBI0_CS1_PE_REG        PIOC_PER
#define SBBI0_CS1_OE_REG        PIOC_OER
#define SBBI0_CS1_COD_REG       PIOC_CODR
#define SBBI0_CS1_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SBBI0 chip select output. */
#define SBBI0_CS1_ENA() \
    outr(SBBI0_CS1_PE_REG, _BV(SBBI0_CS1_BIT)); \
    outr(SBBI0_CS1_OE_REG, _BV(SBBI0_CS1_BIT))
/*! \brief Set SBBI0 chip select output low. */
#define SBBI0_CS1_CLR()   outr(SBBI0_CS1_COD_REG, _BV(SBBI0_CS1_BIT))
/*! \brief Set SBBI0 chip select output high. */
#define SBBI0_CS1_SET()   outr(SBBI0_CS1_SOD_REG, _BV(SBBI0_CS1_BIT))

#else /* SBBI0_CS1_BIT */

#define SBBI0_CS1_ENA()
#define SBBI0_CS1_CLR()
#define SBBI0_CS1_SET()

#endif /* SBBI0_CS1_BIT */

#ifdef SBBI0_CS2_BIT

#ifndef SBBI0_CS2_PORT
#define SBBI0_CS2_PORT  NUTGPIO_PORT
#endif

#if SBBI0_CS2_PORT == NUTGPIO_PORT
#define SBBI0_CS2_PE_REG        PIO_PER
#define SBBI0_CS2_OE_REG        PIO_OER
#define SBBI0_CS2_COD_REG       PIO_CODR
#define SBBI0_CS2_SOD_REG       PIO_SODR
#elif SBBI0_CS2_PORT == NUTGPIO_PORTA
#define SBBI0_CS2_PE_REG        PIOA_PER
#define SBBI0_CS2_OE_REG        PIOA_OER
#define SBBI0_CS2_COD_REG       PIOA_CODR
#define SBBI0_CS2_SOD_REG       PIOA_SODR
#elif SBBI0_CS2_PORT == NUTGPIO_PORTB
#define SBBI0_CS2_PE_REG        PIOB_PER
#define SBBI0_CS2_OE_REG        PIOB_OER
#define SBBI0_CS2_COD_REG       PIOB_CODR
#define SBBI0_CS2_SOD_REG       PIOB_SODR
#elif SBBI0_CS2_PORT == NUTGPIO_PORTC
#define SBBI0_CS2_PE_REG        PIOC_PER
#define SBBI0_CS2_OE_REG        PIOC_OER
#define SBBI0_CS2_COD_REG       PIOC_CODR
#define SBBI0_CS2_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SBBI0 chip select output. */
#define SBBI0_CS2_ENA() \
    outr(SBBI0_CS2_PE_REG, _BV(SBBI0_CS2_BIT)); \
    outr(SBBI0_CS2_OE_REG, _BV(SBBI0_CS2_BIT))
/*! \brief Set SBBI0 chip select output low. */
#define SBBI0_CS2_CLR()   outr(SBBI0_CS2_COD_REG, _BV(SBBI0_CS2_BIT))
/*! \brief Set SBBI0 chip select output high. */
#define SBBI0_CS2_SET()   outr(SBBI0_CS2_SOD_REG, _BV(SBBI0_CS2_BIT))

#else /* SBBI0_CS2_BIT */

#define SBBI0_CS2_ENA()
#define SBBI0_CS2_CLR()
#define SBBI0_CS2_SET()

#endif /* SBBI0_CS2_BIT */

#ifdef SBBI0_CS3_BIT

#ifndef SBBI0_CS3_PORT
#define SBBI0_CS3_PORT  NUTGPIO_PORT
#endif

#if SBBI0_CS3_PORT == NUTGPIO_PORT
#define SBBI0_CS3_PE_REG        PIO_PER
#define SBBI0_CS3_OE_REG        PIO_OER
#define SBBI0_CS3_COD_REG       PIO_CODR
#define SBBI0_CS3_SOD_REG       PIO_SODR
#elif SBBI0_CS3_PORT == NUTGPIO_PORTA
#define SBBI0_CS3_PE_REG        PIOA_PER
#define SBBI0_CS3_OE_REG        PIOA_OER
#define SBBI0_CS3_COD_REG       PIOA_CODR
#define SBBI0_CS3_SOD_REG       PIOA_SODR
#elif SBBI0_CS3_PORT == NUTGPIO_PORTB
#define SBBI0_CS3_PE_REG        PIOB_PER
#define SBBI0_CS3_OE_REG        PIOB_OER
#define SBBI0_CS3_COD_REG       PIOB_CODR
#define SBBI0_CS3_SOD_REG       PIOB_SODR
#elif SBBI0_CS3_PORT == NUTGPIO_PORTC
#define SBBI0_CS3_PE_REG        PIOC_PER
#define SBBI0_CS3_OE_REG        PIOC_OER
#define SBBI0_CS3_COD_REG       PIOC_CODR
#define SBBI0_CS3_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SBBI0 chip select output. */
#define SBBI0_CS3_ENA() \
    outr(SBBI0_CS3_PE_REG, _BV(SBBI0_CS3_BIT)); \
    outr(SBBI0_CS3_OE_REG, _BV(SBBI0_CS3_BIT))
/*! \brief Set SBBI0 chip select output low. */
#define SBBI0_CS3_CLR()   outr(SBBI0_CS3_COD_REG, _BV(SBBI0_CS3_BIT))
/*! \brief Set SBBI0 chip select output high. */
#define SBBI0_CS3_SET()   outr(SBBI0_CS3_SOD_REG, _BV(SBBI0_CS3_BIT))

#else /* SBBI0_CS3_BIT */

#define SBBI0_CS3_ENA()
#define SBBI0_CS3_CLR()
#define SBBI0_CS3_SET()

#endif /* SBBI0_CS3_BIT */

#ifdef SBBI0_RST0_BIT

#ifndef SBBI0_RST0_PORT
#define SBBI0_RST0_PORT  NUTGPIO_PORT
#endif

#if SBBI0_RST0_PORT == NUTGPIO_PORT
#define SBBI0_RST0_PE_REG      PIO_PER
#define SBBI0_RST0_OE_REG      PIO_OER
#define SBBI0_RST0_COD_REG     PIO_CODR
#define SBBI0_RST0_SOD_REG     PIO_SODR
#elif SBBI0_RST0_PORT == NUTGPIO_PORTA
#define SBBI0_RST0_PE_REG      PIOA_PER
#define SBBI0_RST0_OE_REG      PIOA_OER
#define SBBI0_RST0_COD_REG     PIOA_CODR
#define SBBI0_RST0_SOD_REG     PIOA_SODR
#elif SBBI0_RST0_PORT == NUTGPIO_PORTB
#define SBBI0_RST0_PE_REG      PIOB_PER
#define SBBI0_RST0_OE_REG      PIOB_OER
#define SBBI0_RST0_COD_REG     PIOB_CODR
#define SBBI0_RST0_SOD_REG     PIOB_SODR
#elif SBBI0_RST0_PORT == NUTGPIO_PORTC
#define SBBI0_RST0_PE_REG      PIOC_PER
#define SBBI0_RST0_OE_REG      PIOC_OER
#define SBBI0_RST0_COD_REG     PIOC_CODR
#define SBBI0_RST0_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SBBI0 reset output. */
#define SBBI0_RST0_ENA() \
    outr(SBBI0_RST0_PE_REG, _BV(SBBI0_RST0_BIT)); \
    outr(SBBI0_RST0_OE_REG, _BV(SBBI0_RST0_BIT))
/*! \brief Set SBBI0 reset output low. */
#define SBBI0_RST0_CLR()   outr(SBBI0_RST0_COD_REG, _BV(SBBI0_RST0_BIT))
/*! \brief Set SBBI0 reset output high. */
#define SBBI0_RST0_SET()   outr(SBBI0_RST0_SOD_REG, _BV(SBBI0_RST0_BIT))

#else /* SBBI0_RST0_BIT */

#define SBBI0_RST0_ENA()
#define SBBI0_RST0_CLR()
#define SBBI0_RST0_SET()

#endif /* SBBI0_RST0_BIT */

#ifdef SBBI0_RST1_BIT

#ifndef SBBI0_RST1_PORT
#define SBBI0_RST1_PORT  NUTGPIO_PORT
#endif

#if SBBI0_RST1_PORT == NUTGPIO_PORT
#define SBBI0_RST1_PE_REG      PIO_PER
#define SBBI0_RST1_OE_REG      PIO_OER
#define SBBI0_RST1_COD_REG     PIO_CODR
#define SBBI0_RST1_SOD_REG     PIO_SODR
#elif SBBI0_RST1_PORT == NUTGPIO_PORTA
#define SBBI0_RST1_PE_REG      PIOA_PER
#define SBBI0_RST1_OE_REG      PIOA_OER
#define SBBI0_RST1_COD_REG     PIOA_CODR
#define SBBI0_RST1_SOD_REG     PIOA_SODR
#elif SBBI0_RST1_PORT == NUTGPIO_PORTB
#define SBBI0_RST1_PE_REG      PIOB_PER
#define SBBI0_RST1_OE_REG      PIOB_OER
#define SBBI0_RST1_COD_REG     PIOB_CODR
#define SBBI0_RST1_SOD_REG     PIOB_SODR
#elif SBBI0_RST1_PORT == NUTGPIO_PORTC
#define SBBI0_RST1_PE_REG      PIOC_PER
#define SBBI0_RST1_OE_REG      PIOC_OER
#define SBBI0_RST1_COD_REG     PIOC_CODR
#define SBBI0_RST1_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SBBI0 reset output. */
#define SBBI0_RST1_ENA() \
    outr(SBBI0_RST1_PE_REG, _BV(SBBI0_RST1_BIT)); \
    outr(SBBI0_RST1_OE_REG, _BV(SBBI0_RST1_BIT))
/*! \brief Set SBBI0 reset output low. */
#define SBBI0_RST1_CLR()   outr(SBBI0_RST1_COD_REG, _BV(SBBI0_RST1_BIT))
/*! \brief Set SBBI0 reset output high. */
#define SBBI0_RST1_SET()   outr(SBBI0_RST1_SOD_REG, _BV(SBBI0_RST1_BIT))

#else /* SBBI0_RST1_BIT */

#define SBBI0_RST1_ENA()
#define SBBI0_RST1_CLR()
#define SBBI0_RST1_SET()

#endif /* SBBI0_RST1_BIT */

#ifdef SBBI0_RST2_BIT

#ifndef SBBI0_RST2_PORT
#define SBBI0_RST2_PORT  NUTGPIO_PORT
#endif

#if SBBI0_RST2_PORT == NUTGPIO_PORT
#define SBBI0_RST2_PE_REG      PIO_PER
#define SBBI0_RST2_OE_REG      PIO_OER
#define SBBI0_RST2_COD_REG     PIO_CODR
#define SBBI0_RST2_SOD_REG     PIO_SODR
#elif SBBI0_RST2_PORT == NUTGPIO_PORTA
#define SBBI0_RST2_PE_REG      PIOA_PER
#define SBBI0_RST2_OE_REG      PIOA_OER
#define SBBI0_RST2_COD_REG     PIOA_CODR
#define SBBI0_RST2_SOD_REG     PIOA_SODR
#elif SBBI0_RST2_PORT == NUTGPIO_PORTB
#define SBBI0_RST2_PE_REG      PIOB_PER
#define SBBI0_RST2_OE_REG      PIOB_OER
#define SBBI0_RST2_COD_REG     PIOB_CODR
#define SBBI0_RST2_SOD_REG     PIOB_SODR
#elif SBBI0_RST2_PORT == NUTGPIO_PORTC
#define SBBI0_RST2_PE_REG      PIOC_PER
#define SBBI0_RST2_OE_REG      PIOC_OER
#define SBBI0_RST2_COD_REG     PIOC_CODR
#define SBBI0_RST2_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SBBI0 reset output. */
#define SBBI0_RST2_ENA() \
    outr(SBBI0_RST2_PE_REG, _BV(SBBI0_RST2_BIT)); \
    outr(SBBI0_RST2_OE_REG, _BV(SBBI0_RST2_BIT))
/*! \brief Set SBBI0 reset output low. */
#define SBBI0_RST2_CLR()   outr(SBBI0_RST2_COD_REG, _BV(SBBI0_RST2_BIT))
/*! \brief Set SBBI0 reset output high. */
#define SBBI0_RST2_SET()   outr(SBBI0_RST2_SOD_REG, _BV(SBBI0_RST2_BIT))

#else /* SBBI0_RST2_BIT */

#define SBBI0_RST2_ENA()
#define SBBI0_RST2_CLR()
#define SBBI0_RST2_SET()

#endif /* SBBI0_RST2_BIT */

#ifdef SBBI0_RST3_BIT

#ifndef SBBI0_RST3_PORT
#define SBBI0_RST3_PORT  NUTGPIO_PORT
#endif

#if SBBI0_RST3_PORT == NUTGPIO_PORT
#define SBBI0_RST3_PE_REG      PIO_PER
#define SBBI0_RST3_OE_REG      PIO_OER
#define SBBI0_RST3_COD_REG     PIO_CODR
#define SBBI0_RST3_SOD_REG     PIO_SODR
#elif SBBI0_RST3_PORT == NUTGPIO_PORTA
#define SBBI0_RST3_PE_REG      PIOA_PER
#define SBBI0_RST3_OE_REG      PIOA_OER
#define SBBI0_RST3_COD_REG     PIOA_CODR
#define SBBI0_RST3_SOD_REG     PIOA_SODR
#elif SBBI0_RST3_PORT == NUTGPIO_PORTB
#define SBBI0_RST3_PE_REG      PIOB_PER
#define SBBI0_RST3_OE_REG      PIOB_OER
#define SBBI0_RST3_COD_REG     PIOB_CODR
#define SBBI0_RST3_SOD_REG     PIOB_SODR
#elif SBBI0_RST3_PORT == NUTGPIO_PORTC
#define SBBI0_RST3_PE_REG      PIOC_PER
#define SBBI0_RST3_OE_REG      PIOC_OER
#define SBBI0_RST3_COD_REG     PIOC_CODR
#define SBBI0_RST3_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SBBI0 reset output. */
#define SBBI0_RST3_ENA() \
    outr(SBBI0_RST3_PE_REG, _BV(SBBI0_RST3_BIT)); \
    outr(SBBI0_RST3_OE_REG, _BV(SBBI0_RST3_BIT))
/*! \brief Set SBBI0 reset output low. */
#define SBBI0_RST3_CLR()   outr(SBBI0_RST3_COD_REG, _BV(SBBI0_RST3_BIT))
/*! \brief Set SBBI0 reset output high. */
#define SBBI0_RST3_SET()   outr(SBBI0_RST3_SOD_REG, _BV(SBBI0_RST3_BIT))

#else /* SBBI0_RST3_BIT */

#define SBBI0_RST3_ENA()
#define SBBI0_RST3_CLR()
#define SBBI0_RST3_SET()

#endif /* SBBI0_RST3_BIT */

#ifdef SBBI0_SCK_BIT

#ifndef SBBI0_SCK_PORT
#define SBBI0_SCK_PORT  NUTGPIO_PORT
#endif

#if SBBI0_SCK_PORT == NUTGPIO_PORT
#define SBBI0_SCK_PE_REG        PIO_PER
#define SBBI0_SCK_OE_REG        PIO_OER
#define SBBI0_SCK_COD_REG       PIO_CODR
#define SBBI0_SCK_SOD_REG       PIO_SODR
#elif SBBI0_SCK_PORT == NUTGPIO_PORTA
#define SBBI0_SCK_PE_REG        PIOA_PER
#define SBBI0_SCK_OE_REG        PIOA_OER
#define SBBI0_SCK_COD_REG       PIOA_CODR
#define SBBI0_SCK_SOD_REG       PIOA_SODR
#elif SBBI0_SCK_PORT == NUTGPIO_PORTB
#define SBBI0_SCK_PE_REG        PIOB_PER
#define SBBI0_SCK_OE_REG        PIOB_OER
#define SBBI0_SCK_COD_REG       PIOB_CODR
#define SBBI0_SCK_SOD_REG       PIOB_SODR
#elif SBBI0_SCK_PORT == NUTGPIO_PORTC
#define SBBI0_SCK_PE_REG        PIOC_PER
#define SBBI0_SCK_OE_REG        PIOC_OER
#define SBBI0_SCK_COD_REG       PIOC_CODR
#define SBBI0_SCK_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SBBI0 clock output. */
#define SBBI0_SCK_ENA() \
    outr(SBBI0_SCK_PE_REG, _BV(SBBI0_SCK_BIT)); \
    outr(SBBI0_SCK_OE_REG, _BV(SBBI0_SCK_BIT))
/*! \brief Set SBBI0 clock output low. */
#define SBBI0_SCK_CLR()     outr(SBBI0_SCK_COD_REG, _BV(SBBI0_SCK_BIT))
/*! \brief Set SBBI0 clock output high. */
#define SBBI0_SCK_SET()     outr(SBBI0_SCK_SOD_REG, _BV(SBBI0_SCK_BIT))

#ifdef SBBI0_MOSI_BIT

#ifndef SBBI0_MOSI_PORT
#define SBBI0_MOSI_PORT  NUTGPIO_PORT
#endif

#if SBBI0_MOSI_PORT == NUTGPIO_PORT
#define SBBI0_MOSI_PE_REG       PIO_PER
#define SBBI0_MOSI_OE_REG       PIO_OER
#define SBBI0_MOSI_COD_REG      PIO_CODR
#define SBBI0_MOSI_SOD_REG      PIO_SODR
#elif SBBI0_MOSI_PORT == NUTGPIO_PORTA
#define SBBI0_MOSI_PE_REG       PIOA_PER
#define SBBI0_MOSI_OE_REG       PIOA_OER
#define SBBI0_MOSI_COD_REG      PIOA_CODR
#define SBBI0_MOSI_SOD_REG      PIOA_SODR
#elif SBBI0_MOSI_PORT == NUTGPIO_PORTB
#define SBBI0_MOSI_PE_REG       PIOB_PER
#define SBBI0_MOSI_OE_REG       PIOB_OER
#define SBBI0_MOSI_COD_REG      PIOB_CODR
#define SBBI0_MOSI_SOD_REG      PIOB_SODR
#elif SBBI0_MOSI_PORT == NUTGPIO_PORTC
#define SBBI0_MOSI_PE_REG       PIOC_PER
#define SBBI0_MOSI_OE_REG       PIOC_OER
#define SBBI0_MOSI_COD_REG      PIOC_CODR
#define SBBI0_MOSI_SOD_REG      PIOC_SODR
#endif

/*! \brief Enable SBBI0 data output. */
#define SBBI0_MOSI_ENA() \
    outr(SBBI0_MOSI_PE_REG, _BV(SBBI0_MOSI_BIT)); \
    outr(SBBI0_MOSI_OE_REG, _BV(SBBI0_MOSI_BIT))
/*! \brief Set SBBI0 data output low. */
#define SBBI0_MOSI_CLR()    outr(SBBI0_MOSI_COD_REG, _BV(SBBI0_MOSI_BIT))
/*! \brief Set SBBI0 data output high. */
#define SBBI0_MOSI_SET()    outr(SBBI0_MOSI_SOD_REG, _BV(SBBI0_MOSI_BIT))

#else                           /* SBBI0_MOSI_BIT */

#define SBBI0_MOSI_ENA()
#define SBBI0_MOSI_CLR()
#define SBBI0_MOSI_SET()

#endif                          /* SBBI0_MOSI_BIT */

#ifdef SBBI0_MISO_BIT

#ifndef SBBI0_MISO_PORT
#define SBBI0_MISO_PORT  NUTGPIO_PORT
#endif

#if SBBI0_MISO_PORT == NUTGPIO_PORT
#define SBBI0_MISO_PE_REG       PIO_PER
#define SBBI0_MISO_OD_REG       PIO_ODR
#define SBBI0_MISO_PDS_REG      PIO_PDSR
#elif SBBI0_MISO_PORT == NUTGPIO_PORTA
#define SBBI0_MISO_PE_REG       PIOA_PER
#define SBBI0_MISO_OD_REG       PIOA_ODR
#define SBBI0_MISO_PDS_REG      PIOA_PDSR
#elif SBBI0_MISO_PORT == NUTGPIO_PORTB
#define SBBI0_MISO_PE_REG       PIOB_PER
#define SBBI0_MISO_OD_REG       PIOB_ODR
#define SBBI0_MISO_PDS_REG      PIOB_PDSR
#elif SBBI0_MISO_PORT == NUTGPIO_PORTC
#define SBBI0_MISO_PE_REG       PIOC_PER
#define SBBI0_MISO_OD_REG       PIOC_ODR
#define SBBI0_MISO_PDS_REG      PIOC_PDSR
#endif

/*! \brief Enable SBBI0 data input. */
#define SBBI0_MISO_ENA() \
    outr(SBBI0_MISO_PE_REG, _BV(SBBI0_MISO_BIT)); \
    outr(SBBI0_MISO_OD_REG, _BV(SBBI0_MISO_BIT))
/*! \brief Query SBBI0 data input. */
#define SBBI0_MISO_TST()    ((inr(SBBI0_MISO_PDS_REG) & _BV(SBBI0_MISO_BIT)) == _BV(SBBI0_MISO_BIT))

#else                           /* SBBI0_MISO_BIT */

#define SBBI0_MISO_ENA()
#define SBBI0_MISO_TST()   0

#endif                          /* SBBI0_MISO_BIT */

#else                           /* SBBI0_SCK_BIT */

#define SBBI0_SCK_ENA()
#define SBBI0_SCK_CLR()
#define SBBI0_SCK_SET()

#define SBBI0_MOSI_ENA()
#define SBBI0_MOSI_CLR()
#define SBBI0_MOSI_SET()

#define SBBI0_MISO_ENA()
#define SBBI0_MISO_TST()   0

#endif                          /* SBBI0_SCK_BIT */

#endif                          /* MCU */

#define SBBI0_INIT() \
{ \
    SBBI0_SCK_CLR(); \
    SBBI0_SCK_ENA(); \
    SBBI0_MOSI_CLR(); \
    SBBI0_MOSI_ENA(); \
    SBBI0_MISO_ENA(); \
}

__BEGIN_DECLS
/* Function prototypes */

extern int Sbbi0SetMode(ureg_t ix, ureg_t mode);
extern void Sbbi0SetSpeed(ureg_t ix, uint32_t rate);
extern void Sbbi0Enable(ureg_t ix);
extern void Sbbi0ChipReset(ureg_t ix, ureg_t hi);
extern void Sbbi0ChipSelect(ureg_t ix, ureg_t hi);
extern void Sbbi0SelectDevice(ureg_t ix);
extern void Sbbi0DeselectDevice(ureg_t ix);
extern void Sbbi0NegSelectDevice(ureg_t ix);
extern void Sbbi0NegDeselectDevice(ureg_t ix);
extern uint8_t Sbbi0Byte(uint8_t data);
extern void Sbbi0Transact(CONST void *wdata, void *rdata, size_t len);

__END_DECLS
/* End of prototypes */

#endif
