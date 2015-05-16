#ifndef _ARCH_ARM_AT91_MC_H_
#define _ARCH_ARM_AT91_MC_H_

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
 * \file arch/arm/at91_mc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2006/07/26 11:21:35  haraldkipp
 * Made it usable for assembler.
 *
 * Revision 1.2  2006/07/18 14:04:55  haraldkipp
 * Base address removed. Should be specified in the upper level header.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

#define MC_RCR_OFF              0x00000000      /*!< \brief MC remap control register offset. */
#define MC_RCR      (MC_BASE + MC_RCR_OFF)      /*!< \brief MC remap control register address. */
#define MC_RCB                  0x00000001      /*!< \brief Remap command. */

#define MC_ASR_OFF              0x00000004      /*!< \brief MC abort status register offset. */
#define MC_ASR      (MC_BASE + MC_ASR_OFF)      /*!< \brief MC abort status register address. */
#define MC_UNDADD               0x00000001      /*!< \brief Undefined Addess Abort status. */
#define MC_MISADD               0x00000002      /*!< \brief Misaligned Addess Abort status. */
#define MC_ABTSZ_MASK           0x00000300      /*!< \brief Abort size status mask. */
#define MC_ABTSZ_BYTE           0x00000000      /*!< \brief Byte size abort. */
#define MC_ABTSZ_HWORD          0x00000100      /*!< \brief Half-word size abort. */
#define MC_ABTSZ_WORD           0x00000200      /*!< \brief Word size abort. */
#define MC_ABTTYP_MASK          0x00000C00      /*!< \brief Abort type status mask. */
#define MC_ABTTYP_DATAR         0x00000000      /*!< \brief Data read abort. */
#define MC_ABTTYP_DATAW         0x00000400      /*!< \brief Data write abort. */
#define MC_ABTTYP_FETCH         0x00000800      /*!< \brief Code fetch abort. */
#define MC_MST_EMAC             0x00010000      /*!< \brief EMAC abort source. */
#define MC_MST_PDC              0x00020000      /*!< \brief PDC abort source. */
#define MC_MST_ARM              0x00040000      /*!< \brief ARM abort source. */
#define MC_SVMST_EMAC           0x01000000      /*!< \brief Saved EMAC abort source. */
#define MC_SVMST_PDC            0x02000000      /*!< \brief Saved PDC abort source. */
#define MC_SVMST_ARM            0x04000000      /*!< \brief Saved ARM abort source. */

#define MC_AASR_OFF             0x00000008      /*!< \brief MC abort address status register offset. */
#define MC_AASR     (MC_BASE + MC_AASR_OFF)     /*!< \brief MC abort address status register address. */

#define MC_FMR_EFC0_OFF         0x00000060      /*!< \brief MC flash mode register bank 0 offset. */
#define MC_FMR_EFC0 (MC_BASE + MC_FMR_EFC0_OFF) /*!< \brief MC flash mode register bank 0 address. */
#define MC_FMR_EFC1_OFF         0x00000070      /*!< \brief MC flash mode register bank 1 offset. */
#define MC_FMR_EFC1 (MC_BASE + MC_FMR_EFC1_OFF) /*!< \brief MC flash mode register bank 1 address. */

#define MC_FMR_OFF         MC_FMR_EFC0_OFF      /*!< \brief MC flash mode register offset. */
#define MC_FMR                 MC_FMR_EFC0      /*!< \brief MC flash mode register address. */
#define MC_FRDY                 0x00000001      /*!< \brief Flash ready. */
#define MC_LOCKE                0x00000004      /*!< \brief Lock error. */
#define MC_PROGE                0x00000008      /*!< \brief Programming error. */
#define MC_NEBP                 0x00000080      /*!< \brief No erase before programming. */
#define MC_FWS_LSB                       8      /*!< \brief Flash wait state LSB. */
#define MC_FWS_MASK             0x00000300      /*!< \brief Flash wait state mask. */
#define MC_FWS_1R2W             0x00000000      /*!< \brief 1 cycle for read, 2 for write operations. */
#define MC_FWS_2R3W             0x00000100      /*!< \brief 2 cycles for read, 3 for write operations. */
#define MC_FWS_3R4W             0x00000200      /*!< \brief 3 cycles for read, 4 for write operations. */
#define MC_FWS_4R4W             0x00000300      /*!< \brief 4 cycles for read and write operations. */
#define MC_FMCN_LSB                     16      /*!< \brief Flash microsecond cycle number LSB. */
#define MC_FMCN_MASK            0x00FF0000      /*!< \brief Flash microsecond cycle number mask. */

