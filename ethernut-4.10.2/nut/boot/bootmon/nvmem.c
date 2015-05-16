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
 * $Id: nvmem.c 2935 2010-04-01 12:14:17Z haraldkipp $
 */

#include "utils.h"
#include "twbbi.h"
#include "npl.h"
#include "nvmem.h"

#ifndef EEPROM_PAGE_SIZE
#define EEPROM_PAGE_SIZE    64
#endif

#ifndef I2C_SLA_RTC
#define I2C_SLA_RTC     0x6F
#endif

#ifndef I2C_SLA_EEPROM
#define I2C_SLA_EEPROM  0x57
#endif

#ifndef I2C_SLA_PLL
#define I2C_SLA_PLL     0x69
#endif

#define DFCMD_BUF1_FLASH        0x83
#define DFCMD_BUF1_WRITE        0x84
#define DFCMD_READ_STATUS       0xD7
#define DFCMD_CONT_READ         0xE8

#define SPI_WRITE       1
#define SPI_READ        2

static int at45d_avail;

#ifndef NVMEM_BUFF_SIZE
#ifdef AT45D_CONF
#define NVMEM_BUFF_SIZE     528
#else
#define NVMEM_BUFF_SIZE     128
#endif
#endif

static unsigned char page_buf[NVMEM_BUFF_SIZE];

#ifdef AT45D_CONF
static int NplSpiTransfer(int dir, unsigned char *ptr, int len)
{
    int cnt = 20000;
    unsigned char b;

    while (len--) {
        outb(NPL_MMCDR, *ptr);
        while ((inb(NPL_SLR) & NPL_MMCREADY) == 0) {
            if (cnt-- <= 0) {
                return -1;
            }
        }
        b = inb(NPL_MMCDR);
        if (dir & SPI_READ) {
            *ptr = b;
        }
        ptr++;
    }
    return 0;
}

static unsigned char At45dStatus(void)
{
    unsigned char cmd[2] = { DFCMD_READ_STATUS, 0xFF };

    outb(NPL_XER, inb(NPL_XER) & ~NPL_NPCS0);
    NplSpiTransfer(SPI_WRITE | SPI_READ, cmd, 2);
    outb(NPL_XER, inb(NPL_XER) | NPL_NPCS0);

    return cmd[1];
}

static int At45dCommand(unsigned char op, unsigned long parm, int oplen, unsigned char *xptr, int xlen)
{
    int rc = -1;
    unsigned char cmd[8];

    memset_(cmd, 0, oplen);
    cmd[0] = op;
    if (parm) {
        cmd[1] = (unsigned char) (parm >> 16);
        cmd[2] = (unsigned char) (parm >> 8);
        cmd[3] = (unsigned char) parm;
    }

    outb(NPL_XER, inb(NPL_XER) & ~NPL_NPCS0);
    rc = NplSpiTransfer(SPI_WRITE, cmd, oplen);
    if (rc == 0) {
        if (xlen > 0) {
            rc = NplSpiTransfer(SPI_WRITE, xptr, xlen);
        } else if (xlen < 0) {
            rc = NplSpiTransfer(SPI_READ, xptr, -xlen);
        }
    }
    outb(NPL_XER, inb(NPL_XER) | NPL_NPCS0);

    return rc;
}
#else
static int NvMemWriteEnable(int on)
{
    int rc;
    unsigned char buf[3];

    buf[0] = 0;
    buf[1] = 0x3F;
    if (on) {
        buf[2] = 0x02;
        if ((rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, 0)) == 0) {
            buf[2] = 0x06;
            rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, 0);
        }
    } else {
        buf[2] = 0x00;
        rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, 0);
    }
    return rc;
}

static int NvMemWaitReady(void)
{
    unsigned char poll;
    int cnt = 20;

    /* Poll for write cycle finished. */
    while (--cnt && TwMasterTransact(I2C_SLA_EEPROM, 0, 0, &poll, 1, 0) == -1) {
        Delay(1);
    }
    return cnt ? 0 : -1;
}
#endif


