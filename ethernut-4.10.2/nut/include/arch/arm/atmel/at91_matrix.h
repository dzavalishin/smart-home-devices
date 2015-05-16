#ifndef _ARCH_ARM_AT91_MATRIX_H_
#define _ARCH_ARM_AT91_MATRIX_H_

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
 * \file arch/arm/at91_matrix.h
 * \brief AT91 bus matrix user interface.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2009/02/17 09:33:20  haraldkipp
 * Added master and slave indices as they appear on the SAM9260.
 * Not sure about other AT91 devices.
 *
 * Revision 1.1  2006/08/31 19:10:37  haraldkipp
 * New peripheral register definitions for the AT91SAM9260.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Matrix
 */
/*@{*/

/*! \name Master Configuration Registers */
/*@{*/
#define MATRIX_MCFG_OFF                 0x00000000      /*!< \brief Master configuration register offset. */
#define MATRIX_MCFG(i)  (MATRIX_BASE + MATRIX_MCFG_OFF + (i) * 4)       /*!< \brief Master configuration register addresses. */
/*@}*/

/*! \name Slave Configuration Registers */
/*@{*/
#define MATRIX_SCFG_OFF                 0x00000040      /*!< \brief Slave configuration register offset. */
#define MATRIX_SCFG(i)  (MATRIX_BASE + MATRIX_SCFG_OFF + (i) * 4)       /*!< \brief Slave configuration register addresses. */
/*@}*/

/*! \name Slave Priority Registers */
/*@{*/
#define MATRIX_PRAS_OFF                 0x00000080      /*!< \brief Slave priority register A offset. */
#define MATRIX_PRAS(i)  (MATRIX_BASE + MATRIX_PRAS_OFF + (i) * 8)       /*!< \brief Slave priority register A addresses. */
/*@}*/

/*! \name Master Remap Control Register */
/*@{*/
#define MATRIX_MRCR_OFF                 0x00000100      /*!< \brief Remap control register offset. */
#define MATRIX_MRCR (MATRIX_BASE + MATRIX_MRCR_OFF)     /*!< \brief Remap control register address. */
#define MATRIX_MRCR_RCB0                0x00000001      /*!< \brief Enable remap for master 0. */
#define MATRIX_MRCR_RCB1                0x00000002      /*!< \brief Enable remap for master 1. */
/*@}*/

#define MATRIX_MASTER_I                 0
#define MATRIX_MASTER_D                 1
#define MATRIX_MASTER_PDC               2
#define MATRIX_MASTER_ISI               3
#define MATRIX_MASTER_EMAC              4
#define MATRIX_MASTER_USB               5

#define MATRIX_SLAVE_SRAM0              0
#define MATRIX_SLAVE_SRAM1              1
#define MATRIX_SLAVE_ROM_USB            2
#define MATRIX_SLAVE_EBI                3
#define MATRIX_SLAVE_PERIPHERALS        4

/*@} xgNutArchArmAt91Matrix */

#endif                          /* _ARCH_ARM_AT91_MATRIX_H_ */
