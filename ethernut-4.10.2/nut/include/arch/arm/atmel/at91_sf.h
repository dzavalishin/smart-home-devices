#ifndef _ARCH_ARM_AT91_SF_H_
#define _ARCH_ARM_AT91_SF_H_

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
 * \file arch/arm/at91_sf.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Sf
 */
/*@{*/

/*! \name Chip Identification Registers */
/*@{*/
#define SF_CIDR         (SF_BASE + 0x00)        /*!< \brief Chip ID register address. */
#define SF_EXID         (SF_BASE + 0x04)        /*!< \brief Chip ID extension register address. */
#define SF_VERSION              0x0000001F      /*!< \brief Version number mask. */

#define SF_NVPSIZ               0x00000F00      /*!< \brief Masks non-volatile program memory size. */
#define SF_NVPSIZ_NONE          0x00000000      /*!< \brief No NV program memory. */
#define SF_NVPSIZ_32K           0x00000300      /*!< \brief 32 kBytes NV program memory. */
#define SF_NVPSIZ_64K           0x00000500      /*!< \brief 64 kBytes NV program memory. */
#define SF_NVPSIZ_128K          0x00000700      /*!< \brief 128 kBytes NV program memory. */
#define SF_NVPSIZ_256K          0x00000900      /*!< \brief 256 kBytes NV program memory. */

#define SF_NVDSIZ               0x0000F000      /*!< \brief Masks non-volatile data memory size. */
#define SF_NVDSIZ_NONE          0x00000000      /*!< \brief No NV data memory. */

#define SF_VDSIZ                0x000F0000      /*!< \brief Masks volatile data memory size. */
#define SF_VDSIZ_NONE           0x00000000      /*!< \brief No volatile data memory. */
#define SF_VDSIZ_1K             0x00010000      /*!< \brief 1 kBytes volatile data memory. */
#define SF_VDSIZ_2K             0x00020000      /*!< \brief 2 kBytes volatile data memory. */
#define SF_VDSIZ_4K             0x00040000      /*!< \brief 4 kBytes volatile data memory. */
#define SF_VDSIZ_8K             0x00080000      /*!< \brief 8 kBytes volatile data memory. */

#define SF_ARCH                 0x0FF00000      /*!< \brief Architecture code mask. */
#define SF_ARCH_AT91x40         0x04000000      /*!< \brief AT91x40 architecture. */
#define SF_ARCH_AT91x55         0x05500000      /*!< \brief AT91x55 architecture. */
#define SF_ARCH_AT91x63         0x06300000      /*!< \brief AT91x63 architecture. */

#define SF_NVPTYP               0x70000000      /*!< \brief Masks non-volatile program memory type. */
#define SF_NVPTYP_M             0x01000000      /*!< \brief M or F series. */
#define SF_NVPTYP_C             0x02000000      /*!< \brief C series. */
#define SF_NVPTYP_S             0x03000000      /*!< \brief S series. */
#define SF_NVPTYP_R             0x04000000      /*!< \brief R series. */

#define SF_EXT                  0x80000000      /*!< \brief Extension flag. */

/*@}*/

/*! \name Reset Status Flag Register */
/*@{*/
#define SF_RSR          (SF_BASE + 0x08)        /*!< \brief Reset status register address. */
#define SF_EXT_RESET            0x0000006C      /*!< \brief Reset caused by external pin. */
#define SF_WD_RESET             0x00000053      /*!< \brief Reset caused by internal watch dog. */
/*@}*/

/*! \name Memory Mode Register */
/*@{*/
#define SF_MMR          (SF_BASE + 0x0C)        /*!< \brief Memory mode register address. */
#define SF_RAMWU                0x00000001      /*!< \brief Internal extended RAM write allowed. */
/*@}*/

/*! \name Protect Mode Register */
/*@{*/
#define SF_PMR          (SF_BASE + 0x18)        /*!< \brief Protect mode register address. */
#define SF_AIC                  0x00000020      /*!< \brief AIC runs in protect mode. */
/*@}*/

/*@} xgNutArchArmAt91Sf */

#endif                          /* _ARCH_ARM_AT91_SF_H_ */

