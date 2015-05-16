/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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
* $Log: macb.c,v $
*
*/

#include <cfg/os.h>
#include <cfg/dev.h>
#include <cfg/arch/gpio.h>

#include <arch/avr32.h>
#include <arch/avr32/gpio.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>

#include <dev/irqreg.h>
#include <dev/gpio.h>

#include <avr32/io.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

#ifndef NUT_THREAD_NICRXSTACK
#define NUT_THREAD_NICRXSTACK   768
#endif

#ifndef EMAC_RX_BUFFERS
#define EMAC_RX_BUFFERS         32
#endif
#define EMAC_RX_BUFSIZ          128

#define EMAC_TX_BUFFERS         2
#ifndef EMAC_TX_BUFSIZ
#define EMAC_TX_BUFSIZ          1536
#endif

#ifndef EMAC_LINK_LOOPS
#define EMAC_LINK_LOOPS         1000000
#endif

/*!
* \addtogroup xgDm9161aRegs
*/
/*@{*/
#define NIC_PHY_BMCR            0x00    /*!< \brief Basic mode control register. */
#define NIC_PHY_BMCR_COLTEST    0x0080  /*!< \brief Collision test. */
#define NIC_PHY_BMCR_FDUPLEX    0x0100  /*!< \brief Full duplex mode. */
#define NIC_PHY_BMCR_ANEGSTART  0x0200  /*!< \brief Restart auto negotiation. */
#define NIC_PHY_BMCR_ISOLATE    0x0400  /*!< \brief Isolate from MII. */
#define NIC_PHY_BMCR_PWRDN      0x0800  /*!< \brief Power-down. */
#define NIC_PHY_BMCR_ANEGENA    0x1000  /*!< \brief Enable auto negotiation. */
#define NIC_PHY_BMCR_100MBPS    0x2000  /*!< \brief Select 100 Mbps. */
#define NIC_PHY_BMCR_LOOPBACK   0x4000  /*!< \brief Enable loopback mode. */
#define NIC_PHY_BMCR_RESET      0x8000  /*!< \brief Software reset. */

#define NIC_PHY_BMSR            0x01    /*!< \brief Basic mode status register. */
#define NIC_PHY_BMSR_ANCOMPL    0x0020  /*!< \brief Auto negotiation complete. */
#define NIC_PHY_BMSR_LINKSTAT   0x0004  /*!< \brief Link status. */

#define NIC_PHY_ID1             0x02    /*!< \brief PHY identifier register 1. */
#define NIC_PHY_ID2             0x03    /*!< \brief PHY identifier register 2. */
#define NIC_PHY_ANAR            0x04    /*!< \brief Auto negotiation advertisement register. */
#define NIC_PHY_ANLPAR          0x05    /*!< \brief Auto negotiation link partner availability register. */
#define NIC_PHY_ANEG_NP         0x8000  /*!< \brief Next page available. */
#define NIC_PHY_ANEG_ACK        0x4000  /*!< \brief Ability data reception acknowledged. */
#define NIC_PHY_ANEG_RF         0x2000  /*!< \brief Remote fault. */
#define NIC_PHY_ANEG_FCS        0x0400  /*!< \brief Flow control supported. */
#define NIC_PHY_ANEG_T4         0x0200  /*!< \brief 100BASE-T4 supported. */
#define NIC_PHY_ANEG_TX_FDX     0x0100  /*!< \brief 100BASE-T full duplex supported. */
#define NIC_PHY_ANEG_TX_HDX     0x0080  /*!< \brief 100BASE-T half duplex supported. */
#define NIC_PHY_ANEG_10_FDX     0x0040  /*!< \brief 10BASE-T full duplex supported. */
#define NIC_PHY_ANEG_10_HDX     0x0020  /*!< \brief 10BASE-T half duplex supported. */
#define NIC_PHY_ANEG_BINSEL     0x001F  /*!< \brief Binary encoded protocol selector. */

#define NIC_PHY_ANER            0x06    /*!< \brief Auto negotiation expansion register. */

/*!
 * \name Advertisement control register.
 */
