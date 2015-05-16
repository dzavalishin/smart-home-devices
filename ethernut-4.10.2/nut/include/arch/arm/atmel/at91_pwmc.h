#ifndef _ARCH_ARM_AT91_PWMC_H_
#define _ARCH_ARM_AT91_PWMC_H_

/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
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

/*!
 * \file arch/arm/at91_pwmc.h
 * \brief AT91 PWM Controller.
 *
 * \verbatim
 * $Log$
 * Revision 1.1  2008/04/18 13:29:37  haraldkipp
 * Missing PWM hardware support added.
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Pwmc
 */
/*@{*/

/*! \name PWMC Mode Register */
/*@{*/
#define PWMC_MR_OFF             0x00000000  /*!< \brief Mode register offset. */
#define PWMC_MR  (PWMC_BASE + PWMC_MR_OFF)  /*!< \brief Mode register address. */

#define PWMC_DIVA               0x000000FF  /*!< \brief Clock divider A mask. */
#define PWMC_DIVA_LSB           0           /*!< \brief Clock divider A LSB. */

#define PWMC_PREA               0x00000F00  /*!< \brief Prescaler A clock selection mask. */
#define PWMC_PREA_MCK2          0x00000100  /*!< \brief Selects prescaler A MCK / 2. */
#define PWMC_PREA_MCK4          0x00000200  /*!< \brief Selects prescaler A MCK / 4. */
#define PWMC_PREA_MCK8          0x00000300  /*!< \brief Selects prescaler A MCK / 8. */
#define PWMC_PREA_MCK16         0x00000400  /*!< \brief Selects prescaler A MCK / 16. */
#define PWMC_PREA_MCK32         0x00000500  /*!< \brief Selects prescaler A MCK / 32. */
#define PWMC_PREA_MCK64         0x00000600  /*!< \brief Selects prescaler A MCK / 64. */
#define PWMC_PREA_MCK128        0x00000700  /*!< \brief Selects prescaler A MCK / 128. */
#define PWMC_PREA_MCK256        0x00000800  /*!< \brief Selects prescaler A MCK / 256. */
#define PWMC_PREA_MCK512        0x00000900  /*!< \brief Selects prescaler A MCK / 512. */
#define PWMC_PREA_MCK1024       0x00000A00  /*!< \brief Selects prescaler A MCK / 1024. */

#define PWMC_DIVB               0x00FF0000  /*!< \brief Clock divider B mask. */
#define PWMC_DIVB_LSB           16          /*!< \brief Clock divider B LSB. */

#define PWMC_PREB               0x0F000000  /*!< \brief Prescaler B clock selection mask. */
#define PWMC_PREB_MCK2          0x01000000  /*!< \brief Selects prescaler B MCK / 2. */
#define PWMC_PREB_MCK4          0x02000000  /*!< \brief Selects prescaler B MCK / 4. */
#define PWMC_PREB_MCK8          0x03000000  /*!< \brief Selects prescaler B MCK / 8. */
#define PWMC_PREB_MCK16         0x04000000  /*!< \brief Selects prescaler B MCK / 16. */
#define PWMC_PREB_MCK32         0x05000000  /*!< \brief Selects prescaler B MCK / 32. */
#define PWMC_PREB_MCK64         0x06000000  /*!< \brief Selects prescaler B MCK / 64. */
#define PWMC_PREB_MCK128        0x07000000  /*!< \brief Selects prescaler B MCK / 128. */
#define PWMC_PREB_MCK256        0x08000000  /*!< \brief Selects prescaler B MCK / 256. */
#define PWMC_PREB_MCK512        0x09000000  /*!< \brief Selects prescaler B MCK / 512. */
#define PWMC_PREB_MCK1024       0x0A000000  /*!< \brief Selects prescaler B MCK / 1024. */
/*@}*/

