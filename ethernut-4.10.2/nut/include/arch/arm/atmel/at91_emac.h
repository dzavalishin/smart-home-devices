#ifndef _ARCH_ARM_AT91_EMAC_H_
#define _ARCH_ARM_AT91_EMAC_H_

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
 * \file arch/arm/at91_emac.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2006/08/31 19:19:55  haraldkipp
 * No time to write comments. ;-)
 *
 * Revision 1.1  2006/07/05 07:45:25  haraldkipp
 * Split on-chip interface definitions.
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Emac
 */
/*@{*/

/*! \name Network Control Register */
/*@{*/
#define EMAC_NCR_OFF                0x00000000  /*!< \brief Network control register offset. */
#define EMAC_NCR    (EMAC_BASE + EMAC_NCR_OFF)  /*!< \brief Network Control register address. */

#define EMAC_LB                     0x00000001  /*!< \brief PHY loopback. */
#define EMAC_LLB                    0x00000002  /*!< \brief EMAC loopback. */
#define EMAC_RE                     0x00000004  /*!< \brief Receive enable. */
#define EMAC_TE                     0x00000008  /*!< \brief Transmit enable. */
#define EMAC_MPE                    0x00000010  /*!< \brief Management port enable. */
#define EMAC_CLRSTAT                0x00000020  /*!< \brief Clear statistics registers. */
#define EMAC_INCSTAT                0x00000040  /*!< \brief Increment statistics registers. */
#define EMAC_WESTAT                 0x00000080  /*!< \brief Write enable for statistics registers. */
#define EMAC_BP                     0x00000100  /*!< \brief Back pressure. */
#define EMAC_TSTART                 0x00000200  /*!< \brief Start Transmission. */
#define EMAC_THALT                  0x00000400  /*!< \brief Transmission halt. */
#define EMAC_TPFR                   0x00000800  /*!< \brief Transmit pause frame. */
#define EMAC_TZQ                    0x00001000  /*!< \brief Transmit zero quantum pause frame. */
/*@}*/

/*! \name Network Configuration Register */
/*@{*/
#define EMAC_NCFGR_OFF              0x00000004  /*!< \brief Network configuration register offset. */
#define EMAC_NCFGR (EMAC_BASE + EMAC_NCFGR_OFF) /*!< \brief Network configuration register address. */

#define EMAC_SPD                    0x00000001  /*!< \brief Speed, set for 100Mb. */
#define EMAC_FD                     0x00000002  /*!< \brief Full duplex. */
#define EMAC_JFRAME                 0x00000008  /*!< \brief Jumbo Frames. */
#define EMAC_CAF                    0x00000010  /*!< \brief Copy all frames. */
#define EMAC_NBC                    0x00000020  /*!< \brief No broadcast. */
#define EMAC_MTI                    0x00000040  /*!< \brief Multicast hash event enable. */
#define EMAC_UNI                    0x00000080  /*!< \brief Unicast hash enable. */
#define EMAC_BIG                    0x00000100  /*!< \brief Receive 1522 bytes. */
#define EMAC_EAE                    0x00000200  /*!< \brief External address match enable. */
#define EMAC_CLK                    0x00000C00  /*!< \brief Clock divider mask. */
#define EMAC_CLK_HCLK_8             0x00000000  /*!< \brief HCLK divided by 8. */
#define EMAC_CLK_HCLK_16            0x00000400  /*!< \brief HCLK divided by 16. */
#define EMAC_CLK_HCLK_32            0x00000800  /*!< \brief HCLK divided by 32. */
#define EMAC_CLK_HCLK_64            0x00000C00  /*!< \brief HCLK divided by 64. */
#define EMAC_RTY                    0x00001000  /*!< \brief Retry test. */
#define EMAC_PAE                    0x00002000  /*!< \brief Pause enable. */
#define EMAC_RBOF                   0x0000C000  /*!< \brief Receive buffer offset. */
#define EMAC_RBOF_OFFSET_0          0x00000000  /*!< \brief No offset from start of receive buffer. */
#define EMAC_RBOF_OFFSET_1          0x00004000  /*!< \brief One byte offset from start of receive buffer. */
#define EMAC_RBOF_OFFSET_2          0x00008000  /*!< \brief Two bytes offset from start of receive buffer. */
#define EMAC_RBOF_OFFSET_3          0x0000C000  /*!< \brief Three bytes offset from start of receive buffer. */
#define EMAC_RLCE                   0x00010000  /*!< \brief Receive length field checking enable. */
#define EMAC_DRFCS                  0x00020000  /*!< \brief Discard receive FCS. */
#define EMAC_EFRHD                  0x00040000  /*!< \brief Allow receive during transmit in half duplex. */
#define EMAC_IRXFCS                 0x00080000  /*!< \brief Ignore received FCS. */
/*@}*/

