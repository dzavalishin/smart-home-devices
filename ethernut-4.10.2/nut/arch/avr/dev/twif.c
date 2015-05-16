/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.7  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2007/07/26 13:03:05  freckle
 * Reduced critical section in TwSlaveListen
 *
 * Revision 1.5  2006/10/08 16:48:08  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2006/06/28 14:31:24  haraldkipp
 * NutEventPostFromIrq() doesn't return a result anymore. We directly
 * inspect the queue's root.
 *
 * Revision 1.3  2005/10/24 10:56:30  haraldkipp
 * Added const modifier to transmit data pointer in TwMasterTransact().
 *
 * Revision 1.2  2005/10/07 22:03:29  hwmaier
 * Using __AVR_ENHANCED__ macro instead of __AVR_ATmega128__ to support also AT90CAN128 MCU
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.9  2005/01/24 21:12:05  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.8  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.7  2004/12/16 08:40:35  haraldkipp
 * Late increment fixes ICCAVR bug.
 *
 * Revision 1.6  2004/11/08 18:12:59  haraldkipp
 * Soooo many fixes, but I'm tired...really.
 *
 * Revision 1.5  2004/09/08 10:19:14  haraldkipp
 * *** empty log message ***
 *
 * Revision 1.4  2003/11/03 17:03:53  haraldkipp
 * Many new comments added
 *
 * Revision 1.3  2003/07/20 18:28:10  haraldkipp
 * Explain how to disable timeout.
 *
 * Revision 1.2  2003/07/17 09:38:07  haraldkipp
 * Setting different speeds is now supported.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:53  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/03/31 14:53:08  harald
 * Prepare release 3.1
 *
 * Revision 1.1  2003/02/04 17:54:59  harald
 * *** empty log message ***
 *
 */

#include <string.h>

#include <dev/irqreg.h>

#include <sys/event.h>
#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include <dev/twif.h>

#ifdef __AVR_ENHANCED__

static volatile uint8_t tw_if_bsy;   /* Set while interface is busy. */

HANDLE tw_mm_mutex;          /* Exclusive master access. */
HANDLE tw_mm_que;            /* Threads waiting for master transfer done. */
HANDLE tw_sr_que;            /* Threads waiting for slave receive. */
HANDLE tw_st_que;            /* Threads waiting for slave transmit done. */

static uint8_t tw_mm_sla;            /* Destination slave address. */
static volatile uint8_t tw_mm_err;   /* Current master mode error. */
static uint8_t tw_mm_error;          /* Last master mode error. */

static CONST uint8_t *tw_mt_buf;     /* Pointer to the master transmit buffer. */
static volatile uint16_t tw_mt_len;  /* Number of bytes to transmit in master mode. */
static volatile uint16_t tw_mt_idx;  /* Current master transmit buffer index. */

static uint8_t *tw_mr_buf;           /* Pointer to the master receive buffer. */
static volatile uint16_t tw_mr_siz;  /* Size of the master receive buffer. */
static volatile uint16_t tw_mr_idx;  /* Current master receive buffer index. */

static volatile uint8_t tw_sm_sla;   /* Slave address received. */
static volatile uint8_t tw_sm_err;   /* Current slave mode error. */
static uint8_t tw_sm_error;          /* Last slave mode error. */

static uint8_t *tw_st_buf;           /* Pointer to the slave transmit buffer. */
static volatile uint16_t tw_st_len;  /* Number of bytes to transmit in slave mode. */
static volatile uint16_t tw_st_idx;  /* Current slave transmit buffer index. */

static uint8_t *tw_sr_buf;           /* Pointer to the slave receive buffer. */
static volatile uint16_t tw_sr_siz;  /* Size of the master receive buffer. */
static volatile uint16_t tw_sr_idx;  /* Current slave receive buffer index. */

/*
TWINT TWEA TWSTA TWSTO TWWC TWEN  0  TWIE
  C                      R        R
*/

#define TWGO    (_BV(TWINT) | _BV(TWEN) | _BV(TWIE))

