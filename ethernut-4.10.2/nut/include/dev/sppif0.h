#ifndef _DEV_SPPIF0_H_
#define _DEV_SPPIF0_H_
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
 * \file dev/sppif0.h
 * \brief Serial bit banged interface 0.
 *
 * \verbatim
 *
 * $Log$
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
#ifndef SPPI0_MAX_DEVICES
#define SPPI0_MAX_DEVICES   4
#endif

#if defined(__AVR__)            /* MCU */
/*
 * AVR implementation.
 * ======================================
 */

#ifdef SPPI0_CS0_BIT

#if (SPPI0_CS0_AVRPORT == AVRPORTB)
#define SPPI0_CS0_SOD_REG PORTB
#define SPPI0_CS0_OE_REG  DDRB
#elif (SPPI0_CS0_AVRPORT == AVRPORTD)
#define SPPI0_CS0_SOD_REG PORTD
#define SPPI0_CS0_OE_REG  DDRD
#elif (SPPI0_CS0_AVRPORT == AVRPORTE)
#define SPPI0_CS0_SOD_REG PORTE
#define SPPI0_CS0_OE_REG  DDRE
#elif (SPPI0_CS0_AVRPORT == AVRPORTF)
#define SPPI0_CS0_SOD_REG PORTF
#define SPPI0_CS0_OE_REG  DDRF
#elif (SPPI0_CS0_AVRPORT == AVRPORTG)
#define SPPI0_CS0_SOD_REG PORTG
#define SPPI0_CS0_OE_REG  DDRG
#elif (SPPI0_CS0_AVRPORT == AVRPORTH)
#define SPPI0_CS0_SOD_REG PORTH
#define SPPI0_CS0_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 chip select 0 output. */
#define SPPI0_CS0_ENA()      sbi(SPPI0_CS0_OE_REG, SPPI0_CS0_BIT)
/*! \brief Set SPPI0 chip select 0 output low. */
#define SPPI0_CS0_CLR()      cbi(SPPI0_CS0_SOD_REG, SPPI0_CS0_BIT)
/*! \brief Set SPPI0 chip select 0 output high. */
#define SPPI0_CS0_SET()      sbi(SPPI0_CS0_SOD_REG, SPPI0_CS0_BIT)

#endif                          /* SPPI0_CS0_BIT */

#ifdef SPPI0_CS1_BIT

#if (SPPI0_CS1_AVRPORT == AVRPORTB)
#define SPPI0_CS1_SOD_REG PORTB
#define SPPI0_CS1_OE_REG  DDRB
#elif (SPPI0_CS1_AVRPORT == AVRPORTD)
#define SPPI0_CS1_SOD_REG PORTD
#define SPPI0_CS1_OE_REG  DDRD
#elif (SPPI0_CS1_AVRPORT == AVRPORTE)
#define SPPI0_CS1_SOD_REG PORTE
#define SPPI0_CS1_OE_REG  DDRE
#elif (SPPI0_CS1_AVRPORT == AVRPORTF)
#define SPPI0_CS1_SOD_REG PORTF
#define SPPI0_CS1_OE_REG  DDRF
#elif (SPPI0_CS1_AVRPORT == AVRPORTG)
#define SPPI0_CS1_SOD_REG PORTG
#define SPPI0_CS1_OE_REG  DDRG
#elif (SPPI0_CS1_AVRPORT == AVRPORTH)
#define SPPI0_CS1_SOD_REG PORTH
#define SPPI0_CS1_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 chip select 1 output. */
#define SPPI0_CS1_ENA()      sbi(SPPI0_CS1_OE_REG, SPPI0_CS1_BIT)
/*! \brief Set SPPI0 chip select 1 output low. */
#define SPPI0_CS1_CLR()      cbi(SPPI0_CS1_SOD_REG, SPPI0_CS1_BIT)
/*! \brief Set SPPI0 chip select 1 output high. */
#define SPPI0_CS1_SET()      sbi(SPPI0_CS1_SOD_REG, SPPI0_CS1_BIT)

#endif                          /* SPPI0_CS1_BIT */

#ifdef SPPI0_CS2_BIT

