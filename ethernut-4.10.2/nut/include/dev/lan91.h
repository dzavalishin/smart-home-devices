#ifndef _DEV_LAN91_H_
#define _DEV_LAN91_H_

/*
 * Copyright (C) 2003 by egnite Software GmbH.
 * Copyright (C) 2008 by egnite GmbH.
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

/*
 * $Id: lan91.h 2282 2008-08-28 11:07:10Z haraldkipp $
 */

#include <dev/netbuf.h>
#include <net/if_var.h>

/*!
 * \file dev/lanc91.h
 * \brief Network interface controller definitions.
 */

/*!
 * \addtogroup xgNicLan91
 */
/*@{*/

#ifndef LAN91_BASE_ADDR
#define LAN91_BASE_ADDR 0xC000
#endif

/*! 
 * \brief Bank select register. 
 */
#define LAN91_BSR         (LAN91_BASE_ADDR + 0x0E)

/*! 
 * \brief Bank 0 - Transmit control register. 
 */
#define LAN91_TCR         (LAN91_BASE_ADDR + 0x00)

#define LAN91_TCR_SWFDUP      0x8000  /*!< \ref LAN91_TCR bit mask, enables full duplex.  */
#define LAN91_TCR_EPH_LOOP    0x2000  /*!< \ref LAN91_TCR bit mask, enables internal loopback. */
#define LAN91_TCR_STP_SQET    0x1000  /*!< \ref LAN91_TCR bit mask, enables transmission stop on SQET error. */
#define LAN91_TCR_FDUPLX      0x0800  /*!< \ref LAN91_TCR bit mask, enables receiving own frames. */
#define LAN91_TCR_MON_CSN     0x0400  /*!< \ref LAN91_TCR bit mask, enables carrier monitoring. */
#define LAN91_TCR_NOCRC       0x0100  /*!< \ref LAN91_TCR bit mask, disables CRC transmission. */
#define LAN91_TCR_PAD_EN      0x0080  /*!< \ref LAN91_TCR bit mask, enables automatic padding. */
#define LAN91_TCR_FORCOL      0x0004  /*!< \ref LAN91_TCR bit mask, forces collision. */
#define LAN91_TCR_LOOP        0x0002  /*!< \ref LAN91_TCR bit mask, enables PHY loopback. */
#define LAN91_TCR_TXENA       0x0001  /*!< \ref LAN91_TCR bit mask, enables transmitter. */


/*! 
 * \brief Bank 0 - EPH status register. 
 */
#define LAN91_EPHSR       (LAN91_BASE_ADDR + 0x02)

/*! 
 * \brief Bank 0 - Receive control register. 
 */
#define LAN91_RCR         (LAN91_BASE_ADDR + 0x04)

#define LAN91_RCR_SOFT_RST    0x8000  /*!< \ref LAN91_RCR bit mask, activates software reset. */
#define LAN91_RCR_FILT_CAR    0x4000  /*!< \ref LAN91_RCR bit mask, enables carrier filter. */
#define LAN91_RCR_ABORT_ENB   0x2000  /*!< \ref LAN91_RCR bit mask, enables receive abort on collision. */
#define LAN91_RCR_STRIP_CRC   0x0200  /*!< \ref LAN91_RCR bit mask, strips CRC. */
#define LAN91_RCR_RXEN        0x0100  /*!< \ref LAN91_RCR bit mask, enables receiver. */
#define LAN91_RCR_ALMUL       0x0004  /*!< \ref LAN91_RCR bit mask, multicast frames accepted when set. */
#define LAN91_RCR_PRMS        0x0002  /*!< \ref LAN91_RCR bit mask, enables promiscuous mode. */
#define LAN91_RCR_RX_ABORT    0x0001  /*!< \ref LAN91_RCR bit mask, set when receive was aborted. */

/*! 
 * \brief Bank 0 - Counter register.
 */
#define LAN91_ECR         (LAN91_BASE_ADDR + 0x06)

/*! 
 * \brief Bank 0 - Memory information register.
 */
#define LAN91_MIR         (LAN91_BASE_ADDR + 0x08)

/*! 
 * \brief Bank 0 - Receive / PHY control register.
 */
#define LAN91_RPCR        (LAN91_BASE_ADDR + 0x0A)

