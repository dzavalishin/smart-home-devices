/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \brief Low Level Multimedia Card Access.
 *
 * Low level MMC hardware routines for the programmable logic provided
 * on the Ethernut 3 reference design.
 *
 * These routines support SPI mode only and are required by the basic MMC 
 * driver.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2008/07/14 13:13:45  haraldkipp
 * Added delays to make slow cards working.
 *
 * Revision 1.3  2006/08/05 12:01:05  haraldkipp
 * Hard coded PLL selections replaced by configurable definitions.
 *
 * Revision 1.2  2006/01/19 18:40:47  haraldkipp
 * MMC clock rate now uses the CY2239x driver routines to calculate a
 * configurable value. Additional NOPs had been added to the SPI I/O,
 * which seems to make the driver more stable.
 *
 * Revision 1.1  2006/01/05 16:31:02  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <cfg/clock.h>

#include <sys/event.h>

#include <dev/twif.h>
#include <dev/npl.h>
#include <dev/mmcard.h>
#include <dev/cy2239x.h>
#include <dev/nplmmc.h>

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
 * \addtogroup xgNplMmc
 */
/*@{*/

#ifndef I2C_SLA_PLL
#define I2C_SLA_PLL     0x69
#endif

/*!
 * \brief Private data of NPL card interface.
 */
typedef struct _MMCDCB {
    int dcb_avail;              /*!< Card is available. */
    int dcb_changed;            /*!< Card has changed. */
} MMCDCB;

static MMCDCB mmc0_dcb;

/*!
 * \brief Initialize the card.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static int NplMmCard0Init(void)
{
    mmc0_dcb.dcb_changed = 0;
    if (mmc0_dcb.dcb_avail) {
        return 0;
    }
    return -1;
}

/*!
 * \brief Activate or deactivate chip select on card slot 0.
 *
 * \param on The card will be selected if 1, deselected if 0.
 *           Any other value can be used to query the status.
 *
 * \return Previous select status. 1 if selected, 0 otherwise.
 */
static int NplMmCard0Select(int on)
{
    int rc = (inb(NPL_XER) & NPL_MMCS) == NPL_MMCS;

    if (on == 1) {
        /*! \brief Activate negated chip select. */
        outb(NPL_XER, inb(NPL_XER) & ~NPL_MMCS);
    } else if (on == 0) {
        /*! \brief Deactivate negated chip select. */
        outb(NPL_XER, inb(NPL_XER) | NPL_MMCS);
    }
    return rc;
}

/*!
 * \brief Read the previously received byte and transmit a new one.
 *
 * \param val Byte to transmit.
 *
 * \return Last byte received.
 */
static uint8_t NplMmCard0Io(uint8_t val)
{
    uint8_t rc;
    unsigned int tmo = 255;

    while ((inb(NPL_SLR) & NPL_MMCREADY) == 0) {
        if (--tmo == 0) {
            break;
        }
    }

    _NOP(); _NOP(); _NOP(); _NOP();
    rc = inb(NPL_MMCDR);
    _NOP(); _NOP(); _NOP(); _NOP();
    outb(NPL_MMCDR, val);
    _NOP(); _NOP(); _NOP(); _NOP();

#ifdef NUTDEBUG
    putchar('[');
    if (rc != 0xFF) {
        printf("r%02X", rc);
    } else if (val != 0xFF) {
        printf("s%02X", val);
    }
    putchar(']');
#endif

    return rc;
}

/*!
 * \brief Check if card is available.
 *
 * \todo Card change should verify the card identifier. Right now
 *       any detection of removing and re-inserting a card counts
 *       as a card change.
 *
 * \return 0 if no card is detected, 1 if a card is available or 2 if
 *         a card change had been detected after the last mount.
 */
int NplMmCard0Avail(void)
{
    if (mmc0_dcb.dcb_avail) {
        if (mmc0_dcb.dcb_changed) {
            return 2;
        }
        return 1;
    }
    return 0;
}

/*!
 * \brief Check if card is write protected.
 *
 * \todo Not implemented.
 *
 * \return Always 0.
 */