#if (SPPI0_CS2_AVRPORT == AVRPORTB)
#define SPPI0_CS2_SOD_REG PORTB
#define SPPI0_CS2_OE_REG  DDRB
#elif (SPPI0_CS2_AVRPORT == AVRPORTD)
#define SPPI0_CS2_SOD_REG PORTD
#define SPPI0_CS2_OE_REG  DDRD
#elif (SPPI0_CS2_AVRPORT == AVRPORTE)
#define SPPI0_CS2_SOD_REG PORTE
#define SPPI0_CS2_OE_REG  DDRE
#elif (SPPI0_CS2_AVRPORT == AVRPORTF)
#define SPPI0_CS2_SOD_REG PORTF
#define SPPI0_CS2_OE_REG  DDRF
#elif (SPPI0_CS2_AVRPORT == AVRPORTG)
#define SPPI0_CS2_SOD_REG PORTG
#define SPPI0_CS2_OE_REG  DDRG
#elif (SPPI0_CS2_AVRPORT == AVRPORTH)
#define SPPI0_CS2_SOD_REG PORTH
#define SPPI0_CS2_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 chip select 2 output. */
#define SPPI0_CS2_ENA()      sbi(SPPI0_CS2_OE_REG, SPPI0_CS2_BIT)
/*! \brief Set SPPI0 chip select 2 output low. */
#define SPPI0_CS2_CLR()      cbi(SPPI0_CS2_SOD_REG, SPPI0_CS2_BIT)
/*! \brief Set SPPI0 chip select 2 output high. */
#define SPPI0_CS2_SET()      sbi(SPPI0_CS2_SOD_REG, SPPI0_CS2_BIT)

#endif                          /* SPPI0_CS2_BIT */

#ifdef SPPI0_CS3_BIT

#if (SPPI0_CS3_AVRPORT == AVRPORTB)
#define SPPI0_CS3_SOD_REG PORTB
#define SPPI0_CS3_OE_REG  DDRB
#elif (SPPI0_CS3_AVRPORT == AVRPORTD)
#define SPPI0_CS3_SOD_REG PORTD
#define SPPI0_CS3_OE_REG  DDRD
#elif (SPPI0_CS3_AVRPORT == AVRPORTE)
#define SPPI0_CS3_SOD_REG PORTE
#define SPPI0_CS3_OE_REG  DDRE
#elif (SPPI0_CS3_AVRPORT == AVRPORTF)
#define SPPI0_CS3_SOD_REG PORTF
#define SPPI0_CS3_OE_REG  DDRF
#elif (SPPI0_CS3_AVRPORT == AVRPORTG)
#define SPPI0_CS3_SOD_REG PORTG
#define SPPI0_CS3_OE_REG  DDRG
#elif (SPPI0_CS3_AVRPORT == AVRPORTH)
#define SPPI0_CS3_SOD_REG PORTH
#define SPPI0_CS3_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 chip select 3 output. */
#define SPPI0_CS3_ENA()      sbi(SPPI0_CS3_OE_REG, SPPI0_CS3_BIT)
/*! \brief Set SPPI0 chip select 3 output low. */
#define SPPI0_CS3_CLR()      cbi(SPPI0_CS3_SOD_REG, SPPI0_CS3_BIT)
/*! \brief Set SPPI0 chip select 3 output high. */
#define SPPI0_CS3_SET()      sbi(SPPI0_CS3_SOD_REG, SPPI0_CS3_BIT)

#endif                          /* SPPI0_CS3_BIT */

#ifdef SPPI0_RST0_BIT

#if (SPPI0_RST0_AVRPORT == AVRPORTB)
#define SPPI0_RST0_SOD_REG PORTB
#define SPPI0_RST0_OE_REG  DDRB
#elif (SPPI0_RST0_AVRPORT == AVRPORTD)
#define SPPI0_RST0_SOD_REG PORTD
#define SPPI0_RST0_OE_REG  DDRD
#elif (SPPI0_RST0_AVRPORT == AVRPORTE)
#define SPPI0_RST0_SOD_REG PORTE
#define SPPI0_RST0_OE_REG  DDRE
#elif (SPPI0_RST0_AVRPORT == AVRPORTF)
#define SPPI0_RST0_SOD_REG PORTF
#define SPPI0_RST0_OE_REG  DDRF
#elif (SPPI0_RST0_AVRPORT == AVRPORTG)
#define SPPI0_RST0_SOD_REG PORTG
#define SPPI0_RST0_OE_REG  DDRG
#elif (SPPI0_RST0_AVRPORT == AVRPORTH)
#define SPPI0_RST0_SOD_REG PORTH
#define SPPI0_RST0_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 reset 0 output. */
#define SPPI0_RST0_ENA()      sbi(SPPI0_RST0_OE_REG, SPPI0_RST0_BIT)
/*! \brief Set SPPI0 reset 0 output low. */
#define SPPI0_RST0_CLR()      cbi(SPPI0_RST0_SOD_REG, SPPI0_RST0_BIT)
/*! \brief Set SPPI0 reset 0 output high. */
#define SPPI0_RST0_SET()      sbi(SPPI0_RST0_SOD_REG, SPPI0_RST0_BIT)

