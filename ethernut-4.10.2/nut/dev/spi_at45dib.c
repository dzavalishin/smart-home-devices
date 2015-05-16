/*
 * Copyright (C) 2008-2010 by egnite GmbH
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

/*!
 * \file dev/spi_at45dib.c
 * \brief Low memory routines for Atmel AT45 serial DataFlash memory chips.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/memory.h>

#include <sys/nutdebug.h>
#include <dev/at45d.h>
#ifndef DFCMD_BUF1_LOAD_PAGE
#define DFCMD_BUF1_LOAD_PAGE    0x53
#endif
#ifndef DFCMD_BUF2_LOAD_PAGE
#define DFCMD_BUF2_LOAD_PAGE    0x55
#endif
#include <stdlib.h>

#include <dev/spi_at45dib.h>

/*!
 * \addtogroup xgAt54dib
 */
/*@{*/

/*! \brief RAM buffer dirty flag. */
#define AT45DIB_FDIRTY      0x0001

/*!
 * \brief Internal information structure.
 *
 * This structure is mainly used to keep track of the serial flash's
 * internal RAM buffers.
 */
typedef struct _AT45DIB {
    /*! \brief Page number.
     *
     * Contains the number of the pages currently loaded into the RAM buffers.
     */
    sf_unit_t dib_page[2];

    /*! \brief Attribute flags.
     *
     * Contains AT45DIB_FDIRTY, if the RAM buffer has been modified and not
     * yet written back to flash memory.
     */
    uint_fast8_t flags[2];

    /*! \brief Lock count.
     *
     * A buffer must not be written back to flash memory when this counter
     * is not equal to zero.
     */
    int dib_locks[2];

    /*! \brief Unlock event queue.
     *
     * Queued threads waiting for an unlocked buffer.
     */
    HANDLE dib_lque;

    /*! \brief Page address shift value.
     *
     * Number of bytes to shift the page address for the Dataflash command
     * parameter.
     */
    uint_fast8_t dib_pshft;
} AT45DIB;

/*!
 * \brief Transmit DataFlash command.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command code.
 * \param parm  Command parameter.
 * \param oplen Command length.
 *
 * \return 0 on success, -1 on errors.
 */