int NplMmCard0WrProt(void)
{
    return 0;
}

/*!
 * \brief Card insertion interrupt routine.
 *
 * \todo A different routine is required to support multiple cards.
 *
 * \param arg Pointer to the device information structure.
 */
static void NplMmCard0InsIrq(void *arg)
{
    NplIrqDisable(&sig_MMCD);
    mmc0_dcb.dcb_avail = 1;
    mmc0_dcb.dcb_changed = 1;
    NplIrqEnable(&sig_NMMCD);
}

/*!
 * \brief Card removal interrupt routine.
 *
 * \todo A different routine is required to support multiple cards.
 *
 * \param arg Pointer to the device information structure.
 */
static void NplMmCard0RemIrq(void *arg)
{
    NplIrqDisable(&sig_NMMCD);
    mmc0_dcb.dcb_avail = 0;
    NplIrqEnable(&sig_MMCD);
}

/*!
 * \brief Initialize MMC hardware interface.
 *
 * This function is automatically executed during during device 
 * registration via NutRegisterDevice().
 *
 * \todo This routine needs some update to support multiple cards.
 * 
 * \todo PLL clock changes should be based on NPL version.
 *
 * \param dev Identifies the device to initialize.
 */
static int NplMmcIfcInit(NUTDEVICE * dev)
{
    int rc;

    /* Disable card select. */
    NplMmCard0Select(0);

#if defined(NUT_PLL_NPLCLK1)
    {
        uint32_t val;

        /* Query the PLL number routed to Clock B. */
        val = Cy2239xGetPll(NUT_PLL_NPLCLK1);
        /* Get the frequency of this PLL. */
        val = Cy2239xPllGetFreq((int)val, 7);
        /* Calculate the required divider value. */
        val = (val + NPL_MMC_CLOCK - 10) / NPL_MMC_CLOCK;
        /* 
         * Not sure about the Cy-routines. The DIVSEL bit specifies which
         * divider is used, which is indirectly connected to S2, which is
         * high by default. For now set both dividers. 
         */
        if (Cy2239xSetDivider(NUT_PLL_NPLCLK1, 1, (int)val)) {
            return -1;
        }
        if (Cy2239xSetDivider(NUT_PLL_NPLCLK1, 0, (int)val)) {
            return -1;
        }
    }
#endif

    /* Initialize the CPLD SPI register. */
    inb(NPL_MMCDR);
    outb(NPL_MMCDR, 0xFF);

    /* Register card detection interrupts. */
    if ((rc = NplRegisterIrqHandler(&sig_MMCD, NplMmCard0InsIrq, 0)) == 0) {
        rc = NplRegisterIrqHandler(&sig_NMMCD, NplMmCard0RemIrq, 0);
    }
    NplIrqEnable(&sig_MMCD);

    return MmCardDevInit(dev);
}

static MMCIFC mmc0_ifc = {
    NplMmCard0Init,             /*!< mmcifc_in */
    NplMmCard0Io,               /*!< mmcifc_io */
    NplMmCard0Select,           /*!< mmcifc_cs */
    NplMmCard0Avail,            /*!< mmcifc_cd */
    NplMmCard0WrProt            /*!< mmcifc_wp */
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
NUTDEVICE devNplMmc0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    &mmc0_ifc,                  /*!< Interface control block, dev_icb. */
    &mmc0_dcb,                  /*!< Driver control block used by the low level part, dev_dcb. */
    NplMmcIfcInit,              /*!< Driver initialization routine, dev_init. */
    MmCardIOCtl,                /*!< Driver specific control function, dev_ioctl. */
    MmCardBlockRead,            /*!< Read data from a file, dev_read. */
    MmCardBlockWrite,           /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    MmCardBlockWrite_P,         /*!< Write data from program space to a file, dev_write_P. */
#endif
    MmCardMount,                /*!< Mount a file system, dev_open. */
    MmCardUnmount,              /*!< Unmount a file system, dev_close. */
    0                           /*!< Return file size, dev_size. */
};

/*@}*/
