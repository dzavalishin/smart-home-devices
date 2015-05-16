#ifndef _ARCH_ARM_AT91_PIO_H_
#define _ARCH_ARM_AT91_PIO_H_

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
 * \file arch/arm/at91_pio.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2006/09/29 12:43:08  haraldkipp
 * Register offsets added, which are quite useful for assembler programming.
 * Added some special PIO features, which are available on the AT91SAM92xx
 * series.
 *
 * Revision 1.2  2006/08/31 19:11:18  haraldkipp
 * Added register definitions for PIOC.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Pio
 */
/*@{*/

/*! \name PIO Register Offsets */
/*@{*/
#define PIO_PER_OFF     0x00000000  /*!< \brief PIO enable register offset. */
#define PIO_PDR_OFF     0x00000004  /*!< \brief PIO disable register offset. */
#define PIO_PSR_OFF     0x00000008  /*!< \brief PIO status register offset. */
#define PIO_OER_OFF     0x00000010  /*!< \brief Output enable register offset. */
#define PIO_ODR_OFF     0x00000014  /*!< \brief Output disable register offset. */
#define PIO_OSR_OFF     0x00000018  /*!< \brief Output status register offset. */
#define PIO_IFER_OFF    0x00000020  /*!< \brief Input filter enable register offset. */
#define PIO_IFDR_OFF    0x00000024  /*!< \brief Input filter disable register offset. */
#define PIO_IFSR_OFF    0x00000028  /*!< \brief Input filter status register offset. */
#define PIO_SODR_OFF    0x00000030  /*!< \brief Set output data register offset. */
#define PIO_CODR_OFF    0x00000034  /*!< \brief Clear output data register offset. */
#define PIO_ODSR_OFF    0x00000038  /*!< \brief Output data status register offset. */
#define PIO_PDSR_OFF    0x0000003C  /*!< \brief Pin data status register offset. */
#define PIO_IER_OFF     0x00000040  /*!< \brief Interrupt enable register offset. */
#define PIO_IDR_OFF     0x00000044  /*!< \brief Interrupt disable register offset. */
#define PIO_IMR_OFF     0x00000048  /*!< \brief Interrupt mask register offset. */
#define PIO_ISR_OFF     0x0000004C  /*!< \brief Interrupt status register offset. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIO_MDER_OFF    0x00000050  /*!< \brief Multi-driver enable register offset. */
#define PIO_MDDR_OFF    0x00000054  /*!< \brief Multi-driver disable register offset. */
#define PIO_MDSR_OFF    0x00000058  /*!< \brief Multi-driver status register offset. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIO_PUDR_OFF    0x00000060  /*!< \brief Pull-up disable register offset. */
#define PIO_PUER_OFF    0x00000064  /*!< \brief Pull-up enable register offset. */
#define PIO_PUSR_OFF    0x00000068  /*!< \brief Pull-up status register offset. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIO_ASR_OFF     0x00000070  /*!< \brief PIO peripheral A select register offset. */
#define PIO_BSR_OFF     0x00000074  /*!< \brief PIO peripheral B select register offset. */
#define PIO_ABSR_OFF    0x00000078  /*!< \brief PIO peripheral AB status register offset. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIO_OWER_OFF    0x000000A0  /*!< \brief PIO output write enable register offset. */
#define PIO_OWDR_OFF    0x000000A4  /*!< \brief PIO output write disable register offset. */
#define PIO_OWSR_OFF    0x000000A8  /*!< \brief PIO output write status register offset. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
/*@}*/

