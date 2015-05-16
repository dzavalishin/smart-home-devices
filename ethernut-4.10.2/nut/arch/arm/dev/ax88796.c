/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 * --
 * Initially taken from Marek Hummel's port of the Realtek driver.
 *
 * Revision 1.0  2004/07/20 17:29:08  MarekH
 */

/*
 * $Log$
 * Revision 1.9  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.8  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.7  2008/08/11 06:59:07  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.5  2006/06/28 17:10:15  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.4  2006/03/02 19:48:19  haraldkipp
 * Replaced inline assembly nops with their portable counterparts.
 *
 * Revision 1.3  2006/01/23 17:27:47  haraldkipp
 * Previous hack to fix missing network configuration routines disabled
 * non-volatile memory access for ARM.
 *
 * Revision 1.2  2005/10/22 08:55:47  haraldkipp
 * CPU specific headers moved to subdirectories for the CPU family.
 *
 * Revision 1.1  2005/07/26 18:02:26  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/04/05 17:47:48  haraldkipp
 * Initial check in. For ARM only.
 *
 */

#include <arch/arm.h>

#include <string.h>
//#include <stdio.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <dev/irqreg.h>
#include <dev/ax88796.h>
#include "reg_ax88796.h"

#define ASIX_RESET_PIN 10

static NICINFO dcb_eth0;

/*!
 * \addtogroup xgNicAsix
 */
/*@{*/


/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0 = {
    IFT_ETHER,                  /*!< \brief Interface type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address. */
    0,                          /*!< \brief IP address. */
    0,                          /*!< \brief Remote IP address for point to point. */
    0,                          /*!< \brief IP network mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit. */
    0,                          /*!< \brief Packet identifier. */
    0,                          /*!< \brief Linked list of arp entries. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    AsixOutput,                 /*!< \brief Driver output routine, if_send(). */
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
NUTDEVICE devAx88796 = {
    0,                          /* Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /* Unique device name. */
    IFTYP_NET,                  /* Type of device. */
    0,                          /* Base address. */
    0,                          /* First interrupt number. */
    &ifn_eth0,                  /* Interface control block. */
    &dcb_eth0,                  /* Driver control block. */
    AsixInit,                   /* Driver initialization routine. */
    0,                          /* Driver specific control function. */
    0,                          /* Read from device. */
    0,                          /* Write to device. */
    0,                          /* Open a device or file. */
    0,                          /* Close a device or file. */
    0                           /* Request file size. */
};

/*!
 * Asix packet header.
 */
struct nic_pkt_header {
    uint8_t ph_status;           /*!< \brief Status, contents of RSR register */
    uint8_t ph_nextpg;           /*!< \brief Page for next packet */
    uint16_t ph_size;            /*!< \brief Size of header and packet in octets */
};

/*
 * This delay has been added by Bengt Florin and is used to minimize 
 * the effect of the IORDY line during reads. Bengt contributed a
 * more versatile loop, which unfortunately wasn't portable to the
 * ImageCraft compiler.
 *
 * Both versions depend on the CPU clock and had been tested with
 * 14.7456 MHz.
 */
void Delay16Cycles(void)
{
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
}

//==============================================================================
/*!
 * \brief Read and write MII bus.
 *
 *
 */
static uint16_t MIIPutGet(uint16_t data, uint8_t bitCount)
{
    uint16_t rc = 0;
    uint16_t mask;
    uint8_t i;

    mask = 1 << (bitCount - 1);

    for (i = 0; i < bitCount; i++) {

        /* send data to MII */
        if (data & mask) {
            Asix_Write(MII_EEP, (Asix_Read(MII_EEP) | MII_EEP_MDO));
        } else {
            Asix_Write(MII_EEP, (Asix_Read(MII_EEP) & ~(MII_EEP_MDO)));
        }
        /* clock and data recieve from MII */
        Asix_Write(MII_EEP, (Asix_Read(MII_EEP) | MII_EEP_MDC));        //clock up
        Delay16Cycles();

        data <<= 1;
        rc <<= 1;
        rc |= (Asix_Read(MII_EEP) & MII_EEP_MDI) != 0;  //read MII
        Asix_Write(MII_EEP, (Asix_Read(MII_EEP) & ~(MII_EEP_MDC)));     //clock down
    }
    return rc;
}

//==============================================================================
/*!
 * \brief Read contents of internel PHY register on 0x10 adress.
 *
 *
 * \return Contents of the specified register.
 */
uint16_t NicPhyRead(uint8_t reg)
{
    uint16_t rc = 0;

    /* Select Bank 0. */
    Asix_Write(CR, (Asix_Read(CR) & ~(CR_PS0)));

    /* Preamble. Send idle pattern, 32 '1's to synchronize MII. */
    MIIPutGet(0xFFFF, 16);
    MIIPutGet(0xFFFF, 16);

    /* Start(01), Read(10), PhyAdr(10000) */
    MIIPutGet(0xD0, 9);

    /* Reg address<5:0> */
    MIIPutGet(reg, 5);

    /* TA(Z0), no support high-Z */
    MIIPutGet(0x0, 1);

    /* Read data from internel PHY */
    rc = MIIPutGet(0, 16);

    return rc;
}

//==============================================================================
/*!
 * \brief Write value to PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
void NicPhyWrite(uint8_t reg, uint16_t val)
{

    /* Select Bank 0. */
    Asix_Write(CR, (Asix_Read(CR) & ~(CR_PS0)));

    /* Preamble. Send idle pattern, 32 '1's to synchronize MII. */
    MIIPutGet(0xFFFF, 16);
    MIIPutGet(0xFFFF, 16);

    /* Start(01), Write(01), PhyAdr(10000) */
    MIIPutGet(0xB0, 9);

    /* Reg address<5:0> */
    MIIPutGet(reg, 5);

    /* TA(01) */
    MIIPutGet(0x02, 2);

    /* Write data to internel PHY */
    MIIPutGet(val, 16);
}

