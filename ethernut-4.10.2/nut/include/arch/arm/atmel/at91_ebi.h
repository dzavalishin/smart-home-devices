#ifndef _ARCH_ARM_AT91_EBI_H_
#define _ARCH_ARM_AT91_EBI_H_

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
 * \file arch/arm/at91_ebi.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/02/15 16:59:42  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.1  2006/07/05 07:45:25  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Ebi
 */
/*@{*/

#if defined(EBI_HAS_CSA)

#define EBI_CSA_OFF             0x00000000      /*!< \brief Chip select assignment register offset. */
#define EBI_CSA     (EBI_BASE + EBI_CSA_OFF)    /*!< \brief Chip select assignment register address. */

#define EBI_CS1A                0x00000002      /*!< \brief Chip select 1 assignment (SDRAM=1). */
#define EBI_CS2A                0x00000004      /*!< \brief Chip select 2 assignment (CF2=1). */
#define EBI_CS3A                0x00000008      /*!< \brief Chip select 3 assignment (NAND=1). */
#define EBI_CS4A                0x00000010      /*!< \brief Chip select 4 assignment (CF1=1). */
#define EBI_CS5A                0x00000020      /*!< \brief Chip select 5 assignment (CF2=1). */
#define EBI_DBPUC               0x00000100      /*!< \brief Data bus pull-up disable. */
#define EBI_NWPC                0x00010000      /*!< \brief NWAIT pin enable. */
#define EBI_DRIVE               0x00030000      /*!< \brief I/O drive configuration mask. */
#define EBI_DRIVE_18L           0x00000000      /*!< \brief Optimized for 1.8V with low drive. */
#define EBI_DRIVE_33L           0x00010000      /*!< \brief Optimized for 3.3V with low drive. */
#define EBI_DRIVE_18H           0x00020000      /*!< \brief Optimized for 1.8V with high drive. */
#define EBI_DRIVE_33H           0x00030000      /*!< \brief Optimized for 3.3V with high drive. */

#else /* EBI_HAS_CSA */

/*! \name Chip Select Register */
/*@{*/
#define EBI_CSR(i)      (EBI_BASE + i * 4)      /*!< \brief Chip select register address. */

#define EBI_DBW                 0x00000003      /*!< \brief Masks data bus width. */
#define EBI_DBW_16              0x00000001      /*!< \brief 16-bit data bus width. */
#define EBI_DBW_8               0x00000002      /*!< \brief 8-bit data bus width. */

#define EBI_NWS                 0x0000001C      /*!< \brief Masks number of wait states. */
#define EBI_NWS_1               0x00000000      /*!< \brief 1 wait state. */
#define EBI_NWS_2               0x00000004      /*!< \brief 2 wait states. */
#define EBI_NWS_3               0x00000008      /*!< \brief 3 wait states. */
#define EBI_NWS_4               0x0000000C      /*!< \brief 4 wait states. */
#define EBI_NWS_5               0x00000010      /*!< \brief 5 wait states. */
#define EBI_NWS_6               0x00000014      /*!< \brief 6 wait states. */
#define EBI_NWS_7               0x00000018      /*!< \brief 7 wait states. */
#define EBI_NWS_8               0x0000001C      /*!< \brief 8 wait states. */

#define EBI_WSE                 0x00000020      /*!< \brief Wait state enable. */

#define EBI_PAGES               0x00000180      /*!< \brief Page size mask. */
#define EBI_PAGES_1M            0x00000000      /*!< \brief 1 MByte page size. */
#define EBI_PAGES_4M            0x00000080      /*!< \brief 4 MBytes page size. */
#define EBI_PAGES_16M           0x00000100      /*!< \brief 16 MBytes page size. */
#define EBI_PAGES_64M           0x00000180      /*!< \brief 64 MBytes page size. */

#define EBI_TDF                 0x00000E00      /*!< \brief Masks data float output time clock cycles. */
#define EBI_TDF_0               0x00000000      /*!< \brief No added cycles. */
#define EBI_TDF_1               0x00000200      /*!< \brief 1 cycle. */
#define EBI_TDF_2               0x00000400      /*!< \brief 2 cycles. */
#define EBI_TDF_3               0x00000600      /*!< \brief 3 cycles. */
#define EBI_TDF_4               0x00000800      /*!< \brief 4 cycles. */
#define EBI_TDF_5               0x00000A00      /*!< \brief 5 cycles. */
#define EBI_TDF_6               0x00000C00      /*!< \brief 6 cycles. */
#define EBI_TDF_7               0x00000E00      /*!< \brief 7 cycles. */

#define EBI_BAT                 0x00001000      /*!< \brief Byte access type */
#define EBI_BAT_BYTE_WRITE      0x00000000      /*!< \brief Byte write access type */
#define EBI_BAT_BYTE_SELECT     0x00001000      /*!< \brief Byte select access type */

#define EBI_CSEN                0x00002000      /*!< \brief Chip select enable */

#define EBI_BA                  0xFFF00000      /*!< \brief Page base address mask. */
/*@}*/

/*! \name Remap Control Register */
/*@{*/
#define EBI_RCR         (EBI_BASE + 0x20)       /*!< \brief Remap control register address. */
#define EBI_RCB                 0x00000001      /*!< \brief Remap command. */
/*@}*/

/*! \name Memory Control Register */
/*@{*/
#define EBI_MCR         (EBI_BASE + 0x24)       /*!< \brief Memory control register address. */
#define EBI_ALE                 0x00000007      /*!< \brief Address line enable */
#define EBI_ALE_16M             0x00000000      /*!< \brief 16 Mbytes total address space. */
#define EBI_ALE_8M              0x00000004      /*!< \brief 8 Mbytes total address space. */
#define EBI_ALE_4M              0x00000005      /*!< \brief 4 Mbytes total address space. */
#define EBI_ALE_2M              0x00000006      /*!< \brief 2 Mbytes total address space. */
#define EBI_ALE_1M              0x00000007      /*!< \brief 1 Mbyte total address space. */

#define EBI_DRP                 0x00000010      /*!< \brief Data read protocol mask. */
#define EBI_DRP_STANDARD        0x00000000      /*!< \brief Standard read protocol. */
#define EBI_DRP_EARLY           0x00000010      /*!< \brief Early read protocol. */
/*@}*/

#endif

/*@} xgNutArchArmAt91Ebi */

#endif                          /* _ARCH_ARM_AT91_EBI_H_ */
