/*
 * Copyright (C) 2001-2003 by Cyber Integration, LLC. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY CYBER INTEGRATION, LLC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CYBER
 * INTEGRATION, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 */

/*
 * $Log$
 * Revision 1.4  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/05/24 07:29:10  haraldkipp
 * Update provided by Przemyslaw Rudy.
 *
 * Revision 1.1  2005/11/24 11:24:06  haraldkipp
 * Initial check-in.
 * Many thanks to William Basser for this code and also to Przemyslaw Rudy
 * for several enhancements.
 *
 */

// system include files
#include <string.h>
#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/device.h>
#include <dev/irqreg.h>
#include <dev/tlc16c550.h>
#include <fcntl.h>
#include <stdio.h>

/*
 * Not nice because stdio already defined them. But in order to save memory,
 * we do the whole buffering and including stdio here would be more weird.
 */
#ifndef _IOFBF
#define _IOFBF	    0x00
#define _IOLBF	    0x01
#define _IONBF	    0x02
#endif

/*!
 * \
 */
/*@{*/


// define the register offset
// define the UART Register offsets
#define	ACE_RBR_OFS	0
#define	ACE_THR_OFS	0
#define	ACE_DLL_OFS	0
#define	ACE_DLM_OFS	1
#define	ACE_IER_OFS	1
#define	ACE_FCR_OFS	2
#define	ACE_IIR_OFS	2
#define	ACE_LCR_OFS	3
#define	ACE_MCR_OFS	4
#define	ACE_LSR_OFS	5
#define	ACE_MSR_OFS	6
#define	ACE_SRC_OFS	7

// define the interrupt enable masks
#define	IER_RDA_MSK	0x01    // receiver data avaialbe
#define	IER_THE_MSK	0x02    // transmit holding empty
#define	IER_LST_MSK	0x04    // line status
#define	IER_MST_MSK	0x08    // modem status

// define the fifo control mask
#define FCR_ENABLE     0x01     //fifo enable
#define FCR_PURGE_I	0x02    //purge all data from input fifo
#define FCR_PURGE_O	0x04    //purge all data from output fifo
#define FCR_LEVEL_1    0x00     //receive trigger level 1
#define FCR_LEVEL_4    0x40     //receive trigger level 4
#define FCR_LEVEL_8    0x80     //receive trigger level 8
#define FCR_LEVEL_14   0xc0     //receive trigger level 14

// define the interrupt id masks
#define	IIR_MST_MSK	0x00    // modem status interrupt
#define	IIR_TXE_MSK	0x02    // transmit buffer empty
#define	IIR_RDA_MSK	0x04    // receive data available
#define	IIR_TDA_MSK	0x0c    // timeout receive data available
#define	IIR_LST_MSK	0x06    // line status interrupt
#define	IIR_NON_MSK	0x01    // no interrupt
#define IIR_FIFO_MSK   0xc0     // mask to eliminate the fifo status

// define the line control masks
#define	LCR_WS0_MSK	0x01
#define	LCR_WS1_MSK	0x02
#define	LCR_STB_MSK	0x04
#define	LCR_PEN_MSK	0x08
#define	LCR_PRE_MSK	0x10
#define	LCR_PRS_MSK	0x20
#define	LCR_BRK_MSK	0x40
#define	LCR_ENB_MSK	0x80

// define the modem control masks
#define	MCR_DTR_MSK	0x01
#define	MCR_RTS_MSK	0x02
#define	MCR_GP1_MSK	0x04
#define	MCR_GP2_MSK	0x08
#define	MCR_LOP_MSK	0x10

// define the line status masks
#define	LSR_RDR_MSK	0x01
#define	LSR_OVR_MSK	0x02
#define	LSR_PER_MSK	0x04
#define	LSR_FER_MSK	0x08
#define	LSR_BDT_MSK	0x10
#define	LSR_THE_MSK	0x20
#define	LSR_TXE_MSK	0x40
#define	LSR_EIF_MSK	0x80

// define the modem status masks
#define	MSR_DCTS_MSK	0x01
#define	MSR_DDSR_MSK	0x02
#define	MSR_DRI_MSK	0x04
#define	MSR_DDCD_MSK	0x08
#define	MSR_CTS_MSK	0x10
#define	MSR_DSR_MSK	0x20
#define	MSR_RI_MSK		0x40
#define	MSR_DCD_MSK	0x80

// define the irq structure
typedef struct tagIRQDEFS {
    IRQ_HANDLER *pvIrq;
    volatile uint8_t *pnIrqMskPort;
    uint8_t nMask;
} IRQDEFS;