/*! \name Network Status Register */
/*@{*/
#define EMAC_NSR_OFF                0x00000008  /*!< \brief Network Status register offset. */
#define EMAC_NSR    (EMAC_BASE + EMAC_NSR_OFF)  /*!< \brief Network Status register address. */
#define EMAC_LINKR                  0x00000001  /*!< \brief . */
#define EMAC_MDIO                   0x00000002  /*!< \brief Status of MDIO input pin. */
#define EMAC_IDLE                   0x00000004  /*!< \brief Set when PHY is running. */
/*@}*/

/*! \name Transmit Status Register */
/*@{*/
#define EMAC_TSR_OFF                0x00000014  /*!< \brief Transmit Status register offset. */
#define EMAC_TSR    (EMAC_BASE + EMAC_TSR_OFF)  /*!< \brief Transmit Status register address. */

#define EMAC_UBR                    0x00000001  /*!< \brief Used bit read. */
#define EMAC_COL                    0x00000002  /*!< \brief Collision occurred. */
#define EMAC_RLES                   0x00000004  /*!< \brief Retry limit exceeded. */
#define EMAC_TGO                    0x00000008  /*!< \brief Transmit active. */
#define EMAC_BEX                    0x00000010  /*!< \brief Buffers exhausted mid frame. */
#define EMAC_COMP                   0x00000020  /*!< \brief Transmit complete. */
#define EMAC_UND                    0x00000040  /*!< \brief Transmit underrun. */
/*@}*/

/*! \name Buffer Queue Pointer Register */
/*@{*/
#define EMAC_RBQP_OFF               0x00000018  /*!< \brief Receive buffer queue pointer. */
#define EMAC_RBQP   (EMAC_BASE + EMAC_RBQP_OFF) /*!< \brief Receive buffer queue pointer. */
#define EMAC_TBQP_OFF               0x0000001C  /*!< \brief Transmit buffer queue pointer. */
#define EMAC_TBQP   (EMAC_BASE + EMAC_TBQP_OFF) /*!< \brief Transmit buffer queue pointer. */
/*@}*/

/*! \name Receive Status Register */
/*@{*/
#define EMAC_RSR_OFF                0x00000020  /*!< \brief Receive status register offset. */
#define EMAC_RSR    (EMAC_BASE + EMAC_RSR_OFF)  /*!< \brief Receive status register address. */

#define EMAC_BNA                    0x00000001  /*!< \brief Buffer not available. */
#define EMAC_REC                    0x00000002  /*!< \brief Frame received. */
#define EMAC_OVR                    0x00000004  /*!< \brief Receive overrun. */
/*@}*/

/*! \name Interrupt Registers */
/*@{*/
#define EMAC_ISR_OFF                0x00000024  /*!< \brief Status register offset. */
#define EMAC_ISR    (EMAC_BASE + EMAC_ISR_OFF)  /*!< \brief Status register address. */
#define EMAC_IER_OFF                0x00000028  /*!< \brief Enable register offset. */
#define EMAC_IER    (EMAC_BASE + EMAC_IER_OFF)  /*!< \brief Enable register address. */
#define EMAC_IDR_OFF                0x0000002C  /*!< \brief Disable register offset. */
#define EMAC_IDR    (EMAC_BASE + EMAC_IDR_OFF)  /*!< \brief Disable register address. */
#define EMAC_IMR_OFF                0x00000030  /*!< \brief Mask register offset. */
#define EMAC_IMR    (EMAC_BASE + EMAC_IMR_OFF)  /*!< \brief Mask register address. */