/*
 * TWI interrupt handler.
 */
static void TwInterrupt(void *arg)
{
    uint8_t twsr;
    register uint8_t twcr = inb(TWCR);

    /*
     * Read the status and interpret its contents.
     */
    twsr = inb(TWSR) & 0xF8;
    switch (twsr) {

    /*
     * 0x08: Start condition has been transmitted.
     * 0x10: Repeated start condition has been transmitted.
     */
    case TW_START:
    case TW_REP_START:
        /* We are entering the master mode. Mark the interface busy. */
        tw_if_bsy = 1;
        tw_mt_idx = 0;
        tw_mr_idx = 0;

        /*
         * If outgoing data is available, transmit SLA+W. Logic is in
         * master transmit mode.
         */
        if (tw_mt_len) {
            outb(TWDR, tw_mm_sla);
        }

        /*
         * If outgoing data not available, transmit SLA+R. Logic will
         * switch to master receiver mode.
         */
        else {
            outb(TWDR, tw_mm_sla | 1);
        }
        outb(TWCR, TWGO | (twcr & _BV(TWEA)));
        break;

    /*
     * 0x18: SLA+W has been transmitted and ACK has been received.
     * 0x28: Data byte has been transmitted and ACK has been received.
     */
    case TW_MT_SLA_ACK:
    case TW_MT_DATA_ACK:
        /*
         * If outgoing data left to send, put the next byte in the data
         * register.
         */
        if (tw_mt_idx < tw_mt_len) {
            outb(TWDR, tw_mt_buf[tw_mt_idx]);
            /* Late increment fixes ICCAVR bug. Thanks to Andreas Siebert and Michael Fischer. */
            tw_mt_idx++;
            outb(TWCR, TWGO | (twcr & _BV(TWEA)));
            break;
        }

        /*
         * All outgoing data has been sent. If a response is expected,
         * transmit a repeated start condition.
         */
        tw_mt_len = 0;
        if (tw_mr_siz) {
            outb(TWCR, TWGO | (twcr & _BV(TWEA)) | _BV(TWSTA));
            break;
        }

    /*
     * 0x20: SLA+W has been transmitted, but not acknowledged.
     * 0x30: Data byte has been transmitted, but not acknowledged.
     * 0x48: SLA+R has been transmitted, but not acknowledged.
     */
    case TW_MT_SLA_NACK:
    case TW_MT_DATA_NACK:
    case TW_MR_SLA_NACK:
        /* Set unique error code. */
        if (twsr == TW_MT_SLA_NACK || twsr == TW_MR_SLA_NACK) {
            tw_mm_err = TWERR_SLA_NACK;
            tw_mt_len = 0;
            tw_mr_siz = 0;
        }

        /* Wake up the application. */
        NutEventPostFromIrq(&tw_mm_que);

        /*
         * Send a stop condition. If we have a listener, generate
         * an acknowlegde on an incoming address byte.
         */
        if(tw_sr_siz) {
            outb(TWCR, TWGO | _BV(TWEA) | _BV(TWSTO));
        }
        else {
            outb(TWCR, TWGO | _BV(TWSTO));
        }

        /* The interface is idle. */
        tw_if_bsy = 0;
        break;

    /*
     * 0x38: Arbitration lost while in master mode.
     */
    case TW_MT_ARB_LOST:
        /*
         * The start condition will be automatically resend after
         * the bus becomes available.
         */
        sbi(TWCR, TWSTA);
        /* The interface is idle. */
        tw_if_bsy = 0;
        break;

    /*
     * 0x50: Data byte has been received and acknowledged.
     */
    case TW_MR_DATA_ACK:
        /*
         * Store the data byte in the master receive buffer.
         */
        tw_mr_buf[tw_mr_idx] = inb(TWDR);
        /* Late increment fixes ICCAVR bug. Thanks to Andreas Siebert and Michael Fischer. */
        tw_mr_idx++;

    /*
     * 0x40: SLA+R has been transmitted and ACK has been received.
     */
    case TW_MR_SLA_ACK:
        /*
         * Acknowledge next data bytes except the last one.
         */
        if (tw_mr_idx + 1 < tw_mr_siz) {
            outb(TWCR, TWGO | _BV(TWEA));
        }
        else {
            outb(TWCR, TWGO);
        }
        break;

    /*
     * 0x58: Data byte has been received, but not acknowledged.
     */
    case TW_MR_DATA_NACK:
        /*
         * Store the last data byte.
         */
        tw_mr_buf[tw_mr_idx] = inb(TWDR);
        /* Late increment fixes ICCAVR bug. Thanks to Andreas Siebert and Michael Fischer. */
        tw_mr_idx++;
        tw_mr_siz = 0;

        /* Wake up the application. */
        NutEventPostFromIrq(&tw_mm_que);

        /*
         * Send a stop condition. If we have a listener, generate
         * an acknowlegde on an incoming address byte.
         */
        if(tw_sr_siz) {
            outb(TWCR, TWGO | _BV(TWEA) | _BV(TWSTO));
        }
        else {
            outb(TWCR, TWGO | _BV(TWSTO));
        }

        /* The interface is idle. */
        tw_if_bsy = 0;
        break;

    /*
     * 0x60: Own SLA+W has been received and acknowledged.
     * 0x68: Arbitration lost as master. Own SLA+W has been received
     *       and acknowledged.
     * 0x70: General call address has been received and acknowledged.
     * 0x78: Arbitration lost as master. General call address has been
     *       received and acknowledged.
     */
    case TW_SR_SLA_ACK:
    case TW_SR_ARB_LOST_SLA_ACK:
    case TW_SR_GCALL_ACK:
    case TW_SR_ARB_LOST_GCALL_ACK:
        /*
         * Do only acknowledge incoming data bytes, if we got receive
         * buffer space. Fetch the slave address from the data register
         * and reset the receive index.
         */
        if (tw_sr_siz) {
            /* We are entering the slave receive mode. Mark the interface busy. */
            tw_if_bsy = 1;

            tw_sm_sla = inb(TWDR);
            outb(TWCR, TWGO | _BV(TWEA));
            tw_sr_idx = 0;
        }

        /*
         * Do not acknowledge incoming data.
         */
        else {
            outb(TWCR, TWGO);
        }
        break;

    /*
     * 0x80: Data byte for own SLA has been received and acknowledged.
     * 0x90: Data byte for general call address has been received and
     *       acknowledged.
     */
    case TW_SR_DATA_ACK:
    case TW_SR_GCALL_DATA_ACK:
        /*
         * If the receive buffer isn't filled up, store data byte.
         */
        if (tw_sr_idx < tw_sr_siz) {
            tw_sr_buf[tw_sr_idx] = inb(TWDR);
            /* Late increment fixes ICCAVR bug. Thanks to Andreas Siebert and Michael Fischer. */
            tw_sr_idx++;
        }
        else {
            tw_sr_siz = 0;
        }

        /*
         * If more space is available for incoming data, then continue
         * receiving. Otherwise do not acknowledge new data bytes.
         */
        if (tw_sr_siz) {
            outb(TWCR, TWGO | _BV(TWEA));
            break;
        }

    /*
     * 0x88: Data byte received, but not acknowledged.
     * 0x98: Data byte for general call address received, but not
     *       acknowledged.
     */
    case TW_SR_DATA_NACK:
    case TW_SR_GCALL_DATA_NACK:
        /*
         * Continue not accepting more data.
         */
        if (tw_mt_len || tw_mr_siz) {
            outb(TWCR, inb(TWCR) | _BV(TWEA) | _BV(TWSTA));
        }
        else {
            outb(TWCR, inb(TWCR) | _BV(TWEA));
        }
        break;

    /*
     * 0xA0: Stop condition or repeated start condition received.
     */
    case TW_SR_STOP:
        /*
         * Wake up the application. If successful, do nothing. This
         * will keep SCL low and thus block the bus. The application
         * must now setup the transmit buffer and re-enable the
         * interface.
         */
        if (tw_sr_que == 0 || tw_sm_err) {
            /*
             * If no one has been waiting on the queue, the application
             * probably gave up waiting. So we continue on our own, either
             * in idle mode or switching to master mode if a master
             * request is waiting.
             */
            if (tw_mt_len || tw_mr_siz) {
                outb(TWCR, TWGO | _BV(TWSTA));
            }
            else {
                outb(TWCR, TWGO);
            }
            tw_if_bsy = 0;
        }
        else {
            NutEventPostFromIrq(&tw_sr_que);
            tw_sr_siz = 0;
            outb(TWCR, twcr & ~(_BV(TWINT) | _BV(TWIE)));
        }
        break;

    /*
     * 0xA8: Own SLA+R has been received and acknowledged.
     * 0xB0: Arbitration lost in master mode. Own SLA has been received
     *       and acknowledged.
     */
    case TW_ST_SLA_ACK:
    case TW_ST_ARB_LOST_SLA_ACK:
        /* Not idle. */
        tw_if_bsy = 1;
        /* Reset transmit index and fall through for outgoing data. */
        tw_st_idx = 0;

    /*
     * 0xB8: Data bytes has been transmitted and acknowledged.
     */
    case TW_ST_DATA_ACK:
        /*
         * If outgoing data left to send, put the next byte in the
         * data register. Otherwise transmit a dummy byte.
         */
        if (tw_st_idx < tw_st_len) {
            outb(TWDR, tw_st_buf[tw_st_idx]);
            /* Do not set acknowledge on the last data byte. */
            /* Early increment fixes ICCAVR bug. Thanks to Andreas Siebert and Michael Fischer. */
            ++tw_st_idx;
            if (tw_st_idx < tw_st_len) {
                outb(TWCR, TWGO | _BV(TWEA));
            }
            else {
                tw_st_len = 0;
                outb(TWCR, TWGO);
            }
            break;
        }

        /* No more data. Continue sending dummies. */
        outb(TWDR, 0);
        outb(TWCR, TWGO);
        break;

    /*
     * 0xC0: Data byte has been transmitted, but not acknowledged.
     * 0xC8: Last data byte has been transmitted and acknowledged.
     */
    case TW_ST_DATA_NACK:
    case TW_ST_LAST_DATA:
        NutEventPostFromIrq(&tw_st_que);

        /* Transmit start condition, if a master transfer is waiting. */
        if (tw_mt_len || tw_mr_siz) {
            outb(TWCR, TWGO | _BV(TWSTA) | /**/ _BV(TWEA));
        }
        /* Otherwise enter idle state. */
        else {
            outb(TWCR, TWGO | _BV(TWEA));
        }
        tw_if_bsy = 0;
        break;

    /*
     * 0x00: Bus error.
     */
    case TW_BUS_ERROR:
        outb(TWCR, inb(TWCR) | _BV(TWSTO));
#if 1
        tw_if_bsy = 0;
        tw_mm_err = TWERR_BUS;
        tw_sm_err = TWERR_BUS;
        NutEventPostFromIrq(&tw_sr_que);
        NutEventPostFromIrq(&tw_st_que);
        NutEventPostFromIrq(&tw_mm_que);
#endif
        break;
    }
}