#endif                          /* SPPI0_RST0_BIT */

#ifdef SPPI0_RST1_BIT

#if (SPPI0_RST1_AVRPORT == AVRPORTB)
#define SPPI0_RST1_SOD_REG PORTB
#define SPPI0_RST1_OE_REG  DDRB
#elif (SPPI0_RST1_AVRPORT == AVRPORTD)
#define SPPI0_RST1_SOD_REG PORTD
#define SPPI0_RST1_OE_REG  DDRD
#elif (SPPI0_RST1_AVRPORT == AVRPORTE)
#define SPPI0_RST1_SOD_REG PORTE
#define SPPI0_RST1_OE_REG  DDRE
#elif (SPPI0_RST1_AVRPORT == AVRPORTF)
#define SPPI0_RST1_SOD_REG PORTF
#define SPPI0_RST1_OE_REG  DDRF
#elif (SPPI0_RST1_AVRPORT == AVRPORTG)
#define SPPI0_RST1_SOD_REG PORTG
#define SPPI0_RST1_OE_REG  DDRG
#elif (SPPI0_RST1_AVRPORT == AVRPORTH)
#define SPPI0_RST1_SOD_REG PORTH
#define SPPI0_RST1_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 reset 1 output. */
#define SPPI0_RST1_ENA()      sbi(SPPI0_RST1_OE_REG, SPPI0_RST1_BIT)
/*! \brief Set SPPI0 reset 1 output low. */
#define SPPI0_RST1_CLR()      cbi(SPPI0_RST1_SOD_REG, SPPI0_RST1_BIT)
/*! \brief Set SPPI0 reset 1 output high. */
#define SPPI0_RST1_SET()      sbi(SPPI0_RST1_SOD_REG, SPPI0_RST1_BIT)

#endif                          /* SPPI0_RST1_BIT */

#ifdef SPPI0_RST2_BIT

#if (SPPI0_RST2_AVRPORT == AVRPORTB)
#define SPPI0_RST2_SOD_REG PORTB
#define SPPI0_RST2_OE_REG  DDRB
#elif (SPPI0_RST2_AVRPORT == AVRPORTD)
#define SPPI0_RST2_SOD_REG PORTD
#define SPPI0_RST2_OE_REG  DDRD
#elif (SPPI0_RST2_AVRPORT == AVRPORTE)
#define SPPI0_RST2_SOD_REG PORTE
#define SPPI0_RST2_OE_REG  DDRE
#elif (SPPI0_RST2_AVRPORT == AVRPORTF)
#define SPPI0_RST2_SOD_REG PORTF
#define SPPI0_RST2_OE_REG  DDRF
#elif (SPPI0_RST2_AVRPORT == AVRPORTG)
#define SPPI0_RST2_SOD_REG PORTG
#define SPPI0_RST2_OE_REG  DDRG
#elif (SPPI0_RST2_AVRPORT == AVRPORTH)
#define SPPI0_RST2_SOD_REG PORTH
#define SPPI0_RST2_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 reset 2 output. */
#define SPPI0_RST2_ENA()      sbi(SPPI0_RST2_OE_REG, SPPI0_RST2_BIT)
/*! \brief Set SPPI0 chip reset 2 output low. */
#define SPPI0_RST2_CLR()      cbi(SPPI0_RST2_SOD_REG, SPPI0_RST2_BIT)
/*! \brief Set SPPI0 chip reset 2 output high. */
#define SPPI0_RST2_SET()      sbi(SPPI0_RST2_SOD_REG, SPPI0_RST2_BIT)

#endif                          /* SPPI0_RST2_BIT */

#ifdef SPPI0_RST3_BIT