#define EMAC_MFD                    0x00000001  /*!< \brief Management frame done. */
#define EMAC_RCOMP                  0x00000002  /*!< \brief Receive complete. */
#define EMAC_RXUBR                  0x00000004  /*!< \brief Receive used bit read. */
#define EMAC_TXUBR                  0x00000008  /*!< \brief Transmit used bit read. */
#define EMAC_TUND                   0x00000010  /*!< \brief Ethernet transmit buffer underrun. */
#define EMAC_RLEX                   0x00000020  /*!< \brief Retry limit exceeded. */
#define EMAC_TXERR                  0x00000040  /*!< \brief Transmit error. */
#define EMAC_TCOMP                  0x00000080  /*!< \brief Transmit complete. */
#define EMAC_LINK                   0x00000200  /*!< \brief . */
#define EMAC_ROVR                   0x00000400  /*!< \brief Receive overrun. */
#define EMAC_HRESP                  0x00000800  /*!< \brief DMA bus error. */
#define EMAC_PFR                    0x00001000  /*!< \brief Pause frame received. */
#define EMAC_PTZ                    0x00002000  /*!< \brief Pause time zero. */
/*@}*/

/*! \name PHY Maintenance Register */
/*@{*/
#define EMAC_MAN_OFF                0x00000034  /*!< \brief PHY maintenance register offset. */
#define EMAC_MAN    (EMAC_BASE + EMAC_MAN_OFF)  /*!< \brief PHY maintenance register address. */

#define EMAC_DATA                   0x0000FFFF  /*!< \brief PHY data mask. */
#define EMAC_DATA_LSB                       0   /*!< \brief PHY data LSB. */
#define EMAC_CODE                   0x00020000  /*!< \brief Fixed value. */
#define EMAC_REGA                   0x007C0000  /*!< \brief PHY register address mask. */
#define EMAC_REGA_LSB                       18  /*!< \brief PHY register address LSB. */
#define EMAC_PHYA                   0x0F800000  /*!< \brief PHY address mask. */
#define EMAC_PHYA_LSB                       23  /*!< \brief PHY address LSB. */
#define EMAC_RW                     0x30000000  /*!< \brief PHY read/write command mask. */
#define EMAC_RW_READ                0x20000000  /*!< \brief PHY read command. */
#define EMAC_RW_WRITE               0x10000000  /*!< \brief PHY write command. */
#define EMAC_SOF                    0x40000000  /*!< \brief Fixed value. */
/*@}*/

/*! \name Pause Time Register */
/*@{*/
#define EMAC_PTR_OFF                0x00000038  /*!< \brief Pause time register offset. */
#define EMAC_PTR    (EMAC_BASE + EMAC_PTR_OFF)  /*!< \brief Pause time register address. */

#define EMAC_PTIME                  0x0000FFFF  /*!< \brief Pause time mask. */
/*@}*/