#endif /* __AVR_ENHANCED__ */

/*!
 * \brief Transmit and/or receive data as a master.
 *
 * The two-wire serial interface must have been initialized by calling
 * TwInit() before this function can be used.
 *
 * \note This function is only available on ATmega128 systems.
 *
 * \param sla    Slave address of the destination. This slave address
 *               must be specified as a 7-bit address. For example, the
 *               PCF8574A may be configured to slave addresses from 0x38
 *               to 0x3F.
 * \param txdata Points to the data to transmit. Ignored, if the number
 *               of data bytes to transmit is zero.
 * \param txlen  Number of data bytes to transmit. If zero, then the
 *               interface will not send any data to the slave device
 *               and will directly enter the master receive mode.
 * \param rxdata Points to a buffer, where the received data will be
 *               stored. Ignored, if the maximum number of bytes to
 *               receive is zero.
 * \param rxsiz  Maximum number of bytes to receive. Set to zero, if
 *               no bytes are expected from the slave device.
 * \param tmo    Timeout in milliseconds. To disable timeout, set this
 *               parameter to NUT_WAIT_INFINITE.
 *
 * \return The number of bytes received, -1 in case of an error or timeout.
 */
int TwMasterTransact(uint8_t sla, CONST void *txdata, uint16_t txlen, void *rxdata, uint16_t rxsiz, uint32_t tmo)
{
    int rc = -1;

#ifdef __AVR_ENHANCED__
    /* This routine is marked reentrant, so lock the interface. */
    if(NutEventWait(&tw_mm_mutex, 500)) {
        tw_mm_err = TWERR_IF_LOCKED;
        NutEventPost(&tw_mm_mutex);
        return -1;
    }

    while(tw_if_bsy) {
        NutSleep(63);
    }
    NutEnterCritical();
    /*
     * Set all parameters for master mode.
     */
    tw_mm_sla = sla << 1;
    tw_mm_err = 0;
    tw_mt_len = txlen;
    tw_mt_buf = txdata;
    tw_mr_siz = rxsiz;
    tw_mr_buf = rxdata;

    /*
     * Send a start condition if the interface is idle. If busy, then
     * the interrupt routine will automatically initiate the transfer
     * as soon as the interface becomes ready again.
     */
    if(tw_if_bsy == 0) {
        uint8_t twcr = inb(TWCR);
        uint8_t twsr = inb(TWSR);
        if((twsr & 0xF8) == TW_NO_INFO) {
            if(tw_sr_siz) {
                outb(TWCR, _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWSTA) | (twcr & _BV(TWSTO)));
            }
            else {
                outb(TWCR, _BV(TWEN) | _BV(TWIE) | _BV(TWSTA) | (twcr & _BV(TWSTO)));
            }
        }
    }

    /* Clear the queue. */
    //*broken?! NutEventBroadcastAsync(&tw_mm_que);
    if (tw_mm_que == SIGNALED) {
        tw_mm_que = 0;
    }
    NutExitCritical();

    /*
     * Wait for master transmission done.
     */
    rc = -1;
    if (NutEventWait(&tw_mm_que, tmo)) {
        tw_mm_error = TWERR_TIMEOUT;
    } else {
        NutEnterCritical();
        if (tw_mm_err) {
            tw_mm_error = tw_mm_err;
        } else {
            rc = tw_mr_idx;
        }
        NutExitCritical();
    }

    /*
     * Release the interface.
     */
    NutEventPost(&tw_mm_mutex);

