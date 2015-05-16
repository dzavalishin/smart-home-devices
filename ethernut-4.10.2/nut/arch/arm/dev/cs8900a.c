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
 */

/*
 * $Log$
 * Revision 1.5  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.4  2008/08/11 06:59:07  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.2  2006/05/25 09:09:57  haraldkipp
 * API documentation updated and corrected.
 *
 * Revision 1.1  2006/02/23 15:33:59  haraldkipp
 * Support for Philips LPC2xxx Family and LPC-E2294 Board from Olimex added.
 * Many thanks to Michael Fischer for this port.
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.13  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.12  2005/02/02 19:55:34  haraldkipp
 * If no Ethernet link was available on the LAN91C111, each outgoing packet
 * took 15 seconds and, even worse, the ouput routine doesn't return an error.
 * Now the first attempt to send a packet without Ethernet link will wait for
 * 5 seconds and subsequent attempts take 0.5 seconds only, always returning
 * an error.
 *
 * Revision 1.11  2005/01/24 21:11:49  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.10  2005/01/22 19:24:11  haraldkipp
 * Changed AVR port configuration names from PORTx to AVRPORTx.
 *
 * Revision 1.9  2005/01/21 16:49:45  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.8  2004/09/22 08:14:48  haraldkipp
 * Made configurable
 *
 * Revision 1.7  2004/03/08 11:14:17  haraldkipp
 * Added quick hack for fixed mode.
 *
 * Revision 1.6  2004/02/25 16:22:33  haraldkipp
 * Do not initialize MAC with all zeros
 *
 * Revision 1.5  2004/01/14 19:31:43  drsung
 * Speed improvement to NicWrite applied. Thanks to Kolja Waschk
 *
 * Revision 1.4  2003/11/06 09:26:50  haraldkipp
 * Removed silly line with hardcoded MAC, left over from testing
 *
 * Revision 1.3  2003/11/04 17:54:47  haraldkipp
 * PHY configuration timing changed again for reliable linking
 *
 * Revision 1.2  2003/11/03 17:12:53  haraldkipp
 * Allow linking with RTL8019 driver.
 * Links more reliable to 10 MBit networks now.
 * Reset MMU on allocation failures.
 * Some optimizations.
 *
 * Revision 1.1  2003/10/13 10:13:49  haraldkipp
 * First release
 *
 */

#include <cfg/os.h>
#include <cfg/arch/avr.h>

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
#include <dev/cs8900a.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

/*
 * This file is for the Olimex LPC-E2294 board @@MF
 */
#include <arch/arm/lpc2xxx.h>
#include <__armlib.h>
#define cli()   __ARMLIB_disableIRQ()
#define sei()   __ARMLIB_enableIRQ()   

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/

/* 
 * Cirrus Logic CS8900a I/O Registers
 */
#define	CS_DATA_P0    (cs_base + 0x0000UL)
#define	CS_DATA_P1		(cs_base + 0x0002UL)
#define	CS_TX_CMD_I		(cs_base + 0x0004UL)
#define	CS_TX_LEN_I		(cs_base + 0x0006UL)
#define	CS_INT_STAT   (cs_base + 0x0008UL)
#define	CS_PP_PTR		  (cs_base + 0x000AUL)
#define	CS_PP_DATA0		(cs_base + 0x000CUL)
#define	CS_PP_DATA1		(cs_base + 0x000EUL)


// Cirrus Logic CS8900a Packet Page registers
#define	CS_PROD_ID		0x0000
#define	CS_IO_BASE		0x0020
#define	CS_INT_NUM		0x0022
#define	CS_DMA_CHAN		0x0024
#define	CS_DMA_SOF		0x0026
#define	CS_DMA_FCNT		0x0028
#define	CS_DMA_RXCNT	0x002A
#define	CS_MEM_BASE		0x002C
#define	CS_BOOT_BASE	0x0030
#define	CS_BOOT_MASK	0x0034
#define	CS_EE_CMD		  0x0040
#define	CS_EE_DATA		0x0042
#define	CS_RX_FRM_CNT	0x0050