#if (SPPI0_RST3_AVRPORT == AVRPORTB)
#define SPPI0_RST3_SOD_REG PORTB
#define SPPI0_RST3_OE_REG  DDRB
#elif (SPPI0_RST3_AVRPORT == AVRPORTD)
#define SPPI0_RST3_SOD_REG PORTD
#define SPPI0_RST3_OE_REG  DDRD
#elif (SPPI0_RST3_AVRPORT == AVRPORTE)
#define SPPI0_RST3_SOD_REG PORTE
#define SPPI0_RST3_OE_REG  DDRE
#elif (SPPI0_RST3_AVRPORT == AVRPORTF)
#define SPPI0_RST3_SOD_REG PORTF
#define SPPI0_RST3_OE_REG  DDRF
#elif (SPPI0_RST3_AVRPORT == AVRPORTG)
#define SPPI0_RST3_SOD_REG PORTG
#define SPPI0_RST3_OE_REG  DDRG
#elif (SPPI0_RST3_AVRPORT == AVRPORTH)
#define SPPI0_RST3_SOD_REG PORTH
#define SPPI0_RST3_OE_REG  DDRH
#endif

/*! \brief Enable SPPI0 reset 3 output. */
#define SPPI0_RST3_ENA()      sbi(SPPI0_RST3_OE_REG, SPPI0_RST3_BIT)
/*! \brief Set SPPI0 reset 3 output low. */
#define SPPI0_RST3_CLR()      cbi(SPPI0_RST3_SOD_REG, SPPI0_RST3_BIT)
/*! \brief Set SPPI0 reset 3 output high. */
#define SPPI0_RST3_SET()      sbi(SPPI0_RST3_SOD_REG, SPPI0_RST3_BIT)

#endif                          /* SPPI0_RST3_BIT */


#else                           /* MCU */
/*
 * AT91 implementation.
 * ======================================
 */

#ifdef SPPI0_CS0_BIT

#if !defined(SPPI0_CS0_PIO_ID)
#define SPPI0_CS0_PE_REG        PIO_PER
#define SPPI0_CS0_OE_REG        PIO_OER
#define SPPI0_CS0_COD_REG       PIO_CODR
#define SPPI0_CS0_SOD_REG       PIO_SODR
#elif SPPI0_CS0_PIO_ID == PIO_ID
#define SPPI0_CS0_PE_REG        PIO_PER
#define SPPI0_CS0_OE_REG        PIO_OER
#define SPPI0_CS0_COD_REG       PIO_CODR
#define SPPI0_CS0_SOD_REG       PIO_SODR
#elif SPPI0_CS0_PIO_ID == PIOA_ID
#define SPPI0_CS0_PE_REG        PIOA_PER
#define SPPI0_CS0_OE_REG        PIOA_OER
#define SPPI0_CS0_COD_REG       PIOA_CODR
#define SPPI0_CS0_SOD_REG       PIOA_SODR
#elif SPPI0_CS0_PIO_ID == PIOB_ID
#define SPPI0_CS0_PE_REG        PIOB_PER
#define SPPI0_CS0_OE_REG        PIOB_OER
#define SPPI0_CS0_COD_REG       PIOB_CODR
#define SPPI0_CS0_SOD_REG       PIOB_SODR
#elif SPPI0_CS0_PIO_ID == PIOC_ID
#define SPPI0_CS0_PE_REG        PIOC_PER
#define SPPI0_CS0_OE_REG        PIOC_OER
#define SPPI0_CS0_COD_REG       PIOC_CODR
#define SPPI0_CS0_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SPPI0 chip select 0 output. */
#define SPPI0_CS0_ENA() \
    outr(SPPI0_CS0_PE_REG, _BV(SPPI0_CS0_BIT)); \
    outr(SPPI0_CS0_OE_REG, _BV(SPPI0_CS0_BIT))
/*! \brief Set SPPI0 chip select 0 output low. */
#define SPPI0_CS0_CLR()   outr(SPPI0_CS0_COD_REG, _BV(SPPI0_CS0_BIT))
/*! \brief Set SPPI0 chip select 0 output high. */
#define SPPI0_CS0_SET()   outr(SPPI0_CS0_SOD_REG, _BV(SPPI0_CS0_BIT))

#endif                          /* SPPI0_CS0_BIT */

#ifdef SPPI0_CS1_BIT