static int At45dibTransmitCmd(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen)
{
    uint8_t cmd[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    NUTASSERT(oplen <= sizeof(cmd));

    cmd[0] = op;
    if (parm) {
        cmd[1] = (uint8_t) (parm >> 16);
        cmd[2] = (uint8_t) (parm >> 8);
        cmd[3] = (uint8_t) parm;
    }
    return (*((NUTSPIBUS *) node->node_bus)->bus_transfer) (node, cmd, NULL, oplen);
}

/*!
 * \brief Execute DataFlash command with data transfer.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command code.
 * \param parm  Command parameter.
 * \param oplen Command length.
 * \param txbuf Pointer to the transmit data buffer, may be set to NULL.
 * \param rxbuf Pointer to the receive data buffer, may be set to NULL.
 * \param xlen  Number of byte to receive and/or transmit.
 *
 * \return 0 on success, -1 on errors.
 */
static int At45dibTransfer(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen, 
                           CONST void *txbuf, void *rxbuf, int xlen)
{
    int rc;
    NUTSPIBUS *bus;

    /* Sanity checks. */
    NUTASSERT(node != NULL);
    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

    rc = (*bus->bus_alloc) (node, 0);
    if (rc == 0) {
        rc = At45dibTransmitCmd(node, op, parm, oplen);
        if (rc == 0 && xlen) {
            rc = (*bus->bus_transfer) (node, txbuf, rxbuf, xlen);
        }
        (*bus->bus_release) (node);
    }
    return rc;
}

/*!
 * \brief Execute DataFlash command without data.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 *
 * \return 0 on success, -1 on errors.
 */
static int At45dibCommand(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen)
{
    return At45dibTransfer(node, op, parm, oplen, NULL, NULL, 0);
}

/*!
 * \brief Execute serial flash compare.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 * \param buf   Pointer to the data to compare.
 * \param xlen  Number of byte to compare.
 *
 * \return 0 on success, -1 on errors.
 */
static int At45dibCompare(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen, CONST void *buf, int xlen)
{
    int rc;
    NUTSPIBUS *bus;

    /* Sanity checks. */
    NUTASSERT(node != NULL);
    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

    rc = (*bus->bus_alloc) (node, 0);
    if (rc == 0) {
        rc = At45dibTransmitCmd(node, op, parm, oplen);
        if (rc == 0 && xlen) {
            int i;
            uint8_t c;
            uint8_t *cp = (uint8_t *) buf;

            for (i = 0; i < xlen; i++) {
                rc = (*bus->bus_transfer) (node, NULL, &c, 1);
                if (rc) {
                    break;
                }
                if (c != cp[i]) {
                    rc = (int) c - (int) cp[i];
                    break;
                }
            }
        }
        (*bus->bus_release) (node);
    }
    return rc;
}

/*!
 * \brief Determine number of used bytes in a unit.
 *
 * This function will scan a given page for bytes not equal 0xFF. The
 * last byte found marks the end of the block of used bytes.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 * \param xlen  Number of byte to check.
 *
 * \return Number of bytes or -1 on errors.
 */
static int At45dibUsed(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen, int xlen)
{
    int rc;
    NUTSPIBUS *bus;

    /* Sanity checks. */
    NUTASSERT(node != NULL);

    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

    rc = (*bus->bus_alloc) (node, 0);
    if (rc == 0) {
        rc = At45dibTransmitCmd(node, op, parm, oplen);
        if (rc == 0 && xlen) {
            int i;
            int n = -1;
            uint8_t c;

            for (i = 0; i < xlen; i++) {
                rc = (*bus->bus_transfer) (node, NULL, &c, 1);
                if (rc) {
                    break;
                }
                if (c != 0xff) {
                    n = i;
                }
            }
            if (rc == 0) {
                rc = n + 1;
            }
        }
        (*bus->bus_release) (node);
    }
    return rc;
}


#ifdef AT45D_CRC_PAGE

/*
 * http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
 */
static uint16_t crc_ccitt_update(uint16_t crc, uint8_t data)
{
    data ^= (uint8_t) (crc);
    data ^= data << 4;

    return ((((uint16_t) data << 8) | (crc >> 8)) ^ (uint8_t) (data >> 4) ^ ((uint16_t) data << 3));
}

/*!
 * \brief Execute serial flash CRC.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 * \param crc16 Pointer to the variable that receives the checksum.
 * \param xlen  Number of byte to use for the calculation.
 *
 * \return 0 on success, -1 on errors.
 */
static int At45dibChecksum(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen, uint16_t * crc16, int xlen)
{
    int rc;
    NUTSPIBUS *bus;

    /* Sanity checks. */
    NUTASSERT(node != NULL);
    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

    *crc16 = 0xffff;
    rc = (*bus->bus_alloc) (node, 0);
    if (rc == 0) {
        rc = At45dibTransmitCmd(node, op, parm, oplen);
        if (rc == 0 && xlen) {
            int i;
            uint8_t c;
            uint_fast8_t ne = 0;

            for (i = 0; i < xlen; i++) {
                rc = (*bus->bus_transfer) (node, NULL, &c, 1);
                if (rc) {
                    break;
                }
                if (ne || c != 0xff) {
                    ne = 1;
                    *crc16 = crc_ccitt_update(*crc16, c);
                }
            }
            if (*crc16 == 0) {
                *crc16 = 0xffff;
            }
        }
        (*bus->bus_release) (node);
    }
    return rc;
}

#endif /* AT45D_CRC_PAGE */


/*!
 * \brief Query the status of the serial flash.
 *
 * \param node Specifies the SPI node.
 *
 * \return 0 on success or -1 in case of an error.
 */
static uint8_t At45dibStatus(NUTSPINODE * node)
{
    int rc;
    uint8_t cmd[2] = { DFCMD_READ_STATUS, 0xFF };
    NUTSPIBUS *bus;

    /* Sanity checks. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_wait != NULL);
    NUTASSERT(bus->bus_release != NULL);

    rc = (*bus->bus_alloc) (node, 0);
    if (rc == 0) {
        rc = (*bus->bus_transfer) (node, cmd, cmd, 2);
        if (rc == 0) {
            (*bus->bus_wait) (node, NUT_WAIT_INFINITE);
            rc = cmd[1];
        }
        (*bus->bus_release) (node);
    }
    return (uint8_t) rc;
}

/*!
 * \brief Wait until serial flash memory cycle finished.
 *
 * \param node Specifies the SPI node.
 * \param tmo  Number of loops to wait.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int At45dibWaitReady(NUTSPINODE * node, uint32_t tmo)
{
    uint8_t sr;

    while (((sr = At45dibStatus(node)) & 0x80) == 0) {
        if (tmo-- == 0) {
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief Flash the given buffer.
 *
 * \param sfi Specifies the serial flash interface.
 * \param b Number of the buffer, either 0 or 1.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int At45dibFlash(NUTSERIALFLASH * sfi, int_fast8_t b)
{
    int rc;
    AT45DIB *at;
    uint32_t pga;

#ifdef AT45D_CRC_PAGE
    /* Store a 16 bit CRC at the end of the page, if configured. */
    {
        uint16_t crc16;

        if (At45dibChecksum(sfi->sf_node, b ? DFCMD_BUF2_READ : DFCMD_BUF1_READ, 0, 5, &crc16, sfi->sf_unit_size)) {
            return -1;
        }
        if (At45dibTransfer
            (sfi->sf_node, b ? DFCMD_BUF2_WRITE : DFCMD_BUF1_WRITE, sfi->sf_unit_size, 4, &crc16, NULL, sizeof(crc16))) {
            return -1;
        }
    }
#endif

    at = (AT45DIB *) sfi->sf_info;
    pga = at->dib_page[b];
    pga <<= at->dib_pshft;
    rc = At45dibCommand(sfi->sf_node, b ? DFCMD_BUF2_FLASH_NE : DFCMD_BUF1_FLASH_NE, pga, 4);
    if (rc == 0) {
        rc = At45dibWaitReady(sfi->sf_node, AT45_WRITE_POLLS);
        at->flags[b] &= ~AT45DIB_FDIRTY;
    }
    return rc;
}

/*!
 * \brief Release the internal RAM buffer.
 *
 * \param sfi Specifies the serial flash interface.
 * \param b Number of the buffer, either 0 or 1.
 */
static void At45dibRelease(NUTSERIALFLASH * sfi, int b)
{
    AT45DIB *at = (AT45DIB *) sfi->sf_info;

    at->dib_locks[b]--;
    NutEventPost(&at->dib_lque);
}

/*!
 * \brief Load a given page into the internal RAM buffer.
 *
 * The RAM buffer will be locked and marked dirty. To unlock the buffer,
 * call At45dibRelease(). To clean the buffer, call SpiAt45dibCommit().
 *
 * \return Number of the buffer.
 */
static int_fast8_t At45dibAllocate(NUTSERIALFLASH * sfi, sf_unit_t pgn)
{
    int_fast8_t b;
    AT45DIB *at;

    at = (AT45DIB *) sfi->sf_info;
    for (;;) {
        /* 
        ** Check, if the page is already loaded.
        */
        for (b = 0; b < 2; b++) {
            if (at->dib_page[b] == pgn) {
                at->dib_locks[b]++;
                at->flags[b] |= AT45DIB_FDIRTY;
                return b;
            }
        }

        /*
        ** Search for a clean unlocked buffer and load the page.
        */
        for (b = 0; b < 2; b++) {
            if (at->dib_locks[b] == 0 && (at->flags[b] & AT45DIB_FDIRTY) == 0) {
                int rc;
                uint32_t pga = pgn;

                pga <<= at->dib_pshft;
                at->dib_locks[b]++;
                if (b) {
                    rc = At45dibCommand(sfi->sf_node, DFCMD_BUF2_LOAD_PAGE, pga, 4);
                } else {
                    rc = At45dibCommand(sfi->sf_node, DFCMD_BUF1_LOAD_PAGE, pga, 4);
                }
                if (rc == 0) {
                    rc = At45dibWaitReady(sfi->sf_node, AT45_WRITE_POLLS);
                }
                if (rc == 0) {
                    at->dib_page[b] = pgn;
                    at->flags[b] |= AT45DIB_FDIRTY;

                    return b;
                }
                At45dibRelease(sfi, b);

                return -1;
            }
        }

        /*
        ** No clean buffer. If not locked, save one of them.
        */
        for (b = 0; b < 2; b++) {
            if (at->dib_locks[b] == 0 && (at->flags[b] & AT45DIB_FDIRTY) == AT45DIB_FDIRTY) {
                if (At45dibFlash(sfi, b)) {
                    return -1;
                }
                break;
            }
        }

        /*
        ** All buffers are locked. Wait for an unlock event.
        */
        if (b >= 2) {
            NutEventWait(&at->dib_lque, 0);
        }
    }
}

/*!
 * \brief Initialize the interface.
 *
 * \param sfi Specifies the serial flash interface.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibInit(NUTSERIALFLASH * sfi)
{
    int_fast8_t b;
    uint8_t sr;
    int_fast8_t i;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_node != NULL);

    sr = At45dibStatus(sfi->sf_node);
    sr &= AT45D_STATUS_DENSITY | AT45D_STATUS_PAGE_SIZE;
    for (i = at45d_known_types; --i >= 0;) {
        if (sr == at45d_info[i].at45d_srval) {
            /* Known DataFlash type. */
            AT45DIB *at = calloc(1, sizeof(AT45DIB));
            if (at == NULL) {
                return -1;
            }
            at->dib_pshft = at45d_info[i].at45d_pshft;
            for (b = 0; b < 2; b++) {
                at->dib_page[b] = SERIALFLASH_MAX_UNITS;
            }
            sfi->sf_info = (void *) at;
            sfi->sf_units = (sf_unit_t) at45d_info[i].at45d_pages;
            sfi->sf_unit_size = at45d_info[i].at45d_psize;
#ifdef AT45D_CRC_PAGE
            sfi->sf_unit_size -= 2;
#endif
            return 0;
        }
    }
    /* Unknown DataFlash type. */
    return -1;
}

