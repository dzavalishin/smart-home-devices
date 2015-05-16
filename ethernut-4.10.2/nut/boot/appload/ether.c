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
 *
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include <avr/io.h>

#include "debug.h"
#include "smscregs.h"
#include "arp.h"
#include "ether.h"
#include "appload.h"

/*!
 * \brief Select specified PHY register for reading or writing.
 *
 * \param reg PHY register number.
 * \param we  Should be 1 for write access, 0 for read access.
 *
 * \return Contents of the PHY interface rgister.
 */
u_char NicPhyRegSelect(u_char reg, u_char we)
{
    u_char rs;
    u_char msk;
    u_char i;

    nic_bs(3);
    rs = (nic_inlb(NIC_MGMT) & ~(MGMT_MCLK | MGMT_MDO)) | MGMT_MDOE;

    /* Send idle pattern. */
    for (i = 0; i < 33; i++) {
        nic_outlb(NIC_MGMT, rs | MGMT_MDO);
        nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
    }

    /* Send start sequence. */
    nic_outlb(NIC_MGMT, rs);
    nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
    nic_outlb(NIC_MGMT, rs | MGMT_MDO);
    nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);

    /* Write or read mode. */
    if (we) {
        nic_outlb(NIC_MGMT, rs);
        nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
        nic_outlb(NIC_MGMT, rs | MGMT_MDO);
        nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
    } else {
        nic_outlb(NIC_MGMT, rs | MGMT_MDO);
        nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
        nic_outlb(NIC_MGMT, rs);
        nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
    }

    /* Send PHY address. Zero is used for the internal PHY. */
    for (i = 0; i < 5; i++) {
        nic_outlb(NIC_MGMT, rs);
        nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
    }

    /* Send PHY register number. */
    for (msk = 0x10; msk; msk >>= 1) {
        if (reg & msk) {
            nic_outlb(NIC_MGMT, rs | MGMT_MDO);
            nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
        } else {
            nic_outlb(NIC_MGMT, rs);
            nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
        }
    }
    nic_outlb(NIC_MGMT, rs);

    return rs;
}

/*!
 * \brief Read contents of PHY register.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
u_short NicPhyRead(u_char reg)
{
    u_short rc = 0;
    u_char rs;
    u_char i;

    /* Select register for reading. */
    rs = NicPhyRegSelect(reg, 0);

    /* Switch data direction. */
    rs &= ~MGMT_MDOE;
    nic_outlb(NIC_MGMT, rs);
    nic_outlb(NIC_MGMT, rs | MGMT_MCLK);

    /* Clock data in. */
    for (i = 0; i < 16; i++) {
        nic_outlb(NIC_MGMT, rs);
        nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
        rc <<= 1;
        rc |= (nic_inlb(NIC_MGMT) & MGMT_MDI) != 0;
    }

    /* This will set the clock line to low. */
    nic_outlb(NIC_MGMT, rs);

    return rc;
}

/*!
 * \brief Write value to PHY register.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
void NicPhyWrite(u_char reg, u_short val)
{
    u_short msk;
    u_char rs;

    /* Select register for writing. */
    rs = NicPhyRegSelect(reg, 1);

    /* Switch data direction dummy. */
    nic_outlb(NIC_MGMT, rs | MGMT_MDO);
    nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
    nic_outlb(NIC_MGMT, rs);
    nic_outlb(NIC_MGMT, rs | MGMT_MCLK);

    /* Clock data out. */
    for (msk = 0x8000; msk; msk >>= 1) {
        if (val & msk) {
            nic_outlb(NIC_MGMT, rs | MGMT_MDO);
            nic_outlb(NIC_MGMT, rs | MGMT_MDO | MGMT_MCLK);
        } else {
            nic_outlb(NIC_MGMT, rs);
            nic_outlb(NIC_MGMT, rs | MGMT_MCLK);
        }
    }

    /* Set clock line low and output line int z-state. */
    nic_outlb(NIC_MGMT, rs & ~MGMT_MDOE);
}

/*!
 * \brief Configure the internal PHY.
 *
 * Reset the PHY and initiate auto-negotiation.
 */
