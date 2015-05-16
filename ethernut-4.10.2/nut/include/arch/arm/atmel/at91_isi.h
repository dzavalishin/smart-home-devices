#ifndef _ARCH_ARM_AT91_ISI_H_
#define _ARCH_ARM_AT91_ISI_H_

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
 * \file arch/arm/at91_isi.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Isi
 */
/*@{*/

/*! \name Image Sensor Control Register 1 */
/*@{*/
#define ISI_CR1_OFF             0x00000000  /*!< \brief Control register 1 offset. */
#define ISI_CR1    (ISI_BASE + ISI_CR1_OFF) /*!< \brief Control register 1 address. */
#define ISI_RST                 0x00000001  /*!< \brief Image sensor interface reset. */
#define ISI_DIS                 0x00000002  /*!< \brief Image sensor shut down. */
#define ISI_HSYNC_POL           0x00000004  /*!< \brief HSYNC active low. */
#define ISI_VSYNC_POL           0x00000008  /*!< \brief VSYNC active low. */
#define ISI_PIXCLK_POL          0x00000010  /*!< \brief Data sampled on falling edge of pixel clock. */
#define ISI_EMB_SYNC            0x00000040  /*!< \brief Embedded synchronisation. */
#define ISI_CRC_SYNC            0x00000080  /*!< \brief CRC correction. */
#define ISI_FRATE               0x00000700  /*!< \brief Frame rate capture mask. */
#define ISI_FRATE_LSB                   8   /*!< \brief Frame rate capture LSB. */
#define ISI_FULL                0x00001000  /*!< \brief Full mode is allowed. */
#define ISI_THMASK              0x00006000  /*!< \brief Threshold mask. */
#define ISI_THMASK_4_8_16_BURST 0x00000000  /*!< \brief 4, 8 and 16 AHB bursts are allowed. */
#define ISI_THMASK_8_16_BURST   0x00002000  /*!< \brief 8 and 16 AHB bursts are allowed. */
#define ISI_THMASK_16_BURST     0x00004000  /*!< \brief Only 16 AHB bursts are allowed. */
#define ISI_CODEC_ON            0x00008000  /*!< \brief Codec path enable. */
#define ISI_SLD                 0x00FF0000  /*!< \brief Start of line delay mask. */
#define ISI_SLD_LSB                     16  /*!< \brief Start of line delay LSB. */
#define ISI_SFD                 0xFF000000  /*!< \brief Start of frame delay mask. */
#define ISI_SFD_LSB                     24  /*!< \brief Start of frame delay LSB. */
/*@}*/