#define LAN91_RPCR_SPEED      0x2000  /*!< \ref LAN91_RPCR bit mask, PHY operates at 100 Mbps. */
#define LAN91_RPCR_DPLX       0x1000  /*!< \ref LAN91_RPCR bit mask, PHY operates at full duplex mode. */
#define LAN91_RPCR_ANEG       0x0800  /*!< \ref LAN91_RPCR bit mask, sets PHY in auto-negotiation mode. */
#define LAN91_RPCR_LEDA_PAT   0x0000  /*!< \ref LAN91_RPCR bit mask for LEDA mode. */
#define LAN91_RPCR_LEDB_PAT   0x0010  /*!< \ref LAN91_RPCR bit mask for LEDB mode. */

/*! 
 * \brief Bank 1 - Configuration register.
 */
#define LAN91_CR          (LAN91_BASE_ADDR + 0x00)

#define LAN91_CR_EPH_EN       0x8000  /*!< \ref LAN91_CR bit mask, . */

/*! 
 * \brief Bank 1 - Base address register.
 */
#define LAN91_BAR         (LAN91_BASE_ADDR + 0x02)

/*! 
 * \brief Bank 1 - Individual address register.
 */
#define LAN91_IAR         (LAN91_BASE_ADDR + 0x04)

/*! 
 * \brief Bank 1 - General purpose register.
 */
#define LAN91_GPR         (LAN91_BASE_ADDR + 0x0A)

/*! 
 * \brief Bank 1 - Control register.
 */
#define LAN91_CTR         (LAN91_BASE_ADDR + 0x0C)

#define LAN91_CTR_RCV_BAD     0x4000  /*!< \ref LAN91_CTR bit mask. */
#define LAN91_CTR_AUTO_RELEASE 0x0800 /*!< \ref LAN91_CTR bit mask, transmit packets automatically released. */

/*!
 * \brief Bank 2 - MMU command register.
 */
#define LAN91_MMUCR       (LAN91_BASE_ADDR + 0x00)

#define LAN91_MMUCR_BUSY      0x0001

#define LAN91_MMU_NOP         0
#define LAN91_MMU_ALO         (1<<5)
#define LAN91_MMU_RST         (2<<5)
#define LAN91_MMU_REM         (3<<5)
#define LAN91_MMU_TOP         (4<<5)
#define LAN91_MMU_PKT         (5<<5)
#define LAN91_MMU_ENQ         (6<<5)
#define LAN91_MMU_RTX         (7<<5)

/*!
 * \brief Bank 2 - Packet number register.
 *
 * This byte register specifies the accessible transmit packet number.
 */
#define LAN91_PNR         (LAN91_BASE_ADDR + 0x02)

/*!
 * \brief Bank 2 - Allocation result register.
 *
 * This byte register is updated upon a LAN91_MMU_ALO command.
 */
#define LAN91_ARR         (LAN91_BASE_ADDR + 0x03)

#define LAN91_ARR_FAILED      0x80

/*!
 * \brief Bank 2 - FIFO ports register.
 */
#define LAN91_FIFO        (LAN91_BASE_ADDR + 0x04)

/*!
 * \brief Bank 2 - Pointer register.
 */
#define LAN91_PTR         (LAN91_BASE_ADDR + 0x06)

#define LAN91_PTR_RCV         0x8000  /*! \ref LAN91_PTR bit mask, specifies receive or transmit buffer. */
#define LAN91_PTR_AUTO_INCR   0x4000  /*! \ref LAN91_PTR bit mask, enables automatic pointer increment. */
#define LAN91_PTR_READ        0x2000  /*! \ref LAN91_PTR bit mask, indicates type of access. */
#define LAN91_PTR_ETEN        0x1000  /*! \ref LAN91_PTR bit mask, enables early transmit underrun detection. */
#define LAN91_PTR_NOT_EMPTY   0x0800  /*! \ref LAN91_PTR bit mask, set when write data fifo is not empty. */

/*!
 * \brief Bank 2 - Data register.
 */
#define LAN91_DATA        (LAN91_BASE_ADDR + 0x08)

/*!
 * \brief Bank 2 - Interrupt status register.
 */
#define LAN91_IST         (LAN91_BASE_ADDR + 0x0C)

/*!
 * \brief Bank 2 - Interrupt acknowledge register.
 */
#define LAN91_ACK         (LAN91_BASE_ADDR + 0x0C)

/*!
 * \brief Bank 2 - Interrupt mask register.
 */
#define LAN91_MSK         (LAN91_BASE_ADDR + 0x0D)

