#ifndef _ARCH_ARM_AT91_PMC_H_
#define _ARCH_ARM_AT91_PMC_H_

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_pmc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.10  2009/02/17 09:33:55  haraldkipp
 * Check for PLL B availability instead of target names.
 *
 * Revision 1.9  2008/08/06 12:51:10  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.8  2008/02/15 17:04:59  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.7  2007/10/04 20:26:26  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.6  2006/09/29 12:43:44  haraldkipp
 * Excluded second PLL from SAM7X builds. Corrected USB divider names.
 *
 * Revision 1.5  2006/08/31 19:08:14  haraldkipp
 * Defining register offsets simplifies assembly programming.
 *
 * Revision 1.4  2006/08/05 11:59:23  haraldkipp
 * Wrong PMC register offsets fixed.
 *
 * Revision 1.3  2006/07/26 11:22:05  haraldkipp
 * Added shift values for multi-bit parameters.
 *
 * Revision 1.2  2006/07/18 14:05:26  haraldkipp
 * Changed coding style to follow existing headers.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Pmc
 */
/*@{*/

/*! \name System Clock Enable, Disable and Status Register */
/*@{*/
#define PMC_SCER_OFF                0x00000000  /*!< \brief System clock enable register offset. */
#define PMC_SCER    (PMC_BASE + PMC_SCER_OFF)   /*!< \brief System clock enable register address. */
#define PMC_SCDR_OFF                0x00000004  /*!< \brief System clock disable register offset. */
#define PMC_SCDR    (PMC_BASE + PMC_SCDR_OFF)   /*!< \brief System clock disable register address. */
#define PMC_SCSR_OFF                0x00000008  /*!< \brief System clock status register offset. */
#define PMC_SCSR    (PMC_BASE + PMC_SCSR_OFF)   /*!< \brief System clock status register address. */

#define PMC_PCK                     0x00000001  /*!< \brief Processor clock. */
#define PMC_UHP                     0x00000040  /*!< \brief USB host port clock. */
#define PMC_UDP                     0x00000080  /*!< \brief USB device port clock. */
#define PMC_PCK0                    0x00000100  /*!< \brief Programmable clock 0 output. */
#define PMC_PCK1                    0x00000200  /*!< \brief Programmable clock 1 output. */
#define PMC_PCK2                    0x00000400  /*!< \brief Programmable clock 2 output. */
#define PMC_PCK3                    0x00000800  /*!< \brief Programmable clock 3 output. */
/*@}*/

/*! \name Peripheral Clock Enable, Disable and Status Register */
/*@{*/
#define PMC_PCER_OFF                0x00000010  /*!< \brief Peripheral clock enable register offset. */
#define PMC_PCER    (PMC_BASE + PMC_PCER_OFF)   /*!< \brief Peripheral clock enable register address. */
#define PMC_PCDR_OFF                0x00000014  /*!< \brief Peripheral clock disable register offset. */
#define PMC_PCDR    (PMC_BASE + PMC_PCDR_OFF)   /*!< \brief Peripheral clock disable register address. */
#define PMC_PCSR_OFF                0x00000018  /*!< \brief Peripheral clock status register offset. */
#define PMC_PCSR    (PMC_BASE + PMC_PCSR_OFF)   /*!< \brief Peripheral clock status register address. */
/*@}*/

/*! \name Clock Generator Main Oscillator Register */
/*@{*/
#define CKGR_MOR_OFF                0x00000020  /*!< \brief Main oscillator register offset. */
#define CKGR_MOR    (PMC_BASE + CKGR_MOR_OFF)   /*!< \brief Main oscillator register address. */

#define CKGR_MOSCEN                 0x00000001  /*!< \brief Main oscillator enable. */
#define CKGR_OSCBYPASS              0x00000002  /*!< \brief Main oscillator bypass. */
#define CKGR_OSCOUNT                0x0000FF00  /*!< \brief Main oscillator start-up time mask. */
#define CKGR_OSCOUNT_LSB                    8   /*!< \brief Main oscillator start-up time LSB. */
/*@}*/

/*! \name Clock Generator Main Clock Frequency Register */
/*@{*/
#define CKGR_MCFR_OFF               0x00000024  /*!< \brief Main clock frequency register offset. */
#define CKGR_MCFR   (PMC_BASE + CKGR_MCFR_OFF)  /*!< \brief Main clock frequency register address. */

#define CKGR_MAINF                  0x0000FFFF  /*!< \brief Main clock frequency mask mask. */
#define CKGR_MAINF_OFF                      0   /*!< \brief Main clock frequency mask LSB. */
#define CKGR_MAINRDY                0x00010000  /*!< \brief Main clock ready. */
/*@}*/