// define the interrupt handlers
static CONST PROGMEM IRQDEFS atIrqDefs[] = {
    {&sig_INTERRUPT0, &EICRA, 0x00},
    {&sig_INTERRUPT1, &EICRA, 0x00},
    {&sig_INTERRUPT2, &EICRA, 0x00},
    {&sig_INTERRUPT3, &EICRA, 0x00},
    {&sig_INTERRUPT4, &EICRB, 0x00},
    {&sig_INTERRUPT5, &EICRB, 0x00},
    {&sig_INTERRUPT6, &EICRB, 0x00},
    {&sig_INTERRUPT7, &EICRB, 0x00}
};

// define the dcb's asigned to the interrupt to have more than one device on the same interrupt
// NUT intrnal irq structure could be used instead but that would be a hack
static NUTDEVICE *pIrqDev[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static uint8_t irqMask = 0;
#ifdef ACE_HDX_LINE
static unsigned int ByteOcrTime(NUTDEVICE * dev);
static void AceTmr3Init(void);
static void AceOutComp3AInt(void *arg);
static void AceAddHdxTime(ACEDCB * dev);
#endif

/*
 * Handle AVR ACE interrupts
 */
static void AceIrqHandler(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IFSTREAM *ifs;
    ACEDCB *dcb;
    volatile uint8_t event;
    uint8_t maxData;

    do {
        ifs = dev->dev_icb;
        dcb = dev->dev_dcb;

        // get the interrupt source
        while (((event = *(uint8_t *) (dev->dev_base + ACE_IIR_OFS)) & ~IIR_FIFO_MSK) != IIR_NON_MSK) {
            switch (event & ~IIR_FIFO_MSK) {
            case IIR_RDA_MSK:  // receive data available
            case IIR_TDA_MSK:  // timeout receive data available
                /* maxData can be avoided but it ensures that for slow system and fast uart we will not get stuck
                 * reading incomming data all the time.
                 */
                maxData = (dcb->dcb_rfifo == 0) ? 1 : dcb->dcb_rfifo;
                for (; (*(uint8_t *) (dev->dev_base + ACE_LSR_OFS) & LSR_RDR_MSK) && (maxData > 0); --maxData) {
                    // get the character and store it
                    ifs->if_rx_buf[ifs->if_rx_idx] = *(uint8_t *) (dev->dev_base + ACE_RBR_OFS);
                    /* if we have just received a first byte into the empty buffer */
                    if (ifs->if_rd_idx == ifs->if_rx_idx) {
                        NutEventPostFromIrq(&(dcb->dcb_rx_rdy));
                    }
                    /* Late increment fixes ICCAVR bug on volatile variables. */
                    ifs->if_rx_idx++;
                }
                break;

            case IIR_TXE_MSK:  // transmit buffer empty
                dcb->dcb_wfifo = (event & IIR_FIFO_MSK) ? ACE_FIFO_SIZE : 1;
                if (ifs->if_tx_idx != ifs->if_wr_idx) {
                    for (; (ifs->if_tx_idx != ifs->if_wr_idx) && (dcb->dcb_wfifo > 0); ++ifs->if_tx_idx) {
                        --dcb->dcb_wfifo;
                        // send a character
                        *(uint8_t *) (dev->dev_base + ACE_THR_OFS) = ifs->if_tx_buf[ifs->if_tx_idx];
                    }
                } else {
#ifdef ACE_HDX_LINE
                    if (dcb->dcb_modeflags & ACE_MF_HALFDUPLEX) {
                        AceAddHdxTime(dcb);
                    }
#endif
                    ifs->if_tx_act = 0;
                    NutEventPostFromIrq(&(dcb->dcb_tx_rdy));
                }
                break;

            case IIR_MST_MSK:  // modem status interrupt
                break;

            case IIR_LST_MSK:  // line status interrupt
                break;
            }
        }
        /* get the next device assigned to this interrupt */
        dev = dcb->dev_next;
    } while (dev != NULL);

}

#ifdef ACE_HDX_LINE
static unsigned int ByteOcrTime(NUTDEVICE * dev)
{
    uint8_t bv;
    uint8_t tb = 14;             /* twice of 1 start 5 char min. 1 stop */
    unsigned int sv;
    uint32_t s, c;

    /* get speed */
    *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) |= LCR_ENB_MSK;
    sv = *(uint8_t *) (dev->dev_base + ACE_DLL_OFS);
    sv |= *(uint8_t *) (dev->dev_base + ACE_DLM_OFS) << 8;
    *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) &= (uint8_t) ~ LCR_ENB_MSK;

    bv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS);

    /* character length *2 */
    tb += (bv & (LCR_WS0_MSK | LCR_WS1_MSK)) << 1;

    /* stop bits *2 */
    if (bv & LCR_STB_MSK) {
        tb += 1 + !!(bv & (LCR_WS0_MSK | LCR_WS1_MSK));
    }

    /* parity bit *2 */
    tb += (!!(bv & LCR_PEN_MSK)) << 1;

    s = ACE_CLOCK * 8UL;
    s = s / (uint32_t) (sv);

    c = NutGetCpuClock();
    c = c * (uint32_t) tb;

    sv = ((unsigned int) (c / s) & 0x0000ffff) - 1;

    return sv;
}
#endif

