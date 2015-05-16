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
 * \file dev/spibus.c
 * \brief Generic SPI bus driver routines.
 *
 * \verbatim
 * $Id: spibus.c 2375 2009-01-09 17:54:38Z haraldkipp $
 * \endverbatim
 */

#include <sys/device.h>
#include <sys/nutdebug.h>

#include <dev/spibus.h>

/*!
 * \brief Register and initialize an SPI device attached to a specified bus.
 *
 * Calls the bus controller initialization and, if successful, initializes 
 * the SPI device and adds it to the system device list.
 *
 * Applications should call this function during initialization for each 
 * SPI device they intend to use.
 *
 * \param dev Pointer to the \ref NUTDEVICE structure, which is provided 
 *            by the device driver.
 * \param bus Pointer to the \ref NUTSPIBUS structure, which is provided 
 *            by the bus controller driver.
 * \param cs  Zero based chip select number for this device. Must be set 
 *            to 0, if only one device is attached to the bus and no chip 
 *            select line is provided.
 *
 * \return 0 if the device has been registered for the first time and 
 *         the initialization was successful. The function returns -1, 
 *         if any device with the same name had been registered previously, 
 *         if the \ref NUTDEVICE structure or the chip select is invalid or 
 *         if the device or bus controller initialization failed.
 */
int NutRegisterSpiDevice(NUTDEVICE * dev, NUTSPIBUS * bus, int cs)
{
    int rc;
    NUTSPINODE *node;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    node = (NUTSPINODE *) dev->dev_icb;

    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_initnode != NULL);

    /* Attach the bus controller driver and set the chip select number
       ** before calling the bus controller initialization routine. */
    node->node_bus = bus;
    node->node_cs = cs;
    rc = (*bus->bus_initnode) (node);

    /* If the bus had been successfully initialized for this device,
       ** then set up a mutex lock for the bus and register the device. */
    if (rc == 0) {
        NutEventPost(&bus->bus_mutex);
        rc = NutRegisterDevice(dev, 0, 0);
    }
    return rc;
}

/*! 
 * \brief Wait until all SPI bus transfers are done.
 *
 * This dummy function is used by SPI device drivers, which do not
 * provide asynchronous transfers.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo Timeout in milliseconds. To disable timeout, set this
 *            parameter to NUT_WAIT_INFINITE.
 *
 * \return Always 0.
 */
int NutSpiBusWait(NUTSPINODE * node, uint32_t tmo)
{
    return 0;
}

/*! 
 * \brief Set SPI mode of a specified bus device.
 *
 * The new mode will be used during the next transfer. If its value is 
 * SPI_CURRENT_MODE, then the mode is not updated. This can be used 
 * to query the current mode.
 *
 * Otherwise this parameter may be the or'ed combination of the
 * following bits:
 *
 * - SPI_MODE_CPHA: Data updated on leading edge.
 * - SPI_MODE_CPOL: Idle clock is high.
 * - SPI_MODE_FAULT: Enables mode fault detection. 
 * - SPI_MODE_LOOPBACK: Loopback mode.
 * - SPI_MODE_SLAVE: Slave mode.
 * - SPI_MODE_CSKEEP: Chip select remains active after transfer.
 * - SPI_MODE_CSDEC: Decoded chip selects.
 * - SPI_MODE_CSHIGH: Chip select is high active.
 *
 * Be aware, that SPI drivers may have implemented a subset only.
 *
 * Instead of SPI_MODE_CPHA and SPI_MODE_CPOL one of the following 
 * mode numbers may be used:
 *
 * - SPI_MODE_0: Idle clock is low and data is captured on the rising edge.
 * - SPI_MODE_1: Idle clock is low and data is captured on the falling edge.
 * - SPI_MODE_2: Idle clock is high and data is captured on the falling edge.
 * - SPI_MODE_3: Idle clock is high and data is captured on the rising edge.
 *
 * The return value may additionally contain the bit SPI_MODE_UPDATE,
 * which indicates that here had been no transfer since the last mode
 * update.
 *
 * \param node Specifies the SPI bus node.
 * \param mode New mode.
 *
 * \return Old mode.
 */
uint_fast16_t NutSpiBusSetMode(NUTSPINODE * node, uint_fast16_t mode)
{
    uint16_t rc = node->node_mode;

    if (mode != SPI_CURRENT_MODE) {
        node->node_mode = mode | SPI_MODE_UPDATE;
    }
    return rc;
}

/*!
 * \brief Set clock rate of a specified SPI device.
 *
 * The new clock rate will be used for the next transfer. If the given
 * rate is beyond the capabilities of the bus controller, it will
 * automatically adjusted before the transfer starts.
 *
 * \param node Specifies the SPI bus node.
 * \param rate New clock rate, given in bits per second. If the value is
 *             SPI_CURRENT_RATE, then the current rate is kept.
 *
 * \return Previous rate.
 */
uint_fast32_t NutSpiBusSetRate(NUTSPINODE * node, uint_fast32_t rate)
{
    uint32_t rc = node->node_rate;

    if (rate != SPI_CURRENT_RATE) {
        node->node_rate = rate;
        node->node_mode |= SPI_MODE_UPDATE;
    }
    return rc;
}

/*!
 * \brief Set clock rate of a specified SPI device.
 *
 * The new clock rate will be used for the next transfer. If the given
 * rate is beyond the capabilities of the bus controller, it will
 * automatically adjusted before the transfer starts.
 *
 * \param node Specifies the SPI bus node.
 * \param rate New clock rate, given in bits per second. If the value is
 *             SPI_CURRENT_RATE, then the current rate is kept.
 *
 * \return Previous rate.
 */
uint_fast8_t NutSpiBusSetBits(NUTSPINODE * node, uint_fast8_t bits)
{
    uint_fast8_t rc = node->node_bits;

    if (bits != SPI_CURRENT_BITS) {
        node->node_bits = bits;
        node->node_mode |= SPI_MODE_UPDATE;
    }
    return rc;
}