/*! \name PLL Registers */
/*@{*/
#if defined (PMC_HAS_PLLB)
#define CKGR_PLLAR_OFF              0x00000028  /*!< \brief Clock generator PLL register offset. */
#define CKGR_PLLAR  (PMC_BASE + CKGR_PLLAR_OFF) /*!< \brief Clock generator PLL register address. */
#define CKGR_PLLBR_OFF              0x0000002C  /*!< \brief Clock generator PLL register offset. */
#define CKGR_PLLBR  (PMC_BASE + CKGR_PLLBR_OFF) /*!< \brief Clock generator PLL register address. */
#else
#define CKGR_PLLR_OFF               0x0000002C  /*!< \brief Clock generator PLL register offset. */
#define CKGR_PLLR   (PMC_BASE + CKGR_PLLR_OFF)  /*!< \brief Clock generator PLL register address. */
#endif

#define CKGR_DIV                    0x000000FF  /*!< \brief Divider. */
#define CKGR_DIV_LSB                        0   /*!< \brief Least significant bit of the divider. */
#define CKGR_DIV_0                  0x00000000  /*!< \brief Divider output is 0. */
#define CKGR_DIV_BYPASS             0x00000001  /*!< \brief Divider is bypassed. */
#define CKGR_PLLCOUNT               0x00003F00  /*!< \brief PLL counter mask. */
#define CKGR_PLLCOUNT_LSB                   8   /*!< \brief PLL counter LSB. */
#define CKGR_OUT                    0x0000C000  /*!< \brief PLL output frequency range. */
#define CKGR_OUT_0                  0x00000000  /*!< \brief Please refer to the PLL datasheet. */
#define CKGR_OUT_1                  0x00004000  /*!< \brief Please refer to the PLL datasheet. */
#define CKGR_OUT_2                  0x00008000  /*!< \brief Please refer to the PLL datasheet. */
#define CKGR_OUT_3                  0x0000C000  /*!< \brief Please refer to the PLL datasheet. */
#define CKGR_MUL                    0x07FF0000  /*!< \brief PLL multiplier. */
#define CKGR_MUL_LSB                        16  /*!< \brief Least significant bit of the PLL multiplier. */
#define CKGR_USBDIV                 0x30000000  /*!< \brief Divider for USB clocks. */
#define CKGR_USBDIV_1               0x00000000  /*!< \brief Divider output is PLL clock output. */
#define CKGR_USBDIV_2               0x10000000  /*!< \brief Divider output is PLL clock output divided by 2. */
#define CKGR_USBDIV_4               0x20000000  /*!< \brief Divider output is PLL clock output divided by 4. */
/*@}*/

/*! \name Master Clock Register */
/*@{*/
#define PMC_MCKR_OFF                0x00000030  /*!< \brief Master clock register offset. */
#define PMC_MCKR    (PMC_BASE + PMC_MCKR_OFF)   /*!< \brief Master clock register address. */
#define PMC_ACKR_OFF                0x00000034  /*!< \brief Application clock register offset. */
#define PMC_ACKR    (PMC_BASE + PMC_ACKR_OFF)   /*!< \brief Application clock register address. */
#define PMC_PCKR0_OFF               0x00000040  /*!< \brief Programmable clock 0 register offset. */
#define PMC_PCKR0   (PMC_BASE + PMC_PCKR0_OFF)  /*!< \brief Programmable clock 0 register address. */
#define PMC_PCKR1_OFF               0x00000044  /*!< \brief Programmable clock 1 register offset. */
#define PMC_PCKR1   (PMC_BASE + PMC_PCKR1_OFF)  /*!< \brief Programmable clock 1 register address. */
#define PMC_PCKR2_OFF               0x00000048  /*!< \brief Programmable clock 2 register offset. */
#define PMC_PCKR2   (PMC_BASE + PMC_PCKR2_OFF)  /*!< \brief Programmable clock 2 register address. */
#define PMC_PCKR3_OFF               0x0000004C  /*!< \brief Programmable clock 3 register offset. */
#define PMC_PCKR3   (PMC_BASE + PMC_PCKR3_OFF)  /*!< \brief Programmable clock 3 register address. */