#if !defined(SPPI0_CS1_PIO_ID)
#define SPPI0_CS1_PE_REG        PIO_PER
#define SPPI0_CS1_OE_REG        PIO_OER
#define SPPI0_CS1_COD_REG       PIO_CODR
#define SPPI0_CS1_SOD_REG       PIO_SODR
#elif SPPI0_CS1_PIO_ID == PIO_ID
#define SPPI0_CS1_PE_REG        PIO_PER
#define SPPI0_CS1_OE_REG        PIO_OER
#define SPPI0_CS1_COD_REG       PIO_CODR
#define SPPI0_CS1_SOD_REG       PIO_SODR
#elif SPPI0_CS1_PIO_ID == PIOA_ID
#define SPPI0_CS1_PE_REG        PIOA_PER
#define SPPI0_CS1_OE_REG        PIOA_OER
#define SPPI0_CS1_COD_REG       PIOA_CODR
#define SPPI0_CS1_SOD_REG       PIOA_SODR
#elif SPPI0_CS1_PIO_ID == PIOB_ID
#define SPPI0_CS1_PE_REG        PIOB_PER
#define SPPI0_CS1_OE_REG        PIOB_OER
#define SPPI0_CS1_COD_REG       PIOB_CODR
#define SPPI0_CS1_SOD_REG       PIOB_SODR
#elif SPPI0_CS1_PIO_ID == PIOC_ID
#define SPPI0_CS1_PE_REG        PIOC_PER
#define SPPI0_CS1_OE_REG        PIOC_OER
#define SPPI0_CS1_COD_REG       PIOC_CODR
#define SPPI0_CS1_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SPPI0 chip select output. */
#define SPPI0_CS1_ENA() \
    outr(SPPI0_CS1_PE_REG, _BV(SPPI0_CS1_BIT)); \
    outr(SPPI0_CS1_OE_REG, _BV(SPPI0_CS1_BIT))
/*! \brief Set SPPI0 chip select output low. */
#define SPPI0_CS1_CLR()   outr(SPPI0_CS1_COD_REG, _BV(SPPI0_CS1_BIT))
/*! \brief Set SPPI0 chip select output high. */
#define SPPI0_CS1_SET()   outr(SPPI0_CS1_SOD_REG, _BV(SPPI0_CS1_BIT))

#endif                          /* SPPI0_CS1_BIT */

#ifdef SPPI0_CS2_BIT

#if !defined(SPPI0_CS2_PIO_ID)
#define SPPI0_CS2_PE_REG        PIO_PER
#define SPPI0_CS2_OE_REG        PIO_OER
#define SPPI0_CS2_COD_REG       PIO_CODR
#define SPPI0_CS2_SOD_REG       PIO_SODR
#elif SPPI0_CS2_PIO_ID == PIO_ID
#define SPPI0_CS2_PE_REG        PIO_PER
#define SPPI0_CS2_OE_REG        PIO_OER
#define SPPI0_CS2_COD_REG       PIO_CODR
#define SPPI0_CS2_SOD_REG       PIO_SODR
#elif SPPI0_CS2_PIO_ID == PIOA_ID
#define SPPI0_CS2_PE_REG        PIOA_PER
#define SPPI0_CS2_OE_REG        PIOA_OER
#define SPPI0_CS2_COD_REG       PIOA_CODR
#define SPPI0_CS2_SOD_REG       PIOA_SODR
#elif SPPI0_CS2_PIO_ID == PIOB_ID
#define SPPI0_CS2_PE_REG        PIOB_PER
#define SPPI0_CS2_OE_REG        PIOB_OER
#define SPPI0_CS2_COD_REG       PIOB_CODR
#define SPPI0_CS2_SOD_REG       PIOB_SODR
#elif SPPI0_CS2_PIO_ID == PIOC_ID
#define SPPI0_CS2_PE_REG        PIOC_PER
#define SPPI0_CS2_OE_REG        PIOC_OER
#define SPPI0_CS2_COD_REG       PIOC_CODR
#define SPPI0_CS2_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SPPI0 chip select output. */
#define SPPI0_CS2_ENA() \
    outr(SPPI0_CS2_PE_REG, _BV(SPPI0_CS2_BIT)); \
    outr(SPPI0_CS2_OE_REG, _BV(SPPI0_CS2_BIT))
/*! \brief Set SPPI0 chip select output low. */
#define SPPI0_CS2_CLR()   outr(SPPI0_CS2_COD_REG, _BV(SPPI0_CS2_BIT))
/*! \brief Set SPPI0 chip select output high. */
#define SPPI0_CS2_SET()   outr(SPPI0_CS2_SOD_REG, _BV(SPPI0_CS2_BIT))

