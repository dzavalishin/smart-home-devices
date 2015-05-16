/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
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

/*!
 * \file dev/at45db.c
 * \brief Routines for Atmel AT45 serial dataflash memory chips.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.91 2010/12/15 15:20:42  ve2yag
 * Added MCU_AT91SAM7S family.
 *
 * Revision 1.9  2009/02/06 15:53:42  haraldkipp
 * Corrected a bug with non-negated chip selects.
 *
 * Revision 1.8  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.7  2008/12/15 19:18:49  haraldkipp
 * Enable DataFlash support for EIR board.
 *
 * Revision 1.6  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2008/08/06 12:51:09  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.4  2008/02/15 17:10:43  haraldkipp
 * At45dbPageErase selected the wrong bank. Fixed. Parameter pgn (page number)
 * of At45dbPageWrite() changed from unsigned int to unsigned long.
 * New routines At45dbPages() and At45dbPageSize() allow to determine the
 * chip's layout.
 *
 * Revision 1.3  2007/08/17 10:45:21  haraldkipp
 * Enhanced documentation.
 *
 * Revision 1.2  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.1  2006/09/29 12:41:55  haraldkipp
 * Added support for AT45 serial DataFlash memory chips. Currently limited
 * to AT91 builds.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <sys/timer.h>

#include <string.h>
#include <stdlib.h>

#include <dev/at91_spi.h>
#include <dev/at45db.h>

#ifndef MAX_AT45_DEVICES
#define MAX_AT45_DEVICES        1
#endif

#ifndef MAX_AT45_CMDLEN
#define MAX_AT45_CMDLEN         8
#endif

#ifndef AT45_CONF_DFSPI
#define AT45_CONF_DFSPI         SPI0_BASE
#endif

#ifndef AT45_CONF_DFPCS
#define AT45_CONF_DFPCS         1
#endif

#ifndef AT45_ERASE_WAIT
#define AT45_ERASE_WAIT         3000
#endif

#ifndef AT45_CHIP_ERASE_WAIT
#define AT45_CHIP_ERASE_WAIT    50000
#endif

#ifndef AT45_WRITE_POLLS
#define AT45_WRITE_POLLS        1000
#endif

/*!
 * \name AT45 DataFlash Commands
 */
/*@{*/
/*! \brief Continuos read (low frequency). 
 *
 * Reads a continous stream in low speed mode. Automatically switches 
 * to the the page and wraps to the first page after the last has been 
 * read. Note, that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ_LF      0x03
/*! \brief Continuos read (high frequency).
 *
 * Reads a continous stream in high speed mode. Automatically switches 
 * to the the page and wraps to the first page after the last has been 
 * read. Note, that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ_HF      0x0B
/*! \brief Block erase. 
 */
#define DFCMD_BLOCK_ERASE       0x50
/*! \brief Sector erase. 
 */
#define DFCMD_SECTOR_ERASE      0x7C
/*! \brief Page erase. 
 */
#define DFCMD_PAGE_ERASE        0x81
/*! \brief Main memory page program through buffer 1. 
 */
#define DFCMD_BUF1_PROG         0x82
/*! \brief Buffer 1 flash with page erase. 
 */
#define DFCMD_BUF1_FLASH        0x83
/*! \brief Buffer 1 write. 
 */
#define DFCMD_BUF1_WRITE        0x84
/*! \brief Main memory page program through buffer 2.
 */
#define DFCMD_BUF2_PROG         0x85
/*! \brief Buffer 2 flash with page erase. 
 */
#define DFCMD_BUF2_FLASH        0x86
/*! \brief Buffer 2 write. 
 */
#define DFCMD_BUF2_WRITE        0x87
/*! \brief Buffer 1 flash without page erase. 
 */
#define DFCMD_BUF1_FLASH_NE     0x88
/*! \brief Buffer 2 flash without page erase. 
 */
#define DFCMD_BUF2_FLASH_NE     0x89
/*! \brief Chip erase. 
 */
#define DFCMD_CHIP_ERASE        0xC7
/*! \brief Buffer 1 read (low frequency).
 */
#define DFCMD_BUF1_READ_LF      0xD1
/*! \brief Read main memory page.
 *
 * Automatically wraps to the first byte of the same page after
 * the last byte had been read. The data buffers are left unchanged.
 */
#define DFCMD_READ_PAGE         0xD2
/*! \brief Buffer 2 read (low frequency).
 */
