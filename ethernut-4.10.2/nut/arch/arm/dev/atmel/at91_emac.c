/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
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
 *
 */

/*
 * $Log$
 * Revision 1.16  2009/02/06 15:37:39  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.15  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.14  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.13  2008/08/11 06:59:04  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.12  2008/08/06 12:43:41  haraldkipp
 * First EMAC reset failed on power-up. Initialize EMAC clock and MII mode
 * earlier.
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.11  2007/09/06 20:07:24  olereinhardt
 * Changed phy detection and added support for micel phy (SAM7-EX256 Board)
 *
 * Revision 1.10  2007/08/29 07:43:52  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.9  2007/05/02 11:20:26  haraldkipp
 * Pull-up enable/disable simplified.
 * Added multicast table entry.
 *
 * Revision 1.8  2007/04/20 13:06:08  haraldkipp
 * Previous change failed on SAM7X-EK. We are now using PHY address 0 by
 * default and disable all pull-ups during PHY reset.
 *
 * Revision 1.7  2007/04/12 09:13:10  haraldkipp
 * Bugfix: PHY initialization may fail with pull-ups enabled.
 *
 * Revision 1.6  2007/02/15 16:00:45  haraldkipp
 * Configurable buffer usage and link timeout.
 *
 * Revision 1.5  2006/10/17 11:06:12  haraldkipp
 * Number of loops waiting for links increased to 10000 and NIC resets
 * reduced. This will help to link to auto MDIX via direct cable.
 *
 * Revision 1.4  2006/10/08 16:41:34  haraldkipp
 * PHY address and power down bit are now configurable.
 *
 * Revision 1.3  2006/10/05 17:10:37  haraldkipp
 * Link detection was unreliable. This also caused bug #1567785.
 * Now NutRegisterDevice will return an error, if there is no
 * link available. Applications may then call NutRegisterDevice again.
 *
 * Revision 1.2  2006/09/29 12:29:16  haraldkipp
 * Several fixes to make it running reliably on the AT91SAM9260.
 *
 * Revision 1.1  2006/08/31 18:58:47  haraldkipp
 * More general AT91 MAC driver replaces the SAM7X specific version.
 * This had been tested on the SAM9260, but loses Ethernet packets
 * for a yet unknown reason.
 *
 */

#include <cfg/os.h>
#include <cfg/dev.h>
#include <arch/arm.h>
#include <cfg/arch/gpio.h>

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
#include <dev/at91_emac.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

#ifndef NUT_THREAD_NICRXSTACK
/* arm-elf-gcc used 168 bytes with optimized, 412 bytes with debug code. */
#define NUT_THREAD_NICRXSTACK   320
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

/*@}*/


/*!
 * \brief PHY address.
 *
 * Any other than 0 seems to create problems with Atmel's evaluation kits.
 */
#ifndef NIC_PHY_ADDR
#define NIC_PHY_ADDR	        0
#endif

/*!
 * \brief PHY ID.
 *
 * If set to 0xffffffff, the PHY id will be ignored.
 */
#ifndef NIC_PHY_UID
#define NIC_PHY_UID 0xffffffff
#endif

/*!
 * \brief Check all known PHY IDs.
 *
 * If defined, perform the old PHY checks. This ensures compatibility 
 * at the cost of bloat. Should be removed later on when all boards 
 * have their PHY ids in their configuration.
 */
#define CHECK_ALL_KNOWN_PHY_IDS

#if defined (MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)

/*!
 * The AT91SAM9260-EK board is delivered with RMII by default. Thus.
 * we use the reduced MII for this CPU. However, this should be
 * handled by the Configurator.
 */
#define PHY_MODE_RMII

//#define EMAC_PIO_PER            PIOA_PER
//#define EMAC_PIO_OER            PIOA_OER
//#define EMAC_PIO_CODR           PIOA_CODR
#define EMAC_PIO_ASR            PIOA_ASR
#define EMAC_PIO_BSR            PIOA_BSR
#define EMAC_PIO_PDR            PIOA_PDR

