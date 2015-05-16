/*
 * Copyright (C) 2005-2007 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file dev/twbbif.c
 * \brief Bit banging two wire interface.
 *
 * Tested with AT91R40008 only. Other ARMs or AVR may not work.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.11  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2008/02/15 17:04:49  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.9  2007/10/04 20:16:57  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.8  2007/07/09 13:38:50  olereinhardt
 * 2007-07-09  Ole Reinhardt <ole.reinhardt@embedded-it.de>
 *         * dev/twbbif.c: Added default TWI_DELAY for avr architecture
 *         to allow in-tree compilation
 *         * net/Makefile: Commented out igmp support as long as it does
 *         not exists to allow in-tree compilation
 *
 * Revision 1.7  2007/02/15 16:25:40  haraldkipp
 * Configurable port bits. Should work now on all AT91 MCUs.
 *
 * Revision 1.6  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.5  2006/06/28 17:10:35  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.4  2006/05/25 09:30:23  haraldkipp
 * Compiles for AVR. Still not tested, though.
 *
 * Revision 1.3  2006/04/07 12:50:20  haraldkipp
 * Added additional delays in TwAck.
 * Clock and data forced to high before switching to input gives better
 * waveforms.
 * Additional delay added to set clock to 50% duty cycle. The PCF8563
 * seems to fail, if the clock's high time is much longer than the low time.
 *
 * Revision 1.2  2006/01/23 17:32:35  haraldkipp
 * Automatic initialization added.
 *
 * Revision 1.1  2005/10/24 10:21:57  haraldkipp
 * Initial check in.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/twi.h>
#include <cfg/arch/gpio.h>

#include <dev/twif.h>

#if defined(__arm__)

#include <arch/arm.h>

/*!
 * \brief GPIO controller ID.
 *
 * Target specific defaults are set if the ID is not specified elsewhere.
 */
#if !defined(TWI_PIO_ID)
#if defined(MCU_AT91SAM7X) || defined(MCU_AT91SAM7S256) || defined(MCU_AT91SAM7SE512)
#define TWI_PIO_ID  PIOA_ID
#elif defined(MCU_AT91SAM9260)
#define TWI_PIO_ID  PIOB_ID
#else
#define TWI_PIO_ID  PIO_ID
#endif
#endif

/*!
 * \brief GPIO bit of bit banging TWI data line.
 *
 * Target specific defaults are used if the bit is not specified elsewhere.
 */
#ifndef TWI_SDA_BIT
#if defined(MCU_AT91SAM9260)
#define TWI_SDA_BIT     12
#else
#define TWI_SDA_BIT     10
#endif
#endif

/*!
 * \brief Clock delay.
 */
#ifndef TWI_DELAY
#if defined(MCU_AT91SAM9260)
#define TWI_DELAY   16
#else
#define TWI_DELAY   8
#endif
#endif

/*!
 * \brief GPIO bit of bit banging TWI clock line.
 *
 * Target specific defaults are used if the bit is not specified elsewhere.
 */
#ifndef TWI_SCL_BIT
#if defined(MCU_AT91SAM9260)
#define TWI_SCL_BIT     13
#else
#define TWI_SCL_BIT     11
#endif
#endif

/*!
 * \brief Bit banging TWI GPIO registers.
 *
 * Based on the GPIO controller ID.
 */
#if TWI_PIO_ID == PIOA_ID

#ifndef TWI_SDA_PE_REG
#define TWI_SDA_PE_REG  PIOA_PER
#endif
#ifndef TWI_SDA_OE_REG
#define TWI_SDA_OE_REG  PIOA_OER
#endif
#ifndef TWI_SDA_OD_REG
#define TWI_SDA_OD_REG  PIOA_ODR
#endif
#ifndef TWI_SDA_COD_REG
#define TWI_SDA_COD_REG PIOA_CODR
#endif
#ifndef TWI_SDA_SOD_REG
#define TWI_SDA_SOD_REG PIOA_SODR
#endif
#ifndef TWI_SDA_PDS_REG
#define TWI_SDA_PDS_REG PIOA_PDSR
#endif