/*! \name PWMC Enable/Disable Registers */
/*@{*/
#define PWMC_ENA_OFF            0x00000004  /*!< \brief Enable register offset. */
#define PWMC_ENA (PWMC_BASE + PWMC_ENA_OFF) /*!< \brief Enable register address. */
#define PWMC_DIS_OFF            0x00000008  /*!< \brief Disable register offset. */
#define PWMC_DIS (PWMC_BASE + PWMC_DIS_OFF) /*!< \brief Disable register address. */
#define PWMC_SR_OFF             0x0000000C  /*!< \brief Status register offset. */
#define PWMC_SR   (PWMC_BASE + PWMC_SR_OFF) /*!< \brief Status register address. */
#define PWMC_IER_OFF            0x00000010  /*!< \brief Interrupt enable register offset. */
#define PWMC_IER (PWMC_BASE + PWMC_IER_OFF) /*!< \brief Interrupt enable register address. */
#define PWMC_IDR_OFF            0x00000014  /*!< \brief Interrupt disable register offset. */
#define PWMC_IDR (PWMC_BASE + PWMC_IDR_OFF) /*!< \brief Interrupt disable register address. */
#define PWMC_IMR_OFF            0x00000018  /*!< \brief Interrupt mask register offset. */
#define PWMC_IMR (PWMC_BASE + PWMC_IMR_OFF) /*!< \brief Interrupt mask register address. */
#define PWMC_ISR_OFF            0x0000001C  /*!< \brief Interrupt status register offset. */
#define PWMC_ISR (PWMC_BASE + PWMC_ISR_OFF) /*!< \brief Interrupt status register address. */
#define PWMC_CHID0              0x00000001  /*!< \brief Channel 0 ID. */
#define PWMC_CHID1              0x00000002  /*!< \brief Channel 1 ID. */
#define PWMC_CHID2              0x00000004  /*!< \brief Channel 2 ID. */
#define PWMC_CHID3              0x00000008  /*!< \brief Channel 3 ID. */
/*@}*/

/*! \name PWM Channel Mode Registers */
/*@{*/
#define PWMC_CMR_OFF            0x00000200  /*!< \brief Channel mode register offset. */
/*! \brief Channel mode register addresses. */
#define PWMC_CMR(i)  (PWMC_BASE + ((i) * 32) + PWMC_CMR_OFF)

#define PWMC_CPRE               0x0000000F  /*!< \brief Channel prescaler clock selection mask. */
#define PWMC_CPRE_MCK2          0x00000001  /*!< \brief Selects channel prescaler MCK / 2. */
#define PWMC_CPRE_MCK4          0x00000002  /*!< \brief Selects channel prescaler MCK / 4. */
#define PWMC_CPRE_MCK8          0x00000003  /*!< \brief Selects channel prescaler MCK / 8. */
#define PWMC_CPRE_MCK16         0x00000004  /*!< \brief Selects channel prescaler MCK / 16. */
#define PWMC_CPRE_MCK32         0x00000005  /*!< \brief Selects channel prescaler MCK / 32. */
#define PWMC_CPRE_MCK64         0x00000006  /*!< \brief Selects channel prescaler MCK / 64. */
#define PWMC_CPRE_MCK128        0x00000007  /*!< \brief Selects channel prescaler MCK / 128. */
#define PWMC_CPRE_MCK256        0x00000008  /*!< \brief Selects channel prescaler MCK / 256. */
#define PWMC_CPRE_MCK512        0x00000009  /*!< \brief Selects channel prescaler MCK / 512. */
#define PWMC_CPRE_MCK1024       0x0000000A  /*!< \brief Selects channel prescaler MCK / 1024. */
#define PWMC_CPRE_CLKA          0x0000000B  /*!< \brief Selects channel prescaler CLKA. */
#define PWMC_CPRE_CLKB          0x0000000C  /*!< \brief Selects channel prescaler CLKB. */

#define PWMC_CALG               0x00000100  /*!< \brief Center aligned channel period. */
#define PWMC_CPOL               0x00000200  /*!< \brief Output waveform starts at a high level. */
#define PWMC_CPD                0x00000400  /*!< \brief Channel update period. */
/*@}*/

/*! \name PWM Channel Duty Cycle Registers */
/*@{*/
#define PWMC_CDTY_OFF           0x00000204  /*!< \brief Channel duty cycle register offset. */
/*! \brief Channel duty cycle register addresses. */
#define PWMC_CDTY(i)  (PWMC_BASE + PWMC_CDTY_OFF + ((i) * 32))
/*@}*/

/*! \name PWM Channel Period Registers */
/*@{*/
#define PWMC_CPRD_OFF           0x00000208  /*!< \brief Channel period register offset. */
/*! \brief Channel period register addresses. */
#define PWMC_CPRD(i)  (PWMC_BASE + PWMC_CPRD_OFF + ((i) * 32))
/*@}*/

/*! \name PWM Channel Counter Registers */
/*@{*/
#define PWMC_CCNT_OFF           0x0000020C  /*!< \brief Channel counter register offset. */
/*! \brief Channel counter register addresses. */
#define PWMC_CCNT(i)  (PWMC_BASE + PWMC_CCNT_OFF + ((i) * 32))
/*@}*/

/*! \name PWM Channel Update Registers */
/*@{*/
#define PWMC_CUPD_OFF           0x00000210  /*!< \brief Channel update register offset. */
/*! \brief Channel update register addresses. */
#define PWMC_CUPD(i)  (PWMC_BASE + PWMC_CUPD_OFF + ((i) * 32))
/*@}*/


/*@} xgNutArchArmAt91Pwmc */

#endif  /* _ARCH_ARM_AT91_PWMC_H_ */