/*!
 * \brief Wait for input.
 *
 * This function checks the input buffer for any data. If
 * the buffer is empty, the calling \ref xrThread "thread" 
 * will be blocked until at least one new character is 
 * received or a timeout occurs.
 *
 * \param dev Indicates the ACE device.
 *
 * \return 0 on success, -1 on timeout.
 */
int AceInput(NUTDEVICE * dev)
{
    int rc = 0;
    IFSTREAM *ifs = dev->dev_icb;
    ACEDCB *dcb = dev->dev_dcb;

    if (ifs->if_rd_idx == ifs->if_rx_idx) {
        rc = NutEventWaitNext(&(dcb->dcb_rx_rdy), dcb->dcb_rtimeout);
    }
    return rc;
}

/*!
 * \brief Initiate output.
 *
 * This function checks the output buffer for any data. If
 * the buffer contains at least one character, the transmitter
 * is started, if not already running. The function returns
 * immediately, without waiting for the character being
 * completely transmitted. Any remaining characters in the
 * output buffer are transmitted in the background.
 *
 * \param dev Indicates the ACE device.
 *
 * \return 0 on success, -1 otherwise.
 */
int AceOutput(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;
    ACEDCB *dcb = dev->dev_dcb;
    volatile uint8_t tmp;

    if ((ifs->if_tx_act == 0) && (ifs->if_tx_idx != ifs->if_wr_idx)) {
        ifs->if_tx_act = 1;
        --dcb->dcb_wfifo;
        tmp = ifs->if_tx_idx;
        ++ifs->if_tx_idx;
#ifdef ACE_HDX_LINE
        if (dcb->dcb_modeflags & ACE_MF_HALFDUPLEX) {
            ACE_HDX_TRANSMIT(dev->dev_base);
        }
#endif
        // send a character
        *(uint8_t *) (dev->dev_base + ACE_THR_OFS) = ifs->if_tx_buf[tmp];
        // no need to enable an interrupt here as it should be enabled all the time
    }

    return 0;
}

/*!
 * \brief Wait for output buffer empty.
 *
 * If the output buffer contains any data, the calling
 * thread is suspended until all data has been transmitted.
 *
 * \param dev Indicates the ACE device.
 *
 * \return 0 on success, -1 otherwise.
 */
int AceFlush(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;
    ACEDCB *dcb = dev->dev_dcb;

    /*
     * Start any pending output.
     */
    if (AceOutput(dev))
        return -1;

    /*
     * Wait until output buffer empty.
     */
    while (ifs->if_tx_idx != ifs->if_wr_idx) {
        NutEventWaitNext(&dcb->dcb_tx_rdy, 100);
    }

    return 0;
}

/*
 *
 * \param dev Indicates the ACE device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AceGetStatus(NUTDEVICE * dev, uint32_t * status)
{
    IFSTREAM *ifs = dev->dev_icb;
    uint8_t us;

    *status = 0;
    us = *(uint8_t *) (dev->dev_base + ACE_LSR_OFS);
    if (us & LSR_FER_MSK)
        *status |= ACE_FRAMINGERROR;
    if (us & LSR_OVR_MSK)
        *status |= ACE_OVERRUNERROR;
    if (ifs->if_tx_idx == ifs->if_wr_idx)
        *status |= ACE_TXBUFFEREMPTY;
    if (ifs->if_rd_idx == ifs->if_rx_idx)
        *status |= ACE_RXBUFFEREMPTY;
    return 0;
}

/*
 * Carefully enable ACE functions.
 */
static void AceEnable(uint16_t base)
{
    /*volatile uint8_t* pnBase = *(volatile uint8_t* )base; */

    /*
     * Enable ACE interrupts.
     */
    NutEnterCritical();
    *(uint8_t *) (base + ACE_IER_OFS) = IER_RDA_MSK | IER_THE_MSK;
    NutExitCritical();
}

/*
 * Carefully disable ACE functions.
 */