#define DFCMD_BUF2_READ_LF      0xD3
/*! \brief Buffer 1 read.
 */
#define DFCMD_BUF1_READ         0xD4
/*! \brief Buffer 2 read.
 */
#define DFCMD_BUF2_READ         0xD6
/*! \brief Read status register. 
 */
#define DFCMD_READ_STATUS       0xD7
/*! \brief Continuos read (legacy).
 *
 * Reads a continous stream. Automatically switches to the the page
 * and wraps to the first page after the last has been read. Note,
 * that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ         0xE8
/*@}*/

#define AT45DB_AT91


#if defined(AT45DB_SPI0_DEVICE)

#include <dev/sppif0.h>
#if defined(AT45DB_RESET_ACTIVE_HIGH)
#define SpiReset(act)       Sppi0ChipReset(AT45DB_SPI0_DEVICE, act)
#else
#define SpiReset(act)       Sppi0ChipReset(AT45DB_SPI0_DEVICE, !act)
#endif
#define SpiSetMode()        Sppi0SetMode(AT45DB_SPI0_DEVICE, AT45DB_SPI_MODE)
#define SpiSetSpeed()       Sppi0SetSpeed(AT45DB_SPI0_DEVICE, AT45DB_SPI_RATE)
#if defined(AT45DB_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sppi0SelectDevice(AT45DB_SPI0_DEVICE)
#define SpiDeselect()       Sppi0DeselectDevice(AT45DB_SPI0_DEVICE)
#else
#define SpiSelect()         Sppi0NegSelectDevice(AT45DB_SPI0_DEVICE)
#define SpiDeselect()       Sppi0NegDeselectDevice(AT45DB_SPI0_DEVICE)
#endif
#define SpiByte             Sppi0Byte

#elif defined(AT45DB_SBBI0_DEVICE)

#include <dev/sbbif0.h>
#if defined(AT45DB_RESET_ACTIVE_HIGH)
#define SpiReset(act)       Sbbi0ChipReset(AT45DB_SBBI0_DEVICE, act)
#else
#define SpiReset(act)       Sbbi0ChipReset(AT45DB_SBBI0_DEVICE, !act)
#endif
#define SpiSetMode()        Sbbi0SetMode(AT45DB_SBBI0_DEVICE, AT45DB_SPI_MODE)
#define SpiSetSpeed()       Sbbi0SetSpeed(AT45DB_SBBI0_DEVICE, AT45DB_SPI_RATE)
#if defined(AT45DB_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sbbi0SelectDevice(AT45DB_SBBI0_DEVICE)
#define SpiDeselect()       Sbbi0DeselectDevice(AT45DB_SBBI0_DEVICE)
#else
#define SpiSelect()         Sbbi0NegSelectDevice(AT45DB_SBBI0_DEVICE)
#define SpiDeselect()       Sbbi0NegDeselectDevice(AT45DB_SBBI0_DEVICE)
#endif
#define SpiByte             Sbbi0Byte

#endif

/*!
 * \brief Known device type entry.
 */
typedef struct _AT45_DEVTAB {
    uint32_t devt_pages;
    unsigned int devt_pagsiz;
    unsigned int devt_offs;
    uint8_t devt_srmsk;
    uint8_t devt_srval;
} AT45_DEVTAB;

/*!
 * \brief Active device entry.
 */
typedef struct _AT45DB_DCB {
    AT45_DEVTAB *dcb_devt;
    unsigned int dcb_spibas;
    unsigned int dcb_spipcs;
    uint8_t dcb_cmdbuf[MAX_AT45_CMDLEN];
} AT45DB_DCB;

/*!
 * \brief Table of known Dataflash types.
 */
AT45_DEVTAB at45_devt[] = {
    {512, 264, 9, 0x3C, 0x0C},  /* AT45DB011B - 128kB */
    {1025, 264, 9, 0x3C, 0x14}, /* AT45DB021B - 256kB */
    {2048, 264, 9, 0x3C, 0x1C}, /* AT45DB041B - 512kB */
    {4096, 264, 9, 0x3C, 0x24}, /* AT45DB081B - 1MB */
    {4096, 528, 10, 0x3C, 0x2C},        /* AT45DB0161B - 2MB */
    {8192, 528, 10, 0x3C, 0x34},        /* AT45DB0321B - 4MB */
    {8192, 1056, 11, 0x38, 0x38},       /* AT45DB0642 - 8MB */
    {0, 0, 0, 0, 0}             /* End of table */
};

