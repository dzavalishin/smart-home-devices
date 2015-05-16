/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 * -
 *
 * This software has been inspired by all the valuable work done by
 * Jesper Hansen and Pavel Chromy. Many thanks for all their help.
 */

/*
 * $Log$
 * Revision 1.6  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/10/23 08:54:07  haraldkipp
 * Include the correct header file.
 *
 * Revision 1.4  2008/09/18 09:51:58  haraldkipp
 * Use the correct PORT macros.
 *
 * Revision 1.3  2008/09/02 14:30:28  haraldkipp
 * Make it compile for targets without specific configuration.
 *
 * Revision 1.2  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 08:59:55  haraldkipp
 * VS10XX decoder support added.
 *
 */

#include <cfg/arch/gpio.h>
#include <cfg/audio.h>

#include <dev/irqreg.h>
#include <dev/vs10xx.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/bankmem.h>

#include <stddef.h>

#ifndef INT0
#define INT0    0
#define INT1    1
#define INT2    2
#define INT3    3
#define INT4    4
#define INT5    5
#define INT6    6
#define INT7    7
#endif

/*!
 * \addtogroup xgVs10xx
 */
/*@{*/

#if !defined(AUDIO_VS1001K) && !defined(AUDIO_VS1011E) && !defined(AUDIO_VS1002D) && !defined(AUDIO_VS1003B) && !defined(AUDIO_VS1033C) && !defined(AUDIO_VS1053B)
#define AUDIO_VS1001K
#endif

#ifndef VS10XX_FREQ
/*! \brief Decoder crystal frequency. */
#define VS10XX_FREQ             12288000UL
#endif

#ifndef VS10XX_HWRST_DURATION
/*! \brief Minimum time in milliseconds to held hardware reset low. */
#define VS10XX_HWRST_DURATION   1
#endif

#ifndef VS10XX_HWRST_RECOVER
/*! \brief Milliseconds to wait after hardware reset. */
#define VS10XX_HWRST_RECOVER    10
#endif

#ifndef VS10XX_SWRST_RECOVER
/*! \brief Milliseconds to wait after software reset. */
#define VS10XX_SWRST_RECOVER    VS10XX_HWRST_RECOVER
#endif

#ifndef VS10XX_SCI_MODE
#define VS10XX_SCI_MODE         0
#endif

#ifndef VS10XX_SCI_RATE
#define VS10XX_SCI_RATE         (VS10XX_FREQ / 4)
#endif

#ifndef VS10XX_SDI_MODE
#define VS10XX_SDI_MODE         0
#endif

#ifndef VS10XX_SDI_RATE
#define VS10XX_SDI_RATE         (VS10XX_FREQ / 4)
#endif

#if defined(VS10XX_SCI_SPI0_DEVICE) /* Command SPI device. */

#include <dev/sppif0.h>
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciReset(act)       Sppi0ChipReset(VS10XX_SCI_SPI0_DEVICE, act)
#else
#define SciReset(act)       Sppi0ChipReset(VS10XX_SCI_SPI0_DEVICE, !act)
#endif
#define SciSetMode()        Sppi0SetMode(VS10XX_SCI_SPI0_DEVICE, VS10XX_SCI_MODE)
#define SciSetSpeed()       Sppi0SetSpeed(VS10XX_SCI_SPI0_DEVICE, VS10XX_SCI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciSelect()         Sppi0SelectDevice(VS10XX_SCI_SPI0_DEVICE)
#define SciDeselect()       Sppi0DeselectDevice(VS10XX_SCI_SPI0_DEVICE)
#else
#define SciSelect()         Sppi0NegSelectDevice(VS10XX_SCI_SPI0_DEVICE)
#define SciDeselect()       Sppi0NegDeselectDevice(VS10XX_SCI_SPI0_DEVICE)
#endif
#define SciByte(b)          Sppi0Byte(b)

#elif defined(VS10XX_SCI_SBBI0_DEVICE) /* Command SPI device. */

#include <dev/sbbif0.h>
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciReset(act)       Sbbi0ChipReset(VS10XX_SCI_SBBI0_DEVICE, act)
#else
#define SciReset(act)       Sbbi0ChipReset(VS10XX_SCI_SBBI0_DEVICE, !act)
#endif
#define SciSetMode()        Sbbi0SetMode(VS10XX_SCI_SBBI0_DEVICE, VS10XX_SCI_MODE)
#define SciSetSpeed()       Sbbi0SetSpeed(VS10XX_SCI_SBBI0_DEVICE, VS10XX_SCI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciSelect()         Sbbi0SelectDevice(VS10XX_SCI_SBBI0_DEVICE)
#define SciDeselect()       Sbbi0DeselectDevice(VS10XX_SCI_SBBI0_DEVICE)
#else
#define SciSelect()         Sbbi0NegSelectDevice(VS10XX_SCI_SBBI0_DEVICE)
#define SciDeselect()       Sbbi0NegDeselectDevice(VS10XX_SCI_SBBI0_DEVICE)
#endif
#define SciByte(b)          Sbbi0Byte(b)

#elif defined(VS10XX_SCI_SBBI1_DEVICE) /* Command SPI device. */

#include <dev/sbbif1.h>
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciReset(act)       Sbbi1ChipReset(VS10XX_SCI_SBBI1_DEVICE, act)
#else
#define SciReset(act)       Sbbi1ChipReset(VS10XX_SCI_SBBI1_DEVICE, !act)
#endif
#define SciSetMode()        Sbbi1SetMode(VS10XX_SCI_SBBI1_DEVICE, VS10XX_SCI_MODE)
#define SciSetSpeed()       Sbbi1SetSpeed(VS10XX_SCI_SBBI1_DEVICE, VS10XX_SCI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciSelect()         Sbbi1SelectDevice(VS10XX_SCI_SBBI1_DEVICE)
#define SciDeselect()       Sbbi1DeselectDevice(VS10XX_SCI_SBBI1_DEVICE)
#else
#define SciSelect()         Sbbi1NegSelectDevice(VS10XX_SCI_SBBI1_DEVICE)
#define SciDeselect()       Sbbi1NegDeselectDevice(VS10XX_SCI_SBBI1_DEVICE)
#endif
#define SciByte(b)          Sbbi1Byte(b)

#elif defined(VS10XX_SCI_SBBI2_DEVICE) /* Command SPI device. */

#include <dev/sbbif2.h>
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciReset(act)       Sbbi2ChipReset(VS10XX_SCI_SBBI2_DEVICE, act)
#else
#define SciReset(act)       Sbbi2ChipReset(VS10XX_SCI_SBBI2_DEVICE, !act)
#endif
#define SciSetMode()        Sbbi2SetMode(VS10XX_SCI_SBBI2_DEVICE, VS10XX_SCI_MODE)
#define SciSetSpeed()       Sbbi2SetSpeed(VS10XX_SCI_SBBI2_DEVICE, VS10XX_SCI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciSelect()         Sbbi2SelectDevice(VS10XX_SCI_SBBI2_DEVICE)
#define SciDeselect()       Sbbi2DeselectDevice(VS10XX_SCI_SBBI2_DEVICE)
#else
#define SciSelect()         Sbbi2NegSelectDevice(VS10XX_SCI_SBBI2_DEVICE)
#define SciDeselect()       Sbbi2NegDeselectDevice(VS10XX_SCI_SBBI2_DEVICE)
#endif
#define SciByte(b)          Sbbi2Byte(b)

