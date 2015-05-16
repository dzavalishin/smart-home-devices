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
 * Low level MMC hardware routines for SPI emulation by software
 * (bit banging).
 *
 * These routines support SPI mode only and are required by the
 * basic MMC driver.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.7  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2006/06/28 17:22:34  haraldkipp
 * Make it compile for AT91SAM7X256.
 *
 * Revision 1.5  2006/04/07 12:48:41  haraldkipp
 * Port configuration is now honored.
 *
 * Revision 1.4  2006/03/16 19:07:36  haraldkipp
 * Disabled inline assembly routine for ARM bit banging SPI. Stopped working
 * with optimizing compiler.
 *
 * Revision 1.3  2006/03/02 19:58:16  haraldkipp
 * Too lazy to port the inline assembly to ICCARM.
 *
 * Revision 1.2  2006/02/28 02:16:12  hwmaier
 * Added macro definition MCU_AT90CAN128
 *
 * Revision 1.1  2006/02/23 15:38:03  haraldkipp
 * MMC low level bit banging SPI added. Compiles on all supported platforms,
 * but tested with AT91 only.
 *
 *
 * \endverbatim
 */

#include <cfg/arch.h>
#include <cfg/arch/gpio.h>

#include <sys/event.h>
#include <dev/irqreg.h>
#include <dev/mmcard.h>
#include <dev/sbimmc.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgSbiMmc
 */
/*@{*/

#ifndef SPI0_CS_BIT
/*! \brief Port bit for SPI chip select. */
#define SPI0_CS_BIT      6
#endif

#ifndef SPI0_CLK_BIT
/*! \brief Port bit for SPI clock. */
#define SPI0_CLK_BIT     4
#endif

#ifndef SPI0_MOSI_BIT
/*! \brief Port bit for SPI MOSI line. */
#define SPI0_MOSI_BIT    5
#endif

#ifndef SPI0_MISO_BIT
/*! \brief Port bit for SPI MISO line. */
#define SPI0_MISO_BIT    3
#endif

#if defined(MCU_AT91R40008) || defined(MCU_AT91SAM7X) /* MCU ---------------------- */

#ifndef SPI0_PE_REG
/*! \brief SPI port enable register.
 */
#define SPI0_PE_REG  PIO_PER
#endif

#ifndef SPI0_PD_REG
/*! \brief SPI port disable register.
 */
#define SPI0_PD_REG  PIO_PDR
#endif


#ifndef SPI0_OE_REG
/*! \brief SPI output enable register. */
#define SPI0_OE_REG  PIO_OER
#endif

#ifndef SPI0_OD_REG
/*! \brief SPI output disable register. */
#define SPI0_OD_REG  PIO_ODR
#endif

#ifndef SPI0_SOD_REG
/*! \brief SPI set output data register. */
#define SPI0_SOD_REG PIO_SODR
#endif

#ifndef SPI0_COD_REG
/*! \brief SPI clear output data register. */
#define SPI0_COD_REG PIO_CODR
#endif

#ifndef SPI0_PDS_REG
/*! \brief SPI port data status register. */
#define SPI0_PDS_REG PIO_PDSR
#endif

#ifndef SPI0_ODS_REG
/*! \brief SPI port output status register. */
#define SPI0_ODS_REG PIO_ODSR
#endif

#ifdef MMC0_CD_BIT
#if MMC0_CD_BIT == 9
#define SIG_MMC0DETECT   sig_INTERRUPT0
#elif MMC0_CD_BIT == 10
#define SIG_MMC0DETECT   sig_INTERRUPT1
#elif MMC0_CD_BIT == 11
#define SIG_MMC0DETECT   sig_INTERRUPT2
#else
#warning "No external interrupt for card detect"
#undef MMC0_CD_BIT
#endif
#endif

#elif defined(MCU_ATMEGA2561) || defined(MCU_AT90CAN128) || defined(MCU_ATMEGA128) || defined(MCU_ATMEGA103) /* MCU */

#ifndef inr
#define inr(a)  inb(a)
#endif
#ifndef outr
#define outr(a, v)  outb(a, v)
#endif

#ifndef SPI0_OE_REG
#if SPI0_PORT == AVRPORTA
#define SPI0_OE_REG  DDRA
#elif  SPI0_PORT == AVRPORTC
#define SPI0_OE_REG  DDRC
#elif  SPI0_PORT == AVRPORTD
#define SPI0_OE_REG  DDRD
#elif  SPI0_PORT == AVRPORTE
#define SPI0_OE_REG  DDRE
#elif  SPI0_PORT == AVRPORTF
#define SPI0_OE_REG  DDRF
#elif  SPI0_PORT == AVRPORTG
#define SPI0_OE_REG  DDRG
#else
/*! \brief SPI port enable register. */
#define SPI0_OE_REG  DDRB
#endif
#endif

