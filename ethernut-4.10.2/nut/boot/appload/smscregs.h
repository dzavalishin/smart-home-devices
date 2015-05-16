#ifndef _SMSCREGS_H_
#define _SMSCREGS_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#define NIC_BASE            0xC000

/*! 
 * \brief Bank select register. 
 */
#define NIC_BSR         NIC_BASE + 0x0E

/*! 
 * \brief Bank 0 - Transmit control register. 
 */
#define NIC_TCR         NIC_BASE + 0x00

#define TCR_SWFDUP      0x8000  /*!< \ref NIC_TCR bit mask, enables full duplex.  */
#define TCR_EPH_LOOP    0x2000  /*!< \ref NIC_TCR bit mask, enables internal loopback. */
#define TCR_STP_SQET    0x1000  /*!< \ref NIC_TCR bit mask, enables transmission stop on SQET error. */
#define TCR_FDUPLX      0x0800  /*!< \ref NIC_TCR bit mask, enables receiving own frames. */
#define TCR_MON_CSN     0x0400  /*!< \ref NIC_TCR bit mask, enables carrier monitoring. */
#define TCR_NOCRC       0x0100  /*!< \ref NIC_TCR bit mask, disables CRC transmission. */
#define TCR_PAD_EN      0x0080  /*!< \ref NIC_TCR bit mask, enables automatic padding. */
#define TCR_FORCOL      0x0004  /*!< \ref NIC_TCR bit mask, forces collision. */
#define TCR_LOOP        0x0002  /*!< \ref NIC_TCR bit mask, enables PHY loopback. */
#define TCR_TXENA       0x0001  /*!< \ref NIC_TCR bit mask, enables transmitter. */


/*! 
 * \brief Bank 0 - EPH status register. 
 */
#define NIC_EPHSR       NIC_BASE + 0x02

/*! 
 * \brief Bank 0 - Receive control register. 
 */
#define NIC_RCR         NIC_BASE + 0x04

#define RCR_SOFT_RST    0x8000  /*!< \ref NIC_RCR bit mask, activates software reset. */
#define RCR_FILT_CAR    0x4000  /*!< \ref NIC_RCR bit mask, enables carrier filter. */
#define RCR_ABORT_ENB   0x2000  /*!< \ref NIC_RCR bit mask, enables receive abort on collision. */
#define RCR_STRIP_CRC   0x0200  /*!< \ref NIC_RCR bit mask, strips CRC. */
#define RCR_RXEN        0x0100  /*!< \ref NIC_RCR bit mask, enables receiver. */
#define RCR_ALMUL       0x0004  /*!< \ref NIC_RCR bit mask, multicast frames accepted when set. */
#define RCR_PRMS        0x0002  /*!< \ref NIC_RCR bit mask, enables promiscuous mode. */
#define RCR_RX_ABORT    0x0001  /*!< \ref NIC_RCR bit mask, set when receive was aborted. */

/*! 
 * \brief Bank 0 - Counter register.
 */
#define NIC_ECR         NIC_BASE + 0x06

/*! 
 * \brief Bank 0 - Memory information register.
 */
#define NIC_MIR         NIC_BASE + 0x08

/*! 
 * \brief Bank 0 - Receive / PHY control register.
 */
#define NIC_RPCR        NIC_BASE + 0x0A

#define RPCR_SPEED      0x2000  /*!< \ref NIC_RPCR bit mask, PHY operates at 100 Mbps. */
#define RPCR_DPLX       0x1000  /*!< \ref NIC_RPCR bit mask, PHY operates at full duplex mode. */
#define RPCR_ANEG       0x0800  /*!< \ref NIC_RPCR bit mask, sets PHY in auto-negotiation mode. */
#define RPCR_LEDA_PAT   0x0000  /*!< \ref NIC_RPCR bit mask for LEDA mode. */
#define RPCR_LEDB_PAT   0x0010  /*!< \ref NIC_RPCR bit mask for LEDB mode. */

/*! 
 * \brief Bank 1 - Configuration register.
 */
#define NIC_CR          NIC_BASE + 0x00

#define CR_EPH_EN       0x8000  /*!< \ref NIC_CR bit mask, . */