#define PMC_CSS                     0x00000003  /*!< \brief Clock selection mask. */
#define PMC_CSS_SLOW_CLK            0x00000000  /*!< \brief Slow clock selected. */
#define PMC_CSS_MAIN_CLK            0x00000001  /*!< \brief Main clock selected. */
#if defined (PMC_HAS_PLLB)
#define PMC_CSS_PLLA_CLK            0x00000002  /*!< \brief PLL A clock selected. */
#define PMC_CSS_PLLB_CLK            0x00000003  /*!< \brief PLL B clock selected. */
#else
#define PMC_CSS_PLL_CLK             0x00000003  /*!< \brief PLL clock selected. */
#endif
#define PMC_PRES                    0x0000001C  /*!< \brief Clock prescaler mask. */
#define PMC_PRES_LSB                        2   /*!< \brief Clock prescaler LSB. */
#define PMC_PRES_CLK                0x00000000  /*!< \brief Selected clock, not divided. */
#define PMC_PRES_CLK_2              0x00000004  /*!< \brief Selected clock divided by 2. */
#define PMC_PRES_CLK_4              0x00000008  /*!< \brief Selected clock divided by 4. */
#define PMC_PRES_CLK_8              0x0000000C  /*!< \brief Selected clock divided by 8. */
#define PMC_PRES_CLK_16             0x00000010  /*!< \brief Selected clock divided by 16. */
#define PMC_PRES_CLK_32             0x00000014  /*!< \brief Selected clock divided by 32. */
#define PMC_PRES_CLK_64             0x00000018  /*!< \brief Selected clock divided by 64. */
#if defined (PMC_HAS_MDIV)
#define PMC_MDIV                    0x00000300  /*!< \brief Master clock division mask. */
#define PMC_MDIV_1                  0x00000000  /*!< \brief Processor clock, not divided. */
#define PMC_MDIV_2                  0x00000100  /*!< \brief Processor clock divided by 2. */
#define PMC_MDIV_4                  0x00000200  /*!< \brief Processor clock divided by 4. */
#define PMC_MDIV_3                  0x00000300  /*!< \brief Processor clock divided by 3. */
#ifdef MCU_AT91SAM9G45
#define PMC_PLLADIV2                0x00001000  /*!< \brief PLLA clock frequency devided by 2. */
#endif
#endif
/*@}*/

/*! \name Power Management Status and Interrupt Registers */
/*@{*/
#define PMC_IER_OFF                 0x00000060  /*!< \brief Interrupt enable register offset. */
#define PMC_IER     (PMC_BASE + PMC_IER_OFF)    /*!< \brief Interrupt enable register address. */
#define PMC_IDR_OFF                 0x00000064  /*!< \brief Interrupt disable register offset. */
#define PMC_IDR     (PMC_BASE + PMC_IDR_OFF)    /*!< \brief Interrupt disable register address. */
#define PMC_SR_OFF                  0x00000068  /*!< \brief Status register offset. */
#define PMC_SR      (PMC_BASE + PMC_SR_OFF)     /*!< \brief Status register address. */
#define PMC_IMR_OFF                 0x0000006C  /*!< \brief Interrupt mask register offset. */
#define PMC_IMR     (PMC_BASE + PMC_IMR_OFF)    /*!< \brief Interrupt mask register address. */

#define PMC_MOSCS                   0x00000001  /*!< \brief Main oscillator. */
#if defined (PMC_HAS_PLLB)
#define PMC_LOCKA                   0x00000002  /*!< \brief PLL A lock. */
#define PMC_LOCKB                   0x00000004  /*!< \brief PLL B lock. */
#else
#define PMC_LOCK                    0x00000004  /*!< \brief PLL lock. */
#endif
#define PMC_MCKRDY                  0x00000008  /*!< \brief Master clock ready. */
#define PMC_OSC_SEL                 0x00000080  /*!< \brief 32 kHz external slow clock. */
#define PMC_PCKRDY0                 0x00000100  /*!< \brief Programmable clock 0 ready. */
#define PMC_PCKRDY1                 0x00000200  /*!< \brief Programmable clock 1 ready. */
#define PMC_PCKRDY2                 0x00000400  /*!< \brief Programmable clock 2 ready. */
#define PMC_PCKRDY3                 0x00000800  /*!< \brief Programmable clock 3 ready. */
/*@}*/

/*! \name Power Management Charge Pump Current Register */
/*@{*/
#define PMC_PLLICPR_OFF             0x00000080  /*!< \brief Charge pump current register offset. */
#define PMC_PLLICPR (PMC_BASE + PMC_PLLICPR_OFF)/*!< \brief Charge pump current register address. */
#define PMC_PLLICPR_ICPPLLA         0x00000001  /*!< \brief PLL A charge pump current. */
#define PMC_PLLICPR_ICPPLLB         0x00000100  /*!< \brief PLL B charge pump current. */
/*@}*/

/*@} xgNutArchArmAt91Pmc */


#endif                          /* _ARCH_ARM_AT91_PMC_H_ */
