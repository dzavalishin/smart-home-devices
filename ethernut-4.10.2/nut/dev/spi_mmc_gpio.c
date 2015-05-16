/*
 * Copyright (C) 2011 by Thermotemp GmbH
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
 * \brief Low Level Multimedia Card Support.
 *
 * MMC support routines for gpio usage
 *
 * \verbatim
 *
 * $Id$
 *
 * \endverbatim
 */

#include <cfg/clock.h>
#include <cfg/mmci.h>

#include <dev/mmcard.h>
#include <dev/spibus.h>
#include <dev/gpio.h>

#ifdef MMC_CD_PORT
    #if (MMC_CD_PORT==NUTGPIO_PORTA)
        #define MMC_CD_SIGNAL  sig_GPIO1
    #elif (MMC_CD_PORT==NUTGPIO_PORTB)
        #define MMC_CD_SIGNAL  sig_GPIO2
    #elif (MMC_CD_PORT==NUTGPIO_PORTC)
        #define MMC_CD_SIGNAL  sig_GPIO3
    #endif
#endif

#if !defined(SPI_MMC_CLK)
#define SPI_MMC_CLK   33000000
#endif

#if !defined(SPI_MMC_CS)
#define SPI_MMC_CS    1
#endif


/*!
 * \addtogroup xgSPIBusMmcSupport
 */

/*@{*/

static int  SpiMmcGpioReset(NUTDEVICE * dev);
static void SpiMmcGpioLed(int mode);
static int  SpiMmcGpioPower(int mode);

/*!
 * \brief Memory card support structure for MMC with GPIO support for controle lines.
 */
static MEMCARDSUPP mcsSpiMmcGpio = {
    0,                  /*!< \brief Status change flag (mcs_cf). */
    0,                  /*!< \brief Socket status (mcs_sf). */
    SpiMmcGpioReset,    /*!< \brief Status reset (mcs_reset). */
    SpiMmcGpioLed,      /*!< \brief Set activity indicator (mcs_act). */
    SpiMmcGpioPower,    /*!< \brief Power control (mcs_power). */
};

#if defined(MMC_CD_PORT) && defined(MMC_CD_PIN)
/*!
 * \brief Card insertion and removal interrupt routine.
 *
 * \param arg Pointer to the memory card support structure.
 */
static void SpiMmcGpioIrq(void *arg)
{
    MEMCARDSUPP *mcs = (MEMCARDSUPP *) arg;

    if (GpioPinGet(MMC_CD_PORT, MMC_CD_PIN) == 0) {
        /* Card was inserted */

        /* Set the change flag. */
        mcs->mcs_cf = 1;
        /* Set the card detect flag. */
        mcs->mcs_sf = NUTMC_SF_CD; 

#if defined(MMC_WP_PORT) && defined(MMC_WP_PIN)
        if (GpioPinGet(MMC_WP_PORT, MMC_WP_PIN) == 0) {
            mcs->mcs_sf |= NUTMC_SF_WP
        }
#endif
    } else {
        /* Card was removed */

        /* Set the change flag. */
        mcs->mcs_cf = 1;
        /* Clear card detect and write protect flags. */
        mcs->mcs_sf = 0;
    }
}
#endif

/*!
 * \brief Card activity indicator control.
 *
 * Not implemented.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static void SpiMmcGpioLed(int mode)
{
    if (mode == NUTMC_IND_OFF) {
        // switch off LED
    } else {
        // switch on LED
    }
}

/*!
 * \brief Card 0 power control.
 *
 * Not implemented.
 *
 * \param mode If zero, the card will be put into power down mode.
 *             Otherwise the card will be activated.
 *
 * \return Always -1.
 */
static int SpiMmcGpioPower(int mode)
{
    return -1;
}

/*!
 * \brief Card socket 0 reset.
 *
 * \return Always 0.
 */