static void AceDisable(uint16_t base)
{
    /*volatile uint8_t* pnBase = *(volatile uint8_t* )base; */

    /*
     * Disable ACE interrupts.
     */
    NutEnterCritical();
    *(uint8_t *) (base + ACE_IER_OFS) &= (uint8_t) ~ (IER_RDA_MSK);
    NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    NutDelay(10);
}

/*!
 * \brief Perform ACE control functions.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - ACE_SETSPEED, conf points to an uint32_t value containing the baudrate.
 *             - ACE_GETSPEED, conf points to an uint32_t value receiving the current baudrate.
 *             - ACE_SETDATABITS, conf points to an uint32_t value containing the number of data bits, 5, 6, 7 or 8.
 *             - ACE_GETDATABITS, conf points to an uint32_t value receiving the number of data bits, 5, 6, 7 or 8.
 *             - ACE_SETPARITY, conf points to an uint32_t value containing the parity, 0 (no), 1 (odd) or 2 (even).
 *             - ACE_GETPARITY, conf points to an uint32_t value receiving the parity, 0 (no), 1 (odd) or 2 (even).
 *             - ACE_SETSTOPBITS, conf points to an uint32_t value containing the number of stop bits 1 or 2.
 *             - ACE_GETSTOPBITS, conf points to an uint32_t value receiving the number of stop bits 1 or 2.
 *             - ACE_SETSTATUS
 *             - ACE_GETSTATUS
 *             - ACE_SETREADTIMEOUT, conf points to an uint32_t value containing the read timeout.
 *             - ACE_GETREADTIMEOUT, conf points to an uint32_t value receiving the read timeout.
 *             - ACE_SETWRITETIMEOUT, conf points to an uint32_t value containing the write timeout.
 *             - ACE_GETWRITETIMEOUT, conf points to an uint32_t value receiving the write timeout.
 *             - ACE_SETLOCALECHO, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - ACE_GETLOCALECHO, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *             - ACE_SETFLOWCONTROL, conf points to an uint32_t value containing combined ACE_FCR_ values.
 *             - ACE_GETFLOWCONTROL, conf points to an uint32_t value containing receiving ACE_FCR_ values.
 *             - ACE_SETCOOKEDMODE, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - ACE_GETCOOKEDMODE, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \warning Timeout values are given in milliseconds and are limited to 
 *          the granularity of the system timer. To disable timeout,
 *          set the parameter to NUT_WAIT_INFINITE.
 *
 *
 */
int AceIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    ACEDCB *dcb;
    IFSTREAM *ifs;
    uint32_t *lvp = (uint32_t *) conf;
    uint32_t lv = *lvp;
    uint8_t bv = (uint8_t) lv;
    uint16_t sv;
    uint16_t devnum;
    uint8_t tv;

    if (dev == 0) {
        return -1;
    }

    devnum = dev->dev_base;
    dcb = dev->dev_dcb;

    switch (req) {
    case ACE_SETSPEED:
        AceDisable(devnum);
        sv = (uint16_t) (ACE_CLOCK / (lv * 16UL));
        *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) |= LCR_ENB_MSK;
        *(uint8_t *) (dev->dev_base + ACE_DLL_OFS) = (uint8_t) (sv & 0xFF);
        *(uint8_t *) (dev->dev_base + ACE_DLM_OFS) = (uint8_t) (sv >> 8);
        *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) &= (uint8_t) ~ LCR_ENB_MSK;
#ifdef ACE_HDX_LINE
        dcb->hdxByteTime = ByteOcrTime(dev);
#endif
        AceEnable(devnum);
        break;

    case ACE_GETSPEED:
        *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) |= LCR_ENB_MSK;
        sv = *(uint8_t *) (dev->dev_base + ACE_DLL_OFS);
        sv |= *(uint8_t *) (dev->dev_base + ACE_DLM_OFS) << 8;
        *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) &= (uint8_t) ~ LCR_ENB_MSK;
        *lvp = ACE_CLOCK / (16UL * (uint32_t) (sv));
        break;

    case ACE_SETDATABITS:
        AceDisable(devnum);
        if ((bv >= 5) && (bv <= 8)) {
            bv -= 5;
            tv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS);
            tv &= (uint8_t) ~ (LCR_WS0_MSK | LCR_WS1_MSK);
            tv |= (bv & (LCR_WS0_MSK | LCR_WS1_MSK));
            *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) = tv;
#ifdef ACE_HDX_LINE
            dcb->hdxByteTime = ByteOcrTime(dev);