#define NIC_PHY_ADVERTISE_SLCT          0x001f  //!< Selector bits
#define NIC_PHY_ADVERTISE_CSMA          0x0001  //!< Only selector supported
#define NIC_PHY_ADVERTISE_10HALF        0x0020  //!< Try for 10mbps half-duplex
#define NIC_PHY_ADVERTISE_1000XFULL     0x0020  //!< Try for 1000BASE-X full-duplex
#define NIC_PHY_ADVERTISE_10FULL        0x0040  //!< Try for 10mbps full-duplex
#define NIC_PHY_ADVERTISE_1000XHALF     0x0040  //!< Try for 1000BASE-X half-duplex
#define NIC_PHY_ADVERTISE_100HALF       0x0080  //!< Try for 100mbps half-duplex
#define NIC_PHY_ADVERTISE_1000XPAUSE    0x0080  //!< Try for 1000BASE-X pause
#define NIC_PHY_ADVERTISE_100FULL       0x0100  //!< Try for 100mbps full-duplex
#define NIC_PHY_ADVERTISE_1000XPSE_ASYM 0x0100  //!< Try for 1000BASE-X asym pause
#define NIC_PHY_ADVERTISE_100BASE4      0x0200  //!< Try for 100mbps 4k packets
#define NIC_PHY_ADVERTISE_PAUSE_CAP     0x0400  //!< Try for pause
#define NIC_PHY_ADVERTISE_PAUSE_ASYM    0x0800  //!< Try for asymetric pause
#define NIC_PHY_ADVERTISE_RESV          0x1000  //!< Unused...
#define NIC_PHY_ADVERTISE_RFAULT        0x2000  //!< Say we can detect faults
#define NIC_PHY_ADVERTISE_LPACK         0x4000  //!< Ack link partners response
#define NIC_PHY_ADVERTISE_NPAGE         0x8000  //!< Next page bit

#define NIC_PHY_ADVERTISE_FULL (NIC_PHY_ADVERTISE_100FULL | NIC_PHY_ADVERTISE_10FULL | NIC_PHY_ADVERTISE_CSMA)
#define NIC_PHY_ADVERTISE_ALL (NIC_PHY_ADVERTISE_10HALF | NIC_PHY_ADVERTISE_10FULL | \
	NIC_PHY_ADVERTISE_100HALF | NIC_PHY_ADVERTISE_100FULL)

/*@}*/


/*!
* \brief PHY address.
*
* Any other than 0 seems to create problems with Atmel's evaluation kits.
*/
#ifndef NIC_PHY_ADDR
#define NIC_PHY_ADDR	        0
#endif

#ifndef NIC_PHY_UID
#define NIC_PHY_UID 0xffffffff
#endif

/*!
* The EVK1100 board is delivered with RMII by default. Thus.
* we use the reduced MII for this CPU. However, this should be
* handled by the Configurator.
*/
#define PHY_MODE_RMII


/*!
* \brief Network interface controller information structure.
*/
struct _EMACINFO {
#ifdef NUT_PERFMON
    uint32_t ni_rx_packets;     /*!< Number of packets received. */
    uint32_t ni_tx_packets;     /*!< Number of packets sent. */
    uint32_t ni_overruns;       /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;        /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;  /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors;       /*!< Number of missed packets. */
#endif
    HANDLE volatile ni_rx_rdy;  /*!< Receiver event queue. */
    HANDLE volatile ni_tx_rdy;  /*!< Transmitter event queue. */
    HANDLE ni_mutex;            /*!< Access mutex semaphore. */
    volatile int ni_tx_queued;  /*!< Number of packets in transmission queue. */
    volatile int ni_tx_quelen;  /*!< Number of bytes in transmission queue not sent. */
    volatile int ni_insane;     /*!< Set by error detection. */
    int ni_iomode;              /*!< 8 or 16 bit access. 32 bit is not supported. */
};

/*!
* \brief Network interface controller information type.
*/
typedef struct _EMACINFO EMACINFO;

/*
* TODO: Buffers and their descriptors should be part of the EMACINFO
* structure. Actually there will be no dual Ethernet chip (sure?),
* but just to keep the code clean.
*/
/*! Receive Transfer descriptor structure.
*/
typedef struct _RxTdDescriptor {
    uint32_t addr;
    uint32_t status;
} RxTdDescriptor;
//! @}

/*! Transmit Transfer descriptor structure.
*/
//! @{
typedef struct _TxTdDescriptor {
    uint32_t addr;
    uint32_t status;
} TxTdDescriptor;
//! @}

static volatile TxTdDescriptor txBufTab[EMAC_TX_BUFFERS];
static volatile uint8_t txBuf[EMAC_TX_BUFFERS * EMAC_TX_BUFSIZ] __attribute__ ((aligned(4)));
static unsigned int txBufIdx;

static volatile RxTdDescriptor rxBufTab[EMAC_RX_BUFFERS];
static volatile uint8_t rxBuf[EMAC_RX_BUFFERS * EMAC_RX_BUFSIZ] __attribute__ ((aligned(4)));
static unsigned int rxBufIdx;