/*!
 * \brief Table of active devices.
 */
static AT45DB_DCB dcbtab[MAX_AT45_DEVICES];

/* Number of active chips. */
static uint_least8_t dcbnum;

/* Chip used for parameter storage. */
static int dd_param = -1;

/*!
 * \brief Send DataFlash command.
 *
 * \param dd      Device descriptor.
 * \param op      Command operation code.
 * \param parm    Command parameter.
 * \param len     Command length.
 * \param tdata   Transmit data.
 * \param rdata   Receive data buffer.
 * \param datalen Data length.
 */
int At45dbSendCmd(int dd, uint8_t op, uint32_t parm, int len, CONST void *tdata, void *rdata, int datalen)
{
    uint8_t *cb = dcbtab[dd].dcb_cmdbuf;

    if (len > MAX_AT45_CMDLEN) {
        return -1;
    }
    memset(cb, 0, len);
    cb[0] = op;
    if (parm) {
        cb[1] = (uint8_t) (parm >> 16);
        cb[2] = (uint8_t) (parm >> 8);
        cb[3] = (uint8_t) parm;
    }
    return At91SpiTransfer2(dcbtab[dd].dcb_spibas, dcbtab[dd].dcb_spipcs, cb, cb, len, tdata, rdata, datalen);
}

uint8_t At45dbGetStatus(int dd)
{
    uint8_t buf[2] = { DFCMD_READ_STATUS, 0xFF };

    if (At91SpiTransfer2(dcbtab[dd].dcb_spibas, dcbtab[dd].dcb_spipcs, buf, buf, 2, NULL, NULL, 0)) {
        return (uint8_t) - 1;
    }
    return buf[1];
}

/*!
 * \brief Wait until flash memory cycle finished.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At45dbWaitReady(int dd, uint32_t tmo, int poll)
{
    uint8_t sr;

    while (((sr = At45dbGetStatus(dd)) & 0x80) == 0) {
        if (!poll) {
            NutSleep(1);
        }
        if (tmo-- == 0) {
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief Initialize dataflash at specified interface and chip select.
 *
 * \param spibas Interface base address. For ARM MCUs this may be the
 *               I/O base address of the hardware SPI.
 * \param spipcs Device chip select.
 *
 * \return Device descriptor or -1 in case of an error.
 */
int At45dbInit(unsigned int spibas, unsigned int spipcs)
{
    int dd = -1;
    uint8_t sr;
    uint_fast8_t i;

    for (i = 0; i < dcbnum; i++) {
        if (dcbtab[i].dcb_spibas == spibas && dcbtab[i].dcb_spipcs == spipcs) {
            return i;
        }
    }

    if (dcbnum >= MAX_AT45_DEVICES) {
        return -1;
    }

#if defined(MCU_AT91SAM7S) || defined(MCU_AT91SAM7X) || defined(MCU_AT91SAM7SE) || defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE)
    At91SpiInit(spibas);
    At91SpiReset(spibas);
    At91SpiInitChipSelects(spibas, _BV(spipcs));
    At91SpiSetRate(spibas, spipcs, 1000000);
    At91SpiSetModeFlags(spibas, spipcs, SPIMF_MASTER | SPIMF_SCKIAHI | SPIMF_CAPRISE);
#elif defined(MCU_AT91R40008)
#endif

    dcbtab[dcbnum].dcb_spibas = spibas;
    dcbtab[dcbnum].dcb_spipcs = spipcs;
    sr = At45dbGetStatus(dcbnum);

    for (i = 0; at45_devt[i].devt_pages; i++) {
        if ((sr & at45_devt[i].devt_srmsk) == at45_devt[i].devt_srval) {
            dcbtab[dcbnum].dcb_devt = &at45_devt[i];
            dd = dcbnum++;
            break;
        }
    }
    return dd;
}

/*!
 * \brief Erase sector at the specified offset.
 */
int At45dbPageErase(int dd, uint32_t pgn)
{
    pgn <<= dcbtab[dd].dcb_devt->devt_offs;
    return At45dbSendCmd(dd, DFCMD_PAGE_ERASE, pgn, 4, NULL, NULL, 0);
}

/*!
 * \brief Erase entire flash memory chip.
 */
int At45dbChipErase(void)
{
    return -1;
}

