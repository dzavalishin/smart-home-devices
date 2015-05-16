/*
 * Copyright (C) 2001-2007 by egnite Software GmbH
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

/*
 * $Id: ether.c 3422 2011-05-18 09:46:57Z haraldkipp $
 */

#include "dialog.h"
#include "arp.h"
#include "ether.h"
#include "bootmon.h"

#define PIO_BASE    0xFFFF0000  /*!< \brief PIO base address. */
#define PIO_OER     (PIO_BASE + 0x10)   /*!< \brief Output enable register. */
#define PIO_ODR     (PIO_BASE + 0x14)   /*!< \brief Output disable register. */
#define PIO_SODR    (PIO_BASE + 0x30)   /*!< \brief Set output data register. */
#define PIO_CODR    (PIO_BASE + 0x34)   /*!< \brief Clear output data register. */

#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x20000000
#endif

#ifndef NIC_DATA_ADDR
#define NIC_DATA_ADDR   (NIC_BASE_ADDR + 4)
#endif

#define NIC_NCR     0x00        /* Network control register (0x00). */
#define NIC_NCR_LBM     0x06    /* Loopback mode. */
#define NIC_NCR_LBNORM  0x00    /* Normal mode. */
#define NIC_NCR_LBMAC   0x02    /* MAC loopback. */
#define NIC_NCR_LBPHY   0x04    /* PHY loopback. */
#define NIC_NCR_RST     0x01    /* Software reset, auto clear. */

#define NIC_NSR     0x01        /* Network status register (0x00). */
#define NIC_NSR_SPEED   0x80
#define NIC_NSR_LINKST  0x40
#define NIC_NSR_WAKEST  0x20
#define NIC_NSR_TX2END  0x08
#define NIC_NSR_TX1END  0x04
#define NIC_NSR_RXOV    0x02

#define NIC_TCR     0x02        /* TX control register (0x00). */
#define NIC_TCR_TXREQ    0x01   /* TX request */

#define NIC_TSR1    0x03        /* TX status register I (0x00). */

#define NIC_TSR2    0x04        /* TX status register II (0x00). */

#define NIC_RCR     0x05        /* RX control register (0x00). */
#define NIC_RCR_DIS_LONG 0x20   /* Discard long packets. */
#define NIC_RCR_DIS_CRC 0x10    /* Discard CRC error packets. */
#define NIC_RCR_PRMSC   0x02    /* Enable promiscuous mode. */
#define NIC_RCR_RXEN    0x01    /* Enable receiver. */

#define NIC_RSR     0x06        /* RX status register (0x00). */
#define NIC_RSR_ERRORS  0xBF    /* Error bit mask. */
#define NIC_RSR_RF      0x80    /* Runt frame. */
#define NIC_RSR_MF      0x40    /* Multicast frame. */
#define NIC_RSR_LCS     0x20    /* Late collision. */
#define NIC_RSR_RWTO    0x10    /* Receiver watchdog time out. */
#define NIC_RSR_PLE     0x08    /* Physical layer error. */
#define NIC_RSR_AE      0x04    /* Alignment error. */
#define NIC_RSR_CE      0x02    /* CRC error. */
#define NIC_RSR_FOE     0x01    /* FIFO overflow error. */

#define NIC_ROCR    0x07        /* Receive overflow counter register (0x00). */

#define NIC_BPTR    0x08        /* Back pressure threshold register (0x37). */

#define NIC_FCTR    0x09        /* Flow control threshold register (0x38). */

#define NIC_FCR     0x0A        /* RX flow control register (0x00). */

#define NIC_EPCR    0x0B        /* EEPROM and PHY control register. */

#define NIC_EPAR    0x0C        /* EEPROM and PHY address register. */

#define NIC_EPDRL   0x0D        /* EEPROM and PHY low byte data register. */

#define NIC_EPDRH   0x0E        /* EEPROM and PHY high byte data register. */

#define NIC_WCR     0x0F        /* Wake up control register (0x00). */

#define NIC_PAR     0x10        /* 6 byte physical address register. */

#define NIC_MAR     0x16        /* 8 byte multicast address register. */

#define NIC_GPCR    0x1E        /* General purpose control register (?). */

#define NIC_GPR     0x1F        /* General purpose register (?). */

#define NIC_TRPA    0x22        /* 2 byte TX SRAM read pointer address, low/high (0x0000). */

#define NIC_RWPA    0x24        /* 2 byte RX SRAM write pointer address, low/high (0x0000). */

#define NIC_VID     0x28        /* 2 byte vendor ID (0x0A46). */

#define NIC_PID     0x2A        /* 2 byte product ID (0x0900). */

#define NIC_CHIPR   0x2C        /* Chip revision (0x00). */

#define NIC_SMCR    0x2F        /* Special mode register (0x00). */