/*!
 * \brief Release the interface.
 *
 * \param sfi Specifies the serial flash interface.
 *
 * \return 0 on success or -1 in case of an error.
 */
static void SpiAt45dibExit(NUTSERIALFLASH * sfi)
{
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_info != NULL);

    free(sfi->sf_info);
}

/*!
 * \brief Verify CRC of consecutive pages.
 *
 * \param sfi Specifies the serial flash interface.
 * \param pgn First page to verify.
 * \param cnt Number of consecutive pages to verify.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibCheck(NUTSERIALFLASH * sfi, sf_unit_t pgn, int cnt)
{
    int rc = 0;

#ifdef AT45D_CRC_PAGE
    AT45DIB *at;
    int_fast8_t b;
    uint16_t crc16;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_info != NULL);
    NUTASSERT(pgn + cnt <= sfi->sf_units);
    NUTASSERT(cnt >= 0);

    at = (AT45DIB *) sfi->sf_info;
    while (cnt--) {
        /* If this page is buffered, then read from the buffer. */
        for (b = 0; b < 2 && at->dib_page[b] != pgn + cnt; b++);
        if (b < 2) {
            At45dibChecksum(sfi->sf_node, b ? DFCMD_BUF2_READ : DFCMD_BUF1_READ, 0, 5, &crc16, sfi->sf_unit_size + 2);
        }
        /* If not buffered, then directly read from the flash. */
        else {
            uint32_t pga = pgn + cnt;
            pga <<= at->dib_pshft;
            At45dibChecksum(sfi->sf_node, DFCMD_READ_PAGE, pga, 8, &crc16, sfi->sf_unit_size + 2);
        }
        if (crc16 != 0xFFFF) {
            rc = -1;
            break;
        }
    }