/*!
 * \brief Read data from flash memory.
 *
 * \param dd   Device descriptor.
 * \param pgn  Page number to read, starting at 0.
 * \param data Points to a buffer that receives the data.
 * \param len  Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At45dbPageRead(int dd, uint32_t pgn, void *data, unsigned int len)
{
    pgn <<= dcbtab[dd].dcb_devt->devt_offs;
    return At45dbSendCmd(dd, DFCMD_CONT_READ, pgn, 8, data, data, len);
}

/*!
 * \brief Write data into flash memory.
 *
 * The related sector must have been erased before calling this function.
 *
 * \param dd   Device descriptor.
 * \param pgn  Start location within the chip, starting at 0.
 * \param data Points to a buffer that contains the bytes to be written.
 * \param len  Number of bytes to write.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At45dbPageWrite(int dd, uint32_t pgn, CONST void *data, unsigned int len)
{
    int rc = -1;
    void *rp;

    if ((rp = malloc(len)) != NULL) {
        /* Copy data to dataflash RAM buffer. */
        if (At45dbSendCmd(dd, DFCMD_BUF1_WRITE, 0, 4, data, rp, len) == 0) {
            /* Flash RAM buffer. */
            pgn <<= dcbtab[dd].dcb_devt->devt_offs;
            if (At45dbSendCmd(dd, DFCMD_BUF1_FLASH, pgn, 4, NULL, NULL, 0) == 0) {
                rc = At45dbWaitReady(dd, AT45_WRITE_POLLS, 1);
            }
        }
        free(rp);
    }
    return rc;
}

uint32_t At45dbPages(int dd)
{
    return dcbtab[dd].dcb_devt->devt_pages;
}

unsigned int At45dbPageSize(int dd)
{
    return dcbtab[dd].dcb_devt->devt_pagsiz;
}

uint32_t At45dbParamPage(void)
{
#ifdef AT45_CONF_PAGE
    return AT45_CONF_PAGE;
#else
    return dcbtab[dd_param].dcb_devt->devt_pages - 1;
#endif
}

/*!
 * \brief Get size of configuration area.
 *
 * A part of the DataFlash may be used to store configuration parameters,
 * like the network interface MAC address, the local hostname etc. The
 * size of this area may be configured by defining AT45_CONF_SIZE.
 * Otherwise one full page is used.
 *
 * \return The number of bytes available for configuration data. In case of
 *         an error, -1 is returned.
 */
int At45dbParamSize(void)
{
    int rc;

    if (dd_param == -1 && (dd_param = At45dbInit(AT45_CONF_DFSPI, AT45_CONF_DFPCS)) == -1) {
        return -1;
    }
#ifdef AT45_CONF_SIZE
    rc = AT45_CONF_SIZE;
#else
    rc = dcbtab[dd_param].dcb_devt->devt_pagsiz;
#endif
    return rc;
}

/*!
 * \brief Load configuration parameters from flash memory.
 *
 * \param pos  Start location within configuration sector.
 * \param data Points to a buffer that receives the contents.
 * \param len  Number of bytes to read.
 *
 * \return Always 0.
 */
int At45dbParamRead(unsigned int pos, void *data, unsigned int len)
{
    int rc = -1;
    uint8_t *buff;
    int csize = At45dbParamSize();
    uint32_t cpage = At45dbParamPage();

    /* Load the complete configuration area. */
    if (csize > len && (buff = malloc(csize)) != NULL) {
        rc = At45dbPageRead(dd_param, cpage, buff, csize);
        /* Copy requested contents to caller's buffer. */
        memcpy(data, buff + pos, len);
        free(buff);
    }
    return rc;
}

/*!
 * \brief Store configuration parameters in flash memory.
 *
 * \param pos   Start location within configuration sector.
 * \param data  Points to a buffer that contains the bytes to store.
 * \param len   Number of bytes to store.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At45dbParamWrite(unsigned int pos, CONST void *data, unsigned int len)
{
    int rc = -1;
    uint8_t *buff;
    int csize = At45dbParamSize();
    uint32_t cpage = At45dbParamPage();

    /* Load the complete configuration area. */
    if (csize > len && (buff = malloc(csize)) != NULL) {
        rc = At45dbPageRead(dd_param, cpage, buff, csize);
        /* Compare old with new contents. */
        if (memcmp(buff + pos, data, len)) {
            /* New contents differs. Copy it into the sector buffer. */
            memcpy(buff + pos, data, len);
            /* Erase sector and write new data. */
            rc = At45dbPageWrite(dd_param, cpage, buff, csize);
        }
        free(buff);
    }
    return rc;
}