#define LAN91_INT_MD          0x80    /*!< \brief PHY state change interrupt bit mask. */
#define LAN91_INT_ERCV        0x40    /*!< \brief Early receive interrupt bit mask. */
#define LAN91_INT_EPH         0x20    /*!< \brief Ethernet protocol interrupt bit mask. */
#define LAN91_INT_RX_OVRN     0x10    /*!< \brief Receive overrun interrupt bit mask. */
#define LAN91_INT_ALLOC       0x08    /*!< \brief Transmit allocation interrupt bit mask. */
#define LAN91_INT_TX_EMPTY    0x04    /*!< \brief Transmitter empty interrupt bit mask. */
#define LAN91_INT_TX          0x02    /*!< \brief Transmit complete interrupt bit mask. */
#define LAN91_INT_RCV         0x01    /*!< \brief Receive interrupt bit mask. */

/*!
 * \brief Bank 3 - Multicast table register.
 */
#define LAN91_MT          (LAN91_BASE_ADDR + 0x00)

/*!
 * \brief Bank 3 - Management interface register.
 */
#define LAN91_MGMT        (LAN91_BASE_ADDR + 0x08)

#define LAN91_MGMT_MDOE       0x08    /*!< \ref LAN91_MGMT bit mask, enables MDO pin. */
#define LAN91_MGMT_MCLK       0x04    /*!< \ref LAN91_MGMT bit mask, drives MDCLK pin. */
#define LAN91_MGMT_MDI        0x02    /*!< \ref LAN91_MGMT bit mask, reflects MDI pin status. */
#define LAN91_MGMT_MDO        0x01    /*!< \ref LAN91_MGMT bit mask, drives MDO pin. */

/*!
 * \brief Bank 3 - Revision register.
 */
#define LAN91_REV         (LAN91_BASE_ADDR + 0x0A)

/*!
 * \brief Bank 3 - Early RCV register.
 */
#define LAN91_ERCV        (LAN91_BASE_ADDR + 0x0C)

/*!
 * \brief PHY control register.
 */
#define LAN91_PHYCR       0

#define LAN91_PHYCR_RST       0x8000  /*!< \ref LAN91_PHYCR bit mask, resets PHY. */
#define LAN91_PHYCR_LPBK      0x4000  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_SPEED     0x2000  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_ANEG_EN   0x1000  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_PDN       0x0800  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_MII_DIS   0x0400  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_ANEG_RST  0x0200  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_DPLX      0x0100  /*!< \ref LAN91_PHYCR bit mask, . */
#define LAN91_PHYCR_COLST     0x0080  /*!< \ref LAN91_PHYCR bit mask, . */


/*!
 * \brief PHY status register.
 */
#define LAN91_PHYSR       1

#define LAN91_PHYSR_CAP_T4    0x8000  /*!< \ref LAN91_PHYSR bit mask, indicates 100BASE-T4 capability. */
#define LAN91_PHYSR_CAP_TXF   0x4000  /*!< \ref LAN91_PHYSR bit mask, indicates 100BASE-TX full duplex capability. */
#define LAN91_PHYSR_CAP_TXH   0x2000  /*!< \ref LAN91_PHYSR bit mask, indicates 100BASE-TX half duplex capability. */
#define LAN91_PHYSR_CAP_TF    0x1000  /*!< \ref LAN91_PHYSR bit mask, indicates 10BASE-T full duplex capability. */
#define LAN91_PHYSR_CAP_TH    0x0800  /*!< \ref LAN91_PHYSR bit mask, indicates 10BASE-T half duplex capability. */
#define LAN91_PHYSR_CAP_SUPR  0x0040  /*!< \ref LAN91_PHYSR bit mask, indicates preamble suppression capability. */
#define LAN91_PHYSR_ANEG_ACK  0x0020  /*!< \ref LAN91_PHYSR bit mask, auto-negotiation completed. */
#define LAN91_PHYSR_REM_FLT   0x0010  /*!< \ref LAN91_PHYSR bit mask, remote fault detected. */
#define LAN91_PHYSR_CAP_ANEG  0x0008  /*!< \ref LAN91_PHYSR bit mask, indicates auto-negotiation capability. */
#define LAN91_PHYSR_LINK      0x0004  /*!< \ref LAN91_PHYSR bit mask, valid link status. */
#define LAN91_PHYSR_JAB       0x0002  /*!< \ref LAN91_PHYSR bit mask, jabber collision detected. */
#define LAN91_PHYSR_EXREG     0x0001  /*!< \ref LAN91_PHYSR bit mask, extended capabilities available. */


/*!
 * \brief PHY identifier register 1.
 */
#define LAN91_PHYID1      2

/*!
 * \brief PHY identifier register 1.
 */
#define LAN91_PHYID2      3

/*!
 * \brief PHY auto-negotiation advertisement register.
 */
#define LAN91_PHYANAD     4