#endif
    return rc;
}

/*!
 * \brief Read data from a given page.
 *
 * \param sfi  Specifies the serial flash interface.
 * \param pgn  Page to read from.
 * \param off  Read offset into the page. If negative, the offset counts 
 *             from the end of the page.
 * \param data Pointer to the buffer that receives the data.
 * \param len  Number of data bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibRead(NUTSERIALFLASH * sfi, sf_unit_t pgn, int off, void *data, int len)
{
    int rc = 0;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_info != NULL);
    NUTASSERT(pgn < sfi->sf_units);
    NUTASSERT(off >= -(int) sfi->sf_unit_size && off <= (int) sfi->sf_unit_size);
    NUTASSERT(data != NULL);
    NUTASSERT(len >= 0 && len <= sfi->sf_unit_size);

    if (len) {
        int_fast8_t b;
        AT45DIB *at = (AT45DIB *) sfi->sf_info;

        /* Normalize the offset. */
        if (off < 0) {
            off += sfi->sf_unit_size;
        }
        NUTASSERT(off + len <= (int) sfi->sf_unit_size);

        /* If this page is buffered, then read from the buffer. */
        for (b = 0; b < 2 && at->dib_page[b] != pgn; b++);
        if (b < 2) {
            rc = At45dibTransfer(sfi->sf_node, b ? DFCMD_BUF2_READ : DFCMD_BUF1_READ, off, 5, NULL, data, len);
        }
        /* If not buffered, then directly read from the flash. */
        else {
            uint32_t pga = pgn;

            pga <<= at->dib_pshft;
            pga |= off;
            rc = At45dibTransfer(sfi->sf_node, DFCMD_READ_PAGE, pga, 8, NULL, data, len);
        }
    }
    return rc;
}