/*! 
 * \brief Bank 1 - Base address register.
 */
#define NIC_BAR         NIC_BASE + 0x02

/*! 
 * \brief Bank 1 - Individual address register.
 */
#define NIC_IAR         NIC_BASE + 0x04

/*! 
 * \brief Bank 1 - General purpose register.
 */
#define NIC_GPR         NIC_BASE + 0x0A

/*! 
 * \brief Bank 1 - Control register.
 */
#define NIC_CTR         NIC_BASE + 0x0C

#define CTR_RCV_BAD     0x4000  /*!< \ref NIC_CTR bit mask. */
#define CTR_AUTO_RELEASE 0x0800 /*!< \ref NIC_CTR bit mask, transmit packets automatically released. */

/*!
 * \brief Bank 2 - MMU command register.
 */
#define NIC_MMUCR       NIC_BASE + 0x00

#define MMUCR_BUSY      0x0001

#define MMU_NOP         0
#define MMU_ALO         (1<<5)
#define MMU_RST         (2<<5)
#define MMU_REM         (3<<5)
#define MMU_TOP         (4<<5)
#define MMU_PKT         (5<<5)
#define MMU_ENQ         (6<<5)
#define MMU_RTX         (7<<5)

/*!
 * \brief Bank 2 - Packet number register.
 *
 * This byte register specifies the accessible transmit packet number.
 */
#define NIC_PNR         NIC_BASE + 0x02

/*!
 * \brief Bank 2 - Allocation result register.
 *
 * This byte register is updated upon a \ref MMU_ALO command.
 */
#define NIC_ARR         NIC_BASE + 0x03

#define ARR_FAILED      0x80

/*!
 * \brief Bank 2 - FIFO ports register.
 */
#define NIC_FIFO        NIC_BASE + 0x04

/*!
 * \brief Bank 2 - Pointer register.
 */
#define NIC_PTR         NIC_BASE + 0x06

#define PTR_RCV         0x8000  /*! \ref NIC_PTR bit mask, specifies receive or transmit buffer. */
#define PTR_AUTO_INCR   0x4000  /*! \ref NIC_PTR bit mask, enables automatic pointer increment. */
#define PTR_READ        0x2000  /*! \ref NIC_PTR bit mask, indicates type of access. */
#define PTR_ETEN        0x1000  /*! \ref NIC_PTR bit mask, enables early transmit underrun detection. */
#define PTR_NOT_EMPTY   0x0800  /*! \ref NIC_PTR bit mask, set when write data fifo is not empty. */

/*!
 * \brief Bank 2 - Data register.
 */
#define NIC_DATA        NIC_BASE + 0x08

/*!
 * \brief Bank 2 - Interrupt status register.
 */
#define NIC_IST         NIC_BASE + 0x0C

/*!
 * \brief Bank 2 - Interrupt acknowledge register.
 */
#define NIC_ACK         NIC_BASE + 0x0C

/*!
 * \brief Bank 2 - Interrupt mask register.
 */
#define NIC_MSK         NIC_BASE + 0x0D

#define INT_MD          0x80    /*!< \ref PHY state change interrupt bit mask. */
#define INT_ERCV        0x40    /*!< \ref Early receive interrupt bit mask. */
#define INT_EPH         0x20    /*!< \ref Ethernet protocol interrupt bit mask. */
#define INT_RX_OVRN     0x10    /*!< \ref Receive overrun interrupt bit mask. */
#define INT_ALLOC       0x08    /*!< \ref Transmit allocation interrupt bit mask. */
#define INT_TX_EMPTY    0x04    /*!< \ref Transmitter empty interrupt bit mask. */
#define INT_TX          0x02    /*!< \ref Transmit complete interrupt bit mask. */
#define INT_RCV         0x01    /*!< \ref Receive interrupt bit mask. */

/*!
 * \brief Bank 3 - Multicast table register.
 */
#define NIC_MT          NIC_BASE + 0x00

/*!
 * \brief Bank 3 - Management interface register.
 */
#define NIC_MGMT        NIC_BASE + 0x08

