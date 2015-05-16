/*
 * Copyright (C) 2008-2011 by egnite GmbH
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
 * \file dev/spi_node_at45d.c
 * \brief Low level routines for Atmel AT45D SPI Flash.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/memory.h>

#include <dev/blockdev.h>
#include <sys/nutdebug.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <string.h>

#include <dev/spi_node_at45d.h>

/*!
 * \addtogroup xgSpiNodeAt45d
 */
/*@{*/

typedef struct _AT45D_DCB {
    HANDLE dcb_lock;
} AT45D_DCB;

static AT45D_DCB dcbAt45d0 = {
    SIGNALED
};

static AT45D_DCB dcbAt45d1 = {
    SIGNALED
};

static AT45D_DCB dcbAt45d2 = {
    SIGNALED
};

static AT45D_DCB dcbAt45d3 = {
    SIGNALED
};

int At45dNodeLock(NUTSPINODE * node)
{
    AT45D_DCB *dcb = (AT45D_DCB *) node->node_dcb;

    return NutEventWait(&dcb->dcb_lock, NUT_WAIT_INFINITE);
}

void At45dNodeUnlock(NUTSPINODE * node)
{
    AT45D_DCB *dcb = (AT45D_DCB *) node->node_dcb;

    NutEventPost(&dcb->dcb_lock);
}

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
static int At45dNodeTransmitCmd(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen)
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
int At45dNodeTransfer(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen,
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
        rc = At45dNodeTransmitCmd(node, op, parm, oplen);
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
int At45dNodeCommand(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen)
{
    return At45dNodeTransfer(node, op, parm, oplen, NULL, NULL, 0);
}

/*!
 * \brief Query the status of the serial flash.
 *
 * \param node Specifies the SPI node.
 *
 * \return 0 on success or -1 in case of an error.
 */
uint8_t At45dNodeStatus(NUTSPINODE * node)
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
 * \brief Wait until DataFlash memory cycle finished.
 *
 * \param node Specifies the SPI node.
 * \param tmo  Number of loops (or milliseconds) to wait at max.
 * \param poll If 0, the current thread will be suspended for
 *             1 millisecond between each retry. Otherwise the
 *             polling loop will not sleep, but may be still suspended
 *             by the lower level SPI bus driver.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At45dNodeWaitReady(NUTSPINODE * node, uint32_t tmo, int poll)
{
    uint8_t sr;

    while (((sr = At45dNodeStatus(node)) & 0x80) == 0) {
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
 * \brief Determine the DataFlash type.
 *
 * If the board contains a known DataFlash chip, but couldn't be
 * determined by this routine, then most probably the chip select
 * configuration is wrong.
 *
 * \param node Specifies the SPI node.
 *
 * \return Pointer to a \ref AT45D_INFO structure, which contains
 *         the relevant DataFlash parameters. If no known DataFlash
 *         is available, then NULL is returned.
 */
AT45D_INFO *At45dNodeProbe(NUTSPINODE * node)
{
    int_fast8_t i;
    uint8_t sr;

    if (At45dNodeWaitReady(node, 10, 1) == 0) {
        sr = At45dNodeStatus(node);
        if (sr != 0xff) {
            sr &= AT45D_STATUS_DENSITY | AT45D_STATUS_PAGE_SIZE;
            for (i = at45d_known_types; --i >= 0;) {
                if (sr == at45d_info[i].at45d_srval) {
                    return &at45d_info[i];
                }
            }
        }
    }
    return NULL;
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

#endif                          /* SPI_MODE_AT45D0 */

/*!
 * \brief First AT45D DataFlash SPI node implementation structure.
 */
NUTSPINODE nodeAt45d0 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to the bus device driver specific settings, node_stat. */
    SPI_RATE_AT45D0,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D0,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0,                          /*!< \brief Chip select, node_cs. */
    &dcbAt45d0                  /*!< \brief Pointer to our private device control block, node_dcb. */
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

#endif                          /* SPI_MODE_AT45D1 */

/*!
 * \brief Second AT45D DataFlash SPI node implementation structure.
 */
NUTSPINODE nodeAt45d1 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to the bus device driver specific settings, node_stat. */
    SPI_RATE_AT45D1,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D1,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0,                          /*!< \brief Chip select, node_cs. */
    &dcbAt45d1                  /*!< \brief Pointer to our private device control block, node_dcb. */
};

#ifndef SPI_RATE_AT45D2
#define SPI_RATE_AT45D2  33000000
#endif

#ifndef SPI_MODE_AT45D2

#ifdef SPI_CSHIGH_AT45D2
#define SPI_MODE_AT45D2 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#else
#define SPI_MODE_AT45D2 SPI_MODE_3
#endif

#elif defined(SPI_CSHIGH_AT45D2)

/* Same problem problem as above. */
#if SPI_MODE_AT45D2 == SPI_MODE_0
#undef SPI_MODE_AT45D2
#define SPI_MODE_AT45D2 (SPI_MODE_0 | SPI_MODE_CSHIGH)
#elif SPI_MODE_AT45D2 == SPI_MODE_3
#undef SPI_MODE_AT45D2
#define SPI_MODE_AT45D2 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#endif

#endif                          /* SPI_MODE_AT45D2 */

/*!
 * \brief Third AT45D DataFlash SPI node implementation structure.
 */
NUTSPINODE nodeAt45d2 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to the bus device driver specific settings, node_stat. */
    SPI_RATE_AT45D2,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D2,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0,                          /*!< \brief Chip select, node_cs. */
    &dcbAt45d2                  /*!< \brief Pointer to our private device control block, node_dcb. */
};

#ifndef SPI_RATE_AT45D3
#define SPI_RATE_AT45D3  33000000
#endif

#ifndef SPI_MODE_AT45D3

#ifdef SPI_CSHIGH_AT45D3
#define SPI_MODE_AT45D3 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#else
#define SPI_MODE_AT45D3 SPI_MODE_3
#endif

#elif defined(SPI_CSHIGH_AT45D3)

/* Same problem problem as above. */
#if SPI_MODE_AT45D3 == SPI_MODE_0
#undef SPI_MODE_AT45D3
#define SPI_MODE_AT45D3 (SPI_MODE_0 | SPI_MODE_CSHIGH)
#elif SPI_MODE_AT45D3 == SPI_MODE_3
#undef SPI_MODE_AT45D3
#define SPI_MODE_AT45D3 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#endif

#endif /* SPI_MODE_AT45D3 */

/*!
 * \brief Forth AT45D DataFlash SPI node implementation structure.
 */
NUTSPINODE nodeAt45d3 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to the bus device driver specific settings, node_stat. */
    SPI_RATE_AT45D3,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D3,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0,                          /*!< \brief Chip select, node_cs. */
    &dcbAt45d3                  /*!< \brief Pointer to our private device control block, node_dcb. */
};

/*@}*/