#define RXBUF_OWNERSHIP     0x00000001
#define RXBUF_WRAP          0x00000002
#define RXBUF_ADDRMASK      0xFFFFFFFC

#define RXS_BROADCAST_ADDR  0x80000000  /*!< \brief Broadcast address detected. */
#define RXS_MULTICAST_HASH  0x40000000  /*!< \brief Multicast hash match. */
#define RXS_UNICAST_HASH    0x20000000  /*!< \brief Unicast hash match. */
#define RXS_EXTERNAL_ADDR   0x10000000  /*!< \brief External address match. */
#define RXS_SA1_ADDR        0x04000000  /*!< \brief Specific address register 1 match. */
#define RXS_SA2_ADDR        0x02000000  /*!< \brief Specific address register 2 match. */
#define RXS_SA3_ADDR        0x01000000  /*!< \brief Specific address register 3 match. */
#define RXS_SA4_ADDR        0x00800000  /*!< \brief Specific address register 4 match. */
#define RXS_TYPE_ID         0x00400000  /*!< \brief Type ID match. */
#define RXS_VLAN_TAG        0x00200000  /*!< \brief VLAN tag detected. */
#define RXS_PRIORITY_TAG    0x00100000  /*!< \brief Priority tag detected. */
#define RXS_VLAN_PRIORITY   0x000E0000  /*!< \brief VLAN priority. */
#define RXS_CFI_IND         0x00010000  /*!< \brief Concatenation format indicator. */
#define RXS_EOF             0x00008000  /*!< \brief End of frame. */
#define RXS_SOF             0x00004000  /*!< \brief Start of frame. */
#define RXS_RBF_OFFSET      0x00003000  /*!< \brief Receive buffer offset mask. */
#define RXS_LENGTH_FRAME    0x000007FF  /*!< \brief Length of frame including FCS. */

#define TXS_USED            0x80000000  /*!< \brief Used buffer. */
#define TXS_WRAP            0x40000000  /*!< \brief Last descriptor. */
#define TXS_ERROR           0x20000000  /*!< \brief Retry limit exceeded. */
#define TXS_UNDERRUN        0x10000000  /*!< \brief Transmit underrun. */
#define TXS_NO_BUFFER       0x08000000  /*!< \brief Buffer exhausted. */
#define TXS_NO_CRC          0x00010000  /*!< \brief CRC not appended. */
#define TXS_LAST_BUFF       0x00008000  /*!< \brief Last buffer of frame. */


/*!
* \addtogroup xgNutArchArmAt91Emac
*/
/*@{*/

/*!
* \brief Read contents of PHY register.
*
* \param reg PHY register number.
*
* \return Contents of the specified register.
*/
static uint16_t phy_inw(volatile avr32_macb_t * macb, uint8_t reg)
{
    uint16_t value;

    // initiate transaction: enable management port
    macb->ncr |= AVR32_MACB_NCR_MPE_MASK;

    // Write the PHY configuration frame to the MAN register
    macb->man = (AVR32_MACB_SOF_MASK & (0x01 << AVR32_MACB_SOF_OFFSET)) // SOF
        | (2 << AVR32_MACB_CODE_OFFSET) // Code
        | (2 << AVR32_MACB_RW_OFFSET)   // Read operation
        | ((NIC_PHY_ADDR & 0x1f) << AVR32_MACB_PHYA_OFFSET)     // Phy Add
        | (reg << AVR32_MACB_REGA_OFFSET);      // Reg Add

    // wait for PHY to be ready
    while (!(macb->nsr & AVR32_MACB_NSR_IDLE_MASK));

    // read the register value in maintenance register
    value = macb->man & 0x0000ffff;

    // disable management port
    macb->ncr &= ~AVR32_MACB_NCR_MPE_MASK;

    return value;
}

/*!
* \brief Write value to PHY register.
*
* \param reg PHY register number.
* \param val Value to write.
*/
static void phy_outw(volatile avr32_macb_t * macb, uint8_t reg, uint16_t val)
{
    // initiate transaction : enable management port
    macb->ncr |= AVR32_MACB_NCR_MPE_MASK;

    // Write the PHY configuration frame to the MAN register
    macb->man = ((AVR32_MACB_SOF_MASK & (0x01 << AVR32_MACB_SOF_OFFSET))        // SOF
                 | (2 << AVR32_MACB_CODE_OFFSET)        // Code
                 | (1 << AVR32_MACB_RW_OFFSET)  // Write operation
                 | ((NIC_PHY_ADDR & 0x1f) << AVR32_MACB_PHYA_OFFSET)    // Phy Add
                 | (reg << AVR32_MACB_REGA_OFFSET))     // Reg Add
        | (val & 0xffff);       // Data

    // wait for PHY to be ready
    while (!(macb->nsr & AVR32_MACB_NSR_IDLE_MASK));

    // disable management port
    macb->ncr &= ~AVR32_MACB_NCR_MPE_MASK;
}