#endif
        }
        AceEnable(devnum);
        break;

    case ACE_GETDATABITS:
        *lvp = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) & (LCR_WS0_MSK | LCR_WS1_MSK);
        break;

    case ACE_SETPARITY:
        AceDisable(devnum);
        if (bv <= 4) {
            switch (bv) {
            case 1:            // odd parity
                bv = LCR_PEN_MSK;
                break;

            case 2:            // event parity
                bv = LCR_PEN_MSK | LCR_PRE_MSK;
                break;

            case 3:            // space
                bv = LCR_PEN_MSK;
                break;

            case 4:            // mark
                bv = LCR_PEN_MSK | LCR_PRS_MSK;

            default:           // no parity
                bv = 0;
                break;
            }

            tv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS);
            tv &= (uint8_t) ~ (LCR_PEN_MSK | LCR_PRE_MSK | LCR_PRS_MSK);
            tv |= bv;
            *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) = tv;
#ifdef ACE_HDX_LINE
            dcb->hdxByteTime = ByteOcrTime(dev);
#endif
        }
        AceEnable(devnum);
        break;

    case ACE_GETPARITY:
        tv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) & (LCR_PEN_MSK | LCR_PRE_MSK | LCR_PRS_MSK);
        switch (tv) {
        case 0:
            *lvp = 0;           // no parity
            break;

        case LCR_PEN_MSK:
            *lvp = 1;           // odd parity
            break;

        case LCR_PEN_MSK | LCR_PRE_MSK:
            *lvp = 2;           // event parity
            break;

        case LCR_PEN_MSK | LCR_PRS_MSK:
            *lvp = 4;           // mark parity
            break;
        }
        break;

    case ACE_SETSTOPBITS:
        AceDisable(devnum);
        if (bv == 1 || bv == 2) {
            tv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS);
            tv &= (uint8_t) ~ (LCR_STB_MSK);
            tv |= (bv == 2) ? LCR_STB_MSK : 0;
            *(uint8_t *) (dev->dev_base + ACE_LCR_OFS) = tv;
#ifdef ACE_HDX_LINE
            dcb->hdxByteTime = ByteOcrTime(dev);
#endif
        }
        AceEnable(devnum);
        break;

    case ACE_GETSTOPBITS:
        tv = *(uint8_t *) (dev->dev_base + ACE_LCR_OFS);
        *lvp = (tv & LCR_STB_MSK) ? 2 : 1;
        break;

    case ACE_SETFIFO:
        AceDisable(devnum);
        dcb->dcb_wfifo = ACE_FIFO_SIZE;
        switch (bv) {
        case 1:
            tv = FCR_ENABLE | FCR_LEVEL_1 | FCR_PURGE_I | FCR_PURGE_O;
            break;

        case 4:
            tv = FCR_ENABLE | FCR_LEVEL_4 | FCR_PURGE_I | FCR_PURGE_O;
            break;

        case 8:
            tv = FCR_ENABLE | FCR_LEVEL_8 | FCR_PURGE_I | FCR_PURGE_O;
            break;

        case 14:
            tv = FCR_ENABLE | FCR_LEVEL_14 | FCR_PURGE_I | FCR_PURGE_O;
            break;

        default:
            bv = 0;
            tv = 0;
            dcb->dcb_wfifo = 1;
            break;
        }
        *(uint8_t *) (dev->dev_base + ACE_FCR_OFS) = tv;
        /* if enabling then must write the level after */
        *(uint8_t *) (dev->dev_base + ACE_FCR_OFS) = tv;
        dcb->dcb_rfifo = bv;

        /* must signal any active and waiting writer, discard pending data */
        ifs = dev->dev_icb;

        ifs->if_tx_act = 0;
        ifs->if_tx_idx = ifs->if_wr_idx;
        NutEventPostAsync(&(dcb->dcb_tx_rdy));

        AceEnable(devnum);
        break;

    case ACE_GETFIFO:
        *lvp = (uint32_t) (dcb->dcb_rfifo);
        break;

    case ACE_GETSTATUS:
        AceGetStatus(dev, lvp);
        break;

    case ACE_SETSTATUS:
        rc = -1;
        break;

    case ACE_SETREADTIMEOUT:
        dcb->dcb_rtimeout = lv;
        break;
    case ACE_GETREADTIMEOUT:
        *lvp = dcb->dcb_rtimeout;
        break;

    case ACE_SETWRITETIMEOUT:
        dcb->dcb_wtimeout = lv;
        break;
    case ACE_GETWRITETIMEOUT:
        *lvp = dcb->dcb_wtimeout;
        break;

    case ACE_SETLOCALECHO:
        if (bv)
            dcb->dcb_modeflags |= ACE_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~ACE_MF_LOCALECHO;
        break;
    case ACE_GETLOCALECHO:
        *lvp = (dcb->dcb_modeflags & ACE_MF_LOCALECHO) ? 1 : 0;
        break;

    case ACE_SETFLOWCONTROL:
#ifdef ACE_HDX_LINE
        if (lv & ACE_MF_HALFDUPLEX) {
            /* next transmission will use HDX pin */
            dcb->dcb_modeflags |= ACE_MF_HALFDUPLEX;
        } else {
            dcb->dcb_modeflags &= ~ACE_MF_HALFDUPLEX;
        }
        dcb->hdxOcrTime = 0;
        /* switch HDX pin off */
        ACE_HDX_RECEIVE(dev->dev_base);
#endif
        break;

    case ACE_GETFLOWCONTROL:
#ifdef ACE_HDX_LINE
        *lvp = (uint32_t) (dcb->dcb_modeflags & ACE_MF_HALFDUPLEX);
#endif
        break;

    case ACE_SETCOOKEDMODE:
        if (bv)
            dcb->dcb_modeflags |= ACE_MF_COOKEDMODE;
        else
            dcb->dcb_modeflags &= ~ACE_MF_COOKEDMODE;
        break;
    case ACE_GETCOOKEDMODE:
        *lvp = (dcb->dcb_modeflags & ACE_MF_COOKEDMODE) ? 1 : 0;
        break;

    default:
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Initialize on chip ACE device.
 *
 * Prepares the device for subsequent reading or writing.
 * Enables ACE transmitter and receiver interrupts.
 *
 * \param dev  Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 */
int AceInit(NUTDEVICE * dev)
{
    IFSTREAM *ifs;
    ACEDCB *dcb, *pFirstDcb;
    uint32_t baudrate = 9600;
    uint32_t databits = 8;
    uint32_t parity = 0;
    uint32_t stopbits = 1;
    IRQ_HANDLER *irq;
    uint8_t *pnPort;
    uint8_t nMask;
#ifdef ACE_HDX_LINE
    uint32_t flowcontrol = 0;
#endif
    /*
     * We only support character devices for on-chip ACEs.
     */
    if (dev->dev_type != IFTYP_STREAM) {
        return -1;
    }

    /*
     * Initialize interface control block.
     */
    ifs = dev->dev_icb;
    memset(ifs, 0, sizeof(IFSTREAM));
    ifs->if_input = AceInput;
    ifs->if_output = AceOutput;
    ifs->if_flush = AceFlush;

    /*
     * Initialize driver control block.
     */
    dcb = dev->dev_dcb;
    memset(dcb, 0, sizeof(ACEDCB));
    dcb->dcb_modeflags = ACE_MF_NOBUFFER;
    dcb->dcb_rfifo = 0;
    dcb->dcb_wfifo = 1;
    dcb->dev_next = NULL;
#ifdef ACE_HDX_LINE
    dcb->hdxOcrTime = 0;
#endif

    /*
     * Register interrupt handler.
     */
    if (dev->dev_base) {
        /* if any ACE is already assigned to this interrupt */
        if (pIrqDev[dev->dev_irq] != NULL) {
            pFirstDcb = pIrqDev[dev->dev_irq]->dev_dcb;
            dcb->dev_next = pFirstDcb->dev_next;
            pFirstDcb->dev_next = dev;
        } else {
#ifdef ACE_HDX_LINE
            if (irqMask == 0) {
                /* Register interrupt handlers */
                if (NutRegisterIrqHandler(&sig_OUTPUT_COMPARE3A, AceOutComp3AInt, NULL)) {
                    return -1;
                }
                AceTmr3Init();
            }
#endif
            // get the appropriate irq handler
            irq = (IRQ_HANDLER *) pgm_read_word(&(atIrqDefs[dev->dev_irq].pvIrq));

            if (NutRegisterIrqHandler(irq, AceIrqHandler, dev)) {
                return -1;
            }
            // enable the interrupts
            pnPort = (uint8_t *) pgm_read_word(&(atIrqDefs[dev->dev_irq].pnIrqMskPort));
            nMask = pgm_read_byte(&(atIrqDefs[dev->dev_irq].nMask));
            *pnPort |= nMask;
            /* remember dcb of the recently initialized device */
            pIrqDev[dev->dev_irq] = dev;
            irqMask |= 0x01 << dev->dev_irq;
        }
    }

    /*
     * Set baudrate and handshake default. This will also
     * enable the ACE functions.
     */
    AceIOCtl(dev, ACE_SETSPEED, (void *) &baudrate);
    AceIOCtl(dev, ACE_SETDATABITS, (void *) &databits);
    AceIOCtl(dev, ACE_SETPARITY, (void *) &parity);
    AceIOCtl(dev, ACE_SETSTOPBITS, (void *) &stopbits);
#ifdef ACE_HDX_LINE
    /* set HDX pin off by defalt */
    AceIOCtl(dev, ACE_SETFLOWCONTROL, (void *) &flowcontrol);
#endif
    sbi(EIMSK, dev->dev_irq);   /* dev->dev_irq to IRQ_INTx map should be used for a clean implementation but it looks like an overhead */

    AceEnable(dev->dev_base);

    return 0;
}