/*!
 * \brief Compare data with the contents of a given page.
 *
 * This function allows to directly compare the contents of a page. It is
 * not required to load the page.
 *
 * \param sfi  Specifies the serial flash interface.
 * \param pgn  Page to compare.
 * \param off  Offset into the page. If negative, the offset counts from 
 *             the end of the page.
 * \param data Pointer to the data to compare.
 * \param len  Number of data bytes to compare.
 *
 * \return 0 on success or -1 if the contents differs or in case of an error.
 */
static int SpiAt45dibCompare(NUTSERIALFLASH * sfi, sf_unit_t pgn, int off, CONST void *data, int len)
{
    int rc = 0;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_info != NULL);
    NUTASSERT(pgn < sfi->sf_units);
    NUTASSERT(off >= -(int) sfi->sf_unit_size && off <= (int) sfi->sf_unit_size);
    NUTASSERT(data != NULL);
    NUTASSERT(len >= 0 && len <= sfi->sf_unit_size);

    if (len) {
        int_fast8_t b;
        AT45DIB *at = (AT45DIB *) sfi->sf_info;

        /* Normalize the offset. */
        if (off < 0) {
            off += sfi->sf_unit_size;
        }
        NUTASSERT(off + len <= (int) sfi->sf_unit_size);

        /* If this page is buffered, then read from the buffer. */
        for (b = 0; b < 2 && at->dib_page[b] != pgn; b++);
        if (b < 2) {
            rc = At45dibCompare(sfi->sf_node, b ? DFCMD_BUF2_READ : DFCMD_BUF1_READ, off, 5, data, len);
        }
        /* If not buffered, then directly read from the flash. */
        else {
            uint32_t pga = pgn;

            pga <<= at->dib_pshft;
            pga |= off;
            rc = At45dibCompare(sfi->sf_node, DFCMD_READ_PAGE, pga, 8, data, len);
        }
    }
    return rc;
}

/*!
 * \brief Return the number of bytes used in a given page.
 *
 * This function allows to directly compare the contents of a page. It is
 * not required to load the page.
 *
 * \param sfi  Specifies the serial flash interface.
 * \param pgn  Page to check.
 * \param skip Number of bytes to ingore. May be a positive or negative
 *             value, to ingnore bytes at the beginning or at the end, resp.
 *
 * \return 0 on success or -1 if the contents differs or in case of an error.
 */
static int SpiAt45dibUsed(NUTSERIALFLASH * sfi, sf_unit_t pgn, int skip)
{
    int rc;
    int len;
    AT45DIB *at;
    int_fast8_t b;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(pgn < sfi->sf_units);
    NUTASSERT(skip <= (int) sfi->sf_unit_size);

    at = (AT45DIB *) sfi->sf_info;

    /* Determine length and offset. */
    len = (int) sfi->sf_unit_size;
    if (skip < 0) {
        len += skip;
        skip = 0;
    } else {
        len -= skip;
    }

    /* If this page is buffered, then read from the buffer. */
    for (b = 0; b < 2 && at->dib_page[b] != pgn; b++);
    if (b < 2) {
        rc = At45dibUsed(sfi->sf_node, b ? DFCMD_BUF2_READ : DFCMD_BUF1_READ, skip, 5, len);
    }
    /* If not buffered, then directly read from the flash. */
    else {
        uint32_t pga = pgn;

        pga <<= at->dib_pshft;
        pga |= skip;
        rc = At45dibUsed(sfi->sf_node, DFCMD_READ_PAGE, pga, 8, len);
    }
    return rc;
}