#endif                          /* SPPI0_CS2_BIT */

#ifdef SPPI0_CS3_BIT

#if !defined(SPPI0_CS3_PIO_ID)
#define SPPI0_CS3_PE_REG        PIO_PER
#define SPPI0_CS3_OE_REG        PIO_OER
#define SPPI0_CS3_COD_REG       PIO_CODR
#define SPPI0_CS3_SOD_REG       PIO_SODR
#elif SPPI0_CS3_PIO_ID == PIO_ID
#define SPPI0_CS3_PE_REG        PIO_PER
#define SPPI0_CS3_OE_REG        PIO_OER
#define SPPI0_CS3_COD_REG       PIO_CODR
#define SPPI0_CS3_SOD_REG       PIO_SODR
#elif SPPI0_CS3_PIO_ID == PIOA_ID
#define SPPI0_CS3_PE_REG        PIOA_PER
#define SPPI0_CS3_OE_REG        PIOA_OER
#define SPPI0_CS3_COD_REG       PIOA_CODR
#define SPPI0_CS3_SOD_REG       PIOA_SODR
#elif SPPI0_CS3_PIO_ID == PIOB_ID
#define SPPI0_CS3_PE_REG        PIOB_PER
#define SPPI0_CS3_OE_REG        PIOB_OER
#define SPPI0_CS3_COD_REG       PIOB_CODR
#define SPPI0_CS3_SOD_REG       PIOB_SODR
#elif SPPI0_CS3_PIO_ID == PIOC_ID
#define SPPI0_CS3_PE_REG        PIOC_PER
#define SPPI0_CS3_OE_REG        PIOC_OER
#define SPPI0_CS3_COD_REG       PIOC_CODR
#define SPPI0_CS3_SOD_REG       PIOC_SODR
#endif

/*! \brief Enable SPPI0 chip select output. */
#define SPPI0_CS3_ENA() \
    outr(SPPI0_CS3_PE_REG, _BV(SPPI0_CS3_BIT)); \
    outr(SPPI0_CS3_OE_REG, _BV(SPPI0_CS3_BIT))
/*! \brief Set SPPI0 chip select output low. */
#define SPPI0_CS3_CLR()   outr(SPPI0_CS3_COD_REG, _BV(SPPI0_CS3_BIT))
/*! \brief Set SPPI0 chip select output high. */
#define SPPI0_CS3_SET()   outr(SPPI0_CS3_SOD_REG, _BV(SPPI0_CS3_BIT))

#endif                          /* SPPI0_CS3_BIT */

#ifdef SPPI0_RST0_BIT

#if !defined(SPPI0_RST0_PIO_ID)
#define SPPI0_RST0_PE_REG      PIO_PER
#define SPPI0_RST0_OE_REG      PIO_OER
#define SPPI0_RST0_COD_REG     PIO_CODR
#define SPPI0_RST0_SOD_REG     PIO_SODR
#elif SPPI0_RST0_PIO_ID == PIO_ID
#define SPPI0_RST0_PE_REG      PIO_PER
#define SPPI0_RST0_OE_REG      PIO_OER
#define SPPI0_RST0_COD_REG     PIO_CODR
#define SPPI0_RST0_SOD_REG     PIO_SODR
#elif SPPI0_RST0_PIO_ID == PIOA_ID
#define SPPI0_RST0_PE_REG      PIOA_PER
#define SPPI0_RST0_OE_REG      PIOA_OER
#define SPPI0_RST0_COD_REG     PIOA_CODR
#define SPPI0_RST0_SOD_REG     PIOA_SODR
#elif SPPI0_RST0_PIO_ID == PIOB_ID
#define SPPI0_RST0_PE_REG      PIOB_PER
#define SPPI0_RST0_OE_REG      PIOB_OER
#define SPPI0_RST0_COD_REG     PIOB_CODR
#define SPPI0_RST0_SOD_REG     PIOB_SODR
#elif SPPI0_RST0_PIO_ID == PIOC_ID
#define SPPI0_RST0_PE_REG      PIOC_PER
#define SPPI0_RST0_OE_REG      PIOC_OER
#define SPPI0_RST0_COD_REG     PIOC_CODR
#define SPPI0_RST0_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SPPI0 reset output. */
#define SPPI0_RST0_ENA() \
    outr(SPPI0_RST0_PE_REG, _BV(SPPI0_RST0_BIT)); \
    outr(SPPI0_RST0_OE_REG, _BV(SPPI0_RST0_BIT))