/*!
 * \brief Probe PHY.
 *
 * \return 0 on success, -1 otherwise.
 */
static int probePhy(volatile avr32_macb_t * macb)
{
    uint32_t physID;
    uint16_t phyval;
    // Read Phy ID. Ignore revision number.
    physID = (phy_inw(macb, NIC_PHY_ID2) & 0xFFF0) | ((phy_inw(macb, NIC_PHY_ID1) << 16) & 0xFFFF0000);
#if NIC_PHY_UID != 0xffffffff
    if (physID != (NIC_PHY_UID & 0xFFFFFFF0)) {
        return -1;
    }
#endif

    phyval = NIC_PHY_ADVERTISE_CSMA | NIC_PHY_ADVERTISE_ALL;
    phy_outw(macb, NIC_PHY_ANAR, phyval);

    phyval = phy_inw(macb, NIC_PHY_BMCR);
    phyval |= (NIC_PHY_BMCR_ANEGSTART | NIC_PHY_BMCR_ANEGENA);
    phy_outw(macb, NIC_PHY_BMCR, phyval);

    /* Handle auto negotiation if configured. */
    phyval = phy_inw(macb, NIC_PHY_BMCR);
    if (phyval & NIC_PHY_BMCR_ANEGENA) {
        int loops = EMAC_LINK_LOOPS;
        /* Wait for auto negotiation completed. */
        phy_inw(macb, NIC_PHY_BMSR);    /* Discard previously latched status. */
        while (--loops) {
            if (phy_inw(macb, NIC_PHY_BMSR) & NIC_PHY_BMSR_ANCOMPL) {
                break;
            }
        }
        /* Return error on link timeout. */
        if (loops == 0) {
            macb->ncr &= ~AVR32_MACB_NCR_MPE_MASK;
            return -1;
        }

        /*
         * Read link partner abilities and configure EMAC.
         */
        phyval = phy_inw(macb, NIC_PHY_ANLPAR);
        if (phyval & NIC_PHY_ANEG_TX_FDX) {
            /* 100Mb full duplex. */
            macb->ncfgr |= AVR32_MACB_SPD_MASK | AVR32_MACB_FD_MASK;
        } else if (phyval & NIC_PHY_ANEG_TX_HDX) {
            /* 100Mb half duplex. */
            macb->ncfgr = (macb->ncfgr & ~AVR32_MACB_FD_MASK) | AVR32_MACB_SPD_MASK;
        } else if (phyval & NIC_PHY_ANEG_10_FDX) {
            /* 10Mb full duplex. */
            macb->ncfgr = (macb->ncfgr & ~AVR32_MACB_SPD_MASK) | AVR32_MACB_FD_MASK;
        } else {
            /* 10Mb half duplex. */
            macb->ncfgr &= ~(AVR32_MACB_SPD_MASK | AVR32_MACB_FD_MASK);
        }
    }
    return 0;
}

/*!
* \brief Reset the Ethernet controller.
*
* \return 0 on success, -1 otherwise.
*/
static int EmacReset(NUTDEVICE * dev)
{
    volatile avr32_macb_t *macb = (avr32_macb_t *) dev->dev_base;
    const uint32_t hclk_hz = NutArchClockGet(NUT_HWCLK_PERIPHERAL_B);

    /* Disable TX and RX */
    macb->ncr = 0;

    /* Clear status registers */
    macb->NCR.clrstat = 1;

    /* Clear all status flags */
    macb->tsr = ~0UL;
    macb->rsr = ~0UL;

    /* Disable all interrupts */
    NutEnterCritical();
    macb->idr = ~0UL;
    macb->isr;
    NutExitCritical();

#if defined(PHY_MODE_RMII)
    macb->usrio &= ~AVR32_MACB_RMII_MASK;
#else
    macb->usrio |= AVR32_MACB_RMII_MASK;
#endif

    /* Set MII management clock divider */
    if (hclk_hz <= 20000000)
        macb->ncfgr |= (AVR32_MACB_NCFGR_CLK_DIV8 << AVR32_MACB_NCFGR_CLK_OFFSET);
    else if (hclk_hz <= 40000000)
        macb->ncfgr |= (AVR32_MACB_NCFGR_CLK_DIV16 << AVR32_MACB_NCFGR_CLK_OFFSET);
    else if (hclk_hz <= 80000000)
        macb->ncfgr |= (AVR32_MACB_NCFGR_CLK_DIV32 << AVR32_MACB_NCFGR_CLK_OFFSET);
    else
        macb->ncfgr |= (AVR32_MACB_NCFGR_CLK_DIV64 << AVR32_MACB_NCFGR_CLK_OFFSET);

    /* Wait for PHY ready. */
    NutDelay(255);

    return probePhy(macb);
}