/*! 
 * \brief Read from device. 
 */
int AceRead(NUTFILE * fp, void *buffer, int size)
{
    int rc;
    NUTDEVICE *dev;
    IFSTREAM *ifs;
    ACEDCB *dcb;
    uint8_t elmode;
    uint8_t ch;
    uint8_t *cp = buffer;

    dev = fp->nf_dev;
    ifs = (IFSTREAM *) dev->dev_icb;
    dcb = dev->dev_dcb;

    if (dcb->dcb_modeflags & ACE_MF_COOKEDMODE)
        elmode = 1;
    else
        elmode = 0;

    /*
     * Call without data pointer discards receive buffer.
     */
    if (buffer == 0) {
        ifs->if_rd_idx = ifs->if_rx_idx;
        return 0;
    }

    /*
     * Get characters from receive buffer.
     */
    for (rc = 0; rc < size;) {
        /* if nothing has been received yet */
        if (ifs->if_rd_idx == ifs->if_rx_idx) {
            /* while incomming buffer is empty */
            while (ifs->if_rd_idx == ifs->if_rx_idx) {
                /* wait (timeout) for incomming data */
                if (AceInput(dev)) {
                    /* if a timeout */
                    return 0;
                }
            }
        }
        ch = ifs->if_rx_buf[ifs->if_rd_idx++];
        if (elmode && (ch == '\r' || ch == '\n')) {
            if ((ifs->if_last_eol == 0) || (ifs->if_last_eol == ch)) {
                ifs->if_last_eol = ch;
                *cp++ = '\n';
                rc++;
            }
        } else {
            ifs->if_last_eol = 0;
            *cp++ = ch;
            rc++;
        }
    }
    return rc;
}

/*! 
 * \brief Write to device. 
 */
int AcePut(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc;
    IFSTREAM *ifs;
    ACEDCB *dcb;
    CONST uint8_t *cp;
    uint8_t lbmode;
    uint8_t elmode;
    uint8_t ch;

    ifs = dev->dev_icb;
    dcb = dev->dev_dcb;

    if (dcb->dcb_modeflags & ACE_MF_LINEBUFFER)
        lbmode = 1;
    else
        lbmode = 0;

    if (dcb->dcb_modeflags & ACE_MF_COOKEDMODE)
        elmode = 1;
    else
        elmode = 0;

    /*
     * Call without data pointer starts transmission.
     */
    if (buffer == 0) {
        rc = AceFlush(dev);
        return rc;
    }

    /*
     * Put characters in transmit buffer.
     */
    cp = buffer;
    for (rc = 0; rc < len;) {
        if ((uint8_t) (ifs->if_wr_idx + 1) == ifs->if_tx_idx) {
            if (AceFlush(dev)) {
                return -1;
            }
        }
        ch = pflg ? PRG_RDB(cp) : *cp;
        if (elmode == 1 && ch == '\n') {
            elmode = 2;
            if (lbmode == 1)
                lbmode = 2;
            ch = '\r';
        } else {
            if (elmode == 2)
                elmode = 1;
            cp++;
            rc++;
        }
        ifs->if_tx_buf[ifs->if_wr_idx++] = ch;
    }

    if (lbmode > 1 || (dcb->dcb_modeflags & ACE_MF_NOBUFFER) != 0) {
        if (AceFlush(dev))
            rc = -1;
    }
    return rc;
}

int AceWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return AcePut(fp->nf_dev, buffer, len, 0);
}

int AceWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return AcePut(fp->nf_dev, (CONST char *) buffer, len, 1);
}


/*! 
 * \brief Open a device or file.
 */
NUTFILE *AceOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp = NutHeapAlloc(sizeof(NUTFILE));
    ACEDCB *dcb;

    if (fp == 0)
        return NUTFILE_EOF;

    dcb = dev->dev_dcb;
    if (mode & _O_BINARY)
        dcb->dcb_modeflags &= ~ACE_MF_COOKEDMODE;
    else
        dcb->dcb_modeflags |= ACE_MF_COOKEDMODE;

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    return fp;
}