#define PHY_TXD0_BIT            PA12_ETX0_A     /*!< \brief Transmit data bit 0 pin. */
#define PHY_TXD1_BIT            PA13_ETX1_A     /*!< \brief Transmit data bit 1 pin. */
#define PHY_RXD0_AD0_BIT        PA14_ERX0_A     /*!< \brief Receive data bit 0 pin. */
#define PHY_RXD1_AD1_BIT        PA15_ERX1_A     /*!< \brief Receive data bit 1 pin. */
#define PHY_TXEN_BIT            PA16_ETXEN_A    /*!< \brief Transmit enable pin. */
#define PHY_RXDV_TESTMODE_BIT   PA17_ERXDV_A    /*!< \brief Data valid pin. */
#define PHY_RXER_RXD4_RPTR_BIT  PA18_ERXER_A    /*!< \brief Receive error pin. */
#define PHY_TXCLK_ISOLATE_BIT   PA19_ETXCK_A    /*!< \brief Transmit clock pin. */
#define PHY_MDC_BIT             PA20_EMDC_A     /*!< \brief Management data clock pin. */
#define PHY_MDIO_BIT            PA21_EMDIO_A    /*!< \brief Management data I/O pin. */

#ifndef PHY_MODE_RMII
#define PHY_TXD2_BIT            PA10_ETX2_B     /*!< \brief Transmit data bit 2 pin. */
#define PHY_TXD3_BIT            PA11_ETX3_B     /*!< \brief Transmit data bit 3 pin. */
#define PHY_TXER_TXD4_BIT       PA22_ETXER_B    /*!< \brief Transmit error pin. */
#define PHY_RXCLK_10BTSER_BIT   PA27_ERXCK_B    /*!< \brief Receive clock pin. */
#define PHY_COL_RMII_BIT        PA29_ECOL_B     /*!< \brief Collision detect pin. */
#endif

#define PHY_RXD2_AD2_BIT        PA25_ERX2_B     /*!< \brief Receive data bit 2 pin. */
#define PHY_RXD3_AD3_BIT        PA26_ERX3_B     /*!< \brief Receive data bit 3 pin. */
#define PHY_CRS_AD4_BIT         PA28_ECRS_B     /*!< \brief Carrier sense pin. */

#define PHY_MII_PINS_A 0 \
    | _BV(PHY_TXD0_BIT) \
    | _BV(PHY_TXD1_BIT) \
    | _BV(PHY_RXD0_AD0_BIT) \
    | _BV(PHY_RXD1_AD1_BIT) \
    | _BV(PHY_TXEN_BIT) \
    | _BV(PHY_RXDV_TESTMODE_BIT) \
    | _BV(PHY_RXER_RXD4_RPTR_BIT) \
    | _BV(PHY_TXCLK_ISOLATE_BIT) \
    | _BV(PHY_MDC_BIT) \
    | _BV(PHY_MDIO_BIT)

#ifdef PHY_MODE_RMII
#define PHY_MII_PINS_B 0
#else
#define PHY_MII_PINS_B 0 \
    | _BV(PHY_TXD2_BIT) \
    | _BV(PHY_TXD3_BIT) \
    | _BV(PHY_TXER_TXD4_BIT) \
    | _BV(PHY_RXD2_AD2_BIT) \
    | _BV(PHY_RXD3_AD3_BIT) \
    | _BV(PHY_RXCLK_10BTSER_BIT) \
    | _BV(PHY_CRS_AD4_BIT) \
    | _BV(PHY_COL_RMII_BIT)
#endif

#elif defined (MCU_AT91SAM7X)

#define EMAC_PIO_PER            PIOB_PER
#define EMAC_PIO_OER            PIOB_OER
#define EMAC_PIO_CODR           PIOB_CODR
#define EMAC_PIO_SODR           PIOB_SODR
#define EMAC_PIO_PUER           PIOB_PUER
#define EMAC_PIO_PUDR           PIOB_PUDR
#define EMAC_PIO_ASR            PIOB_ASR
#define EMAC_PIO_BSR            PIOB_BSR
#define EMAC_PIO_PDR            PIOB_PDR