/*! \brief Set SPPI0 reset output low. */
#define SPPI0_RST0_CLR()   outr(SPPI0_RST0_COD_REG, _BV(SPPI0_RST0_BIT))
/*! \brief Set SPPI0 reset output high. */
#define SPPI0_RST0_SET()   outr(SPPI0_RST0_SOD_REG, _BV(SPPI0_RST0_BIT))

#endif                          /* SPPI0_RST0_BIT */

#ifdef SPPI0_RST1_BIT

#if !defined(SPPI0_RST1_PIO_ID)
#define SPPI0_RST1_PE_REG      PIO_PER
#define SPPI0_RST1_OE_REG      PIO_OER
#define SPPI0_RST1_COD_REG     PIO_CODR
#define SPPI0_RST1_SOD_REG     PIO_SODR
#elif SPPI0_RST1_PIO_ID == PIO_ID
#define SPPI0_RST1_PE_REG      PIO_PER
#define SPPI0_RST1_OE_REG      PIO_OER
#define SPPI0_RST1_COD_REG     PIO_CODR
#define SPPI0_RST1_SOD_REG     PIO_SODR
#elif SPPI0_RST1_PIO_ID == PIOA_ID
#define SPPI0_RST1_PE_REG      PIOA_PER
#define SPPI0_RST1_OE_REG      PIOA_OER
#define SPPI0_RST1_COD_REG     PIOA_CODR
#define SPPI0_RST1_SOD_REG     PIOA_SODR
#elif SPPI0_RST1_PIO_ID == PIOB_ID
#define SPPI0_RST1_PE_REG      PIOB_PER
#define SPPI0_RST1_OE_REG      PIOB_OER
#define SPPI0_RST1_COD_REG     PIOB_CODR
#define SPPI0_RST1_SOD_REG     PIOB_SODR
#elif SPPI0_RST1_PIO_ID == PIOC_ID
#define SPPI0_RST1_PE_REG      PIOC_PER
#define SPPI0_RST1_OE_REG      PIOC_OER
#define SPPI0_RST1_COD_REG     PIOC_CODR
#define SPPI0_RST1_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SPPI0 reset output. */
#define SPPI0_RST1_ENA() \
    outr(SPPI0_RST1_PE_REG, _BV(SPPI0_RST1_BIT)); \
    outr(SPPI0_RST1_OE_REG, _BV(SPPI0_RST1_BIT))
/*! \brief Set SPPI0 reset output low. */
#define SPPI0_RST1_CLR()   outr(SPPI0_RST1_COD_REG, _BV(SPPI0_RST1_BIT))
/*! \brief Set SPPI0 reset output high. */
#define SPPI0_RST1_SET()   outr(SPPI0_RST1_SOD_REG, _BV(SPPI0_RST1_BIT))

#endif                          /* SPPI0_RST1_BIT */

#ifdef SPPI0_RST2_BIT

#if !defined(SPPI0_RST2_PIO_ID)
#define SPPI0_RST2_PE_REG      PIO_PER
#define SPPI0_RST2_OE_REG      PIO_OER
#define SPPI0_RST2_COD_REG     PIO_CODR
#define SPPI0_RST2_SOD_REG     PIO_SODR
#elif SPPI0_RST2_PIO_ID == PIO_ID
#define SPPI0_RST2_PE_REG      PIO_PER
#define SPPI0_RST2_OE_REG      PIO_OER
#define SPPI0_RST2_COD_REG     PIO_CODR
#define SPPI0_RST2_SOD_REG     PIO_SODR
#elif SPPI0_RST2_PIO_ID == PIOA_ID
#define SPPI0_RST2_PE_REG      PIOA_PER
#define SPPI0_RST2_OE_REG      PIOA_OER
#define SPPI0_RST2_COD_REG     PIOA_CODR
#define SPPI0_RST2_SOD_REG     PIOA_SODR
#elif SPPI0_RST2_PIO_ID == PIOB_ID
#define SPPI0_RST2_PE_REG      PIOB_PER
#define SPPI0_RST2_OE_REG      PIOB_OER
#define SPPI0_RST2_COD_REG     PIOB_CODR
#define SPPI0_RST2_SOD_REG     PIOB_SODR
#elif SPPI0_RST2_PIO_ID == PIOC_ID
#define SPPI0_RST2_PE_REG      PIOC_PER
#define SPPI0_RST2_OE_REG      PIOC_OER
#define SPPI0_RST2_COD_REG     PIOC_CODR
#define SPPI0_RST2_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SPPI0 reset output. */
#define SPPI0_RST2_ENA() \
    outr(SPPI0_RST2_PE_REG, _BV(SPPI0_RST2_BIT)); \
    outr(SPPI0_RST2_OE_REG, _BV(SPPI0_RST2_BIT))