#define NIC_MRCMDX  0xF0        /* Memory data read command w/o increment (?). */

#define NIC_MRCMD   0xF2        /* Memory data read command with increment (?). */

#define NIC_MRR     0xF4        /* 2 byte memory data read register, low/high (?). */

#define NIC_MWCMDX  0xF6        /* Memory data write command register w/o increment (?). */

#define NIC_MWCMD   0xF8        /* Memory data write command register with increment (?). */

#define NIC_MWR     0xFA        /* Memory data write command register with increment (?). */

#define NIC_TXPL    0xFC        /* 2 byte TX packet length register. (?). */

#define NIC_ISR     0xFE        /* Interrupt status register (0x00). */
#define NIC_ISR_IOM     0xC0    /* I/O mode mask */
#define NIC_ISR_M16     0x00    /* 16-bit I/O mode */
#define NIC_ISR_M32     0x40    /* 32-bit I/O mode */
#define NIC_ISR_M8      0x80    /* 8-bit I/O mode */
#define NIC_ISR_ROOS    0x08    /* Receiver overflow counter interrupt. */
#define NIC_ISR_ROS     0x04    /* Receiver overflow interrupt. */
#define NIC_ISR_PTS     0x02    /* Transmitter interrupt. */
#define NIC_ISR_PRS     0x01    /* Receiver interrupt. */

#define NIC_IMR     0xFF        /* Interrupt mask register (0x00). */
#define NIC_IMR_PAR     0x80    /* Enable read/write pointer wrap around. */
#define NIC_IMR_ROOM    0x08    /* Enable receiver overflow counter interrupts. */
#define NIC_IMR_ROM     0x04    /* Enable receiver overflow interrupts. */
#define NIC_IMR_PTM     0x02    /* Enable transmitter interrupts. */
#define NIC_IMR_PRM     0x01    /* Enable receiver interrupts. */

#define NIC_PHY_BMCR    0x00    /* Basic mode control register. */

#define NIC_PHY_BMSR    0x01    /* Basic mode status register. */
#define NIC_PHY_BMSR_ANCOMPL    0x0020  /* Auto negotiation complete. */
#define NIC_PHY_BMSR_LINKSTAT   0x0004  /* Link status. */

#define NIC_PHY_ID1     0x02    /* PHY identifier register 1. */

#define NIC_PHY_ID2     0x03    /* PHY identifier register 2. */

#define NIC_PHY_ANAR    0x04    /* Auto negotiation advertisement register. */

#define NIC_PHY_ANLPAR  0x05    /* Auto negotiation link partner availability register. */

#define NIC_PHY_ANER    0x06    /* Auto negotiation expansion register. */

#define NIC_PHY_DSCR    0x10    /* Davicom specified configuration register. */

#define NIC_PHY_DSCSR   0x11    /* Davicom specified configuration and status register. */

#define NIC_PHY_10BTCSR 0x12    /* 10BASE-T configuration and status register. */

#define outb(_reg, _val)  (*((volatile unsigned char *)(_reg)) = (_val))
#define outw(_reg, _val)  (*((volatile unsigned short *)(_reg)) = (_val))
#define outr(_reg, _val)  (*((volatile unsigned int *)(_reg)) = (_val))

#define inb(_reg)   (*((volatile unsigned char *)(_reg)))
#define inw(_reg)   (*((volatile unsigned short *)(_reg)))
#define inr(_reg)   (*((volatile unsigned int *)(_reg)))

#define _BV(bit)    (1 << bit)

static inline void nic_outb(unsigned char reg, unsigned char val)
{
    outb(NIC_BASE_ADDR, reg);
    outb(NIC_DATA_ADDR, val);
}

static inline void nic_outw(unsigned char reg, unsigned short val)
{
    outb(NIC_BASE_ADDR, reg);
    outb(NIC_DATA_ADDR, (unsigned char)val);
    outb(NIC_BASE_ADDR, reg + 1);
    outb(NIC_DATA_ADDR, (unsigned char)(val >> 8));
}

static inline unsigned char nic_inb(unsigned short reg)
{
    outb(NIC_BASE_ADDR, reg);
    return inb(NIC_DATA_ADDR);
}

static inline unsigned short nic_inw(unsigned short reg)
{
    unsigned short val;

    outb(NIC_BASE_ADDR, reg);
    val = inb(NIC_DATA_ADDR);
    outb(NIC_BASE_ADDR, reg + 1);
    val |= inb(NIC_DATA_ADDR) << 8;

    return val;
}

/*!
 * \brief Read contents of PHY register.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
static unsigned short phy_inw(unsigned char reg)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* PHY read command. */
    nic_outb(NIC_EPCR, 0x0C);
    MicroDelay(100);
    nic_outb(NIC_EPCR, 0x00);

    /* Get data from PHY data register. */
    return nic_inw(NIC_EPDRL);
}