#define	CS_ISQ			  0x0120
#define	CS_RX_CFG		  0x0102
#define	CS_RX_EVENT		0x0124
#define	CS_RX_CTL		  0x0104
#define	CS_TX_CFG		  0x0106
#define	CS_TX_EVENT		0x0128
#define	CS_TX_CMD_P		0x0108
#define	CS_BUF_CFG		0x010A
#define	CS_BUF_EVENT	0x012C
#define	CS_RX_MISS		0x0130
#define	CS_TX_COLL		0x0132
#define	CS_LINE_CTRL	0x0112
#define	CS_LINE_STAT	0x0134
#define	CS_SELF_CTRL	0x0114
#define	CS_SELF_STAT	0x0136
#define	CS_BUS_CTRL		0x0116
#define	CS_BUS_STAT		0x0138
#define	CS_TEST_CTRL	0x0118
#define	CS_AUI_TDR		0x013C

#define	CS_PP_TX_CMD	0x0144
#define	CS_PP_TX_LEN	0x0146

#define	CS_IEEE_ADDR	0x0158

/*!
 * \addtogroup xgNicCs8900
 */
/*@{*/

/*!
 * \struct _NICINFO
 * \brief Network interface controller information structure.
 */
struct _NICINFO {
    uint32_t ni_rx_packets;       /*!< Number of packets received. */
    uint32_t ni_tx_packets;       /*!< Number of packets sent. */
};

/*!
 * \brief Network interface controller information type.
 */
typedef struct _NICINFO NICINFO;

/*=========================================================================*/
/*  DEFINE: Definition of all local Data                                   */
/*=========================================================================*/
// Ethernet flags byte
// Bit 0 = transmit byte flag
static   uint8_t cs_flags = 0;
volatile uint32_t cs_base  = 0x82000000UL;


/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/
void CSWrite16(uint32_t addr, uint16_t data)
{
    uint8_t *p;

    p = (uint8_t *) addr;
    cli();
    *p = data;
    p++;
    *p = data >> 8;    
    sei();
}

void CSWritePP16(uint16_t addr, uint16_t data)
{
    uint8_t *p;

    cli();
    p = (uint8_t *) CS_PP_PTR;
    *p = addr;
    p++;
    *p = addr >> 8;

    CSWrite16(CS_PP_DATA0, data);

    return;
}

uint16_t CSRead16(uint32_t addr)
{
    uint8_t *p;
    uint16_t d;

    cli();
    p  = (uint8_t *) addr;
    d  = *p;
    p++;
    d |= (*p << 8); 
    sei();

    return d;
}

uint16_t CSReadPP16(uint16_t addr)
{
    uint8_t *p;

    cli();
    p = (uint8_t *) CS_PP_PTR;
    *p = addr;
    p++;
    *p = addr >> 8;

    return CSRead16(CS_PP_DATA0);
}

void CSBeginFrame(void)
{
    cs_flags &= ~1;
}

void CSEndFrame(void)
{
    uint8_t *p;

    cli();
    p = (uint8_t *) CS_DATA_P0 + 1;
    sei();

    // If odd number of bytes in packet pad it out
    if (cs_flags & 1)
        p = 0;
}

void CSWriteFrameByte(uint8_t data)
{
    uint8_t *p;

    if (cs_flags & 1)
        p = (uint8_t *) CS_DATA_P0 + 1;
    else
        p = (uint8_t *) CS_DATA_P0;

    *p = data;
    cs_flags ^= 1;
}