#elif defined(VS10XX_SCI_SBBI3_DEVICE) /* Command SPI device. */

#include <dev/sbbif3.h>
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciReset(act)       Sbbi3ChipReset(VS10XX_SCI_SBBI3_DEVICE, act)
#else
#define SciReset(act)       Sbbi3ChipReset(VS10XX_SCI_SBBI3_DEVICE, !act)
#endif
#define SciSetMode()        Sbbi3SetMode(VS10XX_SCI_SBBI3_DEVICE, VS10XX_SCI_MODE)
#define SciSetSpeed()       Sbbi3SetSpeed(VS10XX_SCI_SBBI3_DEVICE, VS10XX_SCI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SciSelect()         Sbbi3SelectDevice(VS10XX_SCI_SBBI3_DEVICE)
#define SciDeselect()       Sbbi3DeselectDevice(VS10XX_SCI_SBBI3_DEVICE)
#else
#define SciSelect()         Sbbi3NegSelectDevice(VS10XX_SCI_SBBI3_DEVICE)
#define SciDeselect()       Sbbi3NegDeselectDevice(VS10XX_SCI_SBBI3_DEVICE)
#endif
#define SciByte(b)          Sbbi3Byte(b)

#else /* Command SPI device. */

#define SciReset(act)
#define SciSetMode()        (-1)
#define SciSetSpeed()
#define SciSelect()
#define SciDeselect()
#define SciByte(b)          0

#endif /* Command SPI device. */


#if defined(VS10XX_SDI_SPI0_DEVICE) /* Data SPI device. */

#include <dev/sppif0.h>
#define SdiSetMode()        Sppi0SetMode(VS10XX_SDI_SPI0_DEVICE, VS10XX_SDI_MODE)
#define SdiSetSpeed()       Sppi0SetSpeed(VS10XX_SDI_SPI0_DEVICE, VS10XX_SDI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SdiSelect()         Sppi0SelectDevice(VS10XX_SDI_SPI0_DEVICE)
#define SdiDeselect()       Sppi0DeselectDevice(VS10XX_SDI_SPI0_DEVICE)
#else
#define SdiSelect()         Sppi0NegSelectDevice(VS10XX_SDI_SPI0_DEVICE)
#define SdiDeselect()       Sppi0NegDeselectDevice(VS10XX_SDI_SPI0_DEVICE)
#endif
#define SdiByte(b)          Sppi0Byte(b)

#elif defined(VS10XX_SDI_SBBI0_DEVICE) /* Data SPI device. */

#include <dev/sbbif0.h>
#define SdiSetMode()        Sbbi0SetMode(VS10XX_SDI_SBBI0_DEVICE, VS10XX_SDI_MODE)
#define SdiSetSpeed()       Sbbi0SetSpeed(VS10XX_SDI_SBBI0_DEVICE, VS10XX_SDI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SdiSelect()         Sbbi0SelectDevice(VS10XX_SDI_SBBI0_DEVICE)
#define SdiDeselect()       Sbbi0DeselectDevice(VS10XX_SDI_SBBI0_DEVICE)
#else
#define SdiSelect()         Sbbi0NegSelectDevice(VS10XX_SDI_SBBI0_DEVICE)
#define SdiDeselect()       Sbbi0NegDeselectDevice(VS10XX_SDI_SBBI0_DEVICE)
#endif
#define SdiByte(b)          Sbbi0Byte(b)

#elif defined(VS10XX_SDI_SBBI1_DEVICE) /* Data SPI device. */

#include <dev/sbbif1.h>
#define SdiSetMode()        Sbbi1SetMode(VS10XX_SDI_SBBI1_DEVICE, VS10XX_SDI_MODE)
#define SdiSetSpeed()       Sbbi1SetSpeed(VS10XX_SDI_SBBI1_DEVICE, VS10XX_SDI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SdiSelect()         Sbbi1SelectDevice(VS10XX_SDI_SBBI1_DEVICE)
#define SdiDeselect()       Sbbi1DeselectDevice(VS10XX_SDI_SBBI1_DEVICE)
#else
#define SdiSelect()         Sbbi1NegSelectDevice(VS10XX_SDI_SBBI1_DEVICE)
#define SdiDeselect()       Sbbi1NegDeselectDevice(VS10XX_SDI_SBBI1_DEVICE)
#endif
#define SdiByte(b)          Sbbi1Byte(b)

#elif defined(VS10XX_SDI_SBBI2_DEVICE) /* Data SPI device. */

#include <dev/sbbif2.h>
#define SdiSetMode()        Sbbi2SetMode(VS10XX_SDI_SBBI2_DEVICE, VS10XX_SDI_MODE)
#define SdiSetSpeed()       Sbbi2SetSpeed(VS10XX_SDI_SBBI2_DEVICE, VS10XX_SDI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SdiSelect()         Sbbi2SelectDevice(VS10XX_SDI_SBBI2_DEVICE)
#define SdiDeselect()       Sbbi2DeselectDevice(VS10XX_SDI_SBBI2_DEVICE)
#else
#define SdiSelect()         Sbbi2NegSelectDevice(VS10XX_SDI_SBBI2_DEVICE)
#define SdiDeselect()       Sbbi2NegDeselectDevice(VS10XX_SDI_SBBI2_DEVICE)
#endif
#define SdiByte(b)          Sbbi2Byte(b)

#elif defined(VS10XX_SDI_SBBI3_DEVICE) /* Data SPI device. */

#include <dev/sbbif3.h>
#define SdiSetMode()        Sbbi3SetMode(VS10XX_SDI_SBBI3_DEVICE, VS10XX_SDI_MODE)
#define SdiSetSpeed()       Sbbi3SetSpeed(VS10XX_SDI_SBBI3_DEVICE, VS10XX_SDI_RATE)
#if defined(VS10XX_SELECT_ACTIVE_HIGH)
#define SdiSelect()         Sbbi3SelectDevice(VS10XX_SDI_SBBI3_DEVICE)
#define SdiDeselect()       Sbbi3DeselectDevice(VS10XX_SDI_SBBI3_DEVICE)
#else
#define SdiSelect()         Sbbi3NegSelectDevice(VS10XX_SDI_SBBI3_DEVICE)
#define SdiDeselect()       Sbbi3NegDeselectDevice(VS10XX_SDI_SBBI3_DEVICE)
#endif
#define SdiByte(b)          Sbbi3Byte(b)

#else /* Data SPI device. */

#define SdiSetMode()        (-1)
#define SdiSetSpeed()
#define SdiSelect()
#define SdiDeselect()
#define SdiByte(b)          0

#endif /* Data SPI device. */

/* -------------------------------------------------
 * AT91 port specifications.
 */