#ifndef SPI0_SOD_REG
#if SPI0_PORT == AVRPORTA
#define SPI0_SOD_REG  PORTA
#elif  SPI0_PORT == AVRPORTC
#define SPI0_SOD_REG  PORTC
#elif  SPI0_PORT == AVRPORTD
#define SPI0_SOD_REG  PORTD
#elif  SPI0_PORT == AVRPORTE
#define SPI0_SOD_REG  PORTE
#elif  SPI0_PORT == AVRPORTF
#define SPI0_SOD_REG  PORTF
#elif  SPI0_PORT == AVRPORTG
#define SPI0_SOD_REG  PORTG
#else
/*! \brief SPI port output register. */
#define SPI0_SOD_REG PORTB
#endif
#endif

#ifndef SPI0_PDS_REG
#if SPI0_PORT == AVRPORTA
#define SPI0_PDS_REG  PINA
#elif  SPI0_PORT == AVRPORTC
#define SPI0_PDS_REG  PINC
#elif  SPI0_PORT == AVRPORTD
#define SPI0_PDS_REG  PIND
#elif  SPI0_PORT == AVRPORTE
#define SPI0_PDS_REG  PINE
#elif  SPI0_PORT == AVRPORTF
#define SPI0_PDS_REG  PINF
#elif  SPI0_PORT == AVRPORTG
#define SPI0_PDS_REG  PING
#else
/*! \brief SPI port input register. */
#define SPI0_PDS_REG PINB
#endif
#endif

#elif !defined(SPI0_OE_REG) || !defined(SPI0_SOD_REG) || !defined(SPI0_PDS_REG)
#warning "No SPI bit banging registers for unknown CPU."
#endif  /* MCU ---------------------- */

#ifdef SPI0_COD_REG
/*! \brief Set specified MMC line low. */
#define MMC0_CLR_BIT(n)  outr(SPI0_COD_REG, _BV(n))
/*! \brief Set specified MMC line high. */
#define MMC0_SET_BIT(n)  outr(SPI0_SOD_REG, _BV(n))
#else
#define MMC0_CLR_BIT(n)  outr(SPI0_SOD_REG, inr(SPI0_SOD_REG) & ~_BV(n))
#define MMC0_SET_BIT(n)  outr(SPI0_SOD_REG, inr(SPI0_SOD_REG) | _BV(n))
#endif

#ifdef SPI0_ODS_REG
/*! \brief Query status of a specified MMC output. */
#define MMC0_IS_BIT_SET(n)   ((inr(SPI0_ODS_REG) & _BV(n)) == _BV(n))
#else
#define MMC0_IS_BIT_SET(n)   ((inr(SPI0_SOD_REG) & _BV(n)) == _BV(n))
#endif

/*! \brief Query status of a specified MMC input. */
#define MMC0_TST_BIT(n)  ((inr(SPI0_PDS_REG) & _BV(n)) == _BV(n))


/*!
 * \brief Private data of NPL card interface.
 */
typedef struct _MMCDCB {
    int dcb_avail;              /*!< Card is available. */
    int dcb_changed;            /*!< Card has changed. */
} MMCDCB;

static MMCDCB mmc0_dcb;

/*!
 * \brief Initialize the card in slot 0.
 *
 * Called by the MMC driver during card reset. The card change
 * detection flag will be cleared.
 *
 * \return 0 on success, -1 if no card is detected.
 */