#define PHY_TXCLK_ISOLATE_BIT   0
#define PHY_REFCLK_XT2_BIT      0
#define PHY_TXEN_BIT            1
#define PHY_TXD0_BIT            2
#define PHY_TXD1_BIT            3
#define PHY_CRS_AD4_BIT         4
#define PHY_RXD0_AD0_BIT        5
#define PHY_RXD1_AD1_BIT        6
#define PHY_RXER_RXD4_RPTR_BIT  7
#define PHY_MDC_BIT             8
#define PHY_MDIO_BIT            9
#define PHY_TXD2_BIT            10
#define PHY_TXD3_BIT            11
#define PHY_TXER_TXD4_BIT       12
#define PHY_RXD2_AD2_BIT        13
#define PHY_RXD3_AD3_BIT        14
#define PHY_RXDV_TESTMODE_BIT   15
#define PHY_COL_RMII_BIT        16
#define PHY_RXCLK_10BTSER_BIT   17
#define PHY_MDINTR_BIT          26

#define PHY_MII_PINS_A 0 \
    | _BV(PHY_REFCLK_XT2_BIT) \
    | _BV(PHY_TXEN_BIT) \
    | _BV(PHY_TXD0_BIT) \
    | _BV(PHY_TXD1_BIT) \
    | _BV(PHY_CRS_AD4_BIT) \
    | _BV(PHY_RXD0_AD0_BIT) \
    | _BV(PHY_RXD1_AD1_BIT) \
    | _BV(PHY_RXER_RXD4_RPTR_BIT) \
    | _BV(PHY_MDC_BIT) \
    | _BV(PHY_MDIO_BIT) \
    | _BV(PHY_TXD2_BIT) \
    | _BV(PHY_TXD3_BIT) \
    | _BV(PHY_TXER_TXD4_BIT) \
    | _BV(PHY_RXD2_AD2_BIT) \
    | _BV(PHY_RXD3_AD3_BIT) \
    | _BV(PHY_RXDV_TESTMODE_BIT) \
    | _BV(PHY_COL_RMII_BIT) \
    | _BV(PHY_RXCLK_10BTSER_BIT)

#define PHY_MII_PINS_B 0

#endif

/*!
 * \brief Network interface controller information structure.
 */
struct _EMACINFO {
#ifdef NUT_PERFMON
    uint32_t ni_rx_packets;       /*!< Number of packets received. */
    uint32_t ni_tx_packets;       /*!< Number of packets sent. */
    uint32_t ni_overruns;         /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;  /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;    /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors; /*!< Number of missed packets. */
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
typedef struct _BufDescriptor {
    unsigned int addr;
    unsigned int stat;
} BufDescriptor;

static volatile BufDescriptor txBufTab[EMAC_TX_BUFFERS];
static volatile uint8_t txBuf[EMAC_TX_BUFFERS * EMAC_TX_BUFSIZ] __attribute__ ((aligned(8)));
static unsigned int txBufIdx;

static volatile BufDescriptor rxBufTab[EMAC_RX_BUFFERS];
static volatile uint8_t rxBuf[EMAC_RX_BUFFERS * EMAC_RX_BUFSIZ] __attribute__ ((aligned(8)));
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

#define MII_DM9161_ID     0x0181b8a0
#define MII_AM79C875_ID   0x00225540
#define MII_MICREL_ID     0x00221610
#define MII_LAN8700_ID    0x0007c0c0
#define MII_LAN8710_ID    0x0007C0F0


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
static uint16_t phy_inw(uint8_t reg)
{
    /* PHY read command. */
    outr(EMAC_MAN, EMAC_SOF | EMAC_RW_READ | EMAC_CODE | 
        (NIC_PHY_ADDR << EMAC_PHYA_LSB) | (reg << EMAC_REGA_LSB));

    /* Wait until PHY logic completed. */
    while ((inr(EMAC_NSR) & EMAC_IDLE) == 0);

    /* Get data from PHY maintenance register. */
    return (uint16_t) (inr(EMAC_MAN) >> EMAC_DATA_LSB);
}

#if !defined(PHY_MODE_RMII) || (NIC_PHY_UID == MII_LAN8710_ID)
/*!
 * \brief Write value to PHY register.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void phy_outw(uint8_t reg, uint16_t val)
{
    /* PHY write command. */
    outr(EMAC_MAN, EMAC_SOF | EMAC_RW_WRITE | EMAC_CODE | 
        (NIC_PHY_ADDR << EMAC_PHYA_LSB) | (reg << EMAC_REGA_LSB) | val);