#if MCU_AT91 || __AVR32__

#if (VS10XX_SIGNAL_IRQ == INT0)
#define VS10XX_SIGNAL       sig_INTERRUPT0
#elif (VS10XX_SIGNAL_IRQ == INT1)
#define VS10XX_SIGNAL       sig_INTERRUPT1
#else
#define VS10XX_SIGNAL       sig_INTERRUPT2
#endif

#if defined(VS10XX_XCS_BIT)

#if !defined(VS10XX_XCS_PORT)
#define VS10XX_XCS_PE_REG        PIO_PER
#define VS10XX_XCS_OE_REG        PIO_OER
#define VS10XX_XCS_COD_REG       PIO_CODR
#define VS10XX_XCS_SOD_REG       PIO_SODR
#elif VS10XX_XCS_PORT == PIOA_ID
#define VS10XX_XCS_PE_REG        PIOA_PER
#define VS10XX_XCS_OE_REG        PIOA_OER
#define VS10XX_XCS_COD_REG       PIOA_CODR
#define VS10XX_XCS_SOD_REG       PIOA_SODR
#elif VS10XX_XCS_PORT == PIOB_ID
#define VS10XX_XCS_PE_REG        PIOB_PER
#define VS10XX_XCS_OE_REG        PIOB_OER
#define VS10XX_XCS_COD_REG       PIOB_CODR
#define VS10XX_XCS_SOD_REG       PIOB_SODR
#elif VS10XX_XCS_PORT == PIOC_ID
#define VS10XX_XCS_PE_REG        PIOC_PER
#define VS10XX_XCS_OE_REG        PIOC_OER
#define VS10XX_XCS_COD_REG       PIOC_CODR
#define VS10XX_XCS_SOD_REG       PIOC_SODR
#endif
#define VS10XX_XCS_ENA() \
    outr(VS10XX_XCS_PE_REG, _BV(VS10XX_XCS_BIT)); \
    outr(VS10XX_XCS_OE_REG, _BV(VS10XX_XCS_BIT))
#define VS10XX_XCS_CLR()   outr(VS10XX_XCS_COD_REG, _BV(VS10XX_XCS_BIT))
#define VS10XX_XCS_SET()   outr(VS10XX_XCS_SOD_REG, _BV(VS10XX_XCS_BIT))

#else /* VS10XX_XCS_BIT */

#define VS10XX_XCS_ENA()
#define VS10XX_XCS_CLR()
#define VS10XX_XCS_SET()

#endif /* VS10XX_XCS_BIT */

#if defined(VS10XX_XDCS_BIT)

#if !defined(VS10XX_XDCS_PORT)
#define VS10XX_XDCS_PE_REG        PIO_PER
#define VS10XX_XDCS_OE_REG        PIO_OER
#define VS10XX_XDCS_COD_REG       PIO_CODR
#define VS10XX_XDCS_SOD_REG       PIO_SODR
#elif VS10XX_XDCS_PORT == PIOA_ID
#define VS10XX_XDCS_PE_REG        PIOA_PER
#define VS10XX_XDCS_OE_REG        PIOA_OER
#define VS10XX_XDCS_COD_REG       PIOA_CODR
#define VS10XX_XDCS_SOD_REG       PIOA_SODR
#elif VS10XX_XDCS_PORT == PIOB_ID
#define VS10XX_XDCS_PE_REG        PIOB_PER
#define VS10XX_XDCS_OE_REG        PIOB_OER
#define VS10XX_XDCS_COD_REG       PIOB_CODR
#define VS10XX_XDCS_SOD_REG       PIOB_SODR
#elif VS10XX_XDCS_PORT == PIOC_ID
#define VS10XX_XDCS_PE_REG        PIOC_PER
#define VS10XX_XDCS_OE_REG        PIOC_OER
#define VS10XX_XDCS_COD_REG       PIOC_CODR
#define VS10XX_XDCS_SOD_REG       PIOC_SODR
#endif
#define VS10XX_XDCS_ENA() \
    outr(VS10XX_XDCS_PE_REG, _BV(VS10XX_XDCS_BIT)); \
    outr(VS10XX_XDCS_OE_REG, _BV(VS10XX_XDCS_BIT))
#define VS10XX_XDCS_CLR()   outr(VS10XX_XDCS_COD_REG, _BV(VS10XX_XDCS_BIT))
#define VS10XX_XDCS_SET()   outr(VS10XX_XDCS_SOD_REG, _BV(VS10XX_XDCS_BIT))

#else /* VS10XX_XDCS_BIT */

#define VS10XX_XDCS_ENA()
#define VS10XX_XDCS_CLR()
#define VS10XX_XDCS_SET()

#endif /* VS10XX_XDCS_BIT */

#if defined(VS10XX_DREQ_BIT)

#if !defined(VS10XX_DREQ_PIO_ID)
#define VS10XX_DREQ_PE_REG       PIO_PER
#define VS10XX_DREQ_OD_REG       PIO_ODR
#define VS10XX_DREQ_PDS_REG      PIO_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOA_ID
#define VS10XX_DREQ_PE_REG       PIOA_PER
#define VS10XX_DREQ_OD_REG       PIOA_ODR
#define VS10XX_DREQ_PDS_REG      PIOA_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOB_ID
#define VS10XX_DREQ_PE_REG       PIOB_PER
#define VS10XX_DREQ_OD_REG       PIOB_ODR
#define VS10XX_DREQ_PDS_REG      PIOB_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOC_ID
#define VS10XX_DREQ_PE_REG       PIOC_PER
#define VS10XX_DREQ_OD_REG       PIOC_ODR
#define VS10XX_DREQ_PDS_REG      PIOC_PDSR
#endif

#define VS10XX_DREQ_PD_REG      PIO_PDR
#define VS10XX_DREQ_OD_REG      PIO_ODR
#define VS10XX_DREQ_PDS_REG     PIO_PDSR
#define VS10XX_DREQ_ENA() \
    outr(VS10XX_DREQ_PD_REG, _BV(VS10XX_DREQ_BIT)); \
    outr(VS10XX_DREQ_OD_REG, _BV(VS10XX_DREQ_BIT))
#define VS10XX_DREQ_TST()    ((inr(VS10XX_DREQ_PDS_REG) & _BV(VS10XX_DREQ_BIT)) == _BV(VS10XX_DREQ_BIT))

#else /* VS10XX_DREQ_BIT */

#define VS10XX_DREQ_ENA()
#define VS10XX_DREQ_TST()   0

#endif /* VS10XX_DREQ_BIT */

#ifdef VS10XX_BSYNC_BIT

