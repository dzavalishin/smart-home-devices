/*
 * Copyright (C) 2006 by egnite Software GmbH
 * Copyright (C) 2008 by egnite GmbH
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
 * \brief Low Level Multimedia Card Access.
 *
 * Low level MMC hardware routines for SPI emulation by software
 * (bit banging).
 *
 * These routines support SPI mode only and are required by the
 * basic MMC driver.
 *
 * \verbatim
 * $Id: sbi_mmc.c 2467 2009-02-17 09:30:02Z haraldkipp $
 * \endverbatim
 */

/*!
 * \addtogroup xgSbiMmCard
 */
/*@{*/

#if defined(MMC_CLK_PIO_BIT) && defined(MMC_CLK_PIO_ID)
#undef GPIO_ID
#define GPIO_ID MMC_CLK_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void MMC_CLK_LO(void) { GPIO_SET_LO(MMC_CLK_PIO_BIT); }
static INLINE void MMC_CLK_HI(void) { GPIO_SET_HI(MMC_CLK_PIO_BIT); }
static INLINE void MMC_CLK_EN(void) { GPIO_ENABLE(MMC_CLK_PIO_BIT); }
static INLINE void MMC_CLK_SO(void) { GPIO_OUTPUT(MMC_CLK_PIO_BIT); }
#else
#define MMC_CLK_LO()
#define MMC_CLK_HI()
#define MMC_CLK_EN()
#define MMC_CLK_SO()
#endif

#if defined(MMC_MOSI_PIO_BIT) && defined(MMC_MOSI_PIO_ID)
#undef GPIO_ID
#define GPIO_ID    MMC_MOSI_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void MMC_MOSI_LO(void) { GPIO_SET_LO(MMC_MOSI_PIO_BIT); }
static INLINE void MMC_MOSI_HI(void) { GPIO_SET_HI(MMC_MOSI_PIO_BIT); }
static INLINE void MMC_MOSI_EN(void) { GPIO_ENABLE(MMC_MOSI_PIO_BIT); }
static INLINE void MMC_MOSI_SO(void) { GPIO_OUTPUT(MMC_MOSI_PIO_BIT); }
#else
#define MMC_MOSI_LO()
#define MMC_MOSI_HI()
#define MMC_MOSI_EN()
#define MMC_MOSI_SO()
#endif

#if defined(MMC_MISO_PIO_BIT) && defined(MMC_MISO_PIO_ID)
#undef GPIO_ID
#define GPIO_ID    MMC_MISO_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE int MMC_MISO_TEST(void) { return GPIO_GET(MMC_MISO_PIO_BIT); }
static INLINE void MMC_MISO_EN(void) { GPIO_ENABLE(MMC_MISO_PIO_BIT); }
static INLINE void MMC_MISO_SI(void) { GPIO_INPUT(MMC_MISO_PIO_BIT); }
#else
#define MMC_MISO_TEST() (1)
#define MMC_MISO_EN()
#define MMC_MISO_SI()
#endif

#if defined(MMC_CS_PIO_BIT) && defined(MMC_CS_PIO_ID)
#undef GPIO_ID
#define GPIO_ID    MMC_CS_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void MMC_CS_LO(void) { GPIO_SET_LO(MMC_CS_PIO_BIT); }
static INLINE void MMC_CS_HI(void) { GPIO_SET_HI(MMC_CS_PIO_BIT); }
static INLINE int MMC_CS_IS(void) { return GPIO_IS_HI(MMC_CS_PIO_BIT); }
static INLINE void MMC_CS_EN(void) { GPIO_ENABLE(MMC_CS_PIO_BIT); }
static INLINE void MMC_CS_SO(void) { GPIO_OUTPUT(MMC_CS_PIO_BIT); }
#else
#define MMC_CS_LO()
#define MMC_CS_HI()
#define MMC_CS_IS()    (1)
#define MMC_CS_EN()
#define MMC_CS_SO()
#endif


/*!
 * \brief Private data of NPL card interface.
 */
typedef struct _MMCDCB {
    int dcb_avail;              /*!< Card is available. */
    int dcb_changed;            /*!< Card has changed. */
} MMCDCB;

static MMCDCB mmc_dcb;