/*
* NIC interrupt entry.
*/
static void EmacInterrupt(void *arg)
{
    unsigned int isr;
    unsigned int event;
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    volatile avr32_macb_t *macb = (avr32_macb_t *) dev->dev_base;
    EMACINFO *ni = dev->dev_dcb;

    /* Read interrupt status and disable interrupts. */
    isr = macb->isr;
    event = macb->rsr;

    /* Receiver interrupt. */
    if ((isr & AVR32_MACB_IMR_RCOMP_MASK) || (event & AVR32_MACB_REC_MASK)) {
        macb->rsr = AVR32_MACB_REC_MASK;        // Clear
        macb->rsr;              // Read to force the previous write
        macb->idr = AVR32_MACB_IDR_RCOMP_MASK | AVR32_MACB_IDR_ROVR_MASK | AVR32_MACB_IDR_RXUBR_MASK;
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Transmitter interrupt. */
    if (isr & AVR32_MACB_IMR_TCOMP_MASK) {
        macb->tsr = AVR32_MACB_TSR_COMP_MASK;   // Clear
        macb->tsr;              // Read to force the previous write

        NutEventPostFromIrq(&ni->ni_tx_rdy);
    }
}

/*!
* \brief Fetch the next packet out of the receive buffers.
*
* \return 0 on success, -1 otherwise.
*/
static int EmacGetPacket(EMACINFO * ni, NETBUF ** nbp)
{
    int rc = -1;
    unsigned int fbc = 0;
    unsigned int i;
    *nbp = NULL;

    /*
     * Search the next frame start. Release any fragment.
     */
    while ((rxBufTab[rxBufIdx].addr & RXBUF_OWNERSHIP) != 0 && (rxBufTab[rxBufIdx].status & RXS_SOF) == 0) {
        rxBufTab[rxBufIdx].addr &= ~(RXBUF_OWNERSHIP);
        rxBufIdx++;
        if (rxBufIdx >= EMAC_RX_BUFFERS) {
            rxBufIdx = 0;
        }
    }

    /*
     * Determine the size of the next frame.
     */
    i = rxBufIdx;
    while (rxBufTab[i].addr & RXBUF_OWNERSHIP) {
        if (i != rxBufIdx && (rxBufTab[i].status & RXS_SOF) != 0) {
            do {
                rxBufTab[rxBufIdx].addr &= ~(RXBUF_OWNERSHIP);
                rxBufIdx++;
                if (rxBufIdx >= EMAC_RX_BUFFERS) {
                    rxBufIdx = 0;
                }
            } while ((rxBufTab[rxBufIdx].addr & RXBUF_OWNERSHIP) != 0 && (rxBufTab[rxBufIdx].status & RXS_SOF) == 0);
            break;
        }
        if ((fbc = rxBufTab[i].status & RXS_LENGTH_FRAME) != 0) {
            break;
        }
        i++;
        if (i >= EMAC_RX_BUFFERS) {
            i = 0;
        }
    }

    if (fbc) {
        /*
         * Receiving long packets is unexpected. Let's declare the
         * chip insane. Short packets will be handled by the caller.
         */
//              if (fbc > 1536) {
//                      ni->ni_insane = 1;
//              } else 
        {
            *nbp = NutNetBufAlloc(0, NBAF_DATALINK, (uint16_t) fbc);
            if (*nbp != NULL) {
                uint8_t *bp = (uint8_t *) (*nbp)->nb_dl.vp;
                unsigned int len;

                while (fbc) {
                    if (fbc > EMAC_RX_BUFSIZ) {
                        len = EMAC_RX_BUFSIZ;
                    } else {
                        len = fbc;
                    }
                    memcpy(bp, (void *) (rxBufTab[rxBufIdx].addr & RXBUF_ADDRMASK), len);
                    rxBufTab[rxBufIdx].addr &= ~RXBUF_OWNERSHIP;
                    rxBufIdx++;
                    if (rxBufIdx >= EMAC_RX_BUFFERS) {
                        rxBufIdx = 0;
                    }
                    fbc -= len;
                    bp += len;
                }
                rc = 0;
            }
        }
    }
    return rc;
}

/*!
* \brief Load a packet into the nic's transmit ring buffer.
*
* \todo This routine simply does not work. Any idea?
*
* \param nb Network buffer structure containing the packet to be sent.
*           The structure must have been allocated by a previous
*           call NutNetBufAlloc(). This routine will automatically
*           release the buffer in case of an error.
*
* \return 0 on success, -1 in case of any errors. Errors
*         will automatically release the network buffer
*         structure.
*/
static int EmacPutPacket(int bufnum, NUTDEVICE * dev, NETBUF * nb)
{
    volatile avr32_macb_t *macb = (avr32_macb_t *) dev->dev_base;
    int rc = -1;
    unsigned int sz;
    uint8_t *buf;
    EMACINFO *ni = dev->dev_dcb;


    /*
     * Calculate the number of bytes to be send. Do not send packets
     * larger than the Ethernet maximum transfer unit. The MTU
     * consist of 1500 data bytes plus the 14 byte Ethernet header
     * plus 4 bytes CRC. We check the data bytes only.
     */
    if ((sz = nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz) > ETHERMTU) {
        return -1;
    }
    sz += nb->nb_dl.sz;
    if (sz & 1) {
        sz++;
    }

    /* Disable EMAC interrupts. */
    NutIrqDisable(&sig_MACB);

    /* TODO: Check for link. */
    if (ni->ni_insane == 0) {
        buf = (uint8_t *) txBufTab[bufnum].addr;
        memcpy(buf, nb->nb_dl.vp, nb->nb_dl.sz);
        buf += nb->nb_dl.sz;
        memcpy(buf, nb->nb_nw.vp, nb->nb_nw.sz);
        buf += nb->nb_nw.sz;
        memcpy(buf, nb->nb_tp.vp, nb->nb_tp.sz);
        buf += nb->nb_tp.sz;
        memcpy(buf, nb->nb_ap.vp, nb->nb_ap.sz);
        sz |= TXS_LAST_BUFF;
        if (bufnum) {
            sz |= TXS_WRAP;
        }
        txBufTab[bufnum].status = sz;
        macb->ncr |= AVR32_MACB_TSTART_MASK;
        rc = 0;
#ifdef NUT_PERFMON
        ni->ni_tx_packets++;
#endif
    }

    /* Enable EMAC interrupts. */
    NutIrqEnable(&sig_MACB);

    return rc;
}


/*!
* \brief Fire up the network interface.
*
* NIC interrupts must be disabled when calling this function.
*
* \param mac Six byte unique MAC address.
*/
static int EmacStart(volatile avr32_macb_t * macb, CONST uint8_t * mac)
{
    unsigned int i;

    /* Set local MAC address. */
    // Must be written SA1L then SA1H.
    macb->sa1b = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0];
    macb->sa1t = (mac[5] << 8) | mac[4];

    /* Initialize receive buffer descriptors. */
    for (i = 0; i < EMAC_RX_BUFFERS - 1; i++) {
        rxBufTab[i].addr = (unsigned int) (&rxBuf[i * EMAC_RX_BUFSIZ]) & RXBUF_ADDRMASK;
    }
    rxBufTab[i].addr = ((unsigned int) (&rxBuf[i * EMAC_RX_BUFSIZ]) & RXBUF_ADDRMASK) | RXBUF_WRAP;
    macb->rbqp = (unsigned long) rxBufTab;

    /* Initialize transmit buffer descriptors. */
    for (i = 0; i < EMAC_TX_BUFFERS - 1; i++) {
        txBufTab[i].addr = (unsigned int) (&txBuf[i * EMAC_RX_BUFSIZ]);
        txBufTab[i].status = TXS_USED;
    }
    txBufTab[i].addr = (unsigned int) (&txBuf[i * EMAC_RX_BUFSIZ]);
    txBufTab[i].status = TXS_USED | TXS_WRAP;
    macb->tbqp = (unsigned long) txBufTab;

    /* Clear receiver status. */
    macb->rsr = AVR32_MACB_RSR_BNA_MASK | AVR32_MACB_RSR_OVR_MASK | AVR32_MACB_RSR_REC_MASK;
    macb->rsr;

    /* Discard FCS. */
    macb->ncfgr |= AVR32_MACB_NCFGR_DRFCS_MASK;

    /* Enable receiver, transmitter and statistics. */
    macb->ncr |= AVR32_MACB_NCR_RE_MASK | AVR32_MACB_NCR_TE_MASK;

    return 0;
}

