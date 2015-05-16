/*
 * Copyright (C) 2005-2007 by egnite Software GmbH
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
 *
 */

/*
 * $Id: twbbi.c 2935 2010-04-01 12:14:17Z haraldkipp $
 */

#include "utils.h"
#include "dialog.h"
#include "twbbi.h"

#define PIO_BASE    0xFFFF0000  /*!< \brief PIO base address. */
#define PIO_PER     (PIO_BASE + 0x00)   /*!< \brief PIO enable register. */
#define PIO_OER     (PIO_BASE + 0x10)   /*!< \brief Output enable register. */
#define PIO_ODR     (PIO_BASE + 0x14)   /*!< \brief Output disable register. */
#define PIO_CODR    (PIO_BASE + 0x34)   /*!< \brief Clear output data register. */
#define PIO_PDSR    (PIO_BASE + 0x3C)   /*!< \brief Pin data status register. */

#ifndef TWI_SDA_BIT
#define TWI_SDA_BIT     16
#endif
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
#ifndef TWI_SDA_PDS_REG
#define TWI_SDA_PDS_REG PIO_PDSR
#endif

#ifndef TWI_SCL_BIT
#define TWI_SCL_BIT     17
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

#define outr(_reg, _val)  (*((volatile unsigned int *)(_reg)) = (_val))
#define inr(_reg)   (*((volatile unsigned int *)(_reg)))
#define _BV(bit)    (1 << bit)

#ifndef TWI_DELAY
#define TWI_DELAY   8
#endif

#define SDA_LOW()       outr(TWI_SDA_OE_REG, _BV(TWI_SDA_BIT))
#define SDA_HIGH()      outr(TWI_SDA_OD_REG, _BV(TWI_SDA_BIT))
#define SDA_STAT()      (inr(TWI_SDA_PDS_REG) & _BV(TWI_SDA_BIT))

#define SCL_LOW()       outr(TWI_SCL_OE_REG, _BV(TWI_SCL_BIT))
#define SCL_HIGH()      outr(TWI_SCL_OD_REG, _BV(TWI_SCL_BIT))

static void TwDelay(int nops)
{
    while (nops--) {
        _NOP();
    }
}

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

static void TwStop(void)
{
    SDA_LOW();
    TwDelay(TWI_DELAY);
    SCL_HIGH();
    TwDelay(2 * TWI_DELAY);
    SDA_HIGH();
    TwDelay(8 * TWI_DELAY);
}

static void TwAck(void)
{
    SDA_LOW();
    SCL_HIGH();
    TwDelay(2 * TWI_DELAY);
    SCL_LOW();
    SDA_HIGH();
}

static int TwPut(unsigned char octet)
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

static unsigned char TwGet(void)
{
    unsigned char rc = 0;
    int i;

    /* SDA is input. */
    SDA_HIGH();

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

int TwMasterTransact(unsigned char sla, void *txdata, unsigned short txlen, void *rxdata, unsigned short rxsiz, unsigned long tmo)
{
    int rc = 0;
    unsigned char *cp;


    if (txlen) {
        TwStart();
        /* Send SLA+W and check for ACK. */
        if ((rc = TwPut(sla << 1)) == 0) {
            for (cp = (unsigned char *) txdata; txlen--; cp++) {
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

    return rc;
}

void TwInit(void)
{
    SDA_HIGH();
    SCL_HIGH();

    outr(TWI_SDA_COD_REG, _BV(TWI_SDA_BIT));
    outr(TWI_SCL_COD_REG, _BV(TWI_SCL_BIT));
    outr(TWI_SDA_PE_REG, _BV(TWI_SDA_BIT));
    outr(TWI_SCL_PE_REG, _BV(TWI_SCL_BIT));
}