#define LAN91_PHYANAD_NP      0x8000  /*!< \ref LAN91_PHYANAD bit mask, exchanging next page information. */
#define LAN91_PHYANAD_ACK     0x4000  /*!< \ref LAN91_PHYANAD bit mask, acknowledged. */
#define LAN91_PHYANAD_RF      0x2000  /*!< \ref LAN91_PHYANAD bit mask, remote fault. */
#define LAN91_PHYANAD_T4      0x0200  /*!< \ref LAN91_PHYANAD bit mask, indicates 100BASE-T4 capability. */
#define LAN91_PHYANAD_TX_FDX  0x0100  /*!< \ref LAN91_PHYANAD bit mask, indicates 100BASE-TX full duplex capability. */
#define LAN91_PHYANAD_TX_HDX  0x0080  /*!< \ref LAN91_PHYANAD bit mask, indicates 100BASE-TX half duplex capability. */
#define LAN91_PHYANAD_10FDX   0x0040  /*!< \ref LAN91_PHYANAD bit mask, indicates 10BASE-T full duplex capability. */
#define LAN91_PHYANAD_10_HDX  0x0020  /*!< \ref LAN91_PHYANAD bit mask, indicates 10BASE-T half duplex capability. */
#define LAN91_PHYANAD_CSMA    0x0001  /*!< \ref LAN91_PHYANAD bit mask, indicates 802.3 CSMA capability. */

/*!
 * \brief PHY auto-negotiation remote end capability register.
 */
#define LAN91_PHYANRC     5

/*!
 * \brief PHY configuration register 1.
 */
#define LAN91_PHYCFR1     16

/*!
 * \brief PHY configuration register 2.
 */
#define LAN91_PHYCFR2     17

/*!
 * \brief PHY status output register.
 */
#define LAN91_PHYSOR      18

#define LAN91_PHYSOR_INT      0x8000  /*!< \ref LAN91_PHYSOR bit mask, interrupt bits changed. */
#define LAN91_PHYSOR_LNKFAIL  0x4000  /*!< \ref LAN91_PHYSOR bit mask, link failure detected. */
#define LAN91_PHYSOR_LOSSSYNC 0x2000  /*!< \ref LAN91_PHYSOR bit mask, descrambler sync lost detected. */
#define LAN91_PHYSOR_CWRD     0x1000  /*!< \ref LAN91_PHYSOR bit mask, code word error detected. */
#define LAN91_PHYSOR_SSD      0x0800  /*!< \ref LAN91_PHYSOR bit mask, start of stream error detected. */
#define LAN91_PHYSOR_ESD      0x0400  /*!< \ref LAN91_PHYSOR bit mask, end of stream error detected. */
#define LAN91_PHYSOR_RPOL     0x0200  /*!< \ref LAN91_PHYSOR bit mask, reverse polarity detected. */
#define LAN91_PHYSOR_JAB      0x0100  /*!< \ref LAN91_PHYSOR bit mask, jabber detected. */
#define LAN91_PHYSOR_SPDDET   0x0080  /*!< \ref LAN91_PHYSOR bit mask, 100/10 speed detected. */
#define LAN91_PHYSOR_DPLXDET  0x0040  /*!< \ref LAN91_PHYSOR bit mask, duplex detected. */

/*!
 * \brief PHY mask register.
 */
#define LAN91_PHYMSK      19

#define LAN91_PHYMSK_MINT     0x8000  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_INT interrupt. */
#define LAN91_PHYMSK_MLNKFAIL 0x4000  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_LNKFAIL interrupt. */
#define LAN91_PHYMSK_MLOSSSYN 0x2000  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_LOSSSYNC interrupt. */
#define LAN91_PHYMSK_MCWRD    0x1000  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_CWRD interrupt. */
#define LAN91_PHYMSK_MSSD     0x0800  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_SSD interrupt. */
#define LAN91_PHYMSK_MESD     0x0400  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_ESD interrupt. */
#define LAN91_PHYMSK_MRPOL    0x0200  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_RPOL interrupt. */
#define LAN91_PHYMSK_MJAB     0x0100  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_JAB interrupt. */
#define LAN91_PHYMSK_MSPDDT   0x0080  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_SPDDET interrupt. */
#define LAN91_PHYMSK_MDPLDT   0x0040  /*!< \ref LAN91_PHYMSK bit mask, enables \ref LAN91_PHYSOR_DPLXDET interrupt. */

/*@}*/


__BEGIN_DECLS

/*
 * Available drivers.
 */
extern NUTDEVICE devLan91;

#ifndef DEV_ETHER
#define DEV_ETHER   devLan91
#endif


__END_DECLS

#endif