//==============================================================================

/*!
 * Complete remote DMA.
 */
static void NicCompleteDma(void)
{
    uint8_t i;

    /* Check that we have a DMA complete flag. */
    do {
        i = Asix_Read(PG0_ISR);
    } while ((i & ISR_RDC) == 0);

    /* Complete remote dma. */
    Asix_Write(CR, CR_START | CR_RD2);

    /* Reset remote dma complete flag. */
    Asix_Write(PG0_ISR, ISR_RDC);

    /* Wait for intterupt flag. */
    Delay16Cycles();
}

//==============================================================================
/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NicReset(void)
{
    int tmp;
    //uint16_t test;

    //printf("NicReset()\n");
    outr(PIO_PER, _BV(ASIX_RESET_PIN));  /* Set PIO Enable Register */
    outr(PIO_OER, _BV(ASIX_RESET_PIN));  /* Set PIO Status Register */
    outr(PIO_SODR, _BV(ASIX_RESET_PIN)); /* ASIX_RESET_PIN = 1 */
    NutDelay(100);
    outr(PIO_CODR, _BV(ASIX_RESET_PIN)); /* ASIX_RESET_PIN = 0 */

    /* wait for PHY to come out of reset. */
    tmp = 10;
    while (1) {
        NutDelay(255);
        if (!(Asix_Read(TR) & TR_RST_B))
            break;
        if (tmp-- == 0)
            return -1;
    }

    //test = NicPhyRead(PHY_MR2);
    //printf("PHY_MR2 = 0x%.04x\n\r", test);
    //test = NicPhyRead(PHY_MR3);
    //printf("PHY_MR3 = 0x%.04x\n\r", test);


/* Following actions will fix the problem of long auto negotiation. */
//      NicPhyWrite(0x00,0x0800);
//      NutSleep(2500);
//      NicPhyWrite(0x00,0x1200); /* set speed to auto */
    return 0;
}

//==============================================================================
/*
 * Fires up the network interface. NIC interrupts
 * should have been disabled when calling this
 * function.
 *
 * \param mac Six byte unique MAC address.
 */