/*!
 * \brief Write value to PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void phy_outw(unsigned char reg, unsigned short val)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* Store value in PHY data register. */
    nic_outw(NIC_EPDRL, val);

    /* PHY write command. */
    nic_outb(NIC_EPCR, 0x0A);
    MicroDelay(100);
    nic_outb(NIC_EPCR, 0x00);
}

static int NicPhyInit(void)
{
    /* Restart auto negotiation. */
    phy_outw(NIC_PHY_ANAR, 0x01E1);
    phy_outw(NIC_PHY_BMCR, 0x1200);

    nic_outb(NIC_GPCR, 1);
    nic_outb(NIC_GPR, 0);

    return 0;
}

/*!
 * \brief Write data block to the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicWrite16(unsigned char *buf, unsigned int len)
{
    unsigned short val;

    /* Make sure that the total size is an even value. */
    len = (len + 1) / 2;
    while (len--) {
        val = *buf++;
        val |= *buf++ << 8;
        outw(NIC_DATA_ADDR, val);
    }
}

/*!
 * \brief Read data block from the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicRead16(unsigned char *buf, unsigned short len)
{
    unsigned short val;

    /* Make sure that the total size is an even value. */
    len = (len + 1) / 2;
    while (len--) {
        val = inw(NIC_DATA_ADDR);
        *buf++ = (unsigned char) val;
        *buf++ = (unsigned char) (val >> 8);
    }
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NicReset(void)
{
    /* Software reset. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    MicroDelay(10000);

    return NicPhyInit();
}

/*!
 * \brief Initialize the NIC.
 *
 * \return 0 on success, -1 otherwise.
 */
int EtherInit(void)
{
    unsigned long id;
    int link_wait;
    int i;
    unsigned char val;

    /* Software reset. */
    nic_outb(NIC_NCR, NIC_NCR_RST);
    MicroDelay(10000);

    /* Probe chip by verifying the identifier registers. */
    id = (unsigned long) nic_inw(NIC_VID);
    id |= ((unsigned long) nic_inw(NIC_PID)) << 16;

    DEBUG("[ID=");
    DEBUGULONG(id);
    DEBUG("]");
    if (id != 0x90000A46) {
        return -1;
    }

    /* NIC reset. */
    if (NicReset()) {
        DEBUG("[RESET-ERR]");
        return -1;
    }

    /* Determine bus mode. We do not support 32 bit access. */
    val = nic_inb(NIC_ISR);
    if ((val & NIC_ISR_IOM) != NIC_ISR_M16) {
        DEBUG("[BUS-ERR ISR=0x");
        DEBUGUCHAR(val);
        DEBUG("]");
        return -1;
    }

    /* Power up the PHY. */
    nic_outb(NIC_GPR, 0);
    MicroDelay(5000);

    /* Software reset with MAC loopback. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    MicroDelay(5000);
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    MicroDelay(5000);

    /* 
     * PHY power down followed by PHY power up. This should activate 
     * the auto sense link.
     */
    nic_outb(NIC_GPR, 1);
    nic_outb(NIC_GPR, 0);

    /* Set MAC address. */
    DEBUG("[MAC=");
    for (i = 0; i < 6; i++) {
        DEBUGUCHAR(confnet.cdn_mac[i]);
        nic_outb(NIC_PAR + i, confnet.cdn_mac[i]);
    }
    DEBUG("]");

    /* Enable broadcast receive. */
    for (i = 0; i < 7; i++) {
        nic_outb(NIC_MAR + i, 0);
    }
    nic_outb(NIC_MAR + 7, 0x80);

    /* Clear interrupts. */
    nic_outb(NIC_ISR, NIC_ISR_ROOS | NIC_ISR_ROS | NIC_ISR_PTS | NIC_ISR_PRS);

    /* Enable late collision retries on the DM9000A. */
    if (nic_inb(NIC_CHIPR) == 0x19) {
        nic_outb(0x2D, 0x40);
    }

    /* Enable receiver. */
    nic_outb(NIC_RCR, NIC_RCR_DIS_LONG | NIC_RCR_DIS_CRC | NIC_RCR_RXEN);

    /* Wait for link. */
    for (link_wait = 50;; link_wait--) {
        unsigned short bmsr;

        bmsr = phy_inw(NIC_PHY_BMSR);
        DEBUG("[BMSR=");
        DEBUGUSHORT(bmsr);
        DEBUG("]");
        if (bmsr & NIC_PHY_BMSR_ANCOMPL) {
            break;
        }
        if (link_wait == 0) {
            DEBUG("[LINK-ERR]");
            return -1;
        }
        MicroDelay(100000);
    }

    /* Enable interrupts. */
    nic_outb(NIC_IMR, NIC_IMR_PAR | NIC_IMR_PTM | NIC_IMR_PRM);

    DEBUG("[OK]");
    return 0;
}

/*!
 * \brief Send an Ethernet frame.
 *
 * \param dmac Destination MAC address, NULL for broadcast.
 * \param type Frame type.
 * \param len  Frame size.
 *
 * \return 0 on success, -1 otherwise.
 */
int EtherOutput(const unsigned char *dmac, unsigned int type, unsigned int len)
{
    ETHERHDR *eh;
    unsigned char *ed;

    /*
     * Set the Ethernet header.
     */
    if (type == ETHERTYPE_ARP) {
        eh = &arpheader;
        ed = (unsigned char *) &arpframe;
    } else {
        eh = &sheader;
        ed = (unsigned char *) &sframe;
    }
    memcpy_(eh->ether_shost, confnet.cdn_mac, 6);
    if (dmac) {
        memcpy_(eh->ether_dhost, dmac, 6);
    } else {
        memset_(eh->ether_dhost, 0xFF, 6);
    }
    eh->ether_type = (unsigned short)type;

    DEBUG("\nTx(");
    DEBUGUSHORT(len);
    DEBUG(")");

    /* Enable data write. */
    outb(NIC_BASE_ADDR, NIC_MWCMD);

    /* Transfer the Ethernet frame. */
    NicWrite16((unsigned char *)eh, sizeof(ETHERHDR));
    NicWrite16(ed, len);

    /* Start the transmission. */
    nic_outw(NIC_TXPL, len + sizeof(ETHERHDR));
    nic_outb(NIC_TCR, NIC_TCR_TXREQ);

    return 0;
}

/*!
 * \brief Receive an Ethernet frame.
 *
 * \param type Unused?
 * \param tms  Return with timeout after the specified number of polling loops.
 *
 * \return The number of bytes received, 0 on timeout or -1 in case of 
 *         a failure.
 */
int EtherInput(unsigned short type, unsigned int tms)
{
    unsigned short fsw;
    unsigned short fbc;
    unsigned char isr;

    for (;;) {
        /* Read the status word w/o auto increment. */
        nic_inb(NIC_MRCMDX);
        fsw = inb(NIC_DATA_ADDR);

        if (fsw != 1) {
            DEBUG("[ETH-POLL]");
            DEBUGUSHORT(tms);
            while (tms) {
                tms--;
                random_id++;
                DEBUG(".");
                isr = nic_inb(NIC_ISR);
                nic_outb(NIC_ISR, isr);

                /* Receiver interrupt. */
                if (isr & NIC_ISR_PRS) {
                    nic_inb(NIC_MRCMDX);
                    fsw = inb(NIC_DATA_ADDR);
                    if (fsw == 1) {
                        break;
                    }
                }
                MicroDelay(10000);
            }
        }

        if (fsw != 1) {
            return 0;
        }

        /* Now read status word and byte count with auto increment. */
        outb(NIC_BASE_ADDR, NIC_MRCMD);
        fsw = inw(NIC_DATA_ADDR);
        fbc = inw(NIC_DATA_ADDR);

        /*
        * Receiving long packets is unexpected, because we disabled 
        * this during initialization. Let's declare the chip insane.
        * Short packets will be handled by the caller.
        */
        if (fbc > 1536) {
            //ni->ni_insane = 1;
            DEBUG("[FBC ");
            DEBUGUSHORT(fbc);
            DEBUG("]");
            return -1;
        }

        /*
        * The high byte of the status word contains a copy of the 
        * receiver status register.
        */
        fsw >>= 8;
        fsw &= NIC_RSR_ERRORS;

        /*  Discard error packets. */
        if (fsw) {
            DEBUG(" RxError(");
            DEBUGUSHORT(fbc);
            DEBUG(")");
            fbc = (fbc + 1) / 2;
            while (fbc--) {
                fsw = inw(NIC_DATA_ADDR);
            }
            return -1;
        }

        DEBUG(" Rx(");
        DEBUGUSHORT(fbc);
        DEBUG(")");

        /* Read packet data from 16 bit bus. */
        fbc -= 4;
        NicRead16((unsigned char *) &rheader, sizeof(ETHERHDR));
        NicRead16((unsigned char *) &rframe, fbc - sizeof(ETHERHDR));
        /* Read packet CRC. */
        fsw = inw(NIC_DATA_ADDR);
        fsw = inw(NIC_DATA_ADDR);

        if (rheader.ether_type == type) {
            break;
        }
        /*
         * Handle incoming ARP requests.
         */
        if (rheader.ether_type == ETHERTYPE_ARP)
            ArpRespond();
    }
    return fbc;
}