#if !defined(VS10XX_BSYNC_PIO_ID)
#define VS10XX_BSYNC_PE_REG        PIO_PER
#define VS10XX_BSYNC_OE_REG        PIO_OER
#define VS10XX_BSYNC_COD_REG       PIO_CODR
#define VS10XX_BSYNC_SOD_REG       PIO_SODR
#elif VS10XX_BSYNC_PIO_ID == PIOA_ID
#define VS10XX_BSYNC_PE_REG        PIOA_PER
#define VS10XX_BSYNC_OE_REG        PIOA_OER
#define VS10XX_BSYNC_COD_REG       PIOA_CODR
#define VS10XX_BSYNC_SOD_REG       PIOA_SODR
#elif VS10XX_BSYNC_PIO_ID == PIOB_ID
#define VS10XX_BSYNC_PE_REG        PIOB_PER
#define VS10XX_BSYNC_OE_REG        PIOB_OER
#define VS10XX_BSYNC_COD_REG       PIOB_CODR
#define VS10XX_BSYNC_SOD_REG       PIOB_SODR
#elif VS10XX_BSYNC_PIO_ID == PIOC_ID
#define VS10XX_BSYNC_PE_REG        PIOC_PER
#define VS10XX_BSYNC_OE_REG        PIOC_OER
#define VS10XX_BSYNC_COD_REG       PIOC_CODR
#define VS10XX_BSYNC_SOD_REG       PIOC_SODR
#endif
#define VS10XX_BSYNC_ENA() \
    outr(VS10XX_BSYNC_PE_REG, _BV(VS10XX_BSYNC_BIT)); \
    outr(VS10XX_BSYNC_OE_REG, _BV(VS10XX_BSYNC_BIT))
#define VS10XX_BSYNC_CLR()   outr(VS10XX_BSYNC_COD_REG, _BV(VS10XX_BSYNC_BIT))
#define VS10XX_BSYNC_SET()   outr(VS10XX_BSYNC_SOD_REG, _BV(VS10XX_BSYNC_BIT))

#else /* VS10XX_BSYNC_BIT */

#define VS10XX_BSYNC_ENA()
#define VS10XX_BSYNC_CLR()
#define VS10XX_BSYNC_SET()

#endif /* VS10XX_BSYNC_BIT */


/* -------------------------------------------------
 * AVR port specifications.
 */
#elif defined(__AVR__)

#if !defined(VS10XX_SIGNAL_IRQ)
#define VS10XX_SIGNAL_IRQ   INT6
#endif

#if (VS10XX_SIGNAL_IRQ == INT0)
#define VS10XX_SIGNAL       sig_INTERRUPT0
#define VS10XX_DREQ_BIT     0
#define VS10XX_DREQ_PDS_REG PIND
#define VS10XX_DREQ_PUE_REG PORTD
#define VS10XX_DREQ_OE_REG  DDRD

#elif (VS10XX_SIGNAL_IRQ == INT1)
#define VS10XX_SIGNAL       sig_INTERRUPT1
#define VS10XX_DREQ_BIT     1
#define VS10XX_DREQ_PDS_REG PIND
#define VS10XX_DREQ_PUE_REG PORTD
#define VS10XX_DREQ_OE_REG  DDRD

#elif (VS10XX_SIGNAL_IRQ == INT2)
#define VS10XX_SIGNAL       sig_INTERRUPT2
#define VS10XX_DREQ_BIT     2
#define VS10XX_DREQ_PDS_REG PIND
#define VS10XX_DREQ_PUE_REG PORTD
#define VS10XX_DREQ_OE_REG  DDRD

#elif (VS10XX_SIGNAL_IRQ == INT3)
#define VS10XX_SIGNAL       sig_INTERRUPT3
#define VS10XX_DREQ_BIT     3
#define VS10XX_DREQ_PDS_REG PIND
#define VS10XX_DREQ_PUE_REG PORTD
#define VS10XX_DREQ_OE_REG  DDRD

#elif (VS10XX_SIGNAL_IRQ == INT4)
#define VS10XX_SIGNAL       sig_INTERRUPT4
#define VS10XX_DREQ_BIT     4
#define VS10XX_DREQ_PDS_REG PINE
#define VS10XX_DREQ_PUE_REG PORTE
#define VS10XX_DREQ_OE_REG  DDRE

#elif (VS10XX_SIGNAL_IRQ == INT5)
#define VS10XX_SIGNAL       sig_INTERRUPT5
#define VS10XX_DREQ_BIT     5
#define VS10XX_DREQ_PDS_REG PINE
#define VS10XX_DREQ_PUE_REG PORTE
#define VS10XX_DREQ_OE_REG  DDRE

#elif (VS10XX_SIGNAL_IRQ == INT7)
#define VS10XX_SIGNAL       sig_INTERRUPT7
#define VS10XX_DREQ_BIT     7
#define VS10XX_DREQ_PDS_REG PINE
#define VS10XX_DREQ_PUE_REG PORTE
#define VS10XX_DREQ_OE_REG  DDRE

#else
#define VS10XX_SIGNAL       sig_INTERRUPT6
#define VS10XX_DREQ_BIT     6
#define VS10XX_DREQ_PDS_REG PINE
#define VS10XX_DREQ_PUE_REG PORTE
#define VS10XX_DREQ_OE_REG  DDRE

#endif

#define VS10XX_DREQ_ENA() \
    cbi(VS10XX_DREQ_OE_REG, VS10XX_DREQ_BIT); \
    sbi(VS10XX_DREQ_PUE_REG, VS10XX_DREQ_BIT)
#define VS10XX_DREQ_TST()    ((inb(VS10XX_DREQ_PDS_REG) & _BV(VS10XX_DREQ_BIT)) == _BV(VS10XX_DREQ_BIT))


#if defined(VS10XX_XCS_BIT)

#if (VS10XX_XCS_PORT == AVRPORTA)
#define VS10XX_XCS_SOD_REG  PORTA
#define VS10XX_XCS_OE_REG   DDRA
#elif (VS10XX_XCS_PORT == AVRPORTB)
#define VS10XX_XCS_SOD_REG  PORTB
#define VS10XX_XCS_OE_REG   DDRB
#elif (VS10XX_XCS_PORT == AVRPORTD)
#define VS10XX_XCS_SOD_REG  PORTD
#define VS10XX_XCS_OE_REG   DDRD
#elif (VS10XX_XCS_PORT == AVRPORTE)
#define VS10XX_XCS_SOD_REG  PORTE
#define VS10XX_XCS_OE_REG   DDRE
#elif (VS10XX_XCS_PORT == AVRPORTF)
#define VS10XX_XCS_SOD_REG  PORTF
#define VS10XX_XCS_OE_REG   DDRF
#elif (VS10XX_XCS_PORT == AVRPORTG)
#define VS10XX_XCS_SOD_REG  PORTG
#define VS10XX_XCS_OE_REG   DDRG
#elif (VS10XX_XCS_PORT == AVRPORTH)
#define VS10XX_XCS_SOD_REG  PORTH
#define VS10XX_XCS_OE_REG   DDRH
#endif
#define VS10XX_XCS_ENA()      sbi(VS10XX_XCS_OE_REG, VS10XX_XCS_BIT)
#define VS10XX_XCS_CLR()      cbi(VS10XX_XCS_SOD_REG, VS10XX_XCS_BIT)
#define VS10XX_XCS_SET()      sbi(VS10XX_XCS_SOD_REG, VS10XX_XCS_BIT)