static int NicStart(CONST uint8_t * mac)
{
    uint8_t i;

    //printf("NicStart()\n");
    if (NicReset())
        return -1;

    /* Stop the NIC, abort DMA, page 0. */
    Asix_Write(CR, (CR_RD2 | CR_STOP));

    /* Selects word-wide DMA transfers. */
    Asix_Write(PG0_DCR, DCR_WTS);

    /* Load data byte count for remote DMA. */
    Asix_Write(PG0_RBCR0, 0x00);
    Asix_Write(PG0_RBCR1, 0x00);

    /* Temporarily set receiver to monitor mode. */
    Asix_Write(PG0_RCR, RCR_MON);

    /* Transmitter set to internal loopback mode. */
    Asix_Write(PG0_TCR, TCR_LB0);

    /* Initialize Recieve Buffer Ring. */
    Asix_Write(PG0_BNRY, RXSTART_INIT);
    Asix_Write(PG0_PSTART, RXSTART_INIT);
    Asix_Write(PG0_PSTOP, RXSTOP_INIT);

    /* Clear interrupt status. */
    Asix_Write(PG0_ISR, 0xFF);

    /* Initialize interrupt mask. */
    Asix_Write(PG0_IMR, IMR_PRXE | IMR_PTXE | IMR_RXEE | IMR_TXEE | IMR_OVWE);

    /* Stop the NIC, abort DMA, page 1. */
    Asix_Write(CR, (CR_PS0 | CR_RD2 | CR_STOP));
    Delay16Cycles();

    /* Set Physical address - MAC. */
    for (i = 0; i < 6; i++) {
        Asix_Write(PG1_PAR0 + i, mac[i]);
    }

    /* Set Multicast address. */
    for (i = 0; i < 8; i++) {
        Asix_Write(PG1_MAR0 + i, 0x00);
    }

    /* Set Current pointer point. */
    Asix_Write(PG1_CPR, RXSTART_INIT + 1);

    /* Start the NIC, Abort DMA, page 0. */
    Asix_Write(CR, (CR_RD2 | CR_START));        // stop the NIC, abort DMA, page 0   
    Delay16Cycles();

    /* Select media interfac. */
    Asix_Write(GPOC, 0x10);

    /* Check PHY speed setting 100base = full duplex, 10base > half duplex. */
    if (Asix_Read(GPI) & 0x04) {
        //printf("Full duplex\n");
        Asix_Write(PG0_TCR, TCR_FDU);
    }
    else {
        //printf("Half duplex\n");
        Asix_Write(PG0_TCR, 0);
    }

    /* Enable receiver and set accept broadcast. */
    //Asix_Write(PG0_RCR, (RCR_INTT | RCR_AB));
    Asix_Write(PG0_RCR, RCR_AB);

    return 0;
}

//==============================================================================
/*
 * Write data block to the NIC.
 */
static void NicWrite(uint8_t * buf, uint16_t len)
{
    register uint16_t *wp = (uint16_t *) buf;

    if (len & 1)
        len++;
    len >>= 1;
    //printf("Write(%u): ", len);
    while (len--) {
        //printf("%04X ", *wp);
        Asix_WriteWord(DATAPORT, *wp);
        wp++;
    }
    //putchar('\n');
}

//==============================================================================
/*
 * Read data block from the NIC.
 */
static void NicRead(uint8_t * buf, uint16_t len)
{
    register uint16_t *wp = (uint16_t *) buf;

    if (len & 1)
        len++;
    len >>= 1;
    //printf("Read(%u): ", len);
    while (len--) {
        *wp = Asix_ReadWord(DATAPORT);
        //printf("%04X ", *wp);
        wp++;
    }
    //putchar('\n');
}

//==============================================================================
/*!
 * \brief Fetch the next packet out of the receive ring buffer.
 *
 * Nic interrupts must be disabled when calling this funtion.
 *
 * \return Pointer to an allocated ::NETBUF. If there is no
 *         no data available, then the function returns a
 *         null pointer. If the NIC's buffer seems to be
 *         corrupted, a pointer to 0xFFFF is returned.
 */