/*! \name Image Sensor Control Register 2 */
/*@{*/
#define ISI_CR2_OFF             0x00000004  /*!< \brief Control register 2 offset. */
#define ISI_CR2    (ISI_BASE + ISI_CR2_OFF) /*!< \brief Control register 2 address. */
#define ISI_IM_VSIZE            0x000007FF  /*!< \brief Vertical size of the image sensor mask. */
#define ISI_IM_VSIZE_LSB                0   /*!< \brief Vertical size of the image sensor LSB. */
#define ISI_GS_MODE             0x00000800  /*!< \brief 1 pixel grayscale mode. */
#define ISI_RGB_MODE            0x00001000  /*!< \brief 16 bit RGB mode. */
#define ISI_GRAYSCALE           0x00002000  /*!< \brief Grayscale enable. */
#define ISI_RGB_SWAP            0x00004000  /*!< \brief RGB swap. */
#define ISI_COL_SPACE           0x00008000  /*!< \brief RGB color space for the image data. */
#define ISI_IM_HSIZE            0x07FF0000  /*!< \brief Horizontal size of the image sensor mask. */
#define ISI_IM_HSIZE_LSB                16  /*!< \brief Horizontal size of the image sensor LSB. */
#define ISI_YCC_SWAP            0x30000000  /*!< \brief YCC swap mask. */
#define ISI_YCC_SWAP_DEFAULT    0x00000000  /*!< \brief Image data format Cb(i) Y(i) Cr(i) Y(i+1). */
#define ISI_YCC_SWAP_MODE1      0x10000000  /*!< \brief Image data format Cr(i) Y(i) Cb(i) Y(i+1). */
#define ISI_YCC_SWAP_MODE2      0x20000000  /*!< \brief Image data format Y(i) Cb(i) Y(i+1) Cr(i). */
#define ISI_YCC_SWAP_MODE3      0x30000000  /*!< \brief Image data format Y(i) Cr(i) Y(i+1) Cb(i). */
#define ISI_RGB_CFG             0xC0000000  /*!< \brief RGB configuration mask. */
#define ISI_RGB_CFG_DEFAULT     0x00000000  /*!< \brief RGB pattern R/G(MSB) G(LSB)/B R/G(MSB) G(LSB)/B. */
#define ISI_RGB_CFG_MODE1       0x40000000  /*!< \brief RGB pattern B/G(MSB) G(LSB)/R B/G(MSB) G(LSB)/R. */
#define ISI_RGB_CFG_MODE2       0x80000000  /*!< \brief RGB pattern G(LSB)/R B/G(MSB) G(LSB)/R B/G(MSB). */
#define ISI_RGB_CFG_MODE3       0xC0000000  /*!< \brief RGB pattern G(LSB)/B R/G(MSB) G(LSB)/B R/G(MSB). */
/*@}*/

/*! \name Image Sensor Status and Interrupt Registers */
/*@{*/
#define ISI_SR_OFF              0x00000008  /*!< \brief Status register offset. */
#define ISI_SR      (ISI_BASE + ISI_SR_OFF) /*!< \brief Status register address. */
#define ISI_IER_OFF             0x0000000C  /*!< \brief Interrupt enable register offset. */
#define ISI_IER    (ISI_BASE + ISI_IER_OFF) /*!< \brief Interrupt enable address. */
#define ISI_IDR_OFF             0x00000010  /*!< \brief Interrupt disable register offset. */
#define ISI_IDR    (ISI_BASE + ISI_IDR_OFF) /*!< \brief Interrupt disable address. */
#define ISI_IMR_OFF             0x00000014  /*!< \brief Interrupt mask register offset. */
#define ISI_IMR    (ISI_BASE + ISI_IMR_OFF) /*!< \brief Interrupt mask address. */
#define ISI_SOF                 0x00000001  /*!< \brief Start of frame. */
#define ISI_DIS                 0x00000002  /*!< \brief Interface disable. */
#define ISI_SOFTRST             0x00000004  /*!< \brief Software reset. */
#define ISI_CDC_PND             0x00000008  /*!< \brief Codec request pending. */
#define ISI_CRC_ERR             0x00000010  /*!< \brief CRC synchronisation error. */
#define ISI_FO_C_OVF            0x00000020  /*!< \brief Codec overflow. */
#define ISI_FO_P_OVF            0x00000040  /*!< \brief Preview FIFO overflow. */
#define ISI_FO_P_EMP            0x00000080  /*!< \brief Preview FIFO empty. */
#define ISI_FO_C_EMP            0x00000100  /*!< \brief Codec FIFO empty. */
#define ISI_FR_OVR              0x00000200  /*!< \brief Frame rate overun. */
/*@}*/

/*! \name Image Sensor Preview Size Register */
/*@{*/
#define ISI_PSIZE_OFF           0x00000020  /*!< \brief Preview size register offset. */
#define ISI_PSIZE (ISI_BASE + ISI_PSIZE_OFF) /*!< \brief Preview size register address. */
#define ISI_PREV_VSIZE          0x000003FF  /*!< \brief Vertical size for the preview path mask. */
#define ISI_PREV_VSIZE_LSB              0   /*!< \brief Vertical size for the preview path LSB. */
#define ISI_PREV_HSIZE          0x03FF0000  /*!< \brief Horizontal size for the preview path mask. */
#define ISI_PREV_HSIZE_LSB              16  /*!< \brief Horizontal size for the preview path LSB. */
/*@}*/