#else /* VS10XX_XCS_BIT */

#define VS10XX_XCS_ENA()
#define VS10XX_XCS_CLR()
#define VS10XX_XCS_SET()

#endif /* VS10XX_XCS_BIT */

#if defined(VS10XX_XDCS_BIT)

#if (VS10XX_XDCS_PORT == AVRPORTA)
#define VS10XX_XDCS_SOD_REG  PORTA
#define VS10XX_XDCS_OE_REG   DDRA
#elif (VS10XX_XDCS_PORT == AVRPORTB)
#define VS10XX_XDCS_SOD_REG  PORTB
#define VS10XX_XDCS_OE_REG   DDRB
#elif (VS10XX_XDCS_PORT == AVRPORTD)
#define VS10XX_XDCS_SOD_REG  PORTD
#define VS10XX_XDCS_OE_REG   DDRD
#elif (VS10XX_XDCS_PORT == AVRPORTE)
#define VS10XX_XDCS_SOD_REG  PORTE
#define VS10XX_XDCS_OE_REG   DDRE
#elif (VS10XX_XDCS_PORT == AVRPORTF)
#define VS10XX_XDCS_SOD_REG  PORTF
#define VS10XX_XDCS_OE_REG   DDRF
#elif (VS10XX_XDCS_PORT == AVRPORTG)
#define VS10XX_XDCS_SOD_REG  PORTG
#define VS10XX_XDCS_OE_REG   DDRG
#elif (VS10XX_XDCS_PORT == AVRPORTH)
#define VS10XX_XDCS_SOD_REG  PORTH
#define VS10XX_XDCS_OE_REG   DDRH
#endif
#define VS10XX_XDCS_ENA()      sbi(VS10XX_XDCS_OE_REG, VS10XX_XDCS_BIT)
#define VS10XX_XDCS_CLR()      cbi(VS10XX_XDCS_SOD_REG, VS10XX_XDCS_BIT)
#define VS10XX_XDCS_SET()      sbi(VS10XX_XDCS_SOD_REG, VS10XX_XDCS_BIT)

#else /* VS10XX_XDCS_BIT */

#define VS10XX_XDCS_ENA()
#define VS10XX_XDCS_CLR()
#define VS10XX_XDCS_SET()

#endif /* VS10XX_XDCS_BIT */


#if defined(VS10XX_BSYNC_BIT)

#if (VS10XX_BSYNC_AVRPORT == AVRPORTA)
#define VS10XX_BSYNC_SOD_REG  PORTA
#define VS10XX_BSYNC_OE_REG   DDRA
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTB)
#define VS10XX_BSYNC_SOD_REG  PORTB
#define VS10XX_BSYNC_OE_REG   DDRB
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTD)
#define VS10XX_BSYNC_SOD_REG  PORTD
#define VS10XX_BSYNC_OE_REG   DDRD
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTE)
#define VS10XX_BSYNC_SOD_REG  PORTE
#define VS10XX_BSYNC_OE_REG   DDRE
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTF)
#define VS10XX_BSYNC_SOD_REG  PORTF
#define VS10XX_BSYNC_OE_REG   DDRF
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTG)
#define VS10XX_BSYNC_SOD_REG  PORTG
#define VS10XX_BSYNC_OE_REG   DDRG
#elif (VS10XX_BSYNC_AVRPORT == AVRPORTH)
#define VS10XX_BSYNC_SOD_REG  PORTH
#define VS10XX_BSYNC_OE_REG   DDRH
#endif
#define VS10XX_BSYNC_ENA()      sbi(VS10XX_BSYNC_OE_REG, VS10XX_BSYNC_BIT)
#define VS10XX_BSYNC_CLR()      cbi(VS10XX_BSYNC_SOD_REG, VS10XX_BSYNC_BIT)
#define VS10XX_BSYNC_SET()      sbi(VS10XX_BSYNC_SOD_REG, VS10XX_BSYNC_BIT)

#else /* VS10XX_BSYNC_BIT */

#define VS10XX_BSYNC_ENA()
#define VS10XX_BSYNC_CLR()
#define VS10XX_BSYNC_SET()

#endif /* VS10XX_BSYNC_BIT */


/* -------------------------------------------------
 * End of port specifications.
 */
#endif


static volatile ureg_t vs_status = VS_STATUS_STOPPED;
static volatile unsigned int vs_flush;

/*
 * \brief Write a byte to the VS10XX data interface.
 *
 * The caller is responsible for checking the DREQ line. Also make sure,
 * that decoder interrupts are disabled.
 *
 * \param b Byte to be shifted to the decoder's data interface.
 */
static INLINE void VsSdiPutByte(ureg_t b)
{
#if defined(VS10XX_BSYNC_BIT)

#if defined(VS10XX_SDI_SBBI0_DEVICE) /* VS10XX_SDI_DEVICE */
    ureg_t mask;

    VS10XX_BSYNC_SET();

    for (mask = 0x80; mask; mask >>= 1) {
        SBBI0_SCK_CLR();
        if (b & mask) {
            SBBI0_MOSI_SET();
        }
        else {
            SBBI0_MOSI_CLR();
        }
        SBBI0_SCK_SET();
        if (mask & 0x40) {
            VS10XX_BSYNC_CLR();
        }
    }
    SBBI0_SCK_CLR();
#elif defined(VS10XX_SDI_SBBI1_DEVICE) /* VS10XX_SDI_DEVICE */
    ureg_t mask;

    VS10XX_BSYNC_SET();

    for (mask = 0x80; mask; mask >>= 1) {
        SBBI1_SCK_CLR();
        if (b & mask) {
            SBBI1_MOSI_SET();
        }
        else {
            SBBI1_MOSI_CLR();
        }
        SBBI1_SCK_SET();
        if (mask & 0x40) {
            VS10XX_BSYNC_CLR();
        }
    }
    SBBI1_SCK_CLR();
#elif defined(VS10XX_SDI_SBBI2_DEVICE) /* VS10XX_SDI_DEVICE */
    ureg_t mask;

    VS10XX_BSYNC_SET();

    for (mask = 0x80; mask; mask >>= 1) {
        SBBI2_SCK_CLR();
        if (b & mask) {
            SBBI2_MOSI_SET();
        }
        else {
            SBBI2_MOSI_CLR();
        }
        SBBI2_SCK_SET();
        if (mask & 0x40) {
            VS10XX_BSYNC_CLR();
        }
    }
    SBBI2_SCK_CLR();
#elif defined(VS10XX_SDI_SBBI3_DEVICE) /* VS10XX_SDI_DEVICE */
    ureg_t mask;

    VS10XX_BSYNC_SET();

    for (mask = 0x80; mask; mask >>= 1) {
        SBBI3_SCK_CLR();
        if (b & mask) {
            SBBI3_MOSI_SET();
        }
        else {
            SBBI3_MOSI_CLR();
        }
        SBBI3_SCK_SET();
        if (mask & 0x40) {
            VS10XX_BSYNC_CLR();
        }
    }
    SBBI3_SCK_CLR();
#elif defined(VS10XX_SDI_SPI0_DEVICE) /* VS10XX_SDI_DEVICE */
    VS10XX_BSYNC_SET();

    outb(SPDR, b);

    _NOP();
    _NOP();
    _NOP();
    _NOP();
    VS10XX_BSYNC_CLR();

    /* Wait for SPI transfer to finish. */
    loop_until_bit_is_set(SPSR, SPIF);
#endif  /* VS10XX_SDI_DEVICE */

#else  /* !VS10XX_BSYNC_BIT */

    (void)SdiByte(b);

#endif /* !VS10XX_BSYNC_BIT */

}