/*!
 * \brief Write data to a given page.
 *
 * Loads the page into an internal RAM buffer and replaces the contents
 * with the given data. The buffer will be marked dirty, but not written 
 * back. See SpiAt45dibCommit().
 *
 * \param sfi  Specifies the serial flash interface.
 * \param pgn  Page to write to.
 * \param off  Offset into the page, where the new data should be placed.
 *             If negative, the offset counts from the end of the page.
 * \param data Pointer to the data to write.
 * \param len  Number of data bytes to write.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibWrite(NUTSERIALFLASH * sfi, sf_unit_t pgn, int off, CONST void *data, int len)
{
    int rc = 0;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(pgn < sfi->sf_units);
    NUTASSERT(off >= -(int) sfi->sf_unit_size && off <= (int) sfi->sf_unit_size);
    NUTASSERT(len == 0 || data != NULL);
    NUTASSERT(len >= 0 && len <= sfi->sf_unit_size);

    if (len) {
        int_fast8_t b;

        /* Normalize the offset. */
        if (off < 0) {
            off += sfi->sf_unit_size;
        }
        NUTASSERT(off + len <= (int) sfi->sf_unit_size);

        /* Load the page. */
        b = At45dibAllocate(sfi, pgn);
        if (b < 0) {
            return -1;
        }
        /* Transfer the data and release the page. */
        rc = At45dibTransfer(sfi->sf_node, b ? DFCMD_BUF2_WRITE : DFCMD_BUF1_WRITE, off, 4, data, NULL, len);
        At45dibRelease(sfi, b);
    }
    return rc;
}

/*!
 * \brief Copy page.
 *
 * Loads the source page into an internal RAM buffer und designates the
 * buffer to the destination page. The buffer will be marked dirty, but
 * not written back. See SpiAt45dibCommit().
 *
 * \param sfi Specifies the serial flash interface.
 * \param spg Source page to copy from.
 * \param dpg Destination page to copy to.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibCopy(NUTSERIALFLASH * sfi, sf_unit_t spg, sf_unit_t dpg)
{
    int_fast8_t b;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_info != NULL);
    NUTASSERT(spg < sfi->sf_units);
    NUTASSERT(dpg < sfi->sf_units);

    /* If source and destination page numbers are equal, just load it. */
    if (spg == dpg) {
        b = At45dibAllocate(sfi, spg);
    } else {
        AT45DIB *at = (AT45DIB *) sfi->sf_info;

        /* Invalidate any buffered destination. */
        for (b = 0; b < 2; b++) {
            if (at->dib_page[b] == dpg) {
                /* But do not touch locked pages. */
                if (at->dib_locks[b]) {
                    return -1;
                }
                at->dib_page[b] = SERIALFLASH_MAX_UNITS;
            }
        }
        /* Try to load the source page and make it the destination page. */
        b = At45dibAllocate(sfi, spg);
        if (b >= 0) {
            at->dib_page[b] = dpg;
        }
    }
    if (b >= 0) {
        At45dibRelease(sfi, b);
        return 0;
    }
    return -1;
}