static NETBUF *NicGetPacket(void)
{
    NETBUF *nb = 0;
    struct nic_pkt_header hdr;
    uint16_t count;
    uint8_t nextpg;
    uint8_t bnry;
    uint8_t curr;
    uint8_t drop = 0;

    /* we don't want to be interrupted by NIC owerflow */
    NutEnterCritical();

    /*
     * Get the current page pointer. It points to the page where the NIC 
     * will start saving the next incoming packet.
     */
    curr = Asix_Read(PG0_CPR);
    //printf("curr=%02X\n", curr);

    /*
     * Get the pointer to the last page we read from. The following page
     * is the one where we start reading. If it's equal to the current
     * page pointer, then there's nothing to read. In this case we return
     * a null pointer.
     */
    if ((bnry = Asix_Read(PG0_BNRY) + 1) >= RXSTOP_INIT) {
        //printf("bnry=%02X?\n", bnry);
        bnry = RXSTART_INIT;
    }
    if (bnry == curr) {
        //printf("bnry=%02X\n", bnry);
        NutJumpOutCritical();
        return 0;
    }
    /*
     * Read the NIC specific packet header (4 bytes).
     */
    Asix_Write(PG0_RBCR0, sizeof(struct nic_pkt_header));
    Asix_Write(PG0_RBCR1, 0);
    Asix_Write(PG0_RSAR0, 0);
    Asix_Write(PG0_RSAR1, bnry);
    Asix_Write(CR, CR_START | CR_RD0);
    Delay16Cycles();

    NicRead((uint8_t *) & hdr, sizeof(struct nic_pkt_header));
    NicCompleteDma();
    //printf("[S=%02X N=%02X L=%u]", hdr.ph_status, hdr.ph_nextpg, hdr.ph_size);

    /*
     *  Check packet length. Silently discard packets of illegal size.
     */
    if (hdr.ph_size < 60 + sizeof(struct nic_pkt_header) || hdr.ph_size > 1514 + sizeof(struct nic_pkt_header)) {
        //printf("Drop\n");
        drop = 1;
    }

    /*
     * Calculate the page of the next packet. If it differs from the
     * pointer in the packet header, we discard the whole buffer
     * and return a null pointer.
     */


//      nextpg = hdr.ph_nextpg;
//  bnry = hdr.ph_nextpg - 1;
//      if(bnry < RXSTART_INIT) bnry = RXSTOP_INIT - 1; 

//  printf("hdr.ph_size = %02u\n\r",hdr.ph_size);
//  printf("hdr.ph_nextpg = %02x\n\r",hdr.ph_nextpg);

    nextpg = bnry + (hdr.ph_size >> 8) + ((hdr.ph_size & 0xFF) != 0);
    //printf("[nextpg = %02x ->", nextpg);

    if (nextpg >= RXSTOP_INIT) {
        nextpg -= RXSTOP_INIT;
        nextpg += RXSTART_INIT;
    }
    if (nextpg != hdr.ph_nextpg) {
        uint8_t nextpg1 = nextpg + 1;
        if (nextpg1 >= RXSTOP_INIT) {
            nextpg1 -= RXSTOP_INIT;
            nextpg1 += RXSTART_INIT;
        }
        //HK if (nextpg1 != hdr.ph_nextpg) {
        //HK    return (NETBUF *) 0xFFFF;
        //HK }
        nextpg = nextpg1;
    }
    //printf("%02x]", nextpg);


    /*
     * Check packet status. It should have set bit 0, but
     * even without this bit packets seem to be OK.
     */
    //printf("Drop=%u Status=%02X\n", drop, hdr.ph_status & 0x0E);
    if (!drop && ((hdr.ph_status & 0x0E) == 0)) {

        /* Allocate a NETBUF. */
        count = hdr.ph_size - sizeof(struct nic_pkt_header);
        //printf("Count=%u\n", count);
        if ((nb = NutNetBufAlloc(0, NBAF_DATALINK, count))) {
            /*
             * Set remote dma byte count and
             * start address. Don't read the
             * header again.
             */
            Asix_Write(PG0_RBCR0, count);
            Asix_Write(PG0_RBCR1, count >> 8);
            Asix_Write(PG0_RSAR0, sizeof(struct nic_pkt_header));
            Asix_Write(PG0_RSAR1, bnry);

            /*
             * Perform the read.
             */
            Asix_Write(CR, CR_START | CR_RD0);
            Delay16Cycles();
            NicRead(nb->nb_dl.vp, count);
            NicCompleteDma();
        }
    }

    /*
     * Set boundary register to the last page we read.
     * This also drops packets with errors
     */
    if (--nextpg < RXSTART_INIT)
        nextpg = RXSTOP_INIT - 1;
    Asix_Write(PG0_BNRY, nextpg);
    NutExitCritical();
    return nb;

}