/*!
 * \brief Enable or disable player interrupts.
 *
 * This routine is typically used by applications when dealing with 
 * unprotected buffers.
 *
 * \param enable Disables interrupts when zero. Otherwise interrupts
 *               are enabled.
 *
 * \return Zero if interrupts were disabled before this call.
 */
ureg_t VsPlayerInterrupts(ureg_t enable)
{
    static ureg_t is_enabled = 0;
    ureg_t rc;

    rc = is_enabled;
    if(enable) {
        VS10XX_DREQ_ENA();
        NutIrqEnable(&VS10XX_SIGNAL);
    }
    else {
        NutIrqDisable(&VS10XX_SIGNAL);
    }
    is_enabled = enable;

    return rc;
}

/*!
 * \brief Throttle decoder activity.
 *
 * When sharing SPI with other devices, this function should be called 
 * to disable (and re-enable) the SPI interface of the VS10XX.
 *
 * Decoder interrupts must have been disabled before calling this 
 * function.
 *
 * \code
 * #include <dev/vs10xx.h>
 *
 * ureg_t ief = VsPlayerInterrupts(0);
 * ureg_t tef = VsPlayerThrottle(1);
 * ... use other SPI devices here ...
 * VsPlayerThrottle(tef);
 * VsPlayerInterrupts(ief);
 * \endcode
 *
 * \param on Throttles (disables) the decoder interface if not zero.
 *           Otherwise the decoder interface is enabled again.
 *
 * \return Zero if the interface was enabled before this call.
 */
ureg_t VsPlayerThrottle(ureg_t on)
{
    static ureg_t is_throttled = 0;
    ureg_t rc;

    rc = is_throttled;
    if(on) {
        VS10XX_XDCS_SET();
        SdiDeselect();
    }
    else {
        VS10XX_XDCS_CLR();
        SdiSelect();
    }
    is_throttled = on;

    return rc;
}

/*!
 * \brief Switch to or switch back from command mode.
 */
static void VsSciSelect(ureg_t on)
{
    if (on) {
        /* Disable data interface. */
        SdiDeselect();

#if defined(VS10XX_SDI_SPI0_DEVICE) && !defined(VS10XX_SCI_SPI0_DEVICE)
        /* Hint given by Jesper Hansen: If data channel uses HW SPI and 
           command channel uses SW SPI, then disable HW SPI while sending 
           using the command channel. */
        cbi(SPCR, SPE);
#endif

        /* Deselect data channel. */
        VS10XX_XDCS_SET();
        /* Select cmd channel. */
        VS10XX_XCS_CLR();
        /* Enable cmd interface. */
        SciSelect();
    }
    else {
        /* Disable cmd interface. */
        SciDeselect();
        /* Deselect cmd channel. */
        VS10XX_XCS_SET();
        /* Re-select data channel. */
        VS10XX_XDCS_CLR();
        /* Enable data interface. */
        SdiSelect();
    }
}

/*!
 * \brief Wait for decoder ready.
 *
 * This function will check the DREQ line. Decoder interrupts must have 
 * been disabled before calling this function.
 */
static int VsWaitReady(void)
{
    int tmo;

    for (tmo = 0; tmo < 5000; tmo++) {
        if (VS10XX_DREQ_TST()) {
            return 0;
        }
    }
    return -1;
}

/*
 * \brief Write a specified number of bytes to the VS10XX data interface.
 *
 * This function will check the DREQ line. Decoder interrupts must have 
 * been disabled before calling this function.
 */
static int VsSdiWrite(CONST uint8_t * data, size_t len)
{
    while (len--) {
        if (!VS10XX_DREQ_TST() && VsWaitReady()) {
            return -1;
        }
        VsSdiPutByte(*data);
        data++;
    }
    return 0;
}

/*
 * \brief Write a specified number of bytes from program space to the 
 *        VS10XX data interface.
 *
 * This function is similar to VsSdiWrite() except that the data is 
 * located in program space.
 */
static int VsSdiWrite_P(PGM_P data, size_t len)
{
    while (len--) {
        if (!VS10XX_DREQ_TST() && VsWaitReady()) {
            return -1;
        }
        VsSdiPutByte(PRG_RDB(data));
        data++;
    }
    return 0;
}

/*
 * \brief Write to a decoder register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 */
static void VsRegWrite(ureg_t reg, uint16_t data)
{
    /* Select command channel. */
    VsWaitReady();
    VsSciSelect(1);
    /*
     * The VS1011E datasheet demands a minimum of 5 ns between
     * the falling CS and the first rising SCK. This is a very
     * short time and doesn't require any additional delay
     * even on very fast CPUs.
     */
    (void)SciByte(VS_OPCODE_WRITE);
    (void)SciByte((uint8_t) reg);
    (void)SciByte((uint8_t) (data >> 8));
    (void)SciByte((uint8_t) data);

    /* Re-select data channel. */
    VsSciSelect(0);
}

/*
 * \brief Read from a register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 * 
 * \return Register contents.
 */
static uint16_t VsRegRead(ureg_t reg)
{
    uint16_t data;

    /* Select command channel. */
    VsWaitReady();
    VsSciSelect(1);

    (void)SciByte(VS_OPCODE_READ);
    (void)SciByte((uint8_t) reg);
    data = (uint16_t)SciByte(0) << 8;
    data |= SciByte(0);

    /* Select data channel. */
    VsSciSelect(0);

    return data;
}

/*
 * \brief Feed the decoder with data.
 *
 * This function serves two purposes: 
 * - It is called by VsPlayerKick() to initially fill the decoder buffer.
 * - It is used as an interrupt handler for the decoder.
 */