/*! \name Statistics Registers */
/*@{*/
#define EMAC_PFRR_OFF               0x0000003C  /*!< \brief Pause frames received register offset. */
#define EMAC_PFRR  (EMAC_BASE + EMAC_PFRR_OFF)  /*!< \brief Pause frames received register address. */
#define EMAC_FTO_OFF                0x00000040  /*!< \brief Frames transmitted OK register offset. */
#define EMAC_FTO    (EMAC_BASE + EMAC_FTO_OFF)  /*!< \brief Frames transmitted OK register address. */
#define EMAC_SCF_OFF                0x00000044  /*!< \brief Single collision frame register offset. */
#define EMAC_SCF    (EMAC_BASE + EMAC_SCF_OFF)  /*!< \brief Single collision frame register address. */
#define EMAC_MCF_OFF                0x00000048  /*!< \brief Multiple collision frame register offset. */
#define EMAC_MCF    (EMAC_BASE + EMAC_MCF_OFF)  /*!< \brief Multiple collision frame register address. */
#define EMAC_FRO_OFF                0x0000004C  /*!< \brief Frames received OK register offset. */
#define EMAC_FRO    (EMAC_BASE + EMAC_FRO_OFF)  /*!< \brief Frames received OK register address. */
#define EMAC_FCSE_OFF               0x00000050  /*!< \brief Frame check sequence error register offset. */
#define EMAC_FCSE   (EMAC_BASE + EMAC_FCSE_OFF) /*!< \brief Frame check sequence error register address. */
#define EMAC_ALE_OFF                0x00000054  /*!< \brief Alignment error register offset. */
#define EMAC_ALE    (EMAC_BASE + EMAC_ALE_OFF)  /*!< \brief Alignment error register address. */
#define EMAC_DTF_OFF                0x00000058  /*!< \brief Deferred transmission frame register offset. */
#define EMAC_DTF    (EMAC_BASE + EMAC_DTF_OFF)  /*!< \brief Deferred transmission frame register address. */
#define EMAC_LCOL_OFF               0x0000005C  /*!< \brief Late collision register offset. */
#define EMAC_LCOL   (EMAC_BASE + EMAC_LCOL_OFF) /*!< \brief Late collision register address. */
#define EMAC_ECOL_OFF               0x00000060  /*!< \brief Excessive collision register offset. */
#define EMAC_ECOL   (EMAC_BASE + EMAC_ECOL_OFF) /*!< \brief Excessive collision register address. */
#define EMAC_TUNDR_OFF              0x00000064  /*!< \brief Transmit underrun error register offset. */
#define EMAC_TUNDR (EMAC_BASE + EMAC_TUNDR_OFF) /*!< \brief Transmit underrun error register address. */
#define EMAC_CSE_OFF                0x00000068  /*!< \brief Carrier sense error register offset. */
#define EMAC_CSE    (EMAC_BASE + EMAC_CSE_OFF)  /*!< \brief Carrier sense error register address. */
#define EMAC_RRE_OFF                0x0000006C  /*!< \brief Receive resource error register offset. */
#define EMAC_RRE    (EMAC_BASE + EMAC_RRE_OFF)  /*!< \brief Receive resource error register address. */
#define EMAC_ROV_OFF                0x00000070  /*!< \brief Receive overrun errors register offset. */
#define EMAC_ROV    (EMAC_BASE + EMAC_ROV_OFF)  /*!< \brief Receive overrun errors register address. */
#define EMAC_RSE_OFF                0x00000074  /*!< \brief Receive symbol errors register offset. */
#define EMAC_RSE    (EMAC_BASE + EMAC_RSE_OFF)  /*!< \brief Receive symbol errors register address. */
#define EMAC_ELE_OFF                0x00000078  /*!< \brief Excessive length errors register offset. */
#define EMAC_ELE    (EMAC_BASE + EMAC_ELE_OFF)  /*!< \brief Excessive length errors register address. */
#define EMAC_RJA_OFF                0x0000007C  /*!< \brief Receive jabbers register offset. */
#define EMAC_RJA    (EMAC_BASE + EMAC_RJA_OFF)  /*!< \brief Receive jabbers register address. */
#define EMAC_USF_OFF                0x00000080  /*!< \brief Undersize frames register offset. */
#define EMAC_USF    (EMAC_BASE + EMAC_USF_OFF)  /*!< \brief Undersize frames register address. */
#define EMAC_STE_OFF                0x00000084  /*!< \brief SQE test error register offset. */
#define EMAC_STE    (EMAC_BASE + EMAC_STE_OFF)  /*!< \brief SQE test error register address. */
#define EMAC_RLE_OFF                0x00000088  /*!< \brief Receive length field mismatch register offset. */
#define EMAC_RLE    (EMAC_BASE + EMAC_RLE_OFF)  /*!< \brief Receive length field mismatch register address. */
#define EMAC_TPF_OFF                0x0000008C  /*!< \brief Transmitted pause frames register offset. */
#define EMAC_TPF    (EMAC_BASE + EMAC_TPF_OFF)  /*!< \brief Transmitted pause frames register address. */
/*@}*/