/*! \brief Set SPPI0 reset output low. */
#define SPPI0_RST2_CLR()   outr(SPPI0_RST2_COD_REG, _BV(SPPI0_RST2_BIT))
/*! \brief Set SPPI0 reset output high. */
#define SPPI0_RST2_SET()   outr(SPPI0_RST2_SOD_REG, _BV(SPPI0_RST2_BIT))

#endif                          /* SPPI0_RST2_BIT */

#ifdef SPPI0_RST3_BIT

#if !defined(SPPI0_RST3_PIO_ID)
#define SPPI0_RST3_PE_REG      PIO_PER
#define SPPI0_RST3_OE_REG      PIO_OER
#define SPPI0_RST3_COD_REG     PIO_CODR
#define SPPI0_RST3_SOD_REG     PIO_SODR
#elif SPPI0_RST3_PIO_ID == PIO_ID
#define SPPI0_RST3_PE_REG      PIO_PER
#define SPPI0_RST3_OE_REG      PIO_OER
#define SPPI0_RST3_COD_REG     PIO_CODR
#define SPPI0_RST3_SOD_REG     PIO_SODR
#elif SPPI0_RST3_PIO_ID == PIOA_ID
#define SPPI0_RST3_PE_REG      PIOA_PER
#define SPPI0_RST3_OE_REG      PIOA_OER
#define SPPI0_RST3_COD_REG     PIOA_CODR
#define SPPI0_RST3_SOD_REG     PIOA_SODR
#elif SPPI0_RST3_PIO_ID == PIOB_ID
#define SPPI0_RST3_PE_REG      PIOB_PER
#define SPPI0_RST3_OE_REG      PIOB_OER
#define SPPI0_RST3_COD_REG     PIOB_CODR
#define SPPI0_RST3_SOD_REG     PIOB_SODR
#elif SPPI0_RST3_PIO_ID == PIOC_ID
#define SPPI0_RST3_PE_REG      PIOC_PER
#define SPPI0_RST3_OE_REG      PIOC_OER
#define SPPI0_RST3_COD_REG     PIOC_CODR
#define SPPI0_RST3_SOD_REG     PIOC_SODR
#endif

/*! \brief Enable SPPI0 reset output. */
#define SPPI0_RST3_ENA() \
    outr(SPPI0_RST3_PE_REG, _BV(SPPI0_RST3_BIT)); \
    outr(SPPI0_RST3_OE_REG, _BV(SPPI0_RST3_BIT))
/*! \brief Set SPPI0 reset output low. */
#define SPPI0_RST3_CLR()   outr(SPPI0_RST3_COD_REG, _BV(SPPI0_RST3_BIT))
/*! \brief Set SPPI0 reset output high. */
#define SPPI0_RST3_SET()   outr(SPPI0_RST3_SOD_REG, _BV(SPPI0_RST3_BIT))

#endif                          /* SPPI0_RST3_BIT */

#endif                          /* MCU */

__BEGIN_DECLS
/* Function prototypes */

extern int Sppi0SetMode(ureg_t ix, ureg_t mode);
extern void Sppi0SetSpeed(ureg_t ix, uint32_t rate);
extern void Sppi0Enable(ureg_t ix);
extern void Sppi0ChipReset(ureg_t ix, uint8_t hi);
extern void Sppi0ChipSelect(ureg_t ix, uint8_t hi);
extern void Sppi0SelectDevice(ureg_t ix);
extern void Sppi0DeselectDevice(ureg_t ix);
extern void Sppi0NegSelectDevice(ureg_t ix);
extern void Sppi0NegDeselectDevice(ureg_t ix);
extern uint8_t Sppi0Byte(uint8_t data);
extern void Sppi0Transact(CONST void *wdata, void *rdata, size_t len);

__END_DECLS
/* End of prototypes */

#endif