static void VsPlayerFeed(void *arg)
{
    ureg_t j = 32;
    size_t total = 0;

    if (!VS10XX_DREQ_TST()) {
        return;
    }

    /* 
     * Feed the decoder until its buffer is full or we ran out of data.
     */
    if (vs_status == VS_STATUS_RUNNING) {
        char *bp = 0;
        size_t consumed = 0;
        size_t available = 0;
        do {
            if(consumed >= available) {
                /* Commit previously consumed bytes. */
                if(consumed) {
                    NutSegBufReadCommit(consumed);
                    consumed = 0;
                }
                /* All bytes consumed, request new. */
                bp = NutSegBufReadRequest(&available);
                if(available == 0) {
                    /* End of stream. */
                    vs_status = VS_STATUS_EOF;
                    break;
                }
            }
            /* We have some data in the buffer, feed it. */
            VsSdiPutByte(*bp);
            bp++;
            consumed++;
            total++;
            if (total > 4096) {
                vs_status = VS_STATUS_EOF;
                break;
            }

            /* Allow 32 bytes to be sent as long as DREQ is set, This includes
               the one in progress. */
            if (VS10XX_DREQ_TST()) {
                j = 32;
            }
        } while(j--);

        /* Finally re-enable the producer buffer. */
        NutSegBufReadLast(consumed);
    }

    /* 
     * Flush the internal VS buffer. 
     */
    if(vs_status != VS_STATUS_RUNNING && vs_flush) {
        do {
            VsSdiPutByte(0);
            if (--vs_flush == 0) {
                /* Decoder internal buffer is flushed. */
                vs_status = VS_STATUS_EMPTY;
                break;
            }
            /* Allow 32 bytes to be sent as long as DREQ is set, This includes
               the one in progress. */
            if (VS10XX_DREQ_TST()) {
                j = 32;
            }
        } while(j--);
    }
}

/*!
 * \brief Start playback.
 *
 * This routine will send the first MP3 data bytes to the
 * decoder, until it is completely filled. The data buffer
 * should have been filled before calling this routine.
 *
 * Decoder interrupts will be enabled.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerKick(void)
{
    /*
     * Start feeding the decoder with data.
     */
    VsPlayerInterrupts(0);
    vs_status = VS_STATUS_RUNNING;
    VsPlayerFeed(NULL);
    VsPlayerInterrupts(1);

    return 0;
}

/*!
 * \brief Stops the playback.
 *
 * This routine will stops the MP3 playback, VsPlayerKick() may be used 
 * to resume the playback.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerStop(void)
{
    ureg_t ief;

    ief = VsPlayerInterrupts(0);
    /* Check whether we need to stop at all to not overwrite other than running status */
    if (vs_status == VS_STATUS_RUNNING) {
        vs_status = VS_STATUS_STOPPED;
    }
    VsPlayerInterrupts(ief);

    return 0;
}


/*!
 * \brief Sets up decoder internal buffer flushing.
 *
 * This routine will set up internal VS buffer flushing,
 * unless the buffer is already empty and starts the playback
 * if necessary. The internal VS buffer is flushed in VsPlayerFeed()
 * at the end of the stream.
 *
 * Decoder interrupts will be enabled.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerFlush(void)
{
    VsPlayerInterrupts(0);
    /* Set up fluhing unless both buffers are empty. */
    if (vs_status != VS_STATUS_EMPTY || NutSegBufUsed()) {
        if (vs_flush == 0) {
            vs_flush = VS_FLUSH_BYTES;
        }
        /* start the playback if necessary */
        if (vs_status != VS_STATUS_RUNNING) {
            VsPlayerKick();
        }
    }
    VsPlayerInterrupts(1);

    return 0;
}

/*!
 * \brief Initialize the VS10XX hardware interface.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerInit(void)
{
    /* Disable decoder interrupts. */
    VsPlayerInterrupts(0);

    /* Keep decoder in reset state. */
    SciReset(1);

    /* Initialize command channel. */
    if (SciSetMode()) {
        return -1;
    }
    SciSetSpeed();
    SciDeselect();

    /* Initialize data channel. */
    if (SdiSetMode()) {
        return -1;
    }
    SdiSetSpeed();
    SdiDeselect();

    /* Set BSYNC output low. */
    VS10XX_BSYNC_CLR();
    VS10XX_BSYNC_ENA();

    /* Set low active data channel select output low. */
    VS10XX_XDCS_SET();
    VS10XX_XDCS_ENA();

    /* Set low active command channel select output high. */
    VS10XX_XCS_SET();
    VS10XX_XCS_ENA();

    /* Enable DREQ interrupt input. */
    VS10XX_DREQ_ENA();

    /* Register the interrupt routine */
    if (NutRegisterIrqHandler(&VS10XX_SIGNAL, VsPlayerFeed, NULL)) {
        return -1;
    }

    /* Rising edge will generate an interrupt. */
    NutIrqSetMode(&VS10XX_SIGNAL, NUT_IRQMODE_RISINGEDGE);

    /* Release decoder reset line. */
    SciReset(0);
    NutDelay(VS10XX_HWRST_RECOVER);

    return VsPlayerReset(0);
}

/*!
 * \brief Software reset the decoder.
 *
 * This function is typically called after VsPlayerInit() and at the end
 * of each track.
 *
 * \param mode Any of the following flags may be or'ed (check the data sheet)
 * - \ref VS_SM_DIFF Left channel inverted.
 * - \ref VS_SM_LAYER12 Allow MPEG layers I and II.
 * - \ref VS_SM_MP12 Allow MPEG layers I and II on VS1001K.
 * - \ref VS_SM_FFWD VS1001K fast forward.
 * - \ref VS_SM_RESET Software reset.
 * - \ref VS_SM_OUTOFWAV Jump out of WAV decoding.
 * - \ref VS_SM_TESTS Allow SDI tests.
 * - \ref VS_SM_PDOWN Switch to power down mode.
 * - \ref VS_SM_BASS VS1001K bass/treble enhancer.
 * - \ref VS_SM_STREAM Stream mode.
 * - \ref VS_SM_SDISHARE Share SPI chip select.
 * - \ref VS_SM_SDINEW VS1002 native mode (automatically set).
 * - \ref VS_SM_ADPCM VS1033 ADPCM recording.
 * - \ref VS_SM_ADPCM_HP VS1033 ADPCM high pass filter.
 * - \ref VS_SM_LINE_IN VS1033 ADPCM recording selector.
 * - \ref VS_SM_CLK_RANGE VS1033 input clock range.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerReset(uint16_t mode)
{
    /* Disable decoder interrupts and feeding. */
    VsPlayerInterrupts(0);
    vs_status = VS_STATUS_STOPPED;

    /* Software reset. */
    VsRegWrite(VS_MODE_REG, VS_SM_RESET);
    /* The decoder needs 9600 XTAL cycles. This is at least twice. */
    NutDelay(VS10XX_SWRST_RECOVER);

    /*
     * Check for correct reset.
     */
    if ((mode & VS_SM_RESET) != 0 || !VS10XX_DREQ_TST()) {
        /* If not succeeded, give it one more chance and try hw reset. */
        SciReset(1);
        /* No idea how long we must held reset low. */
        NutDelay(VS10XX_HWRST_DURATION);
        SciReset(0);
        /* No idea how long we need to wait here. */
        NutDelay(VS10XX_HWRST_RECOVER);
        if (!VS10XX_DREQ_TST()) {
            return -1;
        }
    }

    /* Set codec mode. */
#if defined(VS10XX_BSYNC_BIT)
    xVsRegWrite(VS_MODE_REG, mode);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_SDINEW | mode);
#endif

#if VS10XX_FREQ < 20000000UL
    VsRegWrite(VS_CLOCKF_REG, (uint16_t)(VS_CF_DOUBLER | (VS10XX_FREQ / 2000UL)));
#else
    VsRegWrite(VS_CLOCKF_REG, (uint16_t)(VS10XX_FREQ / 2000UL));