#ifndef TWI_SCL_PE_REG
#define TWI_SCL_PE_REG  PIOA_PER
#endif
#ifndef TWI_SCL_OE_REG
#define TWI_SCL_OE_REG  PIOA_OER
#endif
#ifndef TWI_SCL_OD_REG
#define TWI_SCL_OD_REG  PIOA_ODR
#endif
#ifndef TWI_SCL_COD_REG
#define TWI_SCL_COD_REG PIOA_CODR
#endif
#ifndef TWI_SCL_SOD_REG
#define TWI_SCL_SOD_REG PIOA_SODR
#endif
#ifndef TWI_SCL_PDS_REG
#define TWI_SCL_PDS_REG PIOA_PDSR
#endif

#elif TWI_PIO_ID == PIOB_ID

#ifndef TWI_SDA_PE_REG
#define TWI_SDA_PE_REG  PIOB_PER
#endif
#ifndef TWI_SDA_OE_REG
#define TWI_SDA_OE_REG  PIOB_OER
#endif
#ifndef TWI_SDA_OD_REG
#define TWI_SDA_OD_REG  PIOB_ODR
#endif
#ifndef TWI_SDA_COD_REG
#define TWI_SDA_COD_REG PIOB_CODR
#endif
#ifndef TWI_SDA_SOD_REG
#define TWI_SDA_SOD_REG PIOB_SODR
#endif
#ifndef TWI_SDA_PDS_REG
#define TWI_SDA_PDS_REG PIOB_PDSR
#endif

#ifndef TWI_SCL_PE_REG
#define TWI_SCL_PE_REG  PIOB_PER
#endif
#ifndef TWI_SCL_OE_REG
#define TWI_SCL_OE_REG  PIOB_OER
#endif
#ifndef TWI_SCL_OD_REG
#define TWI_SCL_OD_REG  PIOB_ODR
#endif
#ifndef TWI_SCL_COD_REG
#define TWI_SCL_COD_REG PIOB_CODR
#endif
#ifndef TWI_SCL_SOD_REG
#define TWI_SCL_SOD_REG PIOB_SODR
#endif
#ifndef TWI_SCL_PDS_REG
#define TWI_SCL_PDS_REG PIOB_PDSR
#endif

#elif TWI_PIO_ID == PIOC_ID

#ifndef TWI_SDA_PE_REG
#define TWI_SDA_PE_REG  PIOC_PER
#endif
#ifndef TWI_SDA_OE_REG
#define TWI_SDA_OE_REG  PIOC_OER
#endif
#ifndef TWI_SDA_OD_REG
#define TWI_SDA_OD_REG  PIOC_ODR
#endif
#ifndef TWI_SDA_COD_REG
#define TWI_SDA_COD_REG PIOC_CODR
#endif
#ifndef TWI_SDA_SOD_REG
#define TWI_SDA_SOD_REG PIOC_SODR
#endif
#ifndef TWI_SDA_PDS_REG
#define TWI_SDA_PDS_REG PIOC_PDSR
#endif

#ifndef TWI_SCL_PE_REG
#define TWI_SCL_PE_REG  PIOC_PER
#endif
#ifndef TWI_SCL_OE_REG
#define TWI_SCL_OE_REG  PIOC_OER
#endif
#ifndef TWI_SCL_OD_REG
#define TWI_SCL_OD_REG  PIOC_ODR
#endif
#ifndef TWI_SCL_COD_REG
#define TWI_SCL_COD_REG PIOC_CODR
#endif
#ifndef TWI_SCL_SOD_REG
#define TWI_SCL_SOD_REG PIOC_SODR
#endif
#ifndef TWI_SCL_PDS_REG
#define TWI_SCL_PDS_REG PIOC_PDSR
#endif

#else

#ifndef TWI_SDA_PE_REG
#define TWI_SDA_PE_REG  PIO_PER
#endif
#ifndef TWI_SDA_OE_REG
#define TWI_SDA_OE_REG  PIO_OER
#endif
#ifndef TWI_SDA_OD_REG
#define TWI_SDA_OD_REG  PIO_ODR
#endif
#ifndef TWI_SDA_COD_REG
#define TWI_SDA_COD_REG PIO_CODR
#endif
#ifndef TWI_SDA_SOD_REG
#define TWI_SDA_SOD_REG PIO_SODR
#endif
#ifndef TWI_SDA_PDS_REG
#define TWI_SDA_PDS_REG PIO_PDSR
#endif

