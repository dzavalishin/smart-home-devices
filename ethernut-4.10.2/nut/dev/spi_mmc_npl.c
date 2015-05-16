/*
 * Copyright (C) 2005 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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
 * MMC support routines for the programmable logic provided
 * on the Ethernut 3 reference design.
 *
 * \verbatim
 *
 * $Id$
 *
 * \endverbatim
 */

#include <cfg/clock.h>
#include <cfg/mmci.h>

#include <dev/npl.h>
#include <dev/mmcard.h>
#include <dev/spibus.h>

#if !defined(NPL_MMC_CLOCK) || (NPL_MMC_CLOCK < 1000)
#undef NPL_MMC_CLOCK
#define NPL_MMC_CLOCK   12500000
#endif

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgNplMemCardSupport
 */
/*@{*/

static int NplMmc0Reset(NUTDEVICE * dev);
static void NplMmc0Led(int mode);
static int NplMmc0Power(int mode);

/*!
 * \brief NPL memory card support structure for MMC0.
 */
static MEMCARDSUPP mcsSpiMmc0Npl = {
    0,              /*!< \brief Status change flag (mcs_cf). */
    0,              /*!< \brief Socket status (mcs_sf). */
    NplMmc0Reset,   /*!< \brief Status reset (mcs_reset). */
    NplMmc0Led,     /*!< \brief Set activity indicator (mcs_act). */
    NplMmc0Power,   /*!< \brief Power control (mcs_power). */
};

/*!
 * \brief Card insertion interrupt routine for socket 0.
 *
 * \param arg Pointer to the memory card support structure.
 */
static void NplMmc0Insertion(void *arg)
{
    MEMCARDSUPP *mcs = (MEMCARDSUPP *) arg;

    /* Set the change flag. */
    mcs->mcs_cf = 1;
    /* Set the card detect flag. */
    mcs->mcs_sf = NUTMC_SF_CD;
    /* Set the card write protect flag. */
    if (inw(NPL_SLR) & NPL_LANWAKEUP) {
        mcs->mcs_sf |= NUTMC_SF_WP;
    }

    /* Disable insert and enable remove interrupts. */
    NplIrqDisable(&sig_MMCD);
    NplIrqEnable(&sig_NMMCD);
}

/*!
 * \brief Card removal interrupt routine.
 *
 * \param arg Pointer to the memory card support structure.
 */
static void NplMmcRemoval(void *arg)
{
    MEMCARDSUPP *mcs = (MEMCARDSUPP *) arg;

    /* Set the change flag. */
    mcs->mcs_cf = 1;
    /* Clear card detect and write protect flags. */
    mcs->mcs_sf = 0;

    /* Disable remove and enable insert interrupts. */
    NplIrqDisable(&sig_NMMCD);
    NplIrqEnable(&sig_MMCD);
}

/*!
 * \brief Card activity indicator control.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static void NplMmc0Led(int mode)
{
#ifdef NPL_MMC0_ULED
    if (mode == NUTMC_IND_OFF) {
        outb(NPL_XER, inb(NPL_XER) | NPL_USRLED);
    } else {
        outb(NPL_XER, inb(NPL_XER) & ~NPL_USRLED);
    }
#endif
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
static int NplMmc0Power(int mode)
{
    return -1;
}

/*!
 * \brief Card socket 0 reset.
 *
 * \return Always 0.
 */
static int NplMmc0Reset(NUTDEVICE * dev)
{
    uint_fast8_t i;

    /* Deactivate negated chip select. */
    outb(NPL_XER, inb(NPL_XER) | NPL_MMCS);
    /* Perform 80 clock cycles. */
    for (i = 0; i < 10; i++) {
        outb(NPL_MMCDR, 0xFF);
        while ((inb(NPL_SLR) & NPL_MMCREADY) == 0);
        inb(NPL_MMCDR);
    }
    /* Switch LED off. */
    NplMmc0Led(NUTMC_IND_OFF);

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
static int SpiMmc0NplInit(NUTDEVICE * dev)
{
    int rc;

    NplMmc0Reset(dev);

    /* Register card detection interrupts. */
    rc = NplRegisterIrqHandler(&sig_MMCD, NplMmc0Insertion, dev->dev_dcb);
    if (rc == 0) {
        rc = NplRegisterIrqHandler(&sig_NMMCD, NplMmcRemoval, dev->dev_dcb);
        if (rc == 0) {
            NplIrqEnable(&sig_MMCD);
            /* Initialize the SPI interface. */
            rc = SpiMmcInit(dev);
        }
    }
    return rc;
}

/*!
 * \brief NPL SPI node implementation structure for MMC0.
 */
NUTSPINODE nodeSpiMmc0Npl = {
    NULL,           /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,           /*!< \brief Pointer to device driver specific settings, node_stat. */
    NPL_MMC_CLOCK,  /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_0,     /*!< \brief Initial mode, node_mode. */
    8,              /*!< \brief Initial data bits, node_bits. */
    0               /*!< \brief Chip select, node_cs. */
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
NUTDEVICE devNplSpiMmc0 = {
    0,                  /*!< Pointer to next device, dev_next. */
    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}
    ,                   /*!< Unique device name, dev_name. */
    0,                  /*!< Type of device, dev_type. Obsolete. */
    0,                  /*!< Base address, dev_base. Unused. */
    0,                  /*!< First interrupt number, dev_irq. Unused. */
    &nodeSpiMmc0Npl,    /*!< Interface control block, dev_icb. */
    &mcsSpiMmc0Npl,     /*!< Driver control block used by the low level part, dev_dcb. */
    SpiMmc0NplInit,     /*!< Driver initialization routine, dev_init. */
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