#endif
#if defined(AUDIO_VS1001K)
    /* Force frequency change (see datasheet). */
    VsRegWrite(VS_INT_FCTLH_REG, 0x8008);
#endif
    NutDelay(1);

#if defined(VS10XX_SDI_SPI0_DEVICE) || defined(VS10XX_SCI_SPI0_DEVICE)
    /* Clear any spurious interrupts. */
    outb(EIFR, BV(VS10XX_DREQ_BIT));
#endif

    return 0;
}

/*!
 * \brief Set mode register of the decoder.
 *
 * \param mode Any of the following flags may be or'ed (check the data sheet)
 * - \ref VS_SM_DIFF Left channel inverted.
 * - \ref VS_SM_LAYER12 Allow MPEG layers I and II.
 * - \ref VS_SM_MP12 Allow MPEG layers I and II on VS1001K.
 * - \ref VS_SM_FFWD VS1001K fast forward.
 * - \ref VS_SM_RESET Software reset.
 * - \ref VS_SM_OUTOFWAV Jump out of WAV decoding.
 * - \ref VS_SM_TESTS Allow SDI tests.
 * - \ref VS_SM_PDOWN Switch to power down mode.
 * - \ref VS_SM_BASS VS1001K bass/treble enhancer.
 * - \ref VS_SM_STREAM Stream mode.
 * - \ref VS_SM_SDISHARE Share SPI chip select.
 * - \ref VS_SM_SDINEW VS1002 native mode (automatically set).
 * - \ref VS_SM_ADPCM VS1033 ADPCM recording.
 * - \ref VS_SM_ADPCM_HP VS1033 ADPCM high pass filter.
 * - \ref VS_SM_LINE_IN VS1033 ADPCM recording selector.
 * - \ref VS_SM_CLK_RANGE VS1033 input clock range.
 *
 * \return Always 0.
 */
int VsPlayerSetMode(uint16_t mode)
{
    ureg_t ief;

    ief = VsPlayerInterrupts(0);
#if defined(VS10XX_BSYNC_BIT)
    VsRegWrite(VS_MODE_REG, mode);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_SDINEW | mode);
#endif
    VsPlayerInterrupts(ief);

    return 0;
}

/*!
 * \brief Returns play time since last reset.
 *
 * \return Play time since reset in seconds
 */
uint16_t VsPlayTime(void)
{
    uint16_t rc;
    ureg_t ief;

    ief = VsPlayerInterrupts(0);
    rc = VsRegRead(VS_DECODE_TIME_REG);
    VsPlayerInterrupts(ief);

    return rc;
}

/*!
 * \brief Returns status of the player.
 *
 * \return Any of the following value:
 * - VS_STATUS_STOPPED Player is ready to be started by VsPlayerKick().
 * - VS_STATUS_RUNNING Player is running.
 * - VS_STATUS_EOF Player has reached the end of a stream after VsPlayerFlush() has been called.
 * - VS_STATUS_EMPTY Player runs out of data. VsPlayerKick() will restart it.
 */
unsigned int VsGetStatus(void)
{
    return vs_status;
}

#ifdef __GNUC__

/*!
 * \brief Query MP3 stream header information.
 *
 * \param vshi Pointer to VS_HEADERINFO structure.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsGetHeaderInfo(VS_HEADERINFO * vshi)
{
    uint16_t *usp = (uint16_t *) vshi;
    ureg_t ief;

    ief = VsPlayerInterrupts(0);
    *usp = VsRegRead(VS_HDAT1_REG);
    *++usp = VsRegRead(VS_HDAT0_REG);
    VsPlayerInterrupts(ief);

    return 0;
}
#endif

/*!
 * \brief Initialize decoder memory test and return result.
 *
 * \return Memory test result.
 * - Bit 0: Good X ROM
 * - Bit 1: Good Y ROM (high)
 * - Bit 2: Good Y ROM (low)
 * - Bit 3: Good Y RAM
 * - Bit 4: Good X RAM
 * - Bit 5: Good Instruction RAM (high)
 * - Bit 6: Good Instruction RAM (low)
 */
uint16_t VsMemoryTest(void)
{
    uint16_t rc;
    ureg_t ief;
    static prog_char mtcmd[] = { 0x4D, 0xEA, 0x6D, 0x54, 0x00, 0x00, 0x00, 0x00 };

    ief = VsPlayerInterrupts(0);
#if defined(VS10XX_BSYNC_BIT)
    VsRegWrite(VS_MODE_REG, VS_SM_TESTS);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_TESTS | VS_SM_SDINEW);
#endif
    VsSdiWrite_P(mtcmd, sizeof(mtcmd));
    while(((rc = VsRegRead(VS_HDAT0_REG)) & 0x8000) == 0) {
        NutDelay(1);
    }
#if defined(VS10XX_BSYNC_BIT)
    VsRegWrite(VS_MODE_REG, 0);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_SDINEW);
#endif
    VsPlayerInterrupts(ief);

    return rc;
}

/*!
 * \brief Set volume.
 *
 * \param left  Left channel attenuation, provided in 0.5 dB steps.
 *              Set to 255 for ananlog power down.
 * \param right Right channel attenuation.
 *
 * \return Always 0.
 */
int VsSetVolume(ureg_t left, ureg_t right)
{
    ureg_t ief;

    ief = VsPlayerInterrupts(0);
    VsRegWrite(VS_VOL_REG, ((uint16_t)left << VS_VOL_LEFT_LSB) | ((uint16_t)right << VS_VOL_RIGHT_LSB));
    VsPlayerInterrupts(ief);

    return 0;
}

/*!
 * \brief Sine wave beep.
 *
 * \param fsin Frequency.
 * \param ms   Duration.
 *
 * \return Always 0.
 */
int VsBeep(uint8_t fsin, uint8_t ms)
{
    ureg_t ief;
    static prog_char on[] = { 0x53, 0xEF, 0x6E };
    static prog_char off[] = { 0x45, 0x78, 0x69, 0x74 };
    static prog_char end[] = { 0x00, 0x00, 0x00, 0x00 };

    /* Disable decoder interrupts. */
    ief = VsPlayerInterrupts(0);

#if defined(VS10XX_BSYNC_BIT)
    VsRegWrite(VS_MODE_REG, VS_SM_TESTS);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_TESTS | VS_SM_SDINEW);
#endif

    fsin = 56 + (fsin & 7) * 9;
    VsSdiWrite_P(on, sizeof(on));
    VsSdiWrite(&fsin, 1);
    VsSdiWrite_P(end, sizeof(end));
    NutDelay(ms);
    VsSdiWrite_P(off, sizeof(off));
    VsSdiWrite_P(end, sizeof(end));

#if defined(VS10XX_BSYNC_BIT)
    VsRegWrite(VS_MODE_REG, 0);
#else
    VsRegWrite(VS_MODE_REG, VS_SM_SDINEW);
#endif

    /* Restore decoder interrupt enable. */
    VsPlayerInterrupts(ief);

    return 0;
}


/*@}*/