/*! \name Image Sensor Preview Decimation Register */
/*@{*/
#define ISI_PDECF_OFF           0x00000024  /*!< \brief Preview decimation factor register offset. */
#define ISI_PDECF (ISI_BASE + ISI_PDECF_OFF) /*!< \brief Preview decimation factor register address. */
#define ISI_DEC_FACTOR          0x000000FF  /*!< \brief Decimation factor mask. */
#define ISI_DEC_FACTOR_LSB              0   /*!< \brief Decimation factor LSB. */
/*@}*/

/*! \name Image Sensor Preview Primary FBD Register */
/*@{*/
#define ISI_PPFBD_OFF           0x00000028  /*!< \brief Preview frame buffer address register offset. */
#define ISI_PPFBD (ISI_BASE + ISI_PPFBD_OFF) /*!< \brief Preview frame buffer address register address. */
/*@}*/

/*! \name Image Sensor Codec DMA Base Address Register */
/*@{*/
#define ISI_CDBA_OFF            0x0000002C  /*!< \brief Codec DMA address register offset. */
#define ISI_CDBA  (ISI_BASE + ISI_CDBA_OFF) /*!< \brief Codec DMA address register address. */
/*@}*/

/*! \name Image Sensor Color Space Conversion YCrCb To RGB Set 0 Register */
/*@{*/
#define ISI_Y2R_SET0_OFF        0x00000030  /*!< \brief YCrCb to RGB conversion set 0 register offset. */
#define ISI_Y2R_SET0 (ISI_BASE + ISI_Y2R_SET0_OFF) /*!< \brief YCrCb to RGB conversion set 0 register address. */
#define ISI_Y2R_C0              0x000000FF  /*!< \brief Conversion matrix coefficient C0 mask. */
#define ISI_Y2R_C0_LSB                  0   /*!< \brief Conversion matrix coefficient C0 LSB. */
#define ISI_Y2R_C1              0x0000FF00  /*!< \brief Conversion matrix coefficient C1 mask. */
#define ISI_Y2R_C1_LSB                  8   /*!< \brief Conversion matrix coefficient C1 LSB. */
#define ISI_Y2R_C2              0x00FF0000  /*!< \brief Conversion matrix coefficient C2 mask. */
#define ISI_Y2R_C2_LSB                  16  /*!< \brief Conversion matrix coefficient C2 LSB. */
#define ISI_Y2R_C3              0xFF000000  /*!< \brief Conversion matrix coefficient C3 mask. */
#define ISI_Y2R_C3_LSB                  24  /*!< \brief Conversion matrix coefficient C3 LSB. */
/*@}*/

/*! \name Image Sensor Color Space Conversion YCrCb To RGB Set 1 Register */
/*@{*/
#define ISI_Y2R_SET1_OFF        0x00000034  /*!< \brief YCrCb to RGB conversion set 1 register offset. */
#define ISI_Y2R_SET1 (ISI_BASE + ISI_Y2R_SET1_OFF) /*!< \brief YCrCb to RGB conversion set 1 register address. */
#define ISI_Y2R_C4              0x000001FF  /*!< \brief Conversion matrix coefficient C4 mask. */
#define ISI_Y2R_C4_LSB                  0   /*!< \brief Conversion matrix coefficient C4 LSB. */
#define ISI_Y2R_YOFF            0x00001000  /*!< \brief Luminance default offset 128. */
#define ISI_Y2R_CROFF           0x00002000  /*!< \brief Red chrominance default offset 16. */
#define ISI_Y2R_CBOFF           0x00004000  /*!< \brief Blue chrominance default offset 16. */
/*@}*/