#define MGMT_MDOE       0x08    /*!< \ref NIC_MGMT bit mask, enables MDO pin. */
#define MGMT_MCLK       0x04    /*!< \ref NIC_MGMT bit mask, drives MDCLK pin. */
#define MGMT_MDI        0x02    /*!< \ref NIC_MGMT bit mask, reflects MDI pin status. */
#define MGMT_MDO        0x01    /*!< \ref NIC_MGMT bit mask, drives MDO pin. */

/*!
 * \brief Bank 3 - Revision register.
 */
#define NIC_REV         NIC_BASE + 0x0A

/*!
 * \brief Bank 3 - Early RCV register.
 */
#define NIC_ERCV        NIC_BASE + 0x0C

/*!
 * \brief PHY control register.
 */
#define NIC_PHYCR       0

#define PHYCR_RST       0x8000  /*!< \ref NIC_PHYCR bit mask, resets PHY. */
#define PHYCR_LPBK      0x4000  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_SPEED     0x2000  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_ANEG_EN   0x1000  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_PDN       0x0800  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_MII_DIS   0x0400  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_ANEG_RST  0x0200  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_DPLX      0x0100  /*!< \ref NIC_PHYCR bit mask, . */
#define PHYCR_COLST     0x0080  /*!< \ref NIC_PHYCR bit mask, . */


/*!
 * \brief PHY status register.
 */
#define NIC_PHYSR       1

#define PHYSR_CAP_T4    0x8000  /*!< \ref NIC_PHYSR bit mask, indicates 100BASE-T4 capability. */
#define PHYSR_CAP_TXF   0x4000  /*!< \ref NIC_PHYSR bit mask, indicates 100BASE-TX full duplex capability. */
#define PHYSR_CAP_TXH   0x2000  /*!< \ref NIC_PHYSR bit mask, indicates 100BASE-TX half duplex capability. */
#define PHYSR_CAP_TF    0x1000  /*!< \ref NIC_PHYSR bit mask, indicates 10BASE-T full duplex capability. */
#define PHYSR_CAP_TH    0x0800  /*!< \ref NIC_PHYSR bit mask, indicates 10BASE-T half duplex capability. */
#define PHYSR_CAP_SUPR  0x0040  /*!< \ref NIC_PHYSR bit mask, indicates preamble suppression capability. */
#define PHYSR_ANEG_ACK  0x0020  /*!< \ref NIC_PHYSR bit mask, auto-negotiation completed. */
#define PHYSR_REM_FLT   0x0010  /*!< \ref NIC_PHYSR bit mask, remote fault detected. */
#define PHYSR_CAP_ANEG  0x0008  /*!< \ref NIC_PHYSR bit mask, indicates auto-negotiation capability. */
#define PHYSR_LINK      0x0004  /*!< \ref NIC_PHYSR bit mask, valid link status. */
#define PHYSR_JAB       0x0002  /*!< \ref NIC_PHYSR bit mask, jabber collision detected. */
#define PHYSR_EXREG     0x0001  /*!< \ref NIC_PHYSR bit mask, extended capabilities available. */


/*!
 * \brief PHY identifier register 1.
 */
#define NIC_PHYID1      2

/*!
 * \brief PHY identifier register 1.
 */
#define NIC_PHYID2      3

/*!
 * \brief PHY auto-negotiation advertisement register.
 */
#define NIC_PHYANAD     4

#define PHYANAD_NP      0x8000  /*!< \ref NIC_PHYANAD bit mask, exchanging next page information. */
#define PHYANAD_ACK     0x4000  /*!< \ref NIC_PHYANAD bit mask, acknowledged. */
#define PHYANAD_RF      0x2000  /*!< \ref NIC_PHYANAD bit mask, remote fault. */
#define PHYANAD_T4      0x0200  /*!< \ref NIC_PHYANAD bit mask, indicates 100BASE-T4 capability. */
#define PHYANAD_TX_FDX  0x0100  /*!< \ref NIC_PHYANAD bit mask, indicates 100BASE-TX full duplex capability. */
#define PHYANAD_TX_HDX  0x0080  /*!< \ref NIC_PHYANAD bit mask, indicates 100BASE-TX half duplex capability. */
#define PHYANAD_10FDX   0x0040  /*!< \ref NIC_PHYANAD bit mask, indicates 10BASE-T full duplex capability. */
#define PHYANAD_10_HDX  0x0020  /*!< \ref NIC_PHYANAD bit mask, indicates 10BASE-T half duplex capability. */
#define PHYANAD_CSMA    0x0001  /*!< \ref NIC_PHYANAD bit mask, indicates 802.3 CSMA capability. */