/*! \name Single PIO Register Addresses */
/*@{*/
#if defined(PIO_BASE)
#define PIO_PER     (PIO_BASE + PIO_PER_OFF)    /*!< \brief PIO enable register address. */
#define PIO_PDR     (PIO_BASE + PIO_PDR_OFF)    /*!< \brief PIO disable register address. */
#define PIO_PSR     (PIO_BASE + PIO_PSR_OFF)    /*!< \brief PIO status register address. */
#define PIO_OER     (PIO_BASE + PIO_OER_OFF)    /*!< \brief Output enable register address. */
#define PIO_ODR     (PIO_BASE + PIO_ODR_OFF)    /*!< \brief Output disable register address. */
#define PIO_OSR     (PIO_BASE + PIO_OSR_OFF)    /*!< \brief Output status register address. */
#define PIO_IFER    (PIO_BASE + PIO_IFER_OFF)   /*!< \brief Input filter enable register address. */
#define PIO_IFDR    (PIO_BASE + PIO_IFDR_OFF)   /*!< \brief Input filter disable register address. */
#define PIO_IFSR    (PIO_BASE + PIO_IFSR_OFF)   /*!< \brief Input filter status register address. */
#define PIO_SODR    (PIO_BASE + PIO_SODR_OFF)   /*!< \brief Set output data register address. */
#define PIO_CODR    (PIO_BASE + PIO_CODR_OFF)   /*!< \brief Clear output data register address. */
#define PIO_ODSR    (PIO_BASE + PIO_ODSR_OFF)   /*!< \brief Output data status register address. */
#define PIO_PDSR    (PIO_BASE + PIO_PDSR_OFF)   /*!< \brief Pin data status register address. */
#define PIO_IER     (PIO_BASE + PIO_IER_OFF)    /*!< \brief Interrupt enable register address. */
#define PIO_IDR     (PIO_BASE + PIO_IDR_OFF)    /*!< \brief Interrupt disable register address. */
#define PIO_IMR     (PIO_BASE + PIO_IMR_OFF)    /*!< \brief Interrupt mask register address. */
#define PIO_ISR     (PIO_BASE + PIO_ISR_OFF)    /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIO_MDER    (PIO_BASE + PIO_MDER_OFF)   /*!< \brief Multi-driver enable register address. */
#define PIO_MDDR    (PIO_BASE + PIO_MDDR_OFF)   /*!< \brief Multi-driver disable register address. */
#define PIO_MDSR    (PIO_BASE + PIO_MDSR_OFF)   /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIO_PUDR    (PIO_BASE + PIO_PUDR_OFF)   /*!< \brief Pull-up disable register address. */
#define PIO_PUER    (PIO_BASE + PIO_PUER_OFF)   /*!< \brief Pull-up enable register address. */
#define PIO_PUSR    (PIO_BASE + PIO_PUSR_OFF)   /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIO_ASR     (PIO_BASE + PIO_ASR_OFF)    /*!< \brief PIO peripheral A select register address. */
#define PIO_BSR     (PIO_BASE + PIO_BSR_OFF)    /*!< \brief PIO peripheral B select register address. */
#define PIO_ABSR    (PIO_BASE + PIO_ABSR_OFF)   /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIO_OWER    (PIO_BASE + PIO_OWER_OFF)   /*!< \brief PIO output write enable register address. */
#define PIO_OWDR    (PIO_BASE + PIO_OWDR_OFF)   /*!< \brief PIO output write disable register address. */
#define PIO_OWSR    (PIO_BASE + PIO_OWSR_OFF)   /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIO_BASE */
/*@}*/

