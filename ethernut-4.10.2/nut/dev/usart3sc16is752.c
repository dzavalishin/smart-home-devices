/*
 * Copyright (C) 2010 by Ulrich Prinz (uprinz2@netscape.net)
 * Copyright (C) 2009 by Rittal GmbH & Co. KG. All rights reserved.
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
 *
 */

/*
 * $Log$
 *
 * Revision 1.0  2009/04/13 ulrichprinz
 * First checkin, new twi driver for SC16IS752 dual usart chip
 * (currently SAM7X256 is tested only)
 *
 */


#include <cfg/os.h>
#include <cfg/clock.h>
#include <cfg/arch.h>
#include <cfg/uart.h>

//#include <string.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>
#include <dev/usartsc16is752.h>

#define SCDEV 	1
#define CH_  	1
#define NUTDEV_	devUsartsc16is752d

#if defined(__linux__)
#define IRQ_	1
#else
extern IRQ_HANDLER sig_INTERRUPT0;
#define IRQ_	sig_INTERRUPT0
#endif

// static void Sc16is752UsartEnable_(void)
// {
//     Sc16is752UsartEnable(SCDEV, CH_);
// }
//
// static void Sc16is752UsartDisable_(void)
// {
//     Sc16is752UsartDisable(SCDEV, CH_);
// }

NUTDEVICE devUsartsc16is752d;

static uint32_t Sc16is752UsartGetSpeed_(void)
{
    return Sc16is752UsartGetSpeed(SCDEV, CH_);
}

static int Sc16is752UsartSetSpeed_(uint32_t rate)
{
    return Sc16is752UsartSetSpeed(rate, SCDEV, CH_);
}

static uint8_t Sc16is752UsartGetDataBits_(void)
{
    return Sc16is752UsartGetDataBits(SCDEV, CH_);
}

static int Sc16is752UsartSetDataBits_(uint8_t bits)
{
    return Sc16is752UsartSetDataBits(bits, SCDEV, CH_);
}

static uint8_t Sc16is752UsartGetParity_(void)
{
    return Sc16is752UsartGetParity(SCDEV, CH_);
}

static int Sc16is752UsartSetParity_(uint8_t mode)
{
    return Sc16is752UsartSetParity(mode, SCDEV, CH_);
}

static uint8_t Sc16is752UsartGetStopBits_(void)
{
    return Sc16is752UsartGetStopBits(SCDEV, CH_);
}

static int Sc16is752UsartSetStopBits_(uint8_t bits)
{
    return Sc16is752UsartSetStopBits(bits, SCDEV, CH_);
}

static uint32_t Sc16is752UsartGetStatus_(void)
{
    return Sc16is752UsartGetStatus(SCDEV, CH_);
}

static int Sc16is752UsartSetStatus_(uint32_t flags)
{
    return Sc16is752UsartSetStatus(flags, SCDEV, CH_);
}

static uint8_t Sc16is752UsartGetClockMode_(void)
{
    return Sc16is752UsartGetClockMode(SCDEV, CH_);
}

static int Sc16is752UsartSetClockMode_(uint8_t mode)
{
    return Sc16is752UsartSetClockMode(mode, SCDEV, CH_);
}

static uint32_t Sc16is752UsartGetFlowControl_(void)
{
    return Sc16is752UsartGetFlowControl(SCDEV, CH_);
}

static int Sc16is752UsartSetFlowControl_(uint32_t flags)
{
    return Sc16is752UsartSetFlowControl(flags, SCDEV, CH_);
}

static void Sc16is752UsartTxStart_(void)
{
    Sc16is752UsartTxStart(SCDEV, CH_);
}

static void Sc16is752UsartRxStart_(void)
{
    Sc16is752UsartRxStart(SCDEV, CH_);
}

static int Sc16is752UsartInit_(void)
{
#if defined(__linux__)
    return Sc16is752UsartInit(SCDEV, CH_, &NUTDEV_, 1);
#else
    return Sc16is752UsartInit(SCDEV, CH_, &NUTDEV_, &IRQ_);
#endif
}

static int Sc16is752UsartDeinit_(void)
{
#if defined(__linux__)
    return Sc16is752UsartDeinit(SCDEV, CH_, 1);
#else
    return Sc16is752UsartDeinit(SCDEV, CH_, &IRQ_);
#endif
}



/*!
 * \addtogroup xgNutArchArmAt91Us
 */
/*@{*/


/*!
 * \brief USART3 device control block structure.
 */
static USARTDCB dcb_usartd = {
    0,                             /* dcb_modeflags */
    0,                             /* dcb_statusflags */
    0,                             /* dcb_rtimeout */
    0,                             /* dcb_wtimeout */
    {0, 0, 0, 0, 0, 0, 0, 0},      /* dcb_tx_rbf */
    {0, 0, 0, 0, 0, 0, 0, 0},      /* dcb_rx_rbf */
    0,                             /* dbc_last_eol */
    Sc16is752UsartInit_,           /* dcb_init */
    Sc16is752UsartDeinit_,         /* dcb_deinit */
    Sc16is752UsartTxStart_,        /* dcb_tx_start */
    Sc16is752UsartRxStart_,        /* dcb_rx_start */
    Sc16is752UsartSetFlowControl_, /* dcb_set_flow_control */
    Sc16is752UsartGetFlowControl_, /* dcb_get_flow_control */
    Sc16is752UsartSetSpeed_,       /* dcb_set_speed */
    Sc16is752UsartGetSpeed_,       /* dcb_get_speed */
    Sc16is752UsartSetDataBits_,    /* dcb_set_data_bits */
    Sc16is752UsartGetDataBits_,    /* dcb_get_data_bits */
    Sc16is752UsartSetParity_,      /* dcb_set_parity */
    Sc16is752UsartGetParity_,      /* dcb_get_parity */
    Sc16is752UsartSetStopBits_,    /* dcb_set_stop_bits */
    Sc16is752UsartGetStopBits_,    /* dcb_get_stop_bits */
    Sc16is752UsartSetStatus_,      /* dcb_set_status */
    Sc16is752UsartGetStatus_,      /* dcb_get_status */
    Sc16is752UsartSetClockMode_,   /* dcb_set_clock_mode */
    Sc16is752UsartGetClockMode_,   /* dcb_get_clock_mode */
};

/*!
 * \name SC16IS752 USARTd Device
 */
/*@{*/
/*!
 * \brief USART3 device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using the serial communication
 * driver of the AT91's on-chip USART3.
 *
 * The device is named \b uartd.
 *
 * \showinitializer
 */
NUTDEVICE devUsartsc16is752d = {
    0,                          /* Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '_', 'd', 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,                 /* Type of device, dev_type. */
    (SCDEV<<8)|CH_,              /* Base address, used for dev & ch: (dev<<8)|ch */
    0,                          /* First interrupt number, dev_irq (not used). */
    0,                          /* Interface control block, dev_icb (not used). */
    &dcb_usartd,                /* Driver control block, dev_dcb. */
    UsartInit,                  /* Driver initialization routine, dev_init. */
    UsartIOCtl,                 /* Driver specific control function, dev_ioctl. */
    UsartRead,                  /* Read from device, dev_read. */
    UsartWrite,                 /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    UsartWrite_P,               /* Write program memory to device, dev_write_P. */
#endif
    UsartOpen,                  /* Open a device or file, dev_open. */
    UsartClose,                 /* Close a device or file, dev_close. */
    UsartSize                   /* Request file size, dev_size. */
};




/*@}*/

/*@}*/

