/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * Low level MMC hardware routines, tested on the AT91SAM7X.
 *
 * These routines support SPI mode only and are required by the
 * basic MMC driver.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.7  2010/12/15 15:14:13  ve2yag
 * Add support for all SAM7S and 7SE family.
 *
 * Revision 1.6  2008/08/11 06:59:13  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2008/08/06 12:51:01  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.4  2008/02/15 16:59:02  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.3  2008/01/31 09:16:03  haraldkipp
 * Failed to compile for SAM7S. Added some default settings.
 *
 * Revision 1.2  2006/09/05 12:27:55  haraldkipp
 * Added support for the AT91SAM9260.
 *
 * Revision 1.1  2006/07/26 11:20:08  haraldkipp
 * Added MMC/SD-Card support for AT91SAM7X Evaluation Kit.
 *
 *
 * \endverbatim
 */

#include <cfg/arch.h>
#include <cfg/arch/gpio.h>

#include <dev/mmcard.h>
#include <dev/spimmc_at91.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgAt91SpiMmc
 */
/*@{*/

#if defined(MCU_AT91SAM9G45)

#ifndef MMC_CS_BIT
#define MMC_CS_BIT      PB3_SPI0_NPCS0_A
#endif
#define MMC_DATAOUT_BIT PB0_SPI0_MISO_A
#define MMC_DATAIN_BIT  PB1_SPI0_MOSI_A
#define MMC_CLK_BIT     PB2_SPI0_SPCK_A

#define MMC_PIO_PDR     PIOB_PDR

#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)

#ifndef MMC_CS_BIT
#define MMC_CS_BIT      PA3_SPI0_NPCS0_A
#endif
#define MMC_DATAOUT_BIT PA0_SPI0_MISO_A
#define MMC_DATAIN_BIT  PA1_SPI0_MOSI_A
#define MMC_CLK_BIT     PA2_SPI0_SPCK_A

#elif defined(MCU_AT91SAM7X)

#ifndef MMC_CS_BIT
#define MMC_CS_BIT      SPI0_NPCS1_PA13A
#endif
#define MMC_DATAOUT_BIT SPI0_MISO_PA16A
#define MMC_DATAIN_BIT  SPI0_MOSI_PA17A
#define MMC_CLK_BIT     SPI0_SPCK_PA18A

//#elif defined(MCU_AT91SAM7S256) || defined(MCU_AT91SAM7SE512)
#elif defined(MCU_AT91SAM7S) || defined(MCU_AT91SAM7SE)

#ifndef MMC_CS_BIT
#define MMC_CS_BIT      SPI0_NPCS0_PA11A
#endif
#define MMC_DATAOUT_BIT SPI0_MISO_PA12A
#define MMC_DATAIN_BIT  SPI0_MOSI_PA13A
#define MMC_CLK_BIT     SPI0_SPCK_PA14A

#else   /* MCU_AT91SAM7X */
#warning "MMC SPI mode not supported on this MCU"
#endif

#ifndef MMC_PIO_ASR
#define MMC_PIO_ASR     PIOA_ASR
#endif

#ifndef MMC_PIO_BSR
#define MMC_PIO_BSR     PIOA_BSR
#endif

#ifndef MMC_PIO_PDR
#define MMC_PIO_PDR     PIOA_PDR
#endif

#ifndef MMC_PINS_A
#define MMC_PINS_A      (_BV(MMC_DATAOUT_BIT) | _BV(MMC_DATAIN_BIT) | _BV(MMC_CLK_BIT))
#endif

#ifndef MMC_PINS_B
#define MMC_PINS_B      0
#endif

#ifndef MMC_CS_PER
#define MMC_CS_PER      PIOA_PER
#endif

#ifndef MMC_CS_OER
#define MMC_CS_OER      PIOA_OER
#endif

#ifndef MMC_CS_SODR
#define MMC_CS_SODR     PIOA_SODR
#endif

#ifndef MMC_CS_CODR
#define MMC_CS_CODR     PIOA_CODR
#endif

#ifndef MMC_SPI_CR
#define MMC_SPI_CR      SPI0_CR
#endif

#ifndef MMC_SPI_MR
#define MMC_SPI_MR      SPI0_MR
#endif

#ifndef MMC_SPI_RDR
#define MMC_SPI_RDR     SPI0_RDR
#endif

