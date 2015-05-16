#ifndef _DEV_SPIBUS_H_
#define _DEV_SPIBUS_H_
/*
 * Copyright (C) 2008-2009 by egnite GmbH
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
 * \file include/dev/spibus.h
 * \brief SPI bus declarations.
 *
 * \verbatim
 * $Id: spibus.h 3653 2011-11-14 15:20:31Z haraldkipp $
 * \endverbatim
 */

#include <dev/irqreg.h>
#include <sys/device.h>
#include <sys/event.h>

#define SPI_CURRENT_MODE    ((uint16_t)-1)

#define SPI_MODE_CPHA       0x0001  /* Data updated on leading edge. */
#define SPI_MODE_CPOL       0x0002  /* Idle clock is high. */
#define SPI_MODE_FAULT      0x0004  /* Enables mode fault detection. */
#define SPI_MODE_LOOPBACK   0x0400  /* Loopback mode. */
#define SPI_MODE_SLAVE      0x0800  /* Slave mode. */
#define SPI_MODE_CSKEEP     0x1000  /* Chip select remains active after transfer. */
#define SPI_MODE_CSDEC      0x2000  /* Decoded chip selects. */
#define SPI_MODE_CSHIGH     0x4000  /* Chip select is high active. */
#define SPI_MODE_UPDATE     0x8000  /* Mode update required. */

#define SPI_MODE_0          0x0000
#define SPI_MODE_1          SPI_MODE_CPHA
#define SPI_MODE_2          SPI_MODE_CPOL
#define SPI_MODE_3          (SPI_MODE_CPOL | SPI_MODE_CPHA)


#define SPI_CURRENT_RATE    ((uint32_t)-1)

#define SPI_CURRENT_BITS    ((uint8_t)-1)


typedef struct _NUTSPIBUS NUTSPIBUS;
typedef struct _NUTSPINODE NUTSPINODE;

/*!
 * \brief SPI bus structure.
 */
struct _NUTSPIBUS {
    /*! \brief Bus lock queue.
     */
    HANDLE bus_mutex;

    /*! \brief Bus wait ready queue.
     */
    HANDLE bus_ready;

    /*! \brief Bus base address.
     */
    uintptr_t bus_base;

    /*! \brief Bus interrupt handler.
     */
    IRQ_HANDLER *bus_sig;

    /*! \brief Initialize bus controller.
     *
     * This routine is called during device registration.
     */
    int (*bus_initnode) (NUTSPINODE *);

    /*! \brief Allocate the bus.
     *
     * Locks the bus and activates the chip select.
     */
    int (*bus_alloc) (NUTSPINODE *, uint32_t);

    /*! \brief Release the bus.
     *
     * Deactivates the chip select and unlocks the bus.
     */
    int (*bus_release) (NUTSPINODE *);

    /*! \brief Transfer data.
     */
    int (*bus_transfer) (NUTSPINODE *, CONST void *, void *, int);

    /*! \brief Wait for transfer ready.
     */
    int (*bus_wait) (NUTSPINODE *, uint32_t);

    /*! \brief Set node's SPI mode.
     */
    uint_fast16_t (*bus_set_mode) (NUTSPINODE *, uint_fast16_t);

    /*! \brief Set node's clock rate.
     */
     uint_fast32_t (*bus_set_rate) (NUTSPINODE *, uint_fast32_t);

    /*! \brief Set node's data bit size.
     */
     uint_fast8_t (*bus_set_bits) (NUTSPINODE *, uint_fast8_t);
};

/*!
 * \brief SPI node structure.
 */
struct _NUTSPINODE {
    /*! \brief Pointer to the bus controller driver.
     *
     * Dynamically set when registering the SPI device.
     */
    NUTSPIBUS *node_bus;

    /*! \brief Pointer to the bus driver's device control block.
     *
     * Dynamically set during bus_initnode().
     */
    void *node_stat;

    /*! \brief SPI clock rate.
     *
     * Statically configured, but may change dynamically.
     */
    uint_fast32_t node_rate;

    /*! \brief SPI mode.
     *
     * Statically configured, but may change dynamically.
     */
    uint_fast16_t node_mode;

    /*! \brief SPI data bits.
     *
     * Statically configured, but may change dynamically.
     */
    uint_fast8_t node_bits;

    /*! \brief Chip select.
     *
     * Dynamically set when registering the SPI device.
     */
    uint_fast8_t node_cs;

    /*! \brief Pointer to the node driver's device control block.
     *
     * Statically configured in most cases.
     */
    void *node_dcb;
};

__BEGIN_DECLS
/* Prototypes */
extern int NutRegisterSpiDevice(NUTDEVICE * dev, NUTSPIBUS * bus, int cs);
extern uint_fast16_t NutSpiBusSetMode(NUTSPINODE * node, uint_fast16_t mode);
extern uint_fast32_t NutSpiBusSetRate(NUTSPINODE * node, uint_fast32_t rate);
extern uint_fast8_t NutSpiBusSetBits(NUTSPINODE * node, uint_fast8_t bits);
extern int NutSpiBusWait(NUTSPINODE * node, uint32_t tmo);
/* Prototypes */
__END_DECLS
#endif