/*! \fn EmacRxThread(void *arg)
* \brief NIC receiver thread.
*
*/
THREAD(EmacRxThread, arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IFNET *ifn;
    EMACINFO *ni;
    NETBUF *nb;
    volatile avr32_macb_t *macb = (avr32_macb_t *) dev->dev_base;

    ifn = (IFNET *) dev->dev_icb;
    ni = (EMACINFO *) dev->dev_dcb;

    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until a valid one
     * has been set.
     */
    while (!ETHER_IS_UNICAST(ifn->if_mac)) {
        NutSleep(10);
    }

    /*
     * Do not continue unless we managed to start the NIC. We are
     * trapped here if the Ethernet link cannot be established.
     * This happens, for example, if no Ethernet cable is plugged
     * in.
     */
    while (EmacStart(macb, ifn->if_mac)) {
        EmacReset(dev);
        NutSleep(1000);
    }

    /* Initialize the access mutex. */
    NutEventPost(&ni->ni_mutex);

    /* Run at high priority. */
    NutThreadSetPriority(9);

    /* Enable receive and transmit interrupts. */
    macb->ier = AVR32_MACB_IER_ROVR_MASK | AVR32_MACB_IER_TCOMP_MASK | AVR32_MACB_IER_TUND_MASK |
        AVR32_MACB_IER_RXUBR_MASK | AVR32_MACB_IER_RCOMP_MASK;
    NutIrqEnable(&sig_MACB);

    for (;;) {
        /*
         * Wait for the arrival of new packets or poll the receiver every
         * 200 milliseconds. This short timeout helps a bit to deal with
         * the SAM9260 Ethernet problem.
         */
        NutEventWait(&ni->ni_rx_rdy, 200);

        /*
         * Fetch all packets from the NIC's internal buffer and pass
         * them to the registered handler.
         */
        while (EmacGetPacket(ni, &nb) == 0) {
            /* Discard short packets. */
            if (nb->nb_dl.sz < 60) {
                NutNetBufFree(nb);
            } else {
                (*ifn->if_recv) (dev, nb);
            }
        }
        macb->ier = AVR32_MACB_IER_ROVR_MASK | AVR32_MACB_IER_RXUBR_MASK | AVR32_MACB_IER_RCOMP_MASK;

        /* We got a weird chip, try to restart it. */
        while (ni->ni_insane) {
            EmacReset(dev);
            if (EmacStart(macb, ifn->if_mac) == 0) {
                ni->ni_insane = 0;
                ni->ni_tx_queued = 0;
                ni->ni_tx_quelen = 0;
                NutIrqEnable(&sig_MACB);
            } else {
                NutSleep(1000);
            }
        }
    }
}