//==============================================================================
/*
 * \brief Handle NIC overflows.
 *
 * When a receiver buffer overflow occurs, the NIC will defer any subsequent 
 * action until properly restarted.
 *
 * This routine is called within interrupt context, which introduces a big
 * problem. It waits for the last transmission to finish, which may take
 * several milliseconds. Since Nut/OS 3.5, we do not support nested interrupts
 * on AVR systems anymore. So this routine may now increase interrupt
 * latency in an unacceptable way. The solution might be to handle overflows
 * in the receiver thread.
 *
 * In any case, this routines needs a major redesign. But it has been
 * tested in its current form to gracefully withstand ping floods. Thanks
 * to Bengt Florin for contributing his code, which provides much more
 * stability than its predecessor.
 */
static uint8_t NicOverflow(volatile uint8_t * base)
{
    unsigned int cr;
    unsigned int resend = 0;
    unsigned int curr;

    /*
     * Wait for any transmission in progress. Save the command register, 
     * so we can later determine, if NIC transmitter has been interrupted. 
     * or reception in progress.
     */
    while (Asix_Read(CR) & CR_TXP);
    cr = Asix_Read(CR);

    /*
     * Get the current page pointer. It points to the page where the NIC 
     * will start saving the next incoming packet.
     */
    Asix_Write(CR, CR_STOP | CR_RD2 | CR_PS0);
    curr = Asix_Read(PG1_CPR);
    Asix_Write(CR, CR_STOP | CR_RD2);

    /* Clear remote byte count register. */
    Asix_Write(PG0_RBCR0, 0);
    Asix_Write(PG0_RBCR1, 0);

    /* Check for any incomplete transmission. */
    if ((cr & CR_TXP) && ((Asix_Read(PG0_ISR) & (ISR_PTX | ISR_TXE)) == 0)) {
        resend = 1;
    }

    /* Enter loopback mode and restart the NIC. */
    Asix_Write(PG0_TCR, TCR_LB0);
    Asix_Write(CR, CR_START | CR_RD2);

    /* 
     * Discard all packets from the receiver buffer. Set boundary 
     * register to the last page we read. 
     */
    if (--curr < TXSTART_INIT) {
        curr = RXSTOP_INIT - 1;
    }
    Asix_Write(PG0_BNRY, curr);

    /* Switch from loopback to normal mode mode. */
    Asix_Write(PG0_TCR, 0);

    /* Re-invoke any interrupted transmission. */
    if (resend) {
        Asix_Write(CR, CR_START | CR_TXP | CR_RD2);
    }

    /* Finally clear the overflow flag */
    Asix_Write(PG0_ISR, ISR_OVW);
    return resend;
}