#ifndef TWI_SCL_PE_REG
#define TWI_SCL_PE_REG  PIO_PER
#endif
#ifndef TWI_SCL_OE_REG
#define TWI_SCL_OE_REG  PIO_OER
#endif
#ifndef TWI_SCL_OD_REG
#define TWI_SCL_OD_REG  PIO_ODR
#endif
#ifndef TWI_SCL_COD_REG
#define TWI_SCL_COD_REG PIO_CODR
#endif
#ifndef TWI_SCL_SOD_REG
#define TWI_SCL_SOD_REG PIO_SODR
#endif
#ifndef TWI_SCL_PDS_REG
#define TWI_SCL_PDS_REG PIO_PDSR
#endif

#endif

#define TWI_ENABLE() { \
    outr(TWI_SDA_COD_REG, _BV(TWI_SDA_BIT)); \
    outr(TWI_SCL_COD_REG, _BV(TWI_SCL_BIT)); \
    outr(TWI_SDA_PE_REG, _BV(TWI_SDA_BIT)); \
    outr(TWI_SCL_PE_REG, _BV(TWI_SCL_BIT)); \
}

#define SDA_LOW() { \
    outr(TWI_SDA_COD_REG, _BV(TWI_SDA_BIT)); \
    outr(TWI_SDA_OE_REG, _BV(TWI_SDA_BIT)); \
}

#define SDA_HIGH() { \
    outr(TWI_SDA_SOD_REG, _BV(TWI_SDA_BIT)); \
    outr(TWI_SDA_OD_REG, _BV(TWI_SDA_BIT)); \
}

#define SDA_STAT()      (inr(TWI_SDA_PDS_REG) & _BV(TWI_SDA_BIT))

#define SCL_LOW() { \
    outr(TWI_SCL_COD_REG, _BV(TWI_SCL_BIT)); \
    outr(TWI_SCL_OE_REG, _BV(TWI_SCL_BIT)); \
}

#define SCL_HIGH() { \
    outr(TWI_SCL_SOD_REG, _BV(TWI_SCL_BIT)); \
    outr(TWI_SCL_OD_REG, _BV(TWI_SCL_BIT)); \
}

#elif defined(__AVR__)

/*
 * AVR not yet tested.
 */
#include <cfg/arch/avr.h>

#ifndef TWI_SDA_BIT
#define TWI_SDA_BIT     0
#endif

#if (TWI_SDA_AVRPORT == AVRPORTD)
#define TWI_SDA_PORT    PORTD
#define TWI_SDA_PIN     PIND
#define TWI_SDA_DDR     DDRD
#elif (TWI_SDA_AVRPORT == AVRPORTE)
#define TWI_SDA_PORT    PORTE
#define TWI_SDA_PIN     PINE
#define TWI_SDA_DDR     DDRE
#elif (TWI_SDA_AVRPORT == AVRPORTF)
#define TWI_SDA_PORT    PORTF
#define TWI_SDA_PIN     PINF
#define TWI_SDA_DDR     DDRF
#else
#define TWI_SDA_PORT    PORTB
#define TWI_SDA_PIN     PINB
#define TWI_SDA_DDR     DDRB
#endif

#ifndef TWI_SCL_BIT
#define TWI_SCL_BIT     1
#endif

#if (TWI_SCL_AVRPORT == AVRPORTD)
#define TWI_SCL_PORT    PORTD
#define TWI_SCL_DDR     DDRD
#elif (TWI_SCL_AVRPORT == AVRPORTE)
#define TWI_SCL_PORT    PORTE
#define TWI_SCL_DDR     DDRE
#elif (TWI_SCL_AVRPORT == AVRPORTF)
#define TWI_SCL_PORT    PORTF
#define TWI_SCL_DDR     DDRF
#else
#define TWI_SCL_PORT    PORTB
#define TWI_SCL_DDR     DDRB
#endif