/*!
 * \brief Read contents from non-volatile EEPROM.
 *
 * \param addr  Start location.
 * \param buff  Points to a buffer that receives the contents.
 * \param len   Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int NvMemRead(unsigned int addr, void *buff, unsigned int len)
{
    int rc = -1;

#ifdef AT45D_CONF
    if (at45d_avail) {
        memcpy_(buff, &page_buf[addr], len);
        rc = 0;
    }
#else
    unsigned char param[2];

    param[0] = (unsigned char) (addr >> 8);
    param[1] = (unsigned char) addr;
    if (TwMasterTransact(I2C_SLA_EEPROM, param, 2, buff, len, 0) == len) {
        rc = 0;
    }
#endif
    return rc;
}

/*!
 * \brief Store buffer contents in non-volatile EEPROM.
 *
 * The EEPROM of the X122x has a capacity of 512 bytes, while the X1286 is
 * able to store 32 kBytes.
 *
 * \param addr  Storage start location.
 * \param buff  Points to a buffer that contains the bytes to store.
 * \param len   Number of valid bytes in the buffer.
 *
 * \return 0 on success or -1 in case of an error.
 */
int NvMemWrite(unsigned int addr, void *buff, unsigned int len)
{
    int rc = 0;

#ifdef AT45D_CONF
    if (at45d_avail) {
        memcpy_(&page_buf[addr], buff, len);
        if (At45dCommand(DFCMD_BUF1_WRITE, 0, 4, page_buf, sizeof(page_buf))) {
            rc = -1;
        }
        else if (At45dCommand(DFCMD_BUF1_FLASH, 8191 << 10, 4, 0, 0)) {
            rc = -1;
        }
        Delay(10);
    } else {
        rc = -1;
    }
#else
    unsigned int wlen;
    unsigned char *wp = buff;

    /*
     * Loop for each page to be written to.
     */
    while (len) {
        /* Do not cross page boundaries. */
        wlen = EEPROM_PAGE_SIZE - (addr & (EEPROM_PAGE_SIZE - 1));
        if (wlen > len) {
            wlen = len;
        }

        /* Set a TWI write buffer. */
        page_buf[0] = (unsigned char) (addr >> 8);
        page_buf[1] = (unsigned char) addr;
        memcpy_(page_buf + 2, wp, wlen);

        /* Enable EEPROM write access and send the write buffer. */
        if ((rc = NvMemWriteEnable(1)) == 0) {
            rc = TwMasterTransact(I2C_SLA_EEPROM, page_buf, wlen + 2, 0, 0, 0);
        }

        /* Check the result. */
        if (rc) {
            break;
        }
        len -= wlen;
        addr += wlen;
        wp += wlen;

        /* Poll for write cycle finished. */
        if ((rc = NvMemWaitReady()) != 0) {
            break;
        }
    }
    NvMemWriteEnable(0);
#endif

    return rc;
}

/*!
 * \brief Initialize the interface to serial EEPROM.
 */
void NvMemInit(void)
{
    TwInit();

#ifdef AT45D_CONF
    {
        unsigned char reg[2];

        reg[0] = 0x09;
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, 0) == 1) {
            reg[1] &= ~0x7F;
            reg[1] |= 2;
            TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, 0);
        }
        reg[0] = 0x08;
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, 0) == 1) {
            reg[1] &= ~0x7F;
            reg[1] |= 2;
            TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, 0);
        }

        /* AT45DB0321B is supported only. */
        if ((At45dStatus() & 0x3D) == 0x34) {
            if (At45dCommand(DFCMD_CONT_READ, 8191 << 10, 8, page_buf, -((int)sizeof(page_buf))) == 0) {
                at45d_avail = 1;
            }
        }
    }
#endif
}