/*!
 * \brief Commit page.
 *
 * If the contents of the given page is in one of the internal RAM 
 * buffers and if the contents had been changed since the last page
 * load, then the buffer is written back to the flash.
 *
 * \param sfi Specifies the serial flash interface.
 * \param pgn Page to commit.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibCommit(NUTSERIALFLASH * sfi, sf_unit_t pgn)
{
    int rc = 0;
    int_fast8_t b;
    AT45DIB *at;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(pgn < sfi->sf_units);

    at = (AT45DIB *) sfi->sf_info;
    for (b = 0; b < 2 && at->dib_page[b] != pgn; b++);
    if (b < 2 && (at->flags[b] & AT45DIB_FDIRTY) == AT45DIB_FDIRTY) {
        rc = At45dibFlash(sfi, b);
    }
    return rc;
}

/*!
 * \brief Erase consecutive pages.
 *
 * \param sfi Specifies the serial flash interface.
 * \param pgn First page to erase.
 * \param cnt Number of consecutive pages to erase.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int SpiAt45dibErase(NUTSERIALFLASH * sfi, sf_unit_t pgn, int cnt)
{
    int_fast8_t b;
    AT45DIB *at;

    /* Sanity checks. */
    NUTASSERT(sfi != NULL);
    NUTASSERT(pgn + cnt <= sfi->sf_units);
    NUTASSERT(cnt >= 0);

    at = (AT45DIB *) sfi->sf_info;
    /* Invalidate any buffered page. */
    for (b = 0; b < 2; b++) {
        if (at->dib_page[b] >= pgn && at->dib_page[b] < pgn + cnt) {
            at->dib_page[b] = SERIALFLASH_MAX_UNITS;
            at->flags[b] &= ~AT45DIB_FDIRTY;
        }
    }
    while (cnt--) {
        uint32_t pga = pgn + cnt;

        pga <<= at->dib_pshft;
        if (At45dibCommand(sfi->sf_node, DFCMD_PAGE_ERASE, pga, 4)) {
            return -1;
        }
        if (At45dibWaitReady(sfi->sf_node, AT45_WRITE_POLLS)) {
            return -1;
        }
    }
    return 0;
}

#ifndef SPI_RATE_AT45D0
#define SPI_RATE_AT45D0  33000000
#endif

#ifndef SPI_MODE_AT45D0

#ifdef SPI_CSHIGH_AT45D0
#define SPI_MODE_AT45D0 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#else
#define SPI_MODE_AT45D0 SPI_MODE_3
#endif

#elif defined(SPI_CSHIGH_AT45D0)

/* This is a tricky problem. Originally we didn't provide mode settings
** in the Configurator, but used mode 3 only. After experiencing problems
** with mode switching on the EIR, we need to set mode 0 for that board,
** which spoils our chip select polarity setting. */
#if SPI_MODE_AT45D0 == SPI_MODE_0
#undef SPI_MODE_AT45D0
#define SPI_MODE_AT45D0 (SPI_MODE_0 | SPI_MODE_CSHIGH)
#elif SPI_MODE_AT45D0 == SPI_MODE_3
#undef SPI_MODE_AT45D0
#define SPI_MODE_AT45D0 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#endif

#endif /* SPI_MODE_AT45D0 */

/*!
 * \brief First AT45D DataFlash SPI node implementation structure.
 */
static NUTSPINODE spiNode0 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to device driver specific settings, node_stat. */
    SPI_RATE_AT45D0,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D0,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0                           /*!< \brief Chip select, node_cs. */
};

#ifndef MOUNT_OFFSET_AT45DIB0
#ifdef MOUNT_OFFSET_AT45D0
#define MOUNT_OFFSET_AT45DIB0       MOUNT_OFFSET_AT45D0
#else
#define MOUNT_OFFSET_AT45DIB0       0
#endif
#endif

#ifndef MOUNT_TOP_RESERVE_AT45DIB0
#ifdef MOUNT_TOP_RESERVE_AT45D0
#define MOUNT_TOP_RESERVE_AT45DIB0  MOUNT_TOP_RESERVE_AT45D0
#else
#define MOUNT_TOP_RESERVE_AT45DIB0  1
#endif
#endif

/*!
 * \brief First AT45D DataFlash interface implementation structure.
 */