int NicPhyConfig(void)
{
    u_short phy_sor;
    u_short phy_sr;
    u_short phy_to;
    u_short mode;

    /* 
     * Reset the PHY and wait until this self clearing bit
     * becomes zero. We sleep 63 ms before each poll and
     * give up after 3 retries. 
     */
    NicPhyWrite(NIC_PHYCR, PHYCR_RST);
    for (phy_to = 0;; phy_to++) {
        Delay(1);
        if ((NicPhyRead(NIC_PHYCR) & PHYCR_RST) == 0)
            break;
        if (phy_to > 3) {
            return -1;
        }
    }
    Delay(1);

    /* Store PHY status output. */
    phy_sor = NicPhyRead(NIC_PHYSOR);

    /* Enable PHY interrupts. */
    NicPhyWrite(NIC_PHYMSK, PHYMSK_MLOSSSYN | PHYMSK_MCWRD | PHYMSK_MSSD |
                PHYMSK_MESD | PHYMSK_MRPOL | PHYMSK_MJAB | PHYMSK_MSPDDT | PHYMSK_MDPLDT);

    /* Set RPC register. */
    mode = RPCR_ANEG | RPCR_LEDA_PAT | RPCR_LEDB_PAT;
    nic_bs(0);
    nic_outw(NIC_RPCR, mode);

    /*
     * Advertise our capabilities, initiate auto negotiation
     * and wait until this has been completed.
     */
    NicPhyWrite(NIC_PHYANAD, PHYANAD_TX_FDX | PHYANAD_TX_HDX | PHYANAD_10FDX | PHYANAD_10_HDX | PHYANAD_CSMA);
    for (phy_to = 0, phy_sr = 0;; phy_to++) {
        /* Give up after long time wait. */
        if (phy_to >= 1024) {
            return -1;
        }
        /* Restart auto negotiation every 4 seconds or on failures. */
        if ((phy_to & 127) == 0 /* || (phy_sr & PHYSR_REM_FLT) != 0 */ ) {
            NicPhyWrite(NIC_PHYCR, PHYCR_ANEG_EN | PHYCR_ANEG_RST);
            Delay(2);
        }
        /* Check if link status detected. */
        phy_sr = NicPhyRead(NIC_PHYSR);
        if (phy_sr & PHYSR_ANEG_ACK)
            break;
        Delay(3);
    }
    return 0;
}

/*!
 * \brief Wait until MMU is ready.
 *
 * Poll the MMU command register until \ref MMUCR_BUSY
 * is cleared.
 *
 * \param tmo Timeout in milliseconds.
 *
 * \return 0 on success or -1 on timeout.
 */