#endif /* __AVR_ENHANCED__ */
    return rc;
}

/*!
 * \brief Get last master mode error.
 *
 * You may call this function to determine the specific cause
 * of an error after TwMasterTransact() failed.
 *
 * \note This function is only available on ATmega128 systems.
 *
 */
int TwMasterError(void)
{
#ifndef __AVR_ENHANCED__
    return -1;
#else
    int rc = (int) tw_mm_error;
    tw_mm_error = 0;
    return rc;
#endif
}

/*!
 * \brief Listen for incoming data from a master.
 *
 * If this function returns without error, the bus is blocked. The caller
 * must immediately process the request and return a response by calling
 * TwSlaveRespond().
 *
 * \note This function is only available on ATmega128 systems. The
 *       function is not reentrant.
 *
 * \param sla    Points to a byte variable, which receives the slave
 *               address sent by the master. This can be used by the
 *               caller to determine whether the the interface has been
 *               addressed by a general call or its individual address.
 * \param rxdata Points to a data buffer where the received data bytes
 *               are stored.
 * \param rxsiz  Specifies the maximum number of data bytes to receive.
 * \param tmo	 Timeout in milliseconds. To disable timeout,
 *               set this parameter to NUT_WAIT_INFINITE.
 *
 * \return The number of bytes received, -1 in case of an error or timeout.
 */