#define TWI_ENABLE() {              \
    cbi(TWI_SDA_PORT, TWI_SDA_BIT); \
    cbi(TWI_SCL_PORT, TWI_SCL_BIT); \
}

#define SDA_LOW()   sbi(TWI_SDA_DDR, TWI_SDA_BIT)
#define SDA_HIGH()  cbi(TWI_SDA_DDR, TWI_SDA_BIT)
#define SDA_STAT()  bit_is_set(TWI_SDA_PIN, TWI_SDA_BIT)

#define SCL_LOW()   sbi(TWI_SCL_DDR, TWI_SCL_BIT)
#define SCL_HIGH()  cbi(TWI_SCL_DDR, TWI_SCL_BIT)

#ifndef TWI_DELAY
#define TWI_DELAY   8
#endif

#endif                          /* __AVR__ */


static uint8_t tw_mm_error;      /* Last master mode error. */
static int twibb_initialized;

/*
 * Short delay. 
 * 
 * Our bit banging code relies on pull-up resistors. The I/O ports mimic
 * open collector outputs by switching to input mode for high level and
 * switching to output mode for low level. This is much slower than
 * switching an output between low to high. Thus we need some delay.
 */
static void TwDelay(int nops)
{
    while (nops--) {
        _NOP();
    }
}

/*
 * Falling edge on the data line while the clock line is high indicates
 * a start condition.
 *
 * Entry: SCL any, SDA any
 * Exit: SCL low, SDA low
 */
static void TwStart(void)
{
    SDA_HIGH();
    TwDelay(TWI_DELAY);
    SCL_HIGH();
    TwDelay(TWI_DELAY);
    SDA_LOW();
    TwDelay(TWI_DELAY);
    SCL_LOW();
    TwDelay(TWI_DELAY);
}

/*
 * Rising edge on the data line while the clock line is high indicates
 * a stop condition.
 *
 * Entry: SCL low, SDA any
 * Exit: SCL high, SDA high
 */
static void TwStop(void)
{
    SDA_LOW();
    TwDelay(TWI_DELAY);
    SCL_HIGH();
    TwDelay(2 * TWI_DELAY);
    SDA_HIGH();
    TwDelay(8 * TWI_DELAY);
}

/*
 * Toggles out a single byte in master mode.
 *
 * Entry: SCL low, SDA any
 * Exit: SCL low, SDA high
 */
static int TwPut(uint8_t octet)
{
    int i;

    for (i = 0x80; i; i >>= 1) {
        /* Set the data bit. */
        if (octet & i) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        /* Wait for data to stabelize. */
        TwDelay(TWI_DELAY);
        /* Toggle the clock. */
        SCL_HIGH();
        TwDelay(2 * TWI_DELAY);
        SCL_LOW();
        TwDelay(TWI_DELAY);
    }

    /* Set data line high to receive the ACK bit. */
    SDA_HIGH();

    /* ACK should appear shortly after the clock's rising edge. */
    SCL_HIGH();
    TwDelay(2 * TWI_DELAY);
    if (SDA_STAT()) {
        i = -1;
    } else {
        i = 0;
    }
    SCL_LOW();

    return i;
}

/*
 * Toggles in a single byte in master mode.
 *
 * Entry: SCL low, SDA any
 * Exit: SCL low, SDA high
 */
static uint8_t TwGet(void)
{
    uint8_t rc = 0;
    int i;

    /* SDA is input. */
    SDA_HIGH();
    TwDelay(TWI_DELAY);
    for (i = 0x80; i; i >>= 1) {
        TwDelay(TWI_DELAY);
        /* Data should appear shortly after the clock's rising edge. */
        SCL_HIGH();
        TwDelay(2 * TWI_DELAY);
        /* SDA read. */
        if (SDA_STAT()) {
            rc |= i;
        }
        SCL_LOW();
    }
    return rc;
}

/*
 * Toggles out an acknowledge bit in master mode.
 *
 * Entry: SCL low, SDA any
 * Exit: SCL low, SDA high
 */