/*! \name MAC Adressing Registers */
/*@{*/
#define EMAC_HRB_OFF                0x00000090  /*!< \brief Hash address bottom[31:0]. */
#define EMAC_HRB    (EMAC_BASE + EMAC_HRB_OFF)  /*!< \brief Hash address bottom[31:0]. */
#define EMAC_HRT_OFF                0x00000094  /*!< \brief Hash address top[63:32]. */
#define EMAC_HRT    (EMAC_BASE + EMAC_HRT_OFF)  /*!< \brief Hash address top[63:32]. */
#define EMAC_SA1L_OFF               0x00000098  /*!< \brief Specific address 1 bottom, first 4 bytes. */
#define EMAC_SA1L   (EMAC_BASE + EMAC_SA1L_OFF) /*!< \brief Specific address 1 bottom, first 4 bytes. */
#define EMAC_SA1H_OFF               0x0000009C  /*!< \brief Specific address 1 top, last 2 bytes. */
#define EMAC_SA1H   (EMAC_BASE + EMAC_SA1H_OFF) /*!< \brief Specific address 1 top, last 2 bytes. */
#define EMAC_SA2L_OFF               0x000000A0  /*!< \brief Specific address 2 bottom, first 4 bytes. */
#define EMAC_SA2L   (EMAC_BASE + EMAC_SA2L_OFF) /*!< \brief Specific address 2 bottom, first 4 bytes. */
#define EMAC_SA2H_OFF               0x000000A4  /*!< \brief Specific address 2 top, last 2 bytes. */
#define EMAC_SA2H   (EMAC_BASE + EMAC_SA2H_OFF) /*!< \brief Specific address 2 top, last 2 bytes. */
#define EMAC_SA3L_OFF               0x000000A8  /*!< \brief Specific address 3 bottom, first 4 bytes. */
#define EMAC_SA3L   (EMAC_BASE + EMAC_SA3L_OFF) /*!< \brief Specific address 3 bottom, first 4 bytes. */
#define EMAC_SA3H_OFF               0x000000AC  /*!< \brief Specific address 3 top, last 2 bytes. */
#define EMAC_SA3H   (EMAC_BASE + EMAC_SA3H_OFF) /*!< \brief Specific address 3 top, last 2 bytes. */
#define EMAC_SA4L_OFF               0x000000B0  /*!< \brief Specific address 4 bottom, first 4 bytes. */
#define EMAC_SA4L   (EMAC_BASE + EMAC_SA4L_OFF) /*!< \brief Specific address 4 bottom, first 4 bytes. */
#define EMAC_SA4H_OFF               0x000000B4  /*!< \brief Specific address 4 top, last 2 bytes. */
#define EMAC_SA4H   (EMAC_BASE + EMAC_SA4H_OFF) /*!< \brief Specific address 4 top, last 2 bytes. */
/*@}*/

/*! \name Type ID Register */
/*@{*/
#define EMAC_TID_OFF                0x000000B8  /*!< \brief Type ID checking register offset. */
#define EMAC_TID    (EMAC_BASE + EMAC_TID_OFF)  /*!< \brief Type ID checking register address. */
#define EMAC_TPQ_OFF                0x000000BC  /*!< \brief Transmit pause quantum register offset. */
#define EMAC_TPQ    (EMAC_BASE + EMAC_TPQ_OFF)  /*!< \brief Transmit pause quantum register address. */
/*@}*/

/*! \name User Input/Output Register */
/*@{*/
#define EMAC_USRIO_OFF              0x000000C0  /*!< \brief User input/output register offset. */
#define EMAC_USRIO (EMAC_BASE + EMAC_USRIO_OFF) /*!< \brief User input/output register address. */

#define EMAC_RMII                   0x00000001  /*!< \brief Enable reduced MII. */
#define EMAC_CLKEN                  0x00000002  /*!< \brief Enable tranceiver input clock. */
/*@}*/

/*! \name Wake On LAN Register */
/*@{*/
#define EMAC_WOL_OFF                0x000000C4  /*!< \brief Wake On LAN register offset. */
#define EMAC_WOL    (EMAC_BASE + EMAC_WOL_OFF)  /*!< \brief Wake On LAN register address. */
#define EMAC_IP                     0x0000FFFF  /*!< \brief ARP request IP address mask. */
#define EMAC_MAG                    0x00010000  /*!< \brief Magic packet event enable. */
#define EMAC_ARP                    0x00020000  /*!< \brief ARP request event enable. */
#define EMAC_SA1                    0x00040000  /*!< \brief Specific address register 1 event enable. */
/*@}*/

/*! \name Revision Register */
/*@{*/
#define EMAC_REV_OFF                0x000000FC  /*!< \brief Revision register offset. */
#define EMAC_REV    (EMAC_BASE + EMAC_REV_OFF)  /*!< \brief Revision register address. */
#define EMAC_REVREF                 0x0000FFFF  /*!< \brief Revision. */
#define EMAC_PARTREF                0xFFFF0000  /*!< \brief Part. */
/*@}*/

/*@} xgNutArchArmAt91Emac */

#endif                          /* _ARCH_ARM_AT91_EMAC_H_ */
