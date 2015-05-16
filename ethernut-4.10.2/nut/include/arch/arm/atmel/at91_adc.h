/*
 * Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@embedded-it.de>,
 *                       Kernelconcepts http://www.embedded-it.de
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
 * Revision 1.2  2007/12/09 22:12:49  olereinhardt
 * Added cvs log tag
 *
 */

#ifndef _ARCH_ARM_AT91_ADC_H_
#define _ARCH_ARM_AT91_ADC_H_

/*!
 * \file arch/arm/at91_adc.h
 * \brief AT91 analog digital converter interface.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2007/12/09 22:12:49  olereinhardt
 * Added cvs log tag
 *
 * Revision 1.1  2007/12/09 21:37:14  olereinhardt
 * Initial checkin of adc driver
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Adc
 */
/*@{*/

/*! \name ADC Control Register */
/*@{*/
#define ADC_CR_OFF              0x00000000      /*!< \brief Control register offset. */
#define ADC_CR      (ADC_BASE + ADC_CR_OFF)     /*!< \brief Control register address. */
#define ADC_SWRST               0x00000001      /*!< \brief Software reset. */
#define ADC_START               0x00000002      /*!< \brief Start a conversion. */
/*@}*/

/*! \name ADC Mode Register */
/*@{*/
#define ADC_MR_OFF              0x00000004      /*!< \brief Mode register offset. */
#define ADC_MR      (ADC_BASE + ADC_MR_OFF)     /*!< \brief Mode register address. */
#define ADC_TRGEN               0x00000001      /*!< \brief Hardware trigger enable. */
#define ADC_TRGSEL              0x0000000E      /*!< \brief Trigger select mask */
#define   ADC_TRGSEL_TIOA0      0x00000000      /*!< \brief Trigger select timer counter 0 output */
#define   ADC_TRGSEL_TIOA1      0x00000002      /*!< \brief Trigger select timer counter 1 output */
#define   ADC_TRGSEL_TIOA2      0x00000004      /*!< \brief Trigger select timer counter 2 output */
#define   ADC_TRGSEL_EXT        0x0000000C      /*!< \brief Single trigger */
#define ADC_LOWRES              0x00000010      /*!< \brief 10bit / 8bit selection */
#define ADC_SLEEP               0x00000020      /*!< \brief Enter sleep mode */
#define ADC_PRESCAL             0x00003F00      /*!< \brief Prescaler mask */
#define ADC_PRESCAL_LSB         8               /*!< \brief Prescaler LSB */
#define ADC_STARTUP             0x001F0000      /*!< \brief Startup time mask */
#define ADC_STARTUP_LSB         16              /*!< \brief Startup time LSB */
#define ADC_SHTIM               0x0F000000      /*!< \brief Sample hold time mask */
#define ADC_SHTIM_LSB           24              /*!< \brief Sample hold time LSB */
/*@}*/

/*! \name ADC Channel Enable Register */
/*@{*/
#define ADC_CHER_OFF            0x00000010      /*!< \brief Channel enable register offset */
#define ADC_CHER    (ADC_BASE + ADC_CHER_OFF)   /*!< \brief Channel enable register address. */

#define ADC_CH(x) ((unsigned long)BV(x))        /*!< \brief macro to access channel by numer */
#define ADC_CH0                 0x00000001      /*!< \brief ADC Channel 0 */
#define ADC_CH1                 0x00000002      /*!< \brief ADC Channel 1 */
#define ADC_CH2                 0x00000004      /*!< \brief ADC Channel 2 */
#define ADC_CH3                 0x00000008      /*!< \brief ADC Channel 3 */
#define ADC_CH4                 0x00000010      /*!< \brief ADC Channel 4 */
#define ADC_CH5                 0x00000020      /*!< \brief ADC Channel 5 */
#define ADC_CH6                 0x00000040      /*!< \brief ADC Channel 6 */
#define ADC_CH7                 0x00000080      /*!< \brief ADC Channel 7 */
/*@}*/

/*! \name ADC Channel Disable Register */
/*@{*/
#define ADC_CHDR_OFF            0x00000014      /*!< \brief Channel disable register offset */
#define ADC_CHDR    (ADC_BASE + ADC_CHDR_OFF)   /*!< \brief Channel disable register address. */
/*@}*/

