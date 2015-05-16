#ifndef _ARCH_ARM_AT91_TWI_H_
#define _ARCH_ARM_AT91_TWI_H_

/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_twi.h
 * \brief AT91 two wire interface.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/01/31 09:14:50  haraldkipp
 * Duplicate Log tag removed.
 *
 * Revision 1.1  2006/08/31 19:19:55  haraldkipp
 * No time to write comments. ;-)
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Twi
 */
/*@{*/

/*! \name TWI Control Register */
/*@{*/
#define TWI_CR_OFF              0x00000000      /*!< \brief Control register offset. */
#define TWI_CR      (TWI_BASE + TWI_CR_OFF)     /*!< \brief Control register address. */
#define TWI_START               0x00000001      /*!< \brief Send start condition. */
#define TWI_STOP                0x00000002      /*!< \brief Send stop condition. */
#define TWI_MSEN                0x00000004      /*!< \brief Enable master mode. */
#define TWI_MSDIS               0x00000008      /*!< \brief Disable master mode. */
#define TWI_SVEN                0x00000010      /*!< \brief Enable slave mode. */
#define TWI_SVDIS               0x00000020      /*!< \brief Disable slave mode. */
#define TWI_SWRST               0x00000080      /*!< \brief Software reset. */
/*@}*/

/*! \name TWI Master Mode Register */
/*@{*/
#define TWI_MMR_OFF             0x00000004      /*!< \brief Master mode register offset. */
#define TWI_MMR     (TWI_BASE + TWI_MMR_OFF)    /*!< \brief Master mode register address. */
#define TWI_IADRSZ              0x00000300      /*!< \brief Internal device address size mask. */
#define TWI_IADRSZ_NONE         0x00000000      /*!< \brief No internal device address. */
#define TWI_IADRSZ_1BYTE        0x00000100      /*!< \brief One byte internal device address. */
#define TWI_IADRSZ_2BYTE        0x00000200      /*!< \brief Two byte internal device address. */
#define TWI_IADRSZ_3BYTE        0x00000300      /*!< \brief Three byte internal device address. */
#define TWI_MREAD               0x00001000      /*!< \brief Master read direction. */
#define TWI_DADR                0x007F0000      /*!< \brief Device address mask. */
#define TWI_DADR_LSB                    16      /*!< \brief Device address LSB. */
/*@}*/

/*! \name TWI Slave Mode Register */
/*@{*/
#define TWI_SMR_OFF             0x00000008      /*!< \brief Slave mode register offset. */
#define TWI_SMR     (TWI_BASE + TWI_SMR_OFF)    /*!< \brief Slave mode register address. */
#define TWI_SADR                0x007F0000      /*!< \brief Slave address mask. */
#define TWI_SADR_LSB                    16      /*!< \brief Slave address LSB. */
/*@}*/

/*! \name TWI Internal Address Register */
/*@{*/
#define TWI_IADRR_OFF           0x0000000C      /*!< \brief Internal address register offset. */
#define TWI_IADRR   (TWI_BASE + TWI_IADRR_OFF)  /*!< \brief Internal address register address. */
#define TWI_IADR                0x00FFFFFF      /*!< \brief Internal address mask. */
#define TWI_IADR_LSB                    0       /*!< \brief Internal address LSB. */
/*@}*/

/*! \name TWI Clock Waveform Generator Register */
/*@{*/
#define TWI_CWGR_OFF            0x00000010      /*!< \brief Clock waveform generator register offset. */
#define TWI_CWGR    (TWI_BASE + TWI_CWGR_OFF)   /*!< \brief Clock waveform generator register address. */
#define TWI_CLDIV                0x000000FF     /*!< \brief Clock low divider mask. */
#define TWI_CLDIV_LSB                    0      /*!< \brief Clock low divider LSB. */
#define TWI_CHDIV                0x0000FF00     /*!< \brief Clock high divider mask. */
#define TWI_CHDIV_LSB                    8      /*!< \brief Clock high divider LSB. */
#define TWI_CKDIV                0x00070000     /*!< \brief Clock divider mask. */
#define TWI_CKDIV_LSB                    16     /*!< \brief Clock divider LSB. */
/*@}*/

/*! \name TWI Status and Interrupt Registers */
/*@{*/
#define TWI_SR_OFF              0x00000020      /*!< \brief Status register offset. */
#define TWI_SR      (TWI_BASE + TWI_SR_OFF)     /*!< \brief Status register address. */

#define TWI_IER_OFF             0x00000024      /*!< \brief Interrupt enable register offset. */
#define TWI_IER     (TWI_BASE + TWI_IER_OFF)    /*!< \brief Interrupt enable register address. */

#define TWI_IDR_OFF             0x00000028      /*!< \brief Interrupt disable register offset. */
#define TWI_IDR     (TWI_BASE + TWI_IDR_OFF)    /*!< \brief Interrupt disable register address. */

#define TWI_IMR_OFF             0x0000002C      /*!< \brief Interrupt mask register offset. */
#define TWI_IMR     (TWI_BASE + TWI_IMR_OFF)    /*!< \brief Interrupt mask register address. */

#define TWI_TXCOMP              0x00000001      /*!< \brief Transmission completed. */
#define TWI_RXRDY               0x00000002      /*!< \brief Receive holding register ready. */
#define TWI_TXRDY               0x00000004      /*!< \brief Transmit holding register ready. */
#define TWI_SVREAD              0x00000008      /*!< \brief Slave read. */
#define TWI_SVACC               0x00000010      /*!< \brief Slave access. */
#define TWI_GACC                0x00000020      /*!< \brief General call access. */
#define TWI_OVRE                0x00000040      /*!< \brief Overrun error. */
#define TWI_NACK                0x00000100      /*!< \brief Not acknowledged. */
#define TWI_ARBLST              0x00000200      /*!< \brief Arbitration lost. */
#define TWI_SCLWS               0x00000400      /*!< \brief Clock wait state. */
#define TWI_EOSACC              0x00000800      /*!< \brief End of slave access. */
/*@}*/

/*! \name TWI Receive Holding Register */
/*@{*/
#define TWI_RHR_OFF             0x00000030      /*!< \brief Receive holding register offset. */
#define TWI_RHR     (TWI_BASE + TWI_RHR_OFF)    /*!< \brief Receive holding register address. */
/*@}*/

/*! \name TWI Transmit Holding Register */
/*@{*/
#define TWI_THR_OFF             0x00000034      /*!< \brief Transmit holding register offset. */
#define TWI_THR     (TWI_BASE + TWI_THR_OFF)    /*!< \brief Transmit holding register address. */
/*@}*/

/*@} xgNutArchArmAt91Twi */

#endif                          /* _ARCH_ARM_AT91_TWI_H_ */