/*!
* \brief Send Ethernet packet.
*
* \param dev Identifies the device to use.
* \param nb  Network buffer structure containing the packet to be sent.
*            The structure must have been allocated by a previous
*            call NutNetBufAlloc().
*
* \return 0 on success, -1 in case of any errors.
*/
int EmacOutput(NUTDEVICE * dev, NETBUF * nb)
{
    volatile avr32_macb_t *macb = (avr32_macb_t *) dev->dev_base;
    static uint32_t mx_wait = 5000;
    int rc = -1;
    EMACINFO *ni = (EMACINFO *) dev->dev_dcb;

    /*
     * After initialization we are waiting for a long time to give
     * the PHY a chance to establish an Ethernet link.
     */
    while (rc) {
        if (ni->ni_insane) {
            break;
        }
        if (NutEventWait(&ni->ni_mutex, mx_wait)) {
            break;
        }

        /* Check for packet queue space. */
        if ((txBufTab[txBufIdx].status & TXS_USED) == 0) {
            if (NutEventWait(&ni->ni_tx_rdy, 500) && (txBufTab[txBufIdx].status & TXS_USED) == 0) {
                /* No queue space. Release the lock and give up. */
                txBufTab[txBufIdx].status |= TXS_USED;
                txBufIdx++;
                txBufIdx &= 1;
                NutEventPost(&ni->ni_mutex);
                break;
            }
        } else {
            if (macb->tsr & AVR32_MACB_TSR_UND_MASK) {
                txBufIdx = 0;
                macb->tsr = AVR32_MACB_TSR_UND_MASK;
            }
            if (macb->tsr & AVR32_MACB_TSR_COMP_MASK) {
                macb->tsr = AVR32_MACB_TSR_COMP_MASK;
            }

            if ((rc = EmacPutPacket(txBufIdx, dev, nb)) == 0) {
                txBufIdx++;
                txBufIdx &= 1;
            }
        }
        NutEventPost(&ni->ni_mutex);
    }

    /*
     * Probably no Ethernet link. Significantly reduce the waiting
     * time, so following transmission will soon return an error.
     */
    if (rc) {
        mx_wait = 500;
    } else {
        /* Ethernet works. Set a long waiting time in case we
           temporarily lose the link next time. */
        mx_wait = 5000;
    }
    return rc;
}