/*! \name PIO A Register Addresses */
/*@{*/
#if defined(PIOA_BASE)
#define PIOA_PER    (PIOA_BASE + PIO_PER_OFF)   /*!< \brief PIO enable register address. */
#define PIOA_PDR    (PIOA_BASE + PIO_PDR_OFF)   /*!< \brief PIO disable register address. */
#define PIOA_PSR    (PIOA_BASE + PIO_PSR_OFF)   /*!< \brief PIO status register address. */
#define PIOA_OER    (PIOA_BASE + PIO_OER_OFF)   /*!< \brief Output enable register address. */
#define PIOA_ODR    (PIOA_BASE + PIO_ODR_OFF)   /*!< \brief Output disable register address. */
#define PIOA_OSR    (PIOA_BASE + PIO_OSR_OFF)   /*!< \brief Output status register address. */
#define PIOA_IFER   (PIOA_BASE + PIO_IFER_OFF)  /*!< \brief Input filter enable register address. */
#define PIOA_IFDR   (PIOA_BASE + PIO_IFDR_OFF)  /*!< \brief Input filter disable register address. */
#define PIOA_IFSR   (PIOA_BASE + PIO_IFSR_OFF)  /*!< \brief Input filter status register address. */
#define PIOA_SODR   (PIOA_BASE + PIO_SODR_OFF)  /*!< \brief Set output data register address. */
#define PIOA_CODR   (PIOA_BASE + PIO_CODR_OFF)  /*!< \brief Clear output data register address. */
#define PIOA_ODSR   (PIOA_BASE + PIO_ODSR_OFF)  /*!< \brief Output data status register address. */
#define PIOA_PDSR   (PIOA_BASE + PIO_PDSR_OFF)  /*!< \brief Pin data status register address. */
#define PIOA_IER    (PIOA_BASE + PIO_IER_OFF)   /*!< \brief Interrupt enable register address. */
#define PIOA_IDR    (PIOA_BASE + PIO_IDR_OFF)   /*!< \brief Interrupt disable register address. */
#define PIOA_IMR    (PIOA_BASE + PIO_IMR_OFF)   /*!< \brief Interrupt mask register address. */
#define PIOA_ISR    (PIOA_BASE + PIO_ISR_OFF)   /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIOA_MDER   (PIOA_BASE + PIO_MDER_OFF)  /*!< \brief Multi-driver enable register address. */
#define PIOA_MDDR   (PIOA_BASE + PIO_MDDR_OFF)  /*!< \brief Multi-driver disable register address. */
#define PIOA_MDSR   (PIOA_BASE + PIO_MDSR_OFF)  /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIOA_PUDR   (PIOA_BASE + PIO_PUDR_OFF)  /*!< \brief Pull-up disable register address. */
#define PIOA_PUER   (PIOA_BASE + PIO_PUER_OFF)  /*!< \brief Pull-up enable register address. */
#define PIOA_PUSR   (PIOA_BASE + PIO_PUSR_OFF)  /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIOA_ASR    (PIOA_BASE + PIO_ASR_OFF)   /*!< \brief PIO peripheral A select register address. */
#define PIOA_BSR    (PIOA_BASE + PIO_BSR_OFF)   /*!< \brief PIO peripheral B select register address. */
#define PIOA_ABSR   (PIOA_BASE + PIO_ABSR_OFF)  /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIOA_OWER   (PIOA_BASE + PIO_OWER_OFF)  /*!< \brief PIO output write enable register address. */
#define PIOA_OWDR   (PIOA_BASE + PIO_OWDR_OFF)  /*!< \brief PIO output write disable register address. */
#define PIOA_OWSR   (PIOA_BASE + PIO_OWSR_OFF)  /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOA_BASE */
/*@}*/

/*! \name PIO B Register Addresses */
/*@{*/
#if defined(PIOB_BASE)
#define PIOB_PER    (PIOB_BASE + PIO_PER_OFF)   /*!< \brief PIO enable register address. */
#define PIOB_PDR    (PIOB_BASE + PIO_PDR_OFF)   /*!< \brief PIO disable register address. */
#define PIOB_PSR    (PIOB_BASE + PIO_PSR_OFF)   /*!< \brief PIO status register address. */
#define PIOB_OER    (PIOB_BASE + PIO_OER_OFF)   /*!< \brief Output enable register address. */
#define PIOB_ODR    (PIOB_BASE + PIO_ODR_OFF)   /*!< \brief Output disable register address. */
#define PIOB_OSR    (PIOB_BASE + PIO_OSR_OFF)   /*!< \brief Output status register address. */
#define PIOB_IFER   (PIOB_BASE + PIO_IFER_OFF)  /*!< \brief Input filter enable register address. */
#define PIOB_IFDR   (PIOB_BASE + PIO_IFDR_OFF)  /*!< \brief Input filter disable register address. */
#define PIOB_IFSR   (PIOB_BASE + PIO_IFSR_OFF)  /*!< \brief Input filter status register address. */
#define PIOB_SODR   (PIOB_BASE + PIO_SODR_OFF)  /*!< \brief Set output data register address. */
#define PIOB_CODR   (PIOB_BASE + PIO_CODR_OFF)  /*!< \brief Clear output data register address. */
#define PIOB_ODSR   (PIOB_BASE + PIO_ODSR_OFF)  /*!< \brief Output data status register address. */
#define PIOB_PDSR   (PIOB_BASE + PIO_PDSR_OFF)  /*!< \brief Pin data status register address. */
#define PIOB_IER    (PIOB_BASE + PIO_IER_OFF)   /*!< \brief Interrupt enable register address. */
#define PIOB_IDR    (PIOB_BASE + PIO_IDR_OFF)   /*!< \brief Interrupt disable register address. */
#define PIOB_IMR    (PIOB_BASE + PIO_IMR_OFF)   /*!< \brief Interrupt mask register address. */
#define PIOB_ISR    (PIOB_BASE + PIO_ISR_OFF)   /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIOB_MDER   (PIOB_BASE + PIO_MDER_OFF)  /*!< \brief Multi-driver enable register address. */
#define PIOB_MDDR   (PIOB_BASE + PIO_MDDR_OFF)  /*!< \brief Multi-driver disable register address. */
#define PIOB_MDSR   (PIOB_BASE + PIO_MDSR_OFF)  /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIOB_PUDR   (PIOB_BASE + PIO_PUDR_OFF)  /*!< \brief Pull-up disable register address. */
#define PIOB_PUER   (PIOB_BASE + PIO_PUER_OFF)  /*!< \brief Pull-up enable register address. */
#define PIOB_PUSR   (PIOB_BASE + PIO_PUSR_OFF)  /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIOB_ASR    (PIOB_BASE + PIO_ASR_OFF)   /*!< \brief PIO peripheral A select register address. */
#define PIOB_BSR    (PIOB_BASE + PIO_BSR_OFF)   /*!< \brief PIO peripheral B select register address. */
#define PIOB_ABSR   (PIOB_BASE + PIO_ABSR_OFF)  /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIOB_OWER   (PIOB_BASE + PIO_OWER_OFF)  /*!< \brief PIO output write enable register address. */
#define PIOB_OWDR   (PIOB_BASE + PIO_OWDR_OFF)  /*!< \brief PIO output write disable register address. */
#define PIOB_OWSR   (PIOB_BASE + PIO_OWSR_OFF)  /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOB_BASE */
/*@}*/