int TwSlaveListen(uint8_t * sla, void *rxdata, uint16_t rxsiz, uint32_t tmo)
{
#ifndef __AVR_ENHANCED__
    return -1;
#else
    int rc = -1;

    NutEnterCritical();

    /* Initialize parameters for slave receive. */
    tw_sm_err = 0;
    tw_sr_siz = rxsiz;
    tw_sr_buf = rxdata;

    /*
     * If the interface is currently not busy then enable it for
     * address recognition.
     */
    if(tw_if_bsy == 0) {
        uint8_t twsr = inb(TWSR);
        if((twsr & 0xF8) == TW_NO_INFO) {
            if(tw_mt_len || tw_mr_siz)
                outb(TWCR, _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA));
            else
                outb(TWCR, _BV(TWEA) | _BV(TWEN) | _BV(TWIE));
        }
    }

    /* Clear the queue. */
    //*broken?! NutEventBroadcastAsync(&tw_sr_que);
    if (tw_sr_que == SIGNALED) {
        tw_sr_que = 0;
    }

    NutExitCritical();

    /* Wait for a frame on the slave mode queue. */
    if (NutEventWait(&tw_sr_que, tmo)) {
        NutEnterCritical();
        tw_sm_err = TWERR_TIMEOUT;
        tw_sr_siz = 0;
        NutExitCritical();
    }

    /*
     * Return the number of bytes received and the destination slave
     * address, if no slave error occured. In this case the bus is
     * blocked.
     */
    if(tw_sm_err == 0) {
        rc = tw_sr_idx;
        *sla = tw_sm_sla;
    }
    return rc;