//==============================================================================
static int NicPutPacket(NETBUF * nb)
{
    uint16_t sz;                 // packed size
    uint16_t send_sz;            // send packed size, min 60
    static uint8_t first_put = 0;
    int tmp;


    /*
     * Fix problem after power on.
     */
    if (first_put != 1) {
        Asix_Write(CR, 0x21);
        NutDelay(1);
        Asix_Write(CR, 0x22);
        first_put = 1;
    }

    /*
     * Calculate the number of bytes to be send. Do not
     * send packets larger than 1518 bytes.
     */
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (sz > 1518)
        return -1;
    //printf("Asix Send %u\n", sz);

    /*
     * Calculate the number of min bytes. Pad will be
     * added when packet lenght less than 60. Enable in TCR reg. PD = 0. 
     */
    send_sz = sz;
    if (sz <= 60)
        send_sz = 60;

    /* Disable Asix interrupts. */
    NutEnterCritical();
    /* start the NIC */
    Asix_Write(CR, (CR_RD2 | CR_START));

    /*
     * Bengt Florin introduces polling mode for the transmitter. Be
     * aware, that this may introduce other problems. If a high
     * priority thread is waiting for the transmitter, it may hold
     * the CPU for more than 1.2 milliseconds in worst cases.
     */
    tmp = 120;
    while ((Asix_Read(CR) & CR_TXP) && tmp--)
        NutDelay(1);


    /* set start address for remote DMA operation */
    Asix_Write(PG0_RSAR0, 0x00);
    Asix_Write(PG0_RSAR1, TXSTART_INIT);

    /* load data byte count for remote DMA */
    Asix_Write(PG0_RBCR0, (unsigned char) (sz));
    Asix_Write(PG0_RBCR1, (unsigned char) (sz >> 8));

    /* do remote write operation */
    Asix_Write(CR, (CR_RD1 | CR_START));

    /* Transfer the Ethernet frame. */
    NicWrite(nb->nb_dl.vp, nb->nb_dl.sz);
    NicWrite(nb->nb_nw.vp, nb->nb_nw.sz);
    NicWrite(nb->nb_tp.vp, nb->nb_tp.sz);
    NicWrite(nb->nb_ap.vp, nb->nb_ap.sz);

    /* Complete remote dma. */
    NicCompleteDma();

    /* Clear interrupt flags. */
    Asix_Write(PG0_ISR, (ISR_PTX | ISR_TXE));
    Delay16Cycles();

    /* Set size and pointer to data. */
    Asix_Write(CR, CR_START | CR_RD2);
    //printf("Asix Start Send\n");
    Asix_Write(PG0_TBCR0, (unsigned char) (send_sz));
    Asix_Write(PG0_TBCR1, (unsigned char) ((send_sz) >> 8));
    Asix_Write(PG0_TPSR, TXSTART_INIT);

    /* Start transmission. */
    Asix_Write(CR, CR_START | CR_TXP | CR_RD2);

    /* Enable Asix interrupts. */
    NutExitCritical();
    return 0;
}

//==============================================================================
/*!
 * \brief NIC receiver thread.
 *
 */
THREAD(NicRxAsix, arg)
{

    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;

    /* Hack! Unfortunately no parameter passing with ARM */
    dev = &devAx88796;

    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until a valid one
     * has been set.
     */
    while (!ETHER_IS_UNICAST(ifn->if_mac)) {
        NutSleep(10);
    }

    NutEnterCritical();
    NicStart(ifn->if_mac);
    NutExitCritical();

    /* Run at high priority. */
    //printf("Increase prio\n");
    NutThreadSetPriority(9);

    while (1) {
        //printf("-------- Wait rx\n");
        NutEventWait(&ni->ni_rx_rdy, 0);
        //printf("-------- Got rx\n");
        /*
         * Fetch all packets from the NIC's internal
         * buffer and pass them to the registered handler.
         */
        do {
            nb = NicGetPacket();
            /* The sanity check may fail because the controller is too busy.
               restart the NIC. */
            if (0) {            //HK if ((u_short)nb == 0xFFFF) {
                NicStart(ifn->if_mac);
//                ni->ni_rx_size_errors++;
            } else if (nb) {
                ni->ni_rx_packets++;
                (*ifn->if_recv) (dev, nb);
            }
        } while (nb);
    }
}