static int CSEthPutPacket(NUTDEVICE * dev, NETBUF * nb)
{
    uint16_t i;
    uint16_t sz;
    uint8_t *p;
    NICINFO *ni;

    ni = (NICINFO *) dev->dev_dcb;

    //
    // Calculate the number of bytes to be send. Do not
    // send packets larger than 1536 bytes.
    //
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (sz >= 0x600) {
        NutNetBufFree(nb);
        return -1;
    }
#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("[ETHTX-%u]\r\n"), sz);
        NutPrintFlush(dev_debug);
    }
#endif

    // Start transmission after entire frame is loaded into CS8900
    CSWrite16(CS_TX_CMD_I, 0xC0);
    // Set frame size
    CSWrite16(CS_TX_LEN_I, sz);

    // Wait for buffer space, but only for a while (200ms)
    // If the cable is disconnected this will never become true
    // If we don't get the go ahead within 200ms return 0 (Sucess)
    // And let the upper layers deal with re-transmission 
    // If we return failure TCP sockets will close straight away which probably
    // isn't the correct behaviour
    i = 0;
    while ((CSReadPP16(CS_BUS_STAT) & 0x0100) == 0) {
        i++;
        if (i > 20)
            return 0;
        NutSleep(10);
    }

    //
    // Transfer ethernet physical header.
    //
    CSBeginFrame();

    p = nb->nb_dl.vp;
    for (i = 0; i < nb->nb_dl.sz; i++) {
        CSWriteFrameByte(*p++);
    }


    p = nb->nb_nw.vp;
    for (i = 0; i < nb->nb_nw.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    p = nb->nb_tp.vp;
    for (i = 0; i < nb->nb_tp.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    p = nb->nb_ap.vp;
    for (i = 0; i < nb->nb_ap.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    CSEndFrame();

    return 0;
}

void CSSoftwareWakeup(void)
{
    volatile uint8_t *p;
    uint16_t          data = CS_SELF_CTRL;

    p = (uint8_t *) CS_PP_PTR;
    *p = data;
    p++;
    *p = data >> 8;

    NutDelay(10);
}


void CSSoftwareReset(void)
{
    volatile uint8_t *p;
    uint16_t          data;

    data = CS_SELF_CTRL;
    p  = (uint8_t *) CS_PP_PTR;    
    *p = data;
    p++;
    *p = data >> 8;
        
    data = 0x0040;
    p  = (uint8_t *) CS_DATA_P0;
    *p = data;
    p++;
    *p = data >> 8;
}


THREAD(CSNICrx, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;
    uint8_t *p;
    uint8_t *q;
    uint16_t i, m;
    volatile uint16_t l;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter ETHReceive\r\n"));
        NutPrintFlush(dev_debug);
    }
#endif

    l = 0;

    NutThreadSetPriority(8);
    for (;;) {
        while ((CSReadPP16(CS_RX_EVENT) & 0x0100) == 0) {
            NutSleep(10);
        }

        l = *(uint8_t *) (CS_DATA_P0 + 1) << 8 | *(uint8_t *) (CS_DATA_P0);
        l = *(uint8_t *) (CS_DATA_P0 + 1) << 8 | *(uint8_t *) (CS_DATA_P0);
        
        //NutPrintFormat_P(dev_debug,PSTR("RxLength = %x \r\n"), l);
        //NutPrintFlush(dev_debug);

        // Account for odd length packets
        if (l & 1)
            m = l - 1;
        else
            m = l;


        nb = NutNetBufAlloc(0, NBAF_DATALINK, l);
        if (nb) {
            q = nb->nb_dl.vp;
            for (i = 0; i < m; i += 2) {
                p = (uint8_t *) CS_DATA_P0;
                *q++ = *p;
                p = (uint8_t *) CS_DATA_P0 + 1;
                *q++ = *p;
            }

            // Odd length packets
            if (m != l) {
                p = (uint8_t *) CS_DATA_P0;
                *q++ = *p;

                p = (uint8_t *) CS_DATA_P0 + 1;
                m = *p;
            }
            ni->ni_rx_packets++;
            (*ifn->if_recv) (dev, nb);
        }
    }
}

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/



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
int cs8900Output(NUTDEVICE * dev, NETBUF * nb)
{
    int rc = -1;
    NICINFO *ni;

    ni = (NICINFO *) dev->dev_dcb;

#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter EthOutput\r\n"));
        NutPrintFlush(dev_debug);
    }
#endif

    if ((rc = CSEthPutPacket(dev, nb)) == 0)
        ni->ni_tx_packets++;

    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets the CS8900 Ethernet controller, initializes all required 
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
int cs8900Init(NUTDEVICE * dev)
{
    uint16_t  i;
    uint16_t  j;
    IFNET   *ifn;
    NICINFO *ni;
    
#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter NicInit  \r\n"));
        NutPrintFlush(dev_debug);
    }
#endif

    cs_base = dev->dev_base;
    
#if defined(OLIMEX_LPCE2294)
    if (cs_base == 0)
    {
      cs_base = 0x82000000UL;
    }
#endif    


    if (confnet.cd_size == 0)
        NutNetLoadConfig(dev->dev_name);

    ifn = dev->dev_icb;
#if 0 /* @@MF */
    memcpy(ifn->if_mac, confnet.cdn_mac, 6);
#else
    ifn->if_mac[0] = 0x00;
    ifn->if_mac[1] = 0x06;
    ifn->if_mac[2] = 0x98;
    ifn->if_mac[3] = 0x00;
    ifn->if_mac[4] = 0x00;
    ifn->if_mac[5] = 0x00;
#endif    
    memset(dev->dev_dcb, 0, sizeof(NICINFO));
    ni = (NICINFO *) dev->dev_dcb;

    // Take CS8900 out of reset and wait for internal reset to complete
    CSSoftwareWakeup();
    CSSoftwareReset();

    NutDelay(100);

    // Check for presence
    if (CSReadPP16(CS_PROD_ID) != 0x630E)
        return -1;

    //
    //  Copy our MAC address to the NIC
    // 
    for (i = 0; i < 6; i += 2) {
        j = ifn->if_mac[i] << 8;
        j |= ifn->if_mac[i + 1];
        CSWritePP16(CS_IEEE_ADDR + i, j);
        j = CSReadPP16(CS_IEEE_ADDR + i);
#if 0
        if (tcp_trace) {
            NutPrintFormat_P(dev_debug, PSTR("ADDR = %x\r\n"), j);
            NutPrintFlush(dev_debug);
        }
#endif
    }

    //
    // Enable the Transmitter and Receiver
    //
    CSWritePP16(CS_LINE_CTRL, 0x00C0);
    //i = CSReadPP16(CS_LINE_CTRL);
    //NutPrintFormat_P(dev_debug,PSTR("CS_LINE_CTRL = %x\r\n"), i);

    CSWritePP16(CS_RX_CTL, 0x0F40);
    //i = CSReadPP16(CS_RX_CTL);
    //NutPrintFormat_P(dev_debug,PSTR("CS_RX_CTL = %x\r\n"), i);

    // 
    // Start receiver thread
    //
    NutThreadCreate("csnicrx", CSNICrx, dev, 768);

    return 0;
}

static NICINFO dcb_eth0;

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
    cs8900Output,               /*!< \brief Driver output routine, if_send(). */
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
NUTDEVICE devCS8900A = {
    0,                          /* Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /* Unique device name. */
    IFTYP_NET,                  /* Type of device. */
    0,                          /* Base address. */
    0,                          /* First interrupt number. */
    &ifn_eth0,                  /* Interface control block. */
    &dcb_eth0,                  /* Driver control block. */
    cs8900Init,                 /* Driver initialization routine. */
    0,                          /* Driver specific control function. */
    0,                          /* Read from device. */
    0,                          /* Write to device. */
    0,                          /* Open a device or file. */
    0,                          /* Close a device or file. */
    0                           /* Request file size. */
};

/*@}*/