static int NicMmuWait(u_short tmo)
{
    while (tmo--) {
        if ((nic_inlb(NIC_MMUCR) & MMUCR_BUSY) == 0)
            break;
        MicroDelay(1000);
    }
    return tmo ? 0 : -1;
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
int NicReset(void)
{
    /* Disable all interrupts. */
    nic_outlb(NIC_MSK, 0);

    /* MAC and PHY software reset. */
    nic_bs(0);
    nic_outw(NIC_RCR, RCR_SOFT_RST);

    /* Enable Ethernet protocol handler. */
    nic_bs(1);
    nic_outw(NIC_CR, CR_EPH_EN);

    Delay(1);

    /* Disable transmit and receive. */
    nic_bs(0);
    nic_outw(NIC_RCR, 0);
    nic_outw(NIC_TCR, 0);

    /* Enable auto release. */
    nic_bs(1);
    nic_outw(NIC_CTR, CTR_AUTO_RELEASE);

    /* Reset MMU. */
    nic_bs(2);
    nic_outlb(NIC_MMUCR, MMU_RST);
    if (NicMmuWait(1000))
        return -1;

    return 0;
}

/*!
 * \brief Initialize the NIC.
 *
 * \return 0 on success, -1 otherwise.
 */
int EtherInit(void)
{
    u_char i;

    /* NIC reset. */
    if (NicReset()) {
        return -1;
    }

    /* Enable receiver. */
    nic_bs(3);
    nic_outlb(NIC_ERCV, 7);
    nic_bs(0);
    nic_outw(NIC_RCR, RCR_RXEN);

    /* Enable transmitter and padding. */
    nic_outw(NIC_TCR, TCR_PAD_EN | TCR_TXENA);

    /* Configure the PHY. */
    if (NicPhyConfig()) {
        return -1;
    }

    /* Set MAC address. */
    nic_bs(1);
    for (i = 0; i < 6; i++) {
        nic_outlb(NIC_IAR + i, confnet.cdn_mac[i]);
    }
    return 0;
}

/*!
 * \brief Send an Ethernet frame.
 *
 * \param dmac Destination MAC address.
 * \param type Frame type.
 * \param len  Frame size.
 *
 * \return 0 on success, -1 otherwise.
 */
int EtherOutput(const u_char * dmac, u_short type, u_short len)
{
    ETHERHDR *eh;
    u_char *cp;

    /*
     * Set the Ethernet header.
     */
    if (type == ETHERTYPE_ARP) {
        cp = (u_char *) & arpframe;
    } else {
        cp = (u_char *) & sframe;
    }
    eh = (ETHERHDR *)cp;
    memcpy_(eh->ether_shost, confnet.cdn_mac, 6);
    memcpy_(eh->ether_dhost, dmac, 6);
    eh->ether_type = type;

    /*
     * The total packet length includes 
     * - status word (2 bytes)
     * - byte count (2 bytes)
     * - destination address (6 bytes)
     * - source address (6 bytes)
     * - Ethernet type (2 bytes)
     * - data bytes (variable)
     * - control word (2 bytes)
     * Thus we add 20 to the number of data bytes. We didn't
     * manage to get an odd number of bytes transmitted, so
     * add another byte.
     */
    if((len += 20) & 1) {
        len++;
    }

    DEBUG(" Tx(");
    DEBUGUSHORT(len);
    DEBUG(")");

    /* Allocate transmit packet buffer space. */
    nic_bs(2);
    nic_outlb(NIC_MMUCR, MMU_ALO);
    if (NicMmuWait(100)) {
        return -1;
    }

    /*
     * An allocation error might appear when incoming packets occupy 
     * all the buffer. Reset the MMU to release all memory. This is
     * very drastic, but OK for our sequential boot loader.
     */
    if ((nic_inlb(NIC_IST) & INT_ALLOC) == 0) {
        DEBUG("[MMURST]");
        nic_outlb(NIC_MMUCR, MMU_RST);
        NicMmuWait(1000);
        nic_outlb(NIC_MMUCR, MMU_ALO);
        if (NicMmuWait(100) || (nic_inlb(NIC_IST) & INT_ALLOC) == 0) {
            return -1;
        }
    }

    /* 
     * Read the number of the allcocated packet from the allocation 
     * result register and write it to the packet number register.
     */
    nic_outlb(NIC_PNR, nic_inhb(NIC_PNR));

    /*
     * Initially set the pointer register address to 2 and enable
     * auto increment. The first two bytes will be used by the CSMA 
     * to store the status word upon transmit completion. 
     */
    nic_outw(NIC_PTR, PTR_AUTO_INCR | 2);

    /* 
     * Transfer the byte count and the data bytes.
     */
    nic_outw(NIC_DATA, len);
    while (len--) {
        nic_outlb(NIC_DATA, *cp);
        cp++;
    }
    
    /* 
     * Transfer the control word. As stated above, we never succeeded
     * in sending an odd number of bytes.
     */
    nic_outw(NIC_DATA, 0);

    /* Enqueue packet. */
    NicMmuWait(100);
    nic_outlb(NIC_MMUCR, MMU_ENQ);

    return 0;
}

/*!
 * \brief Receive an Ethernet frame.
 *
 * \param tms  Return with timeout after the specified
 *             number of waiting loops. On a 14 Mhz ATmega
 *             this value represents approximately the number
 *             of milliseconds to wait.
 *
 * \return The number of bytes received, 0 on timeout 
 *         or -1 in case of a failure.
 */
int EtherInput(u_short type, u_short tms)
{
    register u_short fsw;
    register u_char *buf;
    u_short fbc;

    /* Check the fifo empty bit. If it is set, then there is 
       nothing in the receiver fifo. */
    nic_bs(2);
    while (tms--) {
        if ((nic_inw(NIC_FIFO) & 0x8000) == 0) {
            break;
        }
        MicroDelay(1000);
    }
    if (nic_inw(NIC_FIFO) & 0x8000) {
        return 0;
    }

    /* Inialize pointer register. */
    nic_outw(NIC_PTR, PTR_READ | PTR_RCV | PTR_AUTO_INCR);
    MicroDelay(1);

    /* Read status word and byte count. */
    fsw = nic_inw(NIC_DATA);
    fbc = nic_inw(NIC_DATA) - 3;

    /* Check for frame errors. */
    if ((fsw & 0xAC00) == 0) {
        DEBUG("\nRx(");
        DEBUGUSHORT(fbc);
        DEBUG(")");
        if(fbc > sizeof(rframe)) {
            fbc = sizeof(rframe);
        }
        /* Perform the read. */
        buf = (u_char *) & rframe;
        fsw = fbc;
        while (fsw--) {
            *buf++ = nic_inlb(NIC_DATA);
        }
    }
    else {
        fbc = -1;
    }

    /* Release the packet. */
    nic_outlb(NIC_MMUCR, MMU_TOP);

    return fbc;
}