/*! \name ADC Channel Status Register */
/*@{*/
#define ADC_CHSR_OFF            0x00000018      /*!< \brief Channel status register offset */
#define ADC_CHSR    (ADC_BASE + ADC_CHSR_OFF)   /*!< \brief Channel status register address. */
/*@}*/

/*! \name ADC Status Register */
/*@{*/
#define ADC_SR_OFF              0x0000001C      /*!< \brief ADC status register offset */
#define ADC_SR      (ADC_BASE + ADC_SR_OFF)     /*!< \brief ADC status register address. */

#define ADC_EOC(x)((unsigned long)BV(x))        /*!< \brief Macro to access EOC flag by channel number */

#define ADC_EOC0                0x00000001      /*!< \brief End of conversion channel 0 */
#define ADC_EOC1                0x00000002      /*!< \brief End of conversion channel 1 */
#define ADC_EOC2                0x00000004      /*!< \brief End of conversion channel 2 */
#define ADC_EOC3                0x00000008      /*!< \brief End of conversion channel 3 */
#define ADC_EOC4                0x00000010      /*!< \brief End of conversion channel 4 */
#define ADC_EOC5                0x00000020      /*!< \brief End of conversion channel 5 */
#define ADC_EOC6                0x00000040      /*!< \brief End of conversion channel 6 */
#define ADC_EOC7                0x00000080      /*!< \brief End of conversion channel 7 */

#define ADC_OVRE(x)((unsigned long)BV(x+8))     /*!< \brief Macto to acces overrun error flag by channel number */ 
#define ADC_OVRE0               0x00000100      /*!< \brief Overrun error flag channel 0 */
#define ADC_OVRE1               0x00000200      /*!< \brief Overrun error flag channel 1 */
#define ADC_OVRE2               0x00000400      /*!< \brief Overrun error flag channel 2 */
#define ADC_OVRE3               0x00000800      /*!< \brief Overrun error flag channel 3 */
#define ADC_OVRE4               0x00001000      /*!< \brief Overrun error flag channel 4 */
#define ADC_OVRE5               0x00002000      /*!< \brief Overrun error flag channel 5 */
#define ADC_OVRE6               0x00004000      /*!< \brief Overrun error flag channel 6 */
#define ADC_OVRE7               0x00008000      /*!< \brief Overrun error flag channel 7 */

#define ADC_DRDY                0x00010000      /*!< \brief Data ready flag */
#define ADC_GOVRE               0x00020000      /*!< \brief General data overrun error flag */
#define ADC_ENDRX               0x00040000      /*!< \brief End of rx buffer flag */
#define ADC_RXBUF               0x00080000      /*!< \brief Rx buffer full flag */
/*@}*/

/*! \name ADC Last Converted Data Register */
/*@{*/
#define ADC_LCDR_OFF            0x00000020      /*!< \brief Last converted data register offset */
#define ADC_LCDR    (ADC_BASE + ADC_LCDR_OFF)   /*!< \brief Last converted data register. */

#define ADC_LCDR_MASK           0x000003FF      /*!< \brief Last converted data mask (10bit) */
/*@}*/

/*! \name ADC Interrupt Enable Register */
/*@{*/
#define ADC_IER_OFF             0x00000024      /*!< \brief Interrupt enable register offset */
#define ADC_IER     (ADC_BASE + ADC_IER_OFF)    /*!< \brief Last converted data register. */
/*@}*/

/*! \name ADC Interrupt Disable Register */
/*@{*/
#define ADC_IDR_OFF             0x00000028      /*!< \brief Interrupt disable register offset */
#define ADC_IDR     (ADC_BASE + ADC_IDR_OFF)    /*!< \brief Interrupt disable register. */
/*@}*/

/*! \name ADC Interrupt Mask Register */
/*@{*/
#define ADC_IMR_OFF             0x0000002C      /*!< \brief Interrupt mask register offset */
#define ADC_IMR     (ADC_BASE + ADC_IMR_OFF)    /*!< \brief Interrupt mask register. */
/*@}*/

/*! \name ADC Channel Data Register */
/*@{*/
#define ADC_CDR_OFF             0x00000030      /*!< \brief Channel data register offset */
#define ADC_CDR(x) (ADC_BASE + ADC_CDR_OFF + ((x) << 2))    /*!< \brief Channel data register. */

#define ADC_CDR_MASK           0x000003FF       /*!< \brief Channel data mask (10bit) */
/*@}*/



#endif
