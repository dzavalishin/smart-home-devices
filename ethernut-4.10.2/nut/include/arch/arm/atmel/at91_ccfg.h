#ifndef _ARCH_ARM_AT91_CCFG_H_
#define _ARCH_ARM_AT91_CCFG_H_

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
 * \file arch/arm/at91_ccfg.h
 * \brief AT91 chip configuration.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.1  2006/08/31 19:10:37  haraldkipp
 * New peripheral register definitions for the AT91SAM9260.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Ccfg
 */
/*@{*/


/*! \name Chip Select Assignment Register */
/*@{*/
#define CCFG_CSA_OFF                0x0000000C  /*!< \brief Chip select assignment register offset. */
#define CCFG_CSA    (CCFG_BASE + CCFG_CSA_OFF)  /*!< \brief Chip select assignment register address. */
#define CCFG_CS1A                   0x00000002  /*!< \brief SDRAM at chip select 1. */
#define CCFG_CS3A                   0x00000008  /*!< \brief SmartMedia at chip select 3. */
#define CCFG_CS4A                   0x00000010  /*!< \brief First CompactFlash slot at chip select 4. */
#define CCFG_CS5A                   0x00000020  /*!< \brief Second CompactFlash slot at chip select 5. */
#define CCFG_DBPUC                  0x00000100  /*!< \brief Data bus pull-ups disabled. */
#define CCFG_VDDIOMSEL              0x00010000  /*!< \brief 3.3V memory. */
/*@}*/

/*@} xgNutArchArmAt91Ccfg */

#endif                          /* _ARCH_ARM_AT91_CCFG_H_ */