static int SbiMmCard0Init(void)
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
static int SbiMmCard0Select(int on)
{
    int rc = MMC0_IS_BIT_SET(SPI0_CS_BIT);

    /* MMC select is low active. */
    if (on == 1) {
        MMC0_CLR_BIT(SPI0_CS_BIT);
    } else if (on == 0) {
        MMC0_SET_BIT(SPI0_CS_BIT);
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
static uint8_t SbiMmCard0Io(uint8_t val)
{
    uint8_t msk = 0x80;

#ifdef NUTDEBUG
    putchar('[');
    if (val != 0xFF) {
        printf("s%02X", val);
    }
#endif

#if defined(MCU_AT91R40008) && defined(__GNUC__) && 0
    /* ARM assembly version, tested on AT91R40008 only. */
    asm volatile (
        "\nspi_tran_l:\n\t"
        "str     %7, [%3, %5]"  "\n\t"  /* SCK low. */
        "tst     %0, %2"        "\n\t"  /* Check data bit. */
        "strne   %8, [%3, %4]"  "\n\t"  /* MOSI high. */
        "streq   %8, [%3, %5]"  "\n\t"  /* MOSI low. */
        "bic     %0, %1, %2"    "\n\t"  /* Clear data bit */
        "str     %7, [%3, %4]"  "\n\t"  /* SCK high. */
        "ldr     r1, [%3, %6]"  "\n\t"  /* Read MISO */
        "tst     r1, %9"        "\n\t"  /* Test MISO */
        "orrne   %0, %1, %2"    "\n\t"  /* Set data bit if MOSI was high */
        "movs    %2, %2, lsr #1""\n\t"  /* msk <<= 1 */
        "bne     spi_tran_l"            /* Loop for next bit. */
        : "=r" (val)            /* %0 output */
        : "0" (val)             /* %1 input */
        , "r" (msk)             /* %2 input */
        , "r" (PIO_BASE)        /* %3 input */
        , "J" (PIO_SODR - PIO_BASE)    /* %4 input */
        , "J" (PIO_CODR - PIO_BASE)    /* %5 input */
        , "J" (PIO_PDSR - PIO_BASE)    /* %6 input */
        , "r" _BV(SPI0_CLK_BIT)         /* %7 input */
        , "r" _BV(SPI0_MOSI_BIT)        /* %8 input */
        , "I" _BV(SPI0_MISO_BIT)        /* %9 input */
        : "r1"
        );
#else
    /* Generic code. */
    while (msk) {
        MMC0_CLR_BIT(SPI0_CLK_BIT);
        if (val & msk) {
            MMC0_SET_BIT(SPI0_MOSI_BIT);
        } else {
            MMC0_CLR_BIT(SPI0_MOSI_BIT);
        }
        MMC0_SET_BIT(SPI0_CLK_BIT);
        if (MMC0_TST_BIT(SPI0_MISO_BIT)) {
            val |= msk;
        }
        else {
            val &= ~msk;
        }
        msk >>= 1;
    }
#endif

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
int SbiMmCard0Avail(void)
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
 * \brief Check if card in slot 0 is write protected.
 *
 * \todo Not implemented.
 *
 * \return Always 0.
 */
int SbiMmCard0WrProt(void)
{
    return 0;
}

#ifdef MMC0_CD_BIT
/*!
 * \brief Card slot 0 insertion interrupt routine.
 *
 * \todo A different routine is required to support multiple cards.
 *
 * \param arg Pointer to the device information structure.
 */
static void SbiMmCard0DetectIrq(void *arg)
{
    int mode = NutIrqSetMode(&SIG_MMC0DETECT, NUT_IRQMODE_LOWLEVEL);

    if (mode == NUT_IRQMODE_HIGHLEVEL) {
        mmc0_dcb.dcb_avail = 0;
    }
    else {
        NutIrqSetMode(&SIG_MMC0DETECT, NUT_IRQMODE_HIGHLEVEL);
        mmc0_dcb.dcb_avail = 1;
    }
    mmc0_dcb.dcb_changed = 1;
}
#endif

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
#ifdef SPI0_PE_REG
    /* Enable all SPI ports, if a port enable register is defined. */
    outr(SPI0_PE_REG, _BV(SPI0_CLK_BIT) | _BV(SPI0_MOSI_BIT) | _BV(SPI0_CS_BIT) | _BV(SPI0_MISO_BIT));
#endif

#ifdef SPI0_OD_REG
    /* Enable CLK, MOSI and CS output pins. */
    outr(SPI0_OE_REG, _BV(SPI0_CLK_BIT) | _BV(SPI0_MOSI_BIT) | _BV(SPI0_CS_BIT));
    /* Enable MISO input pin. */
    outr(SPI0_OD_REG, _BV(SPI0_MISO_BIT));
#else
    /*
     * If this CPU hasn't got a specific output disable register, we
     * assume that inputs are enabled by clearing the corresponding bit
     * in the output enable register.
     */
    outr(SPI0_OE_REG, (inr(SPI0_OE_REG) & ~_BV(SPI0_MISO_BIT))
        | _BV(SPI0_CLK_BIT) | _BV(SPI0_MOSI_BIT) | _BV(SPI0_CS_BIT));
#endif

    /* Set all outputs high. */
    outr(SPI0_SOD_REG, _BV(SPI0_CLK_BIT) | _BV(SPI0_MOSI_BIT) | _BV(SPI0_CS_BIT));

#ifdef MMC0_CD_BIT
#ifdef SPI0_PD_REG
    outr(SPI0_PD_REG, _BV(MMC0_CD_BIT));
#endif

    /* Register card detection interrupts. */
    if (NutRegisterIrqHandler(&SIG_MMC0DETECT, SbiMmCard0DetectIrq, 0)) {
        mmc0_dcb.dcb_avail = 1;
    }
    else {
        mmc0_dcb.dcb_avail = 0;
        NutIrqSetMode(&SIG_MMC0DETECT, NUT_IRQMODE_LOWLEVEL);
        NutIrqEnable(&SIG_MMC0DETECT);
    }
#else
    mmc0_dcb.dcb_avail = 1;
#endif /* MMC0_CD_BIT */
    mmc0_dcb.dcb_changed = 0;

    return MmCardDevInit(dev);
}

static MMCIFC mmc0_ifc = {
    SbiMmCard0Init,             /*!< mmcifc_in */
    SbiMmCard0Io,               /*!< mmcifc_io */
    SbiMmCard0Select,           /*!< mmcifc_cs */
    SbiMmCard0Avail,            /*!< mmcifc_cd */
    SbiMmCard0WrProt            /*!< mmcifc_wp */
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
NUTDEVICE devSbiMmc0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    &mmc0_ifc,                  /*!< Interface control block, dev_icb. */
    &mmc0_dcb,                  /*!< Driver control block used by the low level part, dev_dcb. */
    SbiMmcIfcInit,              /*!< Driver initialization routine, dev_init. */
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