/*!
 * \brief PHY auto-negotiation remote end capability register.
 */
#define NIC_PHYANRC     5

/*!
 * \brief PHY configuration register 1.
 */
#define NIC_PHYCFR1     16

/*!
 * \brief PHY configuration register 2.
 */
#define NIC_PHYCFR2     17

/*!
 * \brief PHY status output register.
 */
#define NIC_PHYSOR      18

#define PHYSOR_INT      0x8000  /*!< \ref NIC_PHYSOR bit mask, interrupt bits changed. */
#define PHYSOR_LNKFAIL  0x4000  /*!< \ref NIC_PHYSOR bit mask, link failure detected. */
#define PHYSOR_LOSSSYNC 0x2000  /*!< \ref NIC_PHYSOR bit mask, descrambler sync lost detected. */
#define PHYSOR_CWRD     0x1000  /*!< \ref NIC_PHYSOR bit mask, code word error detected. */
#define PHYSOR_SSD      0x0800  /*!< \ref NIC_PHYSOR bit mask, start of stream error detected. */
#define PHYSOR_ESD      0x0400  /*!< \ref NIC_PHYSOR bit mask, end of stream error detected. */
#define PHYSOR_RPOL     0x0200  /*!< \ref NIC_PHYSOR bit mask, reverse polarity detected. */
#define PHYSOR_JAB      0x0100  /*!< \ref NIC_PHYSOR bit mask, jabber detected. */
#define PHYSOR_SPDDET   0x0080  /*!< \ref NIC_PHYSOR bit mask, 100/10 speed detected. */
#define PHYSOR_DPLXDET  0x0040  /*!< \ref NIC_PHYSOR bit mask, duplex detected. */

/*!
 * \brief PHY mask register.
 */
#define NIC_PHYMSK      19

#define PHYMSK_MINT     0x8000  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_INT interrupt. */
#define PHYMSK_MLNKFAIL 0x4000  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_LNKFAIL interrupt. */
#define PHYMSK_MLOSSSYN 0x2000  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_LOSSSYNC interrupt. */
#define PHYMSK_MCWRD    0x1000  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_CWRD interrupt. */
#define PHYMSK_MSSD     0x0800  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_SSD interrupt. */
#define PHYMSK_MESD     0x0400  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_ESD interrupt. */
#define PHYMSK_MRPOL    0x0200  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_RPOL interrupt. */
#define PHYMSK_MJAB     0x0100  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_JAB interrupt. */
#define PHYMSK_MSPDDT   0x0080  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_SPDDET interrupt. */
#define PHYMSK_MDPLDT   0x0040  /*!< \ref NIC_PHYMSK bit mask, enables \ref PHYSOR_DPLXDET interrupt. */

/*!
 * \brief Control byte flags.
 */
#define CTRLBYTE_CRC    0x1000  /*!< \brief CRC will be appended. */
#define CTRLBYTE_ODD    0x2000  /*!< \brief Indicates odd packet length. */

#define MSBV(bit)       (1 << ((bit) - 8))

#define nic_outlb(addr, val) (*(volatile u_char *)(addr) = (val))
#define nic_outhb(addr, val) (*(volatile u_char *)((addr) + 1) = (val))
#define nic_outwx(addr, val) (*(volatile u_short *)(addr) = (val))
#define nic_outw(addr, val) { \
    *(volatile u_char *)(addr) = (u_char)(val); \
    *((volatile u_char *)(addr) + 1) = (u_char)((val) >> 8); \
}

#define nic_inlb(addr) (*(volatile u_char *)(addr))
#define nic_inhb(addr) (*(volatile u_char *)((addr) + 1))
#define nic_inw(addr) (*(volatile u_short *)(addr))

#define nic_bs(bank)    nic_outlb(NIC_BSR, bank)

#endif