NUTSERIALFLASH flashAt45dib0 = {
    &spiNode0,                  /*!< \brief Pointer to the SPI node structure, sf_node. */
    NULL,                       /*!< \brief Pointer to a local information structure, sf_info. */
    0,                          /*!< \brief Size of an erase/write unit, sf_unit_size. */
    0,                          /*!< \brief Total number of units, sf_units. */
    MOUNT_OFFSET_AT45DIB0,      /*!< \brief Reserved units at the bottom, sf_rsvbot. */
    MOUNT_TOP_RESERVE_AT45DIB0, /*!< \brief Reserved units at the top, sf_rsvtop. */
    SpiAt45dibInit,             /*!< \brief Flash device initialization function, sf_init. */
    SpiAt45dibExit,             /*!< \brief Flash device release function, sf_exit. */
    SpiAt45dibCheck,            /*!< \brief Unit validation function, sf_check. */
    SpiAt45dibRead,             /*!< \brief Data read function, sf_read. */
    SpiAt45dibCompare,          /*!< \brief Data compare function, sf_compare. */
    SpiAt45dibUsed,             /*!< \brief Unit usage query function, sf_used. */
    SpiAt45dibWrite,            /*!< \brief Data write function, sf_write. */
    SpiAt45dibCopy,             /*!< \brief Unit copy function, sf_copy. */
    SpiAt45dibCommit,           /*!< \brief Unit commit function, sf_commit. */
    SpiAt45dibErase             /*!< \brief Unit erase function, sf_erase. */
};

#ifndef SPI_RATE_AT45D1
#define SPI_RATE_AT45D1  33000000
#endif

#ifndef SPI_MODE_AT45D1

#ifdef SPI_CSHIGH_AT45D1
#define SPI_MODE_AT45D1 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#else
#define SPI_MODE_AT45D1 SPI_MODE_3
#endif

#elif defined(SPI_CSHIGH_AT45D1)

/* Same problem problem as above. */
#if SPI_MODE_AT45D1 == SPI_MODE_0
#undef SPI_MODE_AT45D1
#define SPI_MODE_AT45D1 (SPI_MODE_0 | SPI_MODE_CSHIGH)
#elif SPI_MODE_AT45D1 == SPI_MODE_3
#undef SPI_MODE_AT45D1
#define SPI_MODE_AT45D1 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#endif

#endif /* SPI_MODE_AT45D0 */

/*!
 * \brief Second AT45D DataFlash SPI node implementation structure.
 */
static NUTSPINODE spiNode1 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to device driver specific settings, node_stat. */
    SPI_RATE_AT45D1,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D1,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    1                           /*!< \brief Chip select, node_cs. */
};

#ifndef MOUNT_OFFSET_AT45DIB1
#ifdef MOUNT_OFFSET_AT45D1
#define MOUNT_OFFSET_AT45DIB1       MOUNT_OFFSET_AT45D1
#else
#define MOUNT_OFFSET_AT45DIB1       0
#endif
#endif

#ifndef MOUNT_TOP_RESERVE_AT45DIB1
#ifdef MOUNT_TOP_RESERVE_AT45D1
#define MOUNT_TOP_RESERVE_AT45DIB1  MOUNT_TOP_RESERVE_AT45D1
#else
#define MOUNT_TOP_RESERVE_AT45DIB1  1
#endif
#endif

/*!
 * \brief Second AT45D DataFlash interface implementation structure.
 */
NUTSERIALFLASH flashAt45dib1 = {
    &spiNode1,                  /*!< \brief Pointer to the SPI node structure, sf_node. */
    NULL,                       /*!< \brief Pointer to a local information structure, sf_info. */
    0,                          /*!< \brief Size of an erase/write unit, sf_unit_size. */
    0,                          /*!< \brief Total number of units, sf_units. */
    MOUNT_OFFSET_AT45DIB1,      /*!< \brief Reserved units at the bottom, sf_rsvbot. */
    MOUNT_TOP_RESERVE_AT45DIB1, /*!< \brief Reserved units at the top, sf_rsvtop. */
    SpiAt45dibInit,             /*!< \brief Flash device initialization function, sf_init. */
    SpiAt45dibExit,             /*!< \brief Flash device release function, sf_exit. */
    SpiAt45dibCheck,            /*!< \brief Unit validation function, sf_check. */
    SpiAt45dibRead,             /*!< \brief Data read function, sf_read. */
    SpiAt45dibCompare,          /*!< \brief Data compare function, sf_compare. */
    SpiAt45dibUsed,             /*!< \brief Unit usage query function, sf_used. */
    SpiAt45dibWrite,            /*!< \brief Data write function, sf_write. */
    SpiAt45dibCopy,             /*!< \brief Unit copy function, sf_copy. */
    SpiAt45dibCommit,           /*!< \brief Unit commit function, sf_commit. */
    SpiAt45dibErase             /*!< \brief Unit erase function, sf_erase. */
};

/*@}*/