static void TwAck(void)
{
    SDA_LOW();
    TwDelay(TWI_DELAY);
    SCL_HIGH();
    TwDelay(2 * TWI_DELAY);
    SCL_LOW();
    TwDelay(TWI_DELAY);
    SDA_HIGH();
}

/*!
 * \brief Transmit and/or receive data as a master.
 *
 * The two-wire serial interface must have been initialized by calling
 * TwInit() before this function can be used.
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
 *
 * \note Timeout is not used in the bit banging version.
 */
int TwMasterTransact(uint8_t sla, CONST void *txdata, uint16_t txlen, void *rxdata, uint16_t rxsiz, uint32_t tmo)
{
    int rc = 0;
    uint8_t *cp;

    if (!twibb_initialized) {
        TwInit(0);
    }

    if (txlen) {
        TwStart();
        /* Send SLA+W and check for ACK. */
        if ((rc = TwPut(sla << 1)) == 0) {
            for (cp = (uint8_t *)txdata; txlen--; cp++) {
                if ((rc = TwPut(*cp)) != 0) {
                    break;
                }
            }
        }
    }
    if (rc == 0 && rxsiz) {
        TwStart();
        /* Send SLA+R and check for ACK. */
        if ((rc = TwPut((sla << 1) | 1)) == 0) {
            for (cp = rxdata;; cp++) {
                *cp = TwGet();
                if (++rc >= rxsiz) {
                    break;
                }
                TwAck();
            }
        }
    }
    TwStop();

    if (rc == -1) {
        tw_mm_error = TWERR_SLA_NACK;
    }
    return rc;
}

/*!
 * \brief Get last master mode error.
 *
 * You may call this function to determine the specific cause
 * of an error after TwMasterTransact() failed.
 *
 * \return Error code or 0 if no error occurred.
 */
int TwMasterError(void)
{
    int rc = (int) tw_mm_error;
    tw_mm_error = 0;

    return rc;
}

/*!
 * \brief Listen for incoming data from a master.
 *
 * If this function returns without error, the bus is blocked. The caller
 * must immediately process the request and return a response by calling
 * TwSlaveRespond().
 *
 * \note Slave mode is not implemented in the bit banging version.
 *       Thus the function always returns -1.
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
 *
 */
int TwSlaveListen(uint8_t * sla, void *rxdata, uint16_t rxsiz, uint32_t tmo)
{
    return -1;
}

/*!
 * \brief Send response to a master.
 *
 * This function must be called as soon as possible after TwSlaveListen()
 * returned successfully, even if no data needs to be returned. Not doing
 * so will completely block the bus.
 *
 * \note Slave mode is not implemented in the bit banging version.
 *       Thus the function always returns -1.
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
    return -1;
}

/*!
 * \brief Get last slave mode error.
 *
 * You may call this function to determine the specific cause
 * of an error after TwSlaveListen() or TwSlaveRespond() failed.
 *
 * \return Error code or 0 if no error occurred.
 *
 * \note Slave mode is not implemented in the bit banging version.
 *       Thus the function always returns TWERR_BUS.
 */
int TwSlaveError(void)
{
    return TWERR_BUS;
}

/*!
 * \brief Perform TWI control functions.
 *
 * Not implemented in the bit banging version.
 *
 * \param req  Requested control function.
 * \param conf Points to a buffer that contains any data required for
 *	       the given control function or receives data from that
 *	       function.
 *
 * \return Always 0.
 */
int TwIOCtl(int req, void *conf)
{
    return 0;
}

/*!
 * \brief Initialize TWI interface.
 *
 * The specified slave address is used only, if the local system
 * is running as a slave. Anyway, care must be taken that it doesn't
 * conflict with another connected device.
 *
 * \param sla Slave address, must be specified as a 7-bit address,
 *            always lower than 128.
 *
 * \return Always 0.
 *
 * \note Slave mode is not implemented in the bit banging version.
 *       Thus the given slave address is ignored.
 */
int TwInit(uint8_t sla)
{
    SDA_HIGH();
    SCL_HIGH();
    TWI_ENABLE();
    twibb_initialized = 1;

    return 0;
}