#endif /* __AVR_ENHANCED__ */
}

/*!
 * \brief Send response to a master.
 *
 * This function must be called as soon as possible after TwSlaveListen()
 * returned successfully, even if no data needs to be returned. Not doing
 * so will completely block the bus.
 *
 * \note This function is only available on ATmega128 systems.
 *
 * \param txdata Points to the data to transmit. Ignored, if the
 *      		 number of bytes to transmit is zero.
 * \param txlen  Number of data bytes to transmit.
 * \param tmo	 Timeout in milliseconds. To disable timeout,
 *               set this parameter to NUT_WAIT_INFINITE.
 *
 * \return The number of bytes transmitted, -1 in case of an error or timeout.
 */
int TwSlaveRespond(void *txdata, uint16_t txlen, uint32_t tmo)
{
    int rc = -1;
#ifdef __AVR_ENHANCED__

    /* The bus is blocked. No critical section required. */
    tw_st_buf = txdata;
    tw_st_len = txlen;

    /*
     * If there is anything to transmit, start the interface.
     */
    if (txlen) {
        NutEnterCritical();
        /* Clear the queue. */
        //*broken?! NutEventBroadcastAsync(&tw_st_que);
        if (tw_st_que == SIGNALED) {
            tw_st_que = 0;
        }

        /* Release the bus, accepting SLA+R. */
        outb(TWCR, TWGO | _BV(TWEA));

        NutExitCritical();
        if (NutEventWait(&tw_st_que, tmo)) {
            tw_sm_err = TWERR_TIMEOUT;
        }

        NutEnterCritical();
        tw_st_len = 0;
        if (tw_sm_err) {
            tw_sm_error = tw_sm_err;
        } else {
            rc = tw_st_idx;
        }
        NutExitCritical();
    }

    /*
     * Nothing to transmit.
     */
    else {
        uint8_t twcr;
        uint8_t twsr;
        rc = 0;
        /* Release the bus, not accepting SLA+R. */

        NutEnterCritical();
        twcr = inb(TWCR);
        twsr = inb(TWSR);
        /* Transmit start condition, if a master transfer is waiting. */
        if (tw_mt_len || tw_mr_siz) {
            outb(TWCR, TWGO | _BV(TWSTA));
        }
        /* Otherwise enter idle state. */
        else {
            tw_if_bsy = 0;
            outb(TWCR, TWGO);
        }

        NutExitCritical();
    }
#endif /* __AVR_ENHANCED__ */
    return rc;
}

