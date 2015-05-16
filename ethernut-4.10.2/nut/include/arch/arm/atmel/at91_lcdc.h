#ifndef _ARCH_ARM_AT91_LCDC_H_
#define _ARCH_ARM_AT91_LCDC_H_

/*
 * Copyright (C) 2010 by egnite GmbH
 *
 * All rights reserved.
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
 * \addtogroup xgNutArchArmAt91Lcd
 */
/*@{*/

#ifdef LCDC_BASE

/*! \name LCD Controller Register */
/*@{*/

#define LCDC_DMABADDR1_OFF             0x00000000  						/*!< \brief DMA Base Address Register 1. */
#define LCDC_DMABADDR2_OFF             0x00000004  						/*!< \brief DMA Base Address Register 2. */
#define LCDC_DMAFRMPT1_OFF             0x00000008  						/*!< \brief DMA Frame Pointer Register 1. */
#define LCDC_DMAFRMPT2_OFF             0x0000000C  						/*!< \brief DMA Frame Pointer Register 2. */
#define LCDC_DMAFRMADD1_OFF            0x00000010  						/*!< \brief DMA Frame Address Register 1. */
#define LCDC_DMAFRMADD2_OFF            0x00000014  						/*!< \brief DMA Frame Address Register 2. */
#define LCDC_DMAFRMCFG_OFF             0x00000018  						/*!< \brief DMA Frame Configuration Register. */
#define LCDC_DMACON_OFF                0x0000001C  						/*!< \brief DMA Control Register. */
#define LCDC_DMA2DCFG_OFF              0x00000020  						/*!< \brief DMA Control Register. */
#define LCDC_LCDCON1_OFF               0x00000800  						/*!< \brief LCD Control Register 1. */
#define LCDC_LCDCON2_OFF               0x00000804  						/*!< \brief LCD Control Register 2. */
#define LCDC_LCDTIM1_OFF               0x00000808  						/*!< \brief LCD Timing Register 1. */
#define LCDC_LCDTIM2_OFF               0x0000080C  						/*!< \brief LCD Timing Register 2. */
#define LCDC_LCDFRMCFG_OFF             0x00000810  						/*!< \brief LCD Frame Configuration Register. */
#define LCDC_LCDFIFO_OFF               0x00000814  						/*!< \brief LCD FIFO Register. */
#define LCDC_LCDMVAL_OFF               0x00000818  						/*!< \brief LCDMOD Toggle Rate Value Register. */
#define LCDC_DP1_2_OFF                 0x0000081C  						/*!< \brief Dithering Pattern DP1_2. */
#define LCDC_DP4_7_OFF                 0x00000820  						/*!< \brief Dithering Pattern DP4_7. */
#define LCDC_DP3_5_OFF                 0x00000824  						/*!< \brief Dithering Pattern DP3_5. */
#define LCDC_DP2_3_OFF                 0x00000828  						/*!< \brief Dithering Pattern DP2_3. */
#define LCDC_DP5_7_OFF                 0x0000082C  						/*!< \brief Dithering Pattern DP5_7. */
#define LCDC_DP3_4_OFF                 0x00000830  						/*!< \brief Dithering Pattern DP3_4. */
#define LCDC_DP4_5_OFF                 0x00000834  						/*!< \brief Dithering Pattern DP4_5. */
#define LCDC_DP6_7_OFF                 0x00000838  						/*!< \brief Dithering Pattern DP6_7. */
#define LCDC_PWRCON_OFF                0x0000083C  						/*!< \brief Power Control Register. */
#define LCDC_CONTRAST_CTR_OFF          0x00000840  						/*!< \brief Contrast Control Register. */
#define LCDC_CONTRAST_VAL_OFF          0x00000844  						/*!< \brief Contrast Value Register. */
#define LCDC_LCD_IER_OFF               0x00000848  						/*!< \brief LCD Interrupt Enable Register. */
#define LCDC_LCD_IDR_OFF               0x0000084C  						/*!< \brief LCD Interrupt Disable Register. */
#define LCDC_LCD_IMR_OFF               0x00000850  						/*!< \brief LCD Interrupt Mask Register. */
#define LCDC_LCD_ISR_OFF               0x00000854  						/*!< \brief LCD Interrupt Status Register. */
#define LCDC_LCD_ICR_OFF               0x00000858  						/*!< \brief LCD Interrupt Clear Register. */
#define LCDC_LCD_ITR_OFF               0x00000860  						/*!< \brief LCD Interrupt Test Register. */
#define LCDC_LCD_IRR_OFF               0x00000864  						/*!< \brief LCD Interrupt Raw Status Register. */
#define LCDC_LCD_WPCR_OFF              0x000008E4  						/*!< \brief Write Protection Control Register. */
#define LCDC_LCD_WPSR_OFF              0x000008E8  						/*!< \brief Write Protection Status Register. */
#define LCDC_LUT_ENTRY_0_OFF           0x00000C00  						/*!< \brief Palette entry 0. */
#define LCDC_LUT_ENTRY_1_OFF           0x00000C04  						/*!< \brief Palette entry 1. */
#define LCDC_LUT_ENTRY_2_OFF           0x00000C08  						/*!< \brief Palette entry 2. */
/* ... */
#define LCDC_LUT_ENTRY_255_OFF         0x00000FFC  						/*!< \brief Palette entry 255. */