static int SpiMmcGpioReset(NUTDEVICE * dev)
{
    NUTSPINODE * node;
    NUTSPIBUS  * bus;

    int rc;
    
    node = (NUTSPINODE *) dev->dev_icb;
    bus = (NUTSPIBUS *) node->node_bus;
    
    /*
     * 80 bits of ones with deactivated chip select will put the card 
     * in SPI mode.
     */
    node->node_mode |= SPI_MODE_CSHIGH;
    rc = (bus->bus_alloc) (node, 1000);
    if (rc == 0) {       
        uint8_t txb[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        (bus->bus_transfer) (node, &txb, NULL, 10);
    }
    node->node_mode &= ~SPI_MODE_CSHIGH;
    (bus->bus_release) (node);

    /* Switch LED off. */
    SpiMmcGpioLed(NUTMC_IND_OFF);

    return 0;
}

/*!
 * \brief Initialize MMC driver for socket 0.
 *
 * This function is automatically executed during during device 
 * registration via NutRegisterDevice().
 *
 * \param dev Identifies the device to initialize.
 */
static int SpiMmcGpioInit(NUTDEVICE * dev)
{
    int rc;

    SpiMmcGpioReset(dev);

#if defined(MMC_WP_PORT) && defined(MMC_WP_PIN)
    /* Configure write protect detection pin as input with pullups enabled */
    GpioPinConfigSet(MMC_WP_PORT, MMC_WP_PIN, GPIO_CFG_PULLUP);
#endif    
    
#if defined(MMC_CD_PORT) && defined(MMC_CD_PIN)
    /* Configure card detect pin as input with pullups enabled */
    GpioPinConfigSet(MMC_CD_PORT, MMC_CD_PIN, GPIO_CFG_PULLUP);

    /* Register card detection interrupt. */
   
    rc = GpioRegisterIrqHandler(&MMC_CD_SIGNAL, MMC_CD_PIN, SpiMmcGpioIrq, dev->dev_dcb);
    if (rc == 0) {
        GpioIrqEnable(&MMC_CD_SIGNAL, MMC_CD_PIN);
        /* Initialize the SPI interface. */
        rc = SpiMmcInit(dev);
    }
    /* call the IRQ handler routine to check if a card is available just right now */
    SpiMmcGpioIrq(dev->dev_dcb);
#else
    /* No card detect pin is defined, so we assume that the card is always present */
    rc = SpiMmcInit(dev);

    /* Set the change flag. */
    ((MEMCARDSUPP *)dev->dev_dcb)->mcs_cf = 1;
    /* Set the card detect flag. */
    ((MEMCARDSUPP *)dev->dev_dcb)->mcs_sf = NUTMC_SF_CD; 

#if defined(MMC_WP_PORT) && defined(MMC_WP_PIN)
    if (GpioPinGet(MMC_WP_PORT, MMC_WP_PIN) == 0) {
        ((MEMCARDSUPP *)dev->dev_dcb)->mcs_sf |= NUTMC_SF_WP
    }
#endif
    
#endif
    return rc;
}

/*!
 * \brief NPL SPI node implementation structure for MMC.
 */
NUTSPINODE nodeSpiMmcGpio = {
    NULL,           /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,           /*!< \brief Pointer to device driver specific settings, node_stat. */
    SPI_MMC_CLK,    /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_0,     /*!< \brief Initial mode, node_mode. */
    8,              /*!< \brief Initial data bits, node_bits. */
    SPI_MMC_CS      /*!< \brief Chip select, node_cs. */
};

/*!
 * \brief Multimedia card device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this driver to the Nut/OS kernel. An application may then 
 * call
 * /verbatim
 * _open("MMC0:", _O_RDWR | _O_BINARY);
 * /endverbatim
 * to mount the first active primary partition with any previously 
 * registered file system driver (typically devPhat0).
 */
NUTDEVICE devSpiMmcGpio = {
    0,                  /*!< Pointer to next device, dev_next. */
    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}
    ,                   /*!< Unique device name, dev_name. */
    0,                  /*!< Type of device, dev_type. Obsolete. */
    0,                  /*!< Base address, dev_base. Unused. */
    0,                  /*!< First interrupt number, dev_irq. Unused. */
    &nodeSpiMmcGpio,    /*!< Interface control block, dev_icb. */
    &mcsSpiMmcGpio,     /*!< Driver control block used by the low level part, dev_dcb. */
    SpiMmcGpioInit,     /*!< Driver initialization routine, dev_init. */
    SpiMmcIOCtl,        /*!< Driver specific control function, dev_ioctl. */
    SpiMmcBlockRead,    /*!< Read data from a file, dev_read. */
    SpiMmcBlockWrite,   /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    SpiMmcBlockWrite_P, /*!< Write data from program space to a file, dev_write_P. */
#endif
    SpiMmcMount,        /*!< Mount a file system, dev_open. */
    SpiMmcUnmount,      /*!< Unmount a file system, dev_close. */
    0                   /*!< Return file size, dev_size. */
};

/*@}*/