/*!
 * \brief Get last slave mode error.
 *
 * You may call this function to determine the specific cause
 * of an error after TwSlaveListen() or TwSlaveRespond() failed.
 *
 * \note This function is only available on ATmega128 systems.
 *
 */
int TwSlaveError(void)
{
#ifndef __AVR_ENHANCED__
    return -1;
#else
    int rc = (int) tw_sm_error;
    tw_sm_error = 0;
    return rc;
#endif
}

/*!
 * \brief Perform TWI control functions.
 *
 * This function is only available on ATmega128 systems.
 *
 * \param req  Requested control function. May be set to one of the
 *	       following constants:
 *	       - TWI_SETSPEED, if conf points to an uint32_t value containing the bitrate.
 *	       - TWI_GETSPEED, if conf points to an uint32_t value receiving the current bitrate.
 * \param conf Points to a buffer that contains any data required for
 *	       the given control function or receives data from that
 *	       function.
 * \return 0 on success, -1 otherwise.
 *
 * \note Timeout is limited to the granularity of the system timer.
 *
 */
int TwIOCtl(int req, void *conf)
{
#ifndef __AVR_ENHANCED__
    return -1;
#else
    int rc = 0;
    uint32_t lval;

    switch (req) {
    case TWI_SETSLAVEADDRESS:
        TWAR = (*((uint8_t *) conf) << 1) | 1;
        break;
    case TWI_GETSLAVEADDRESS:
        *((uint8_t *) conf) = TWAR;
        break;

    case TWI_SETSPEED:
        lval = ((2UL * NutGetCpuClock() / (*((uint32_t *) conf)) + 1UL) / 2UL - 16UL) / 2UL;
        if (lval > 1020UL) {
            lval /= 16UL;
            sbi(TWSR, TWPS1);
        } else {
            cbi(TWSR, TWPS1);
        }
        if (lval > 255UL) {
            lval /= 4UL;
            sbi(TWSR, TWPS0);
        } else {
            cbi(TWSR, TWPS0);
        }
        if (lval > 9UL && lval < 256UL) {
            outb(TWBR, (uint8_t) lval);
        } else {
            rc = -1;
        }
        break;
    case TWI_GETSPEED:
        lval = 2UL;
        if (bit_is_set(TWSR, TWPS0)) {
            lval *= 4UL;
        }
        if (bit_is_set(TWSR, TWPS1)) {
            lval *= 16UL;
        }
        *((uint32_t *) conf) = NutGetCpuClock() / (16UL + lval * (uint32_t) inb(TWBR));
        break;

    case TWI_GETSTATUS:
        break;
    case TWI_SETSTATUS:
        break;

    default:
        rc = -1;
        break;
    }
    return rc;
#endif /* __AVR_ENHANCED__ */
}

/*!
 * \brief Initialize TWI interface.
 *
 * The specified slave address is used only, if the local system
 * is running as a slave. Anyway, care must be taken that it doesn't
 * conflict with another connected device.
 *
 * \note This function is only available on ATmega128 systems.
 *
 * \param sla Slave address, must be specified as a 7-bit address,
 *            always lower than 128.
 */
int TwInit(uint8_t sla)
{
#ifndef __AVR_ENHANCED__
    return -1;
#else
    uint32_t speed = 2400;

    if (NutRegisterIrqHandler(&sig_2WIRE_SERIAL, TwInterrupt, 0)) {
        return -1;
    }

    /*
     * Set address register, enable general call address, set transfer
     * speed and enable interface.
     */
    outb(TWAR, (sla << 1) | 1);
    TwIOCtl(TWI_SETSPEED, &speed);
    outb(TWCR, _BV(TWINT));
    outb(TWCR, _BV(TWEN) | _BV(TWIE));

    /*
     * Initialize mutex semaphores.
     */
    NutEventPost(&tw_mm_mutex);

    return 0;
#endif /* __AVR_ENHANCED__ */
}