#define MC_FCR_EFC0_OFF         0x00000064      /*!< \brief MC flash mode register bank 0 offset. */
#define MC_FCR_EFC0 (MC_BASE + MC_FCR_EFC0_OFF) /*!< \brief MC flash mode register bank 0 address. */
#define MC_FCR_EFC1_OFF         0x00000074      /*!< \brief MC flash mode register bank 1 offset. */
#define MC_FCR_EFC1 (MC_BASE + MC_FCR_EFC1_OFF) /*!< \brief MC flash mode register bank 1 address. */

#define MC_FCR_OFF         MC_FCR_EFC0_OFF      /*!< \brief MC flash command register offset. */
#define MC_FCR                 MC_FCR_EFC0      /*!< \brief MC flash command register address. */
#define MC_FCMD_MASK            0x0000000F      /*!< \brief Flash command mask. */
#define MC_FCMD_NOP             0x00000000      /*!< \brief No command. */
#define MC_FCMD_WP              0x00000001      /*!< \brief Write page. */
#define MC_FCMD_SLB             0x00000002      /*!< \brief Set lock bit. */
#define MC_FCMD_WPL             0x00000003      /*!< \brief Write page and lock. */
#define MC_FCMD_CLB             0x00000004      /*!< \brief Clear lock bit. */
#define MC_FCMD_EA              0x00000008      /*!< \brief Erase all. */
#define MC_FCMD_SGPB            0x0000000B      /*!< \brief Set general purpose NVM bit. */
#define MC_FCMD_CGPB            0x0000000D      /*!< \brief Clear general purpose NVM bit. */
#define MC_FCMD_SSB             0x0000000F      /*!< \brief Set security bit. */
#define MC_PAGEN_MASK           0x0003FF00      /*!< \brief Page number mask. */
#define MC_KEY                  0x5A000000      /*!< \brief Writing protect key. */

#define MC_FSR_EFC0_OFF         0x00000068      /*!< \brief MC flash mode register bank 0 offset. */
#define MC_FSR_EFC0 (MC_BASE + MC_FSR_EFC0_OFF) /*!< \brief MC flash mode register bank 0 address. */
#define MC_FSR_EFC1_OFF         0x00000078      /*!< \brief MC flash mode register bank 1 offset. */
#define MC_FSR_EFC1 (MC_BASE + MC_FSR_EFC1_OFF) /*!< \brief MC flash mode register bank 1 address. */

#define MC_FSR_OFF         MC_FSR_EFC0_OFF      /*!< \brief MC flash status register offset. */
#define MC_FSR                 MC_FSR_EFC0      /*!< \brief MC flash status register address. */
#define MC_SECURITY             0x00000010      /*!< \brief Security bit status. */

#define MC_GPNVM0               0x00000100      /*!< \brief General purpose NVM bit 0. */
#define MC_GPNVM1               0x00000200      /*!< \brief General purpose NVM bit 1. */
#define MC_GPNVM2               0x00000400      /*!< \brief General purpose NVM bit 2. */

#define MC_LOCKS0               0x00010000      /*!< \brief Lock region 0 lock status. */
#define MC_LOCKS1               0x00020000      /*!< \brief Lock region 1 lock status. */
#define MC_LOCKS2               0x00040000      /*!< \brief Lock region 2 lock status. */
#define MC_LOCKS3               0x00080000      /*!< \brief Lock region 3 lock status. */
#define MC_LOCKS4               0x00100000      /*!< \brief Lock region 4 lock status. */
#define MC_LOCKS5               0x00200000      /*!< \brief Lock region 5 lock status. */
#define MC_LOCKS6               0x00400000      /*!< \brief Lock region 6 lock status. */
#define MC_LOCKS7               0x00800000      /*!< \brief Lock region 7 lock status. */
#define MC_LOCKS8               0x01000000      /*!< \brief Lock region 8 lock status. */
#define MC_LOCKS9               0x02000000      /*!< \brief Lock region 9 lock status. */
#define MC_LOCKS10              0x04000000      /*!< \brief Lock region 10 lock status. */
#define MC_LOCKS11              0x08000000      /*!< \brief Lock region 11 lock status. */
#define MC_LOCKS12              0x10000000      /*!< \brief Lock region 12 lock status. */
#define MC_LOCKS13              0x20000000      /*!< \brief Lock region 13 lock status. */
#define MC_LOCKS14              0x40000000      /*!< \brief Lock region 14 lock status. */
#define MC_LOCKS15              0x80000000      /*!< \brief Lock region 15 lock status. */

#endif                          /* _ARCH_ARM_AT91_MC_H_ */