    /* Wait until PHY logic completed. */
    while ((inr(EMAC_NSR) & EMAC_IDLE) == 0);
}
#endif

/*!
 * \brief Probe PHY.
 * \param timeout for link status from PHY in loops.
 * \return 0 on success, -1 otherwise.
 */
static int probePhy(uint32_t tmo)
{
    uint32_t physID;
    uint16_t phyval;

    /* Read Phy ID. Ignore revision number. */
    physID = (phy_inw(NIC_PHY_ID2) & 0xFFF0) | ((phy_inw(NIC_PHY_ID1) << 16) & 0xFFFF0000);   
#if NIC_PHY_UID != 0xffffffff
    if ( physID != (NIC_PHY_UID & 0xFFFFFFF0) ) {
        outr(EMAC_NCR, inr(EMAC_NCR) & ~EMAC_MPE);
        return -1;
    }
#elif defined(CHECK_ALL_KNOWN_PHY_IDS)
    if ( physID != MII_DM9161_ID && physID != MII_AM79C875_ID && physID != MII_MICREL_ID && physID != MII_LAN8700_ID && physID != MII_LAN8710_ID) {
        outr(EMAC_NCR, inr(EMAC_NCR) & ~EMAC_MPE);
        return -1;
    }
#endif

    /* Handle auto negotiation if configured. */
    phyval = phy_inw(NIC_PHY_BMCR);
    if (phyval & NIC_PHY_BMCR_ANEGENA) {
        /* Wait for auto negotiation completed. */
        phy_inw(NIC_PHY_BMSR);  /* Discard previously latched status. */
        while (--tmo) {
            if (phy_inw(NIC_PHY_BMSR) & NIC_PHY_BMSR_ANCOMPL) {
                break;
            }
        }
        /* Return error on link timeout. */
        if (tmo == 0) {
            outr(EMAC_NCR, inr(EMAC_NCR) & ~EMAC_MPE);
            return -1;
        }

        /*
        * Read link partner abilities and configure EMAC.
        */
        phyval = phy_inw(NIC_PHY_ANLPAR);
        if (phyval & NIC_PHY_ANEG_TX_FDX) {
            /* 100Mb full duplex. */
            outr(EMAC_NCFGR, inr(EMAC_NCFGR) | EMAC_SPD | EMAC_FD);
        }
        else if (phyval & NIC_PHY_ANEG_TX_HDX) {
            /* 100Mb half duplex. */
            outr(EMAC_NCFGR, (inr(EMAC_NCFGR) & ~EMAC_FD) | EMAC_SPD);
        }
        else if (phyval & NIC_PHY_ANEG_10_FDX) {
            /* 10Mb full duplex. */
            outr(EMAC_NCFGR, (inr(EMAC_NCFGR) & ~EMAC_SPD) | EMAC_FD);
        }
        else {
            /* 10Mb half duplex. */
            outr(EMAC_NCFGR, inr(EMAC_NCFGR) & ~(EMAC_SPD | EMAC_FD));
        }
    }

    /* Disable management port. */
    outr(EMAC_NCR, inr(EMAC_NCR) & ~EMAC_MPE);

    return 0;
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int EmacReset(uint32_t tmo)
{
    /* Enable power sources if not yet enabled */
    outr(PMC_PCER, _BV(PIOA_ID));
    outr(PMC_PCER, _BV(PIOB_ID));
    outr(PMC_PCER, _BV(EMAC_ID));

    /* Configure MII port. */
    outr(EMAC_PIO_ASR, PHY_MII_PINS_A);
    outr(EMAC_PIO_BSR, PHY_MII_PINS_B);
    outr(EMAC_PIO_PDR, PHY_MII_PINS_A | PHY_MII_PINS_B);

    /* Enable receive and transmit clocks and set MII mode. */
#ifdef PHY_MODE_RMII
    outr(EMAC_USRIO, EMAC_RMII | EMAC_CLKEN);
#else
    outr(EMAC_USRIO, EMAC_CLKEN);
#endif

    /* Enable management port. */
    outr(EMAC_NCR, inr(EMAC_NCR) | EMAC_MPE);
    outr(EMAC_NCFGR, inr(EMAC_NCFGR) | EMAC_CLK_HCLK_64);

    /* Wait for PHY ready. */
    NutDelay(255);

#if NIC_PHY_UID == MII_LAN8710_ID
    /* Set LAN8710 mode. */
    phy_outw(18, phy_inw(18) | 0x00E0);
    phy_outw(NIC_PHY_BMCR, NIC_PHY_BMCR_RESET);
    NutSleep(100);
#endif

#ifndef PHY_MODE_RMII
    /* Clear MII isolate. */
    phy_inw(NIC_PHY_BMCR);
    phy_outw(NIC_PHY_BMCR, phy_inw(NIC_PHY_BMCR) & ~NIC_PHY_BMCR_ISOLATE);
#endif

    return probePhy(tmo);
}

/*
 * NIC interrupt entry.
 */
static void EmacInterrupt(void *arg)
{
    unsigned int isr;
    EMACINFO *ni = (EMACINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    /* Read interrupt status and disable interrupts. */
    isr = inr(EMAC_ISR);

    /* Receiver interrupt. */
    //if ((isr & EMAC_RCOMP) != 0 || (isr & EMAC_ROVR) != 0 || (inr(EMAC_RSR) & EMAC_REC) != 0) {
    if ((isr & (EMAC_RCOMP | EMAC_ROVR | EMAC_RXUBR)) != 0) {
        //outr(EMAC_RSR, EMAC_REC);
        outr(EMAC_IDR, EMAC_RCOMP | EMAC_ROVR | EMAC_RXUBR);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Transmitter interrupt. */
    if ((isr & EMAC_TCOMP) != 0 || (inr(EMAC_TSR) & EMAC_COMP) != 0) {
        //outr(EMAC_TSR, EMAC_COMP);
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
    while ((rxBufTab[rxBufIdx].addr & RXBUF_OWNERSHIP) != 0 && (rxBufTab[rxBufIdx].stat & RXS_SOF) == 0) {
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
        if (i != rxBufIdx && (rxBufTab[i].stat & RXS_SOF) != 0) {
            do {
                rxBufTab[rxBufIdx].addr &= ~(RXBUF_OWNERSHIP);
                rxBufIdx++;
                if (rxBufIdx >= EMAC_RX_BUFFERS) {
                    rxBufIdx = 0;
                }
            } while ((rxBufTab[rxBufIdx].addr & RXBUF_OWNERSHIP) != 0 && (rxBufTab[rxBufIdx].stat & RXS_SOF) == 0);
            break;
        }
        if ((fbc = rxBufTab[i].stat & RXS_LENGTH_FRAME) != 0) {
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
        if (fbc > 1536) {
            ni->ni_insane = 1;
        } else {
            *nbp = NutNetBufAlloc(0, NBAF_DATALINK, (uint16_t)fbc);
            if (*nbp != NULL) {
                uint8_t *bp = (uint8_t *) (* nbp)->nb_dl.vp;
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
static int EmacPutPacket(int bufnum, EMACINFO * ni, NETBUF * nb)
{
    int rc = -1;
    unsigned int sz;
    uint8_t *buf;

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
    NutIrqDisable(&sig_EMAC);

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
        txBufTab[bufnum].stat = sz;
        outr(EMAC_NCR, inr(EMAC_NCR) | EMAC_TSTART);
        rc = 0;
#ifdef NUT_PERFMON
        ni->ni_tx_packets++;
#endif
    }

    /* Enable EMAC interrupts. */
    NutIrqEnable(&sig_EMAC);

    return rc;
}


/*!
 * \brief Fire up the network interface.
 *
 * NIC interrupts must be disabled when calling this function.
 *
 * \param mac Six byte unique MAC address.
 */
static int EmacStart(CONST uint8_t * mac)
{
    unsigned int i;

    /* Set local MAC address. */
    outr(EMAC_SA1L, (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0]);
    outr(EMAC_SA1H, (mac[5] << 8) | mac[4]);

    /* Initialize receive buffer descriptors. */
    for (i = 0; i < EMAC_RX_BUFFERS - 1; i++) {
        rxBufTab[i].addr = (unsigned int) (&rxBuf[i * EMAC_RX_BUFSIZ]) & RXBUF_ADDRMASK;
    }
    rxBufTab[i].addr = ((unsigned int) (&rxBuf[i * EMAC_RX_BUFSIZ]) & RXBUF_ADDRMASK) | RXBUF_WRAP;
    outr(EMAC_RBQP, (unsigned int) rxBufTab);

    /* Initialize transmit buffer descriptors. */
    txBufTab[0].addr = (unsigned int) (&txBuf[0]);
    txBufTab[0].stat = TXS_USED;
    txBufTab[1].addr = (unsigned int) (&txBuf[EMAC_TX_BUFSIZ]);
    txBufTab[1].stat = TXS_USED | TXS_WRAP;
    outr(EMAC_TBQP, (unsigned int) txBufTab);

    /* Clear receiver status. */
    outr(EMAC_RSR, EMAC_OVR | EMAC_REC | EMAC_BNA);

    /* Discard FCS. */
    outr(EMAC_NCFGR, inr(EMAC_NCFGR) | EMAC_DRFCS);

    /* Enable receiver, transmitter and statistics. */
    outr(EMAC_NCR, inr(EMAC_NCR) | EMAC_TE | EMAC_RE | EMAC_WESTAT);

    return 0;
}

/*! \fn EmacRxThread(void *arg)
 * \brief NIC receiver thread.
 *
 */
THREAD(EmacRxThread, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    EMACINFO *ni;
    NETBUF *nb;

    dev = arg;
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
    while (EmacStart(ifn->if_mac)) {
        EmacReset(EMAC_LINK_LOOPS);
        NutSleep(1000);
    }

    /* Initialize the access mutex. */
    NutEventPost(&ni->ni_mutex);

    /* Run at high priority. */
    NutThreadSetPriority(9);

    /* Enable receive and transmit interrupts. */
    outr(EMAC_IER, EMAC_ROVR | EMAC_TCOMP | EMAC_TUND | EMAC_RXUBR | EMAC_RCOMP);
    NutIrqEnable(&sig_EMAC);

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
        outr(EMAC_IER, EMAC_ROVR | EMAC_RXUBR | EMAC_RCOMP);

        /* We got a weird chip, try to restart it. */
        while (ni->ni_insane) {
            EmacReset(EMAC_LINK_LOOPS);
            if (EmacStart(ifn->if_mac) == 0) {
                ni->ni_insane = 0;
                ni->ni_tx_queued = 0;
                ni->ni_tx_quelen = 0;
                NutIrqEnable(&sig_EMAC);
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
        if ((txBufTab[txBufIdx].stat & TXS_USED) == 0) {
            if (NutEventWait(&ni->ni_tx_rdy, 500) && (txBufTab[txBufIdx].stat & TXS_USED) == 0) {
                /* No queue space. Release the lock and give up. */
                txBufTab[txBufIdx].stat |= TXS_USED;
                txBufIdx++;
                txBufIdx &= 1;
                NutEventPost(&ni->ni_mutex);
                break;
            }
        } else {
            if (inr(EMAC_TSR) & EMAC_UND) {
                txBufIdx = 0;
	            outr(EMAC_TSR, EMAC_UND);
	        }
            if (inr(EMAC_TSR) & EMAC_COMP) {
	            outr(EMAC_TSR, EMAC_COMP);
	        }

            if ((rc = EmacPutPacket(txBufIdx, ni, nb)) == 0) {
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

    /* Reset the controller. */
    if (EmacReset(EMAC_LINK_LOOPS)) {
        if (EmacReset(EMAC_LINK_LOOPS)) {
            return -1;
        }
    }

    /* Clear EMACINFO structure. */
    memset(ni, 0, sizeof(EMACINFO));

    /* Register interrupt handler. */
    if (NutRegisterIrqHandler(&sig_EMAC, EmacInterrupt, dev)) {
        return -1;
    }

    /* Start the receiver thread. */
    if (NutThreadCreate("emacrx", EmacRxThread, dev, 
        (NUT_THREAD_NICRXSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD) == NULL) {
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
NUTDEVICE devAt91Emac = {
    0,                          /*!< \brief Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /*!< \brief Unique device name. */
    IFTYP_NET,                  /*!< \brief Type of device. */
    0,                          /*!< \brief Base address. */
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