/*! 
 * \brief Close a device or file. 
 */
int AceClose(NUTFILE * fp)
{
    NutHeapFree(fp);

    return 0;
}

/*! 
 * \brief Request file size.
 */
long AceSize(NUTFILE * fp)
{
    NUTDEVICE *dev;
    IFSTREAM *ifs;

    dev = fp->nf_dev;
    ifs = (IFSTREAM *) dev->dev_icb;
    return ((uint8_t) (ifs->if_rx_idx - ifs->if_rd_idx));
}

#ifdef ACE_HDX_LINE
static void AceTmr3Init(void)
{
    /* TMR3 also runs in normal mode */
    TCCR3A &= ~(_BV(COM3A1) | _BV(COM3A0) | _BV(WGM31) | _BV(WGM30));   /* normal mode */

    TCCR3B &= ~(_BV(WGM33) | _BV(WGM32) | _BV(CS32) | _BV(CS31) | _BV(CS30));
    TCCR3B |= _BV(CS31) | _BV(CS30);    /* f/64 (155200Hz - 1.75Hz) */

    /* TMR3 output compare A match interrupt disable */
    ETIMSK &= ~_BV(OCIE3A);
}

static void AceOutComp3AInt(void *arg)
{
    NUTDEVICE *dev;
    ACEDCB *dcb;
    int i;
    unsigned int nextOcr = 0xffff, ocr;
    unsigned int timerOcrDiff;

    /* TMR3 stop counting */
    TCCR3B &= ~(_BV(CS31) | _BV(CS30));

    timerOcrDiff = (unsigned int /* modulo max */ )((unsigned int /* modulo max */ )TCNT3 - (unsigned int /* modulo max */ )OCR3A);

    /* Due to interrupt nesting and as TL interrupts are higher priority - disable them.
     * This routine cannot be interrupted by a cal to the AceAddHdxTime()
     */
    EIMSK &= ~irqMask;

    for (i = 0; i < 8; ++i) {
        for (dev = pIrqDev[i]; dev != NULL; dev = dcb->dev_next) {
            dcb = dev->dev_dcb;
            /* only if enabled */
            if (dcb->hdxOcrTime != 0) {
                if ((unsigned int /* modulo max */ )(dcb->hdxOcrTime - (unsigned int /* modulo max */ )OCR3A) <= timerOcrDiff) {
                    dcb->hdxOcrTime = 0;
                    ACE_HDX_RECEIVE(dev->dev_base);
                } else {
                    ocr = (unsigned int /* modulo max */ )(dcb->hdxOcrTime - (unsigned int /* modulo max */ )TCNT3);
                    if (ocr < nextOcr) {
                        nextOcr = ocr;
                    }
                }
            }
        }
    }

    if (nextOcr == 0xffff) {
        /* TMR3 output compare A match interrupt disable */
        ETIMSK &= ~_BV(OCIE3A);
    } else {
        OCR3A = nextOcr;
        /* start timer */
        TCCR3B |= _BV(CS31) | _BV(CS30);        /* f/64 (155200Hz - 1.75Hz) */
    }

    /* Re-enable TL interrupt. */
    EIMSK |= irqMask;
}

static void AceAddHdxTime(ACEDCB * dcb)
{
    /* TMR3 stop counting */
    TCCR3B &= ~(_BV(CS31) | _BV(CS30));

    /* if disabled */
    if ((ETIMSK & _BV(OCIE3A)) == 0) {
        /* initialize */
        OCR3A = 0;
        TCNT3 = 1;
    }

    /* set offset from current counter value */
    dcb->hdxOcrTime = (unsigned int /* modulo max */ )((unsigned int /* modulo max */ )TCNT3 + dcb->hdxByteTime);

    if (dcb->hdxOcrTime == 0) {
        dcb->hdxOcrTime = 1;    /* 0 means disabled, one bit delay is not a problem as there is interrupt latency anyway */
    }

    if (dcb->hdxByteTime < (unsigned int /* modulo max */ )((unsigned int /* modulo max */ )OCR3A - (unsigned int /* modulo max */ )TCNT3)) {
        OCR3A = dcb->hdxOcrTime;
    }
    /* TMR3 output compare A match interrupt enable */
    ETIMSK |= _BV(OCIE3A);

    /* start timer */
    TCCR3B |= _BV(CS31) | _BV(CS30);    /* f/64 (155200Hz - 1.75Hz) */
}
#endif

/*@}*/