#ifndef MMC_SPI_TDR
#define MMC_SPI_TDR     SPI0_TDR
#endif

#ifndef MMC_SPI_SR
#define MMC_SPI_SR      SPI0_SR
#endif

#ifndef MMC_SPI_CSR1
#define MMC_SPI_CSR1    SPI0_CSR1
#endif

#ifndef MMC_SPI_ID
#define MMC_SPI_ID      SPI0_ID
#endif


/*!
 * \brief Initialize the card in slot 0.
 *
 * Called by the MMC driver during card reset. The card change
 * detection flag will be cleared.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static int At91SpiMmCard0Init(void)
{
    return 0;
}

/*!
 * \brief Activate or deactivate chip select on card slot 0.
 *
 * \param on The card will be selected if 1, deselected if 0.
 *           Any other value can be used to query the status.
 *
 * \return Previous select status. 1 if selected, 0 otherwise.
 */
static int At91SpiMmCard0Select(int on)
{
    int rc = (inr(PIOA_ODSR) & _BV(MMC_CS_BIT)) == 0;

    /* MMC select is low active. */
    if (on == 1) {
        outr(MMC_CS_CODR, _BV(MMC_CS_BIT));
    } else if (on == 0) {
        outr(MMC_CS_SODR, _BV(MMC_CS_BIT));
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
static uint8_t At91SpiMmCard0Io(uint8_t val)
{
#ifdef NUTDEBUG
    putchar('[');
    if (val != 0xFF) {
        printf("s%02X", val);
    }
#endif

    /* Transmission is started by writing the transmit data. */
    outr(MMC_SPI_TDR, val);
    /* Wait for receiver data register full. */
    while((inr(MMC_SPI_SR) & SPI_RDRF) == 0);
    /* Read data. */
    val = (uint8_t)inr(MMC_SPI_RDR);

#ifdef NUTDEBUG
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
int At91SpiMmCard0Avail(void)
{
    return 1;
}

/*!
 * \brief Check if card in slot 0 is write protected.
 *
 * \todo Not implemented.
 *
 * \return Always 0.
 */
int At91SpiMmCard0WrProt(void)
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
static int At91SpiMmcIfcInit(NUTDEVICE * dev)
{
    /* Disable PIO lines used for SPI. */
    outr(MMC_PIO_PDR, MMC_PINS_A | MMC_PINS_B);
    /* Enable peripherals. */
    outr(MMC_PIO_ASR, MMC_PINS_A);
    outr(MMC_PIO_BSR, MMC_PINS_B);

    /* MMC chip select is manually controlled. */
    outr(MMC_CS_PER, _BV(MMC_CS_BIT));
	outr(MMC_CS_SODR, _BV(MMC_CS_BIT));
	outr(MMC_CS_OER, _BV(MMC_CS_BIT));

    /* Enable SPI clock. */
    outr(PMC_PCER, _BV(MMC_SPI_ID));

    /* SPI enable and reset. */
    outr(MMC_SPI_CR, SPI_SPIEN | SPI_SWRST);
    outr(MMC_SPI_CR, SPI_SPIEN);

    /* Set SPI to master mode, fixed peripheral at CS1, fault detection disabled. */
    outr(MMC_SPI_MR, (1 << SPI_PCS_LSB) | SPI_MODFDIS | SPI_MSTR);

    /* Data changes during clock low and will be sampled on rising edges. */
    outr(MMC_SPI_CSR1, (3 << SPI_SCBR_LSB) | SPI_CPOL);

    return MmCardDevInit(dev);
}

static MMCIFC mmc0_ifc = {
    At91SpiMmCard0Init,             /*!< mmcifc_in */
    At91SpiMmCard0Io,               /*!< mmcifc_io */
    At91SpiMmCard0Select,           /*!< mmcifc_cs */
    At91SpiMmCard0Avail,            /*!< mmcifc_cd */
    At91SpiMmCard0WrProt            /*!< mmcifc_wp */
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
NUTDEVICE devAt91SpiMmc0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    &mmc0_ifc,                  /*!< Interface control block, dev_icb. */
    0,                          /*!< Driver control block used by the low level part, dev_dcb. */
    At91SpiMmcIfcInit,          /*!< Driver initialization routine, dev_init. */
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