/*! \name PIO C Register Addresses */
/*@{*/
#if defined(PIOC_BASE)
#define PIOC_PER    (PIOC_BASE + PIO_PER_OFF)   /*!< \brief PIO enable register address. */
#define PIOC_PDR    (PIOC_BASE + PIO_PDR_OFF)   /*!< \brief PIO disable register address. */
#define PIOC_PSR    (PIOC_BASE + PIO_PSR_OFF)   /*!< \brief PIO status register address. */
#define PIOC_OER    (PIOC_BASE + PIO_OER_OFF)   /*!< \brief Output enable register address. */
#define PIOC_ODR    (PIOC_BASE + PIO_ODR_OFF)   /*!< \brief Output disable register address. */
#define PIOC_OSR    (PIOC_BASE + PIO_OSR_OFF)   /*!< \brief Output status register address. */
#define PIOC_IFER   (PIOC_BASE + PIO_IFER_OFF)  /*!< \brief Input filter enable register address. */
#define PIOC_IFDR   (PIOC_BASE + PIO_IFDR_OFF)  /*!< \brief Input filter disable register address. */
#define PIOC_IFSR   (PIOC_BASE + PIO_IFSR_OFF)  /*!< \brief Input filter status register address. */
#define PIOC_SODR   (PIOC_BASE + PIO_SODR_OFF)  /*!< \brief Set output data register address. */
#define PIOC_CODR   (PIOC_BASE + PIO_CODR_OFF)  /*!< \brief Clear output data register address. */
#define PIOC_ODSR   (PIOC_BASE + PIO_ODSR_OFF)  /*!< \brief Output data status register address. */
#define PIOC_PDSR   (PIOC_BASE + PIO_PDSR_OFF)  /*!< \brief Pin data status register address. */
#define PIOC_IER    (PIOC_BASE + PIO_IER_OFF)   /*!< \brief Interrupt enable register address. */
#define PIOC_IDR    (PIOC_BASE + PIO_IDR_OFF)   /*!< \brief Interrupt disable register address. */
#define PIOC_IMR    (PIOC_BASE + PIO_IMR_OFF)   /*!< \brief Interrupt mask register address. */
#define PIOC_ISR    (PIOC_BASE + PIO_ISR_OFF)   /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIOC_MDER   (PIOC_BASE + PIO_MDER_OFF)  /*!< \brief Multi-driver enable register address. */
#define PIOC_MDDR   (PIOC_BASE + PIO_MDDR_OFF)  /*!< \brief Multi-driver disable register address. */
#define PIOC_MDSR   (PIOC_BASE + PIO_MDSR_OFF)  /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIOC_PUDR   (PIOC_BASE + PIO_PUDR_OFF)  /*!< \brief Pull-up disable register address. */
#define PIOC_PUER   (PIOC_BASE + PIO_PUER_OFF)  /*!< \brief Pull-up enable register address. */
#define PIOC_PUSR   (PIOC_BASE + PIO_PUSR_OFF)  /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIOC_ASR    (PIOC_BASE + PIO_ASR_OFF)   /*!< \brief PIO peripheral A select register address. */
#define PIOC_BSR    (PIOC_BASE + PIO_BSR_OFF)   /*!< \brief PIO peripheral B select register address. */
#define PIOC_ABSR   (PIOC_BASE + PIO_ABSR_OFF)  /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIOC_OWER   (PIOC_BASE + PIO_OWER_OFF)  /*!< \brief PIO output write enable register address. */
#define PIOC_OWDR   (PIOC_BASE + PIO_OWDR_OFF)  /*!< \brief PIO output write disable register address. */
#define PIOC_OWSR   (PIOC_BASE + PIO_OWSR_OFF)  /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOC_BASE */
/*@}*/