/*! \name Image Sensor Color Space Conversion RGB To YCrCb Set 0 Register */
/*@{*/
#define ISI_R2Y_SET0_OFF        0x00000038  /*!< \brief RGB to YCrCb conversion set 0 register offset. */
#define ISI_R2Y_SET0 (ISI_BASE + ISI_R2Y_SET0_OFF) /*!< \brief RGB to YCrCb conversion set 0 register address. */
#define ISI_R2Y_C0              0x000000FF  /*!< \brief Conversion matrix coefficient C0 mask. */
#define ISI_R2Y_C0_LSB                  0   /*!< \brief Conversion matrix coefficient C0 LSB. */
#define ISI_R2Y_C1              0x0000FF00  /*!< \brief Conversion matrix coefficient C1 mask. */
#define ISI_R2Y_C1_LSB                  8   /*!< \brief Conversion matrix coefficient C1 LSB. */
#define ISI_R2Y_C2              0x00FF0000  /*!< \brief Conversion matrix coefficient C2 mask. */
#define ISI_R2Y_C2_LSB                  16  /*!< \brief Conversion matrix coefficient C2 LSB. */
#define ISI_R2Y_ROFF            0x01000000  /*!< \brief Red component offset 16. */
/*@}*/

/*! \name Image Sensor Color Space Conversion RGB To YCrCb Set 1 Register */
/*@{*/
#define ISI_R2Y_SET1_OFF        0x0000003C  /*!< \brief RGB to YCrCb conversion set 1 register offset. */
#define ISI_R2Y_SET1 (ISI_BASE + ISI_R2Y_SET1_OFF) /*!< \brief RGB to YCrCb conversion set 1 register address. */
#define ISI_R2Y_C3              0x000000FF  /*!< \brief Conversion matrix coefficient C3 mask. */
#define ISI_R2Y_C3_LSB                  0   /*!< \brief Conversion matrix coefficient C3 LSB. */
#define ISI_R2Y_C4              0x0000FF00  /*!< \brief Conversion matrix coefficient C4 mask. */
#define ISI_R2Y_C4_LSB                  8   /*!< \brief Conversion matrix coefficient C4 LSB. */
#define ISI_R2Y_C5              0x00FF0000  /*!< \brief Conversion matrix coefficient C5 mask. */
#define ISI_R2Y_C5_LSB                  16  /*!< \brief Conversion matrix coefficient C5 LSB. */
#define ISI_R2Y_GOFF            0x01000000  /*!< \brief Green component offset 128. */
/*@}*/

/*! \name Image Sensor Color Space Conversion RGB To YCrCb Set 2 Register */
/*@{*/
#define ISI_R2Y_SET2_OFF        0x00000040  /*!< \brief RGB to YCrCb conversion set 2 register offset. */
#define ISI_R2Y_SET2 (ISI_BASE + ISI_R2Y_SET2_OFF) /*!< \brief RGB to YCrCb conversion set 2 register address. */
#define ISI_R2Y_C6              0x000000FF  /*!< \brief Conversion matrix coefficient C6 mask. */
#define ISI_R2Y_C6_LSB                  0   /*!< \brief Conversion matrix coefficient C6 LSB. */
#define ISI_R2Y_C7              0x0000FF00  /*!< \brief Conversion matrix coefficient C7 mask. */
#define ISI_R2Y_C7_LSB                  8   /*!< \brief Conversion matrix coefficient C7 LSB. */
#define ISI_R2Y_C8              0x00FF0000  /*!< \brief Conversion matrix coefficient C8 mask. */
#define ISI_R2Y_C8_LSB                  16  /*!< \brief Conversion matrix coefficient C8 LSB. */
#define ISI_R2Y_BOFF            0x01000000  /*!< \brief Blue component offset 128. */
/*@}*/

/*@} xgNutArchArmAt91Isi */

#endif /* _ARCH_ARM_AT91_ISI_H_ */