#define LCDC_DMABADDR1							(LCDC_BASE + LCDC_DMABADDR1_OFF) 			/*!< \brief DMA Base Address Register 1. */
#define LCDC_DMABADDR2							(LCDC_BASE + LCDC_DMABADDR2_OFF)      /*!< \brief DMA Base Address Register 2. */
#define LCDC_DMAFRMPT1							(LCDC_BASE + LCDC_DMAFRMPT1_OFF)      /*!< \brief DMA Frame Pointer Register 1. */
#define LCDC_DMAFRMPT2							(LCDC_BASE + LCDC_DMAFRMPT2_OFF)      /*!< \brief DMA Frame Pointer Register 2. */
#define LCDC_DMAFRMADD1							(LCDC_BASE + LCDC_DMAFRMADD1_OFF)     /*!< \brief DMA Frame Address Register 1. */
#define LCDC_DMAFRMADD2							(LCDC_BASE + LCDC_DMAFRMADD2_OFF)     /*!< \brief DMA Frame Address Register 2. */
#define LCDC_DMAFRMCFG							(LCDC_BASE + LCDC_DMAFRMCFG_OFF)      /*!< \brief DMA Frame Configuration Register. */
#define LCDC_DMACON								  (LCDC_BASE + LCDC_DMACON_OFF)        	/*!< \brief DMA Control Register. */
#define LCDC_DMA2DCFG								(LCDC_BASE + LCDC_DMA2DCFG_OFF)       /*!< \brief DMA Control Register. */
#define LCDC_LCDCON1								(LCDC_BASE + LCDC_LCDCON1_OFF)        /*!< \brief LCD Control Register 1. */
#define LCDC_LCDCON2								(LCDC_BASE + LCDC_LCDCON2_OFF)        /*!< \brief LCD Control Register 2. */
#define LCDC_LCDTIM1								(LCDC_BASE + LCDC_LCDTIM1_OFF)        /*!< \brief LCD Timing Register 1. */
#define LCDC_LCDTIM2								(LCDC_BASE + LCDC_LCDTIM2_OFF)        /*!< \brief LCD Timing Register 2. */
#define LCDC_LCDFRMCFG							(LCDC_BASE + LCDC_LCDFRMCFG_OFF)       /*!< \brief LCD Frame Configuration Register. */
#define LCDC_LCDFIFO								(LCDC_BASE + LCDC_LCDFIFO_OFF)        /*!< \brief LCD FIFO Register. */
#define LCDC_LCDMVAL					      (LCDC_BASE + LCDC_LCDMVAL_OFF)        /*!< \brief LCDMOD Toggle Rate Value Register. */
#define LCDC_DP1_2							    (LCDC_BASE + LCDC_DP1_2_OFF)        		/*!< \brief Dithering Pattern DP1_2. */
#define LCDC_DP4_7								  (LCDC_BASE + LCDC_DP4_7_OFF)        	/*!< \brief Dithering Pattern DP4_7. */
#define LCDC_DP3_5								  (LCDC_BASE + LCDC_DP3_5_OFF)        	/*!< \brief Dithering Pattern DP3_5. */
#define LCDC_DP2_3								  (LCDC_BASE + LCDC_DP2_3_OFF)        	/*!< \brief Dithering Pattern DP2_3. */
#define LCDC_DP5_7								  (LCDC_BASE + LCDC_DP5_7_OFF)        	/*!< \brief Dithering Pattern DP5_7. */
#define LCDC_DP3_4								  (LCDC_BASE + LCDC_DP3_4_OFF)        	/*!< \brief Dithering Pattern DP3_4. */
#define LCDC_DP4_5								  (LCDC_BASE + LCDC_DP4_5_OFF)        	/*!< \brief Dithering Pattern DP4_5. */
#define LCDC_DP6_7								  (LCDC_BASE + LCDC_DP6_7_OFF)        	/*!< \brief Dithering Pattern DP6_7. */
#define LCDC_PWRCON								  (LCDC_BASE + LCDC_PWRCON_OFF)        	/*!< \brief Power Control Register. */
#define LCDC_CONTRAST_CTR						(LCDC_BASE + LCDC_CONTRAST_CTR_OFF)   /*!< \brief Contrast Control Register. */
#define LCDC_CONTRAST_VAL						(LCDC_BASE + LCDC_CONTRAST_VAL_OFF)   /*!< \brief Contrast Value Register. */
#define LCDC_LCD_IER								(LCDC_BASE + LCDC_LCD_IER_OFF)        /*!< \brief LCD Interrupt Enable Register. */
#define LCDC_LCD_IDR								(LCDC_BASE + LCDC_LCD_IDR_OFF)        /*!< \brief LCD Interrupt Disable Register. */
#define LCDC_LCD_IMR								(LCDC_BASE + LCDC_LCD_IMR_OFF)        /*!< \brief LCD Interrupt Mask Register. */
#define LCDC_LCD_ISR								(LCDC_BASE + LCDC_LCD_ISR_OFF)        /*!< \brief LCD Interrupt Status Register. */
#define LCDC_LCD_ICR								(LCDC_BASE + LCDC_LCD_ICR_OFF)        /*!< \brief LCD Interrupt Clear Register. */
#define LCDC_LCD_ITR								(LCDC_BASE + LCDC_LCD_ITR_OFF)        /*!< \brief LCD Interrupt Test Register. */
#define LCDC_LCD_IRR								(LCDC_BASE + LCDC_LCD_IRR_OFF)        /*!< \brief LCD Interrupt Raw Status Register. */
#define LCDC_LCD_WPCR								(LCDC_BASE + LCDC_LCD_WPCR_OFF)       /*!< \brief Write Protection Control Register. */
#define LCDC_LCD_WPSR								(LCDC_BASE + LCDC_LCD_WPSR_OFF)       /*!< \brief Write Protection Status Register. */
#define LCDC_LUT_ENTRY_0						(LCDC_BASE + LCDC_LUT_ENTRY_0_OFF)    /*!< \brief Palette entry 0. */
#define LCDC_LUT_ENTRY_1						(LCDC_BASE + LCDC_LUT_ENTRY_1_OFF)    /*!< \brief Palette entry 1. */
#define LCDC_LUT_ENTRY_2						(LCDC_BASE + LCDC_LUT_ENTRY_2_OFF)    /*!< \brief Palette entry 2. */
/* ... */
#define LCDC_LUT_ENTRY_255					(LCDC_BASE + LCDC_LUT_ENTRY_255_OFF)  /*!< \brief Palette entry 255. */
/*@}*/

/*@} xgNutArchArmAt91Lcd */

#endif /* LCDC_BASE */

#endif /* _ARCH_ARM_AT91_LCDC_H_ */