/*! \name PIO D Register Addresses */
/*@{*/
#if defined(PIOD_BASE)
#define PIOD_PER    (PIOD_BASE + PIO_PER_OFF)   /*!< \brief PIO enable register address. */
#define PIOD_PDR    (PIOD_BASE + PIO_PDR_OFF)   /*!< \brief PIO disable register address. */
#define PIOD_PSR    (PIOD_BASE + PIO_PSR_OFF)   /*!< \brief PIO status register address. */
#define PIOD_OER    (PIOD_BASE + PIO_OER_OFF)   /*!< \brief Output enable register address. */
#define PIOD_ODR    (PIOD_BASE + PIO_ODR_OFF)   /*!< \brief Output disable register address. */
#define PIOD_OSR    (PIOD_BASE + PIO_OSR_OFF)   /*!< \brief Output status register address. */
#define PIOD_IFER   (PIOD_BASE + PIO_IFER_OFF)  /*!< \brief Input filter enable register address. */
#define PIOD_IFDR   (PIOD_BASE + PIO_IFDR_OFF)  /*!< \brief Input filter disable register address. */
#define PIOD_IFSR   (PIOD_BASE + PIO_IFSR_OFF)  /*!< \brief Input filter status register address. */
#define PIOD_SODR   (PIOD_BASE + PIO_SODR_OFF)  /*!< \brief Set output data register address. */
#define PIOD_CODR   (PIOD_BASE + PIO_CODR_OFF)  /*!< \brief Clear output data register address. */
#define PIOD_ODSR   (PIOD_BASE + PIO_ODSR_OFF)  /*!< \brief Output data status register address. */
#define PIOD_PDSR   (PIOD_BASE + PIO_PDSR_OFF)  /*!< \brief Pin data status register address. */
#define PIOD_IER    (PIOD_BASE + PIO_IER_OFF)   /*!< \brief Interrupt enable register address. */
#define PIOD_IDR    (PIOD_BASE + PIO_IDR_OFF)   /*!< \brief Interrupt disable register address. */
#define PIOD_IMR    (PIOD_BASE + PIO_IMR_OFF)   /*!< \brief Interrupt mask register address. */
#define PIOD_ISR    (PIOD_BASE + PIO_ISR_OFF)   /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIOD_MDER   (PIOD_BASE + PIO_MDER_OFF)  /*!< \brief Multi-driver enable register address. */
#define PIOD_MDDR   (PIOD_BASE + PIO_MDDR_OFF)  /*!< \brief Multi-driver disable register address. */
#define PIOD_MDSR   (PIOD_BASE + PIO_MDSR_OFF)  /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIOD_PUDR   (PIOD_BASE + PIO_PUDR_OFF)  /*!< \brief Pull-up disable register address. */
#define PIOD_PUER   (PIOD_BASE + PIO_PUER_OFF)  /*!< \brief Pull-up enable register address. */
#define PIOD_PUSR   (PIOD_BASE + PIO_PUSR_OFF)  /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIOD_ASR    (PIOD_BASE + PIO_ASR_OFF)   /*!< \brief PIO peripheral A select register address. */
#define PIOD_BSR    (PIOD_BASE + PIO_BSR_OFF)   /*!< \brief PIO peripheral B select register address. */
#define PIOD_ABSR   (PIOD_BASE + PIO_ABSR_OFF)  /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIOD_OWER   (PIOD_BASE + PIO_OWER_OFF)  /*!< \brief PIO output write enable register address. */
#define PIOD_OWDR   (PIOD_BASE + PIO_OWDR_OFF)  /*!< \brief PIO output write disable register address. */
#define PIOD_OWSR   (PIOD_BASE + PIO_OWSR_OFF)  /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOD_BASE */
/*@}*/