/*!
 * \brief Initialize the card in slot 0.
 *
 * Called by the MMC driver during card reset. The card change
 * detection flag will be cleared.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static int SbiMmCardInit(void)
{
    mmc_dcb.dcb_changed = 0;
    if (mmc_dcb.dcb_avail) {
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
static int SbiMmCardSelect(int on)
{
    int rc = MMC_CS_IS();

    /* MMC select is low active. */
    if (on == 1) {
        MMC_CS_LO();
    } else if (on == 0) {
        MMC_CS_HI();
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
static uint8_t SbiMmCardIo(uint8_t val)
{
    uint_fast8_t msk = 0x80;

#if defined(NUTDEBUG)
    putchar('[');
    if (val != 0xFF) {
        printf("s%02X", val);
    }
#endif

    /* SPI bit banging. */
    while (msk) {
        MMC_CLK_LO();
        if (val & msk) {
            MMC_MOSI_HI();
        } else {
            MMC_MOSI_LO();
        }
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        MMC_CLK_HI();
        if (MMC_MISO_TEST()) {
            val |= msk;
        }
        else {
            val &= ~msk;
        }
        msk >>= 1;
    }

#if defined(NUTDEBUG)
    if (val != 0xFF) {
        printf("r%02X", val);
    }
    putchar(']');
#endif
    return val;
}

/*!
 * \brief Check if card is available in slot 0.
 *
 * \todo Card change should verify the card identifier. Right now
 *       any detection of removing and re-inserting a card counts
 *       as a card change.
 *
 * \return 0 if no card is detected, 1 if a card is available or 2 if
 *         a card change had been detected after the last mount.
 */
static int SbiMmCardAvail(void)
{
    if (mmc_dcb.dcb_avail) {
        if (mmc_dcb.dcb_changed) {
            return 2;
        }
        return 1;
    }
    return 0;
}

/*!
 * \brief Check if card in slot 0 is write protected.
 *
 * \todo Not implemented.
 *
 * \return Always 0.
 */
static int SbiMmCardWrProt(void)
{
    return 0;
}

/*!
 * \brief Initialize MMC hardware interface.
 *
 * This function is automatically executed during during device
 * registration via NutRegisterDevice().
 *
 * \param dev Identifies the device to initialize.
 */
static int SbiMmcIfcInit(NUTDEVICE * dev)
{
    /* Enable all clocks. */
#if defined(PMC_PCER)
#if defined(MMC_CLK_PIO_ID)
    outr(PMC_PCER, _BV(MMC_CLK_PIO_ID));
#endif
#if defined(MMC_MOSI_PIO_ID)
    outr(PMC_PCER, _BV(MMC_MOSI_PIO_ID));
#endif
#if defined(MMC_MISO_PIO_ID)
    outr(PMC_PCER, _BV(MMC_MISO_PIO_ID));
#endif
#if defined(MMC_CS_PIO_ID)
    outr(PMC_PCER, _BV(MMC_CS_PIO_ID));
#endif
#if defined(MMC_CD_PIO_ID)
    outr(PMC_PCER, _BV(MMC_CD_PIO_ID));
#endif
#if defined(MMC_WP_PIO_ID)
    outr(PMC_PCER, _BV(MMC_WP_PIO_ID));
#endif
#endif /* PMC_PCER */

    /* Set all outputs high. */
    MMC_CLK_HI();
    MMC_MOSI_HI();
    MMC_CS_HI();

    /* Enable GPIO on all lines. */
    MMC_CLK_EN();
    MMC_MOSI_EN();
    MMC_MISO_EN();
    MMC_CS_EN();

    /* Set line directions. */
    MMC_CLK_SO();
    MMC_MOSI_SO();
    MMC_MISO_SI();
    MMC_CS_SO();

    mmc_dcb.dcb_avail = 1;
    mmc_dcb.dcb_changed = 0;

    return MmCardDevInit(dev);
}

static MMCIFC mmc_ifc = {
    SbiMmCardInit,             /*!< mmcifc_in */
    SbiMmCardIo,               /*!< mmcifc_io */
    SbiMmCardSelect,           /*!< mmcifc_cs */
    SbiMmCardAvail,            /*!< mmcifc_cd */
    SbiMmCardWrProt            /*!< mmcifc_wp */
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
NUTDEVICE devSbiMmCard = {
    0,                  /*!< Pointer to next device, dev_next. */
    MMC_DEV_NAME,       /*!< Unique device name, dev_name. */
    0,                  /*!< Type of device, dev_type. Obsolete. */
    0,                  /*!< Base address, dev_base. Unused. */
    0,                  /*!< First interrupt number, dev_irq. Unused. */
    &mmc_ifc,           /*!< Interface control block, dev_icb. */
    &mmc_dcb,           /*!< Driver control block used by the low level part, dev_dcb. */
    SbiMmcIfcInit,      /*!< Driver initialization routine, dev_init. */
    MmCardIOCtl,        /*!< Driver specific control function, dev_ioctl. */
    MmCardBlockRead,    /*!< Read data from a file, dev_read. */
    MmCardBlockWrite,   /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    MmCardBlockWrite_P, /*!< Write data from program space to a file, dev_write_P. */
#endif
    MmCardMount,        /*!< Mount a file system, dev_open. */
    MmCardUnmount,      /*!< Unmount a file system, dev_close. */
    0                   /*!< Return file size, dev_size. */
};

/*@}*/