/*!
* \brief Initialize Ethernet hardware.
*
* Applications should do not directly call this function. It is
* automatically executed during during device registration by
* NutRegisterDevice().
*
* \param dev Identifies the device to initialize.
*/
int EmacInit(NUTDEVICE * dev)
{
    EMACINFO *ni = (EMACINFO *) dev->dev_dcb;

    /* Reserve Pins with the GPIO Controller */
#if !defined(PHY_MODE_RMII)
    gpio_enable_module_pin(AVR32_MACB_CRS_0_PIN, AVR32_MACB_CRS_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_COL_0_PIN, AVR32_MACB_COL_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_RX_CLK_0_PIN, AVR32_MACB_RX_CLK_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_TX_ER_0_PIN, AVR32_MACB_TX_ER_0_FUNCTION);
#endif
    gpio_enable_module_pin(AVR32_MACB_MDC_0_PIN, AVR32_MACB_MDC_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_MDIO_0_PIN, AVR32_MACB_MDIO_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_RXD_0_PIN, AVR32_MACB_RXD_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_TXD_0_PIN, AVR32_MACB_TXD_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_RXD_1_PIN, AVR32_MACB_RXD_1_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_TXD_1_PIN, AVR32_MACB_TXD_1_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_TX_EN_0_PIN, AVR32_MACB_TX_EN_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_RX_ER_0_PIN, AVR32_MACB_RX_ER_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_RX_DV_0_PIN, AVR32_MACB_RX_DV_0_FUNCTION);
    gpio_enable_module_pin(AVR32_MACB_TX_CLK_0_PIN, AVR32_MACB_TX_CLK_0_FUNCTION);

    /* Reset the controller. */
    if (EmacReset(dev)) {
        return -1;
    }

    /* Clear EMACINFO structure. */
    memset(ni, 0, sizeof(EMACINFO));

    /* Register interrupt handler. */
    if (NutRegisterIrqHandler(&sig_MACB, EmacInterrupt, dev)) {
        return -1;
    }

    /* Start the receiver thread. */
    if (NutThreadCreate("emacrx", EmacRxThread, dev, NUT_THREAD_NICRXSTACK) == NULL) {
        return -1;
    }
    return 0;
}

static EMACINFO dcb_eth0;

/*!
* \brief Network interface information structure.
*
* Used to call.
*/
static IFNET ifn_eth0 = {
    IFT_ETHER,                  /*!< \brief Interface type, if_type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address, if_mac. */
    0,                          /*!< \brief IP address, if_local_ip. */
    0,                          /*!< \brief Remote IP address for point to point, if_remote_ip. */
    0,                          /*!< \brief IP network mask, if_mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit, if_mtu. */
    0,                          /*!< \brief Packet identifier, if_pkt_id. */
    0,                          /*!< \brief Linked list of arp entries, arpTable. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    EmacOutput,                 /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
};

/*!
* \brief Device information structure.
*
* A pointer to this structure must be passed to NutRegisterDevice()
* to bind this Ethernet device driver to the Nut/OS kernel.
* An application may then call NutNetIfConfig() with the name \em eth0
* of this driver to initialize the network interface.
*
*/
NUTDEVICE devAvr32macb = {
    0,                          /*!< \brief Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /*!< \brief Unique device name. */
    IFTYP_NET,                  /*!< \brief Type of device. */
    AVR32_MACB_ADDRESS,         /*!< \brief Base address. */
    0,                          /*!< \brief First interrupt number. */
    &ifn_eth0,                  /*!< \brief Interface control block. */
    &dcb_eth0,                  /*!< \brief Driver control block. */
    EmacInit,                   /*!< \brief Driver initialization routine. */
    0,                          /*!< \brief Driver specific control function. */
    0,                          /*!< \brief Read from device. */
    0,                          /*!< \brief Write to device. */
#ifdef __HARVARD_ARCH__
    0,                          /*!< \brief Write from program space data to device. */
#endif
    0,                          /*!< \brief Open a device or file. */
    0,                          /*!< \brief Close a device or file. */
    0                           /*!< \brief Request file size. */
};

/*@}*/