/*! \name PIO E Register Addresses */
/*@{*/
#if defined(PIOE_BASE)
#define PIOE_PER    (PIOE_BASE + PIO_PER_OFF)   /*!< \brief PIO enable register address. */
#define PIOE_PDR    (PIOE_BASE + PIO_PDR_OFF)   /*!< \brief PIO disable register address. */
#define PIOE_PSR    (PIOE_BASE + PIO_PSR_OFF)   /*!< \brief PIO status register address. */
#define PIOE_OER    (PIOE_BASE + PIO_OER_OFF)   /*!< \brief Output enable register address. */
#define PIOE_ODR    (PIOE_BASE + PIO_ODR_OFF)   /*!< \brief Output disable register address. */
#define PIOE_OSR    (PIOE_BASE + PIO_OSR_OFF)   /*!< \brief Output status register address. */
#define PIOE_IFER   (PIOE_BASE + PIO_IFER_OFF)  /*!< \brief Input filter enable register address. */
#define PIOE_IFDR   (PIOE_BASE + PIO_IFDR_OFF)  /*!< \brief Input filter disable register address. */
#define PIOE_IFSR   (PIOE_BASE + PIO_IFSR_OFF)  /*!< \brief Input filter status register address. */
#define PIOE_SODR   (PIOE_BASE + PIO_SODR_OFF)  /*!< \brief Set output data register address. */
#define PIOE_CODR   (PIOE_BASE + PIO_CODR_OFF)  /*!< \brief Clear output data register address. */
#define PIOE_ODSR   (PIOE_BASE + PIO_ODSR_OFF)  /*!< \brief Output data status register address. */
#define PIOE_PDSR   (PIOE_BASE + PIO_PDSR_OFF)  /*!< \brief Pin data status register address. */
#define PIOE_IER    (PIOE_BASE + PIO_IER_OFF)   /*!< \brief Interrupt enable register address. */
#define PIOE_IDR    (PIOE_BASE + PIO_IDR_OFF)   /*!< \brief Interrupt disable register address. */
#define PIOE_IMR    (PIOE_BASE + PIO_IMR_OFF)   /*!< \brief Interrupt mask register address. */
#define PIOE_ISR    (PIOE_BASE + PIO_ISR_OFF)   /*!< \brief Interrupt status register address. */
#if defined(PIO_HAS_MULTIDRIVER)
#define PIOE_MDER   (PIOE_BASE + PIO_MDER_OFF)  /*!< \brief Multi-driver enable register address. */
#define PIOE_MDDR   (PIOE_BASE + PIO_MDDR_OFF)  /*!< \brief Multi-driver disable register address. */
#define PIOE_MDSR   (PIOE_BASE + PIO_MDSR_OFF)  /*!< \brief Multi-driver status register address. */
#endif /* PIO_HAS_MULTIDRIVER */
#if defined(PIO_HAS_PULLUP)
#define PIOE_PUDR   (PIOE_BASE + PIO_PUDR_OFF)  /*!< \brief Pull-up disable register address. */
#define PIOE_PUER   (PIOE_BASE + PIO_PUER_OFF)  /*!< \brief Pull-up enable register address. */
#define PIOE_PUSR   (PIOE_BASE + PIO_PUSR_OFF)  /*!< \brief Pull-up status register address. */
#endif /* PIO_HAS_PULLUP */
#if defined(PIO_HAS_PERIPHERALSELECT)
#define PIOE_ASR    (PIOE_BASE + PIO_ASR_OFF)   /*!< \brief PIO peripheral A select register address. */
#define PIOE_BSR    (PIOE_BASE + PIO_BSR_OFF)   /*!< \brief PIO peripheral B select register address. */
#define PIOE_ABSR   (PIOE_BASE + PIO_ABSR_OFF)  /*!< \brief PIO peripheral AB status register address. */
#endif /* PIO_HAS_PERIPHERALSELECT */
#if defined(PIO_HAS_OUTPUTWRITEENABLE)
#define PIOE_OWER   (PIOE_BASE + PIO_OWER_OFF)  /*!< \brief PIO output write enable register address. */
#define PIOE_OWDR   (PIOE_BASE + PIO_OWDR_OFF)  /*!< \brief PIO output write disable register address. */
#define PIOE_OWSR   (PIOE_BASE + PIO_OWSR_OFF)  /*!< \brief PIO output write status register address. */
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOD_BASE */
/*@}*/

/*@} xgNutArchArmAt91Pio */

#endif                          /* _ARCH_ARM_AT91_PIO_H_ */