//==============================================================================
/*
 * NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    uint8_t isr;
    volatile uint8_t *base = (uint8_t *) (((NUTDEVICE *) arg)->dev_base);
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    ni->ni_interrupts++;

    isr = Asix_Read(PG0_ISR);
    Delay16Cycles();
    Asix_Write(PG0_ISR, isr);

    //if(isr)
    //    printf("%02X\n", isr);
    //else
    //    return;

    /*
     * Recover from receive buffer overflow. This may take some
     * time, so we enable global interrupts but keep NIC
     * interrupts disabled.
     */
    if (isr & ISR_OVW) {
        ni->ni_rx_pending++;
        if (NicOverflow(base))
            ni->ni_tx_bsy++;
        else {
            NutEventPostAsync(&ni->ni_tx_rdy);
        }
        ni->ni_overruns++;
    } else {

        /*
         * If this is a transmit interrupt, then a packet has been sent. 
         * So we can clear the transmitter busy flag and wake up the 
         * transmitter thread.
         */
        if (isr & (ISR_PTX | ISR_TXE)) {
            ni->ni_tx_bsy = 0;
        }

        /*
         * If this is a receive interrupt, then wake up the receiver 
         * thread.
         */
        if (isr & ISR_PRX) {
            ni->ni_rx_pending++;
            //printf("Post %lX\n", (uint32_t) ni->ni_rx_rdy);
            NutEventPostFromIrq(&ni->ni_rx_rdy);
        }

        /* Rx error. */
        if (isr & ISR_RXE) {
            if (Asix_Read(PG0_RSR) & RSR_FAE)
                ni->ni_rx_frame_errors++;
            if (Asix_Read(PG0_RSR) & RSR_CR)
                ni->ni_rx_crc_errors++;
            if (Asix_Read(PG0_RSR) & RSR_MPA)
                ni->ni_rx_missed_errors++;
        }
    }
}

void NicInterruptEntry(void) __attribute__ ((naked));
void NicInterruptEntry(void)
{
    IRQ_ENTRY();
    //putchar('i');
    NicInterrupt(&devAx88796);
    IRQ_EXIT();
}

//==============================================================================
/*!
 * \brief Send Ethernet packet.
 *
 * \param dev   Identifies the device to use.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
int AsixOutput(NUTDEVICE * dev, NETBUF * nb)
{
    int rc = -1;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

    if (NicPutPacket(nb) == 0) {
        ni->ni_tx_packets++;
        rc = 0;
    }
    return rc;
}

//==============================================================================
/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets the Asix Asix_L Ethernet controller, initializes all required
 * hardware registers and starts a background thread for incoming 
 * Ethernet traffic.
 *
 * Applications should do not directly call this function. It is 
 * automatically executed during during device registration by 
 * NutRegisterDevice().
 *
 * If the network configuration hasn't been set by the application
 * before registering the specified device, this function will
 * call NutNetLoadConfig() to get the MAC address.
 *
 * \param dev Identifies the device to initialize.
 */
int AsixInit(NUTDEVICE * dev)
{
    //printf("Set confnet\n");
    confnet.cd_size = sizeof(CONFNET);
    strcpy(confnet.cd_name, "eth0");
    memset(confnet.cdn_mac, 0xFF, 6);

    /* Disable NIC interrupt and clear NICINFO structure. */
    //printf("Clear nicinfo\n");
    memset(dev->dev_dcb, 0, sizeof(NICINFO));

    /*
     * Start the receiver thread.
     */
    //printf("Create rxi5(%lX)\n", (u_long) dev);
    NutThreadCreate("rxi5", NicRxAsix, dev, 1024);

    outr(PIO_PDR, _BV(9));
    /* Register interrupt handler and enable interrupts. */
    /* Disable timer 0 interrupts. */
    outr(AIC_IDCR, _BV(IRQ0_ID));
    /* Set the TC0 IRQ handler address */
    outr(AIC_SVR(IRQ0_ID), (unsigned int) NicInterruptEntry);
    /* Set the trigg and priority for timer 0 interrupt */
    /* Level 7 is highest, level 0 lowest. */
    outr(AIC_SMR(IRQ0_ID), (AIC_SRCTYPE_EXT_NEGATIVE_EDGE | 5));
    /* Clear timer 0 interrupt */
    outr(AIC_ICCR, _BV(IRQ0_ID));
    /* Enable timer 0 interrupts */
    outr(AIC_IECR, _BV(IRQ0_ID));

    //printf("======== AsixcInit done ========\n");
    return 0;
}

/*@}*/
