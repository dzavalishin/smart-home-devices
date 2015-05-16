/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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
 * \file arch/avr32/dev/debug0.c
 * \brief AVR32 debug output device.
 *
 * \verbatim
 *
 * $Log: debug.c,v $
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/clock.h>

#include <sys/atom.h>
#include <sys/device.h>
#include <sys/file.h>
#include <sys/timer.h>
#include <dev/gpio.h>
#include <dev/uart.h>

#include <avr32/io.h>
#include <arch/avr32/gpio.h>
#include <arch/avr32/pm.h>

#if defined(AVR32_USART0_TXD_0_0_PIN)
#define AVR32_USART0_TXD_PIN        AVR32_USART0_TXD_0_0_PIN
#define AVR32_USART0_TXD_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION
#elif defined(AVR32_USART0_TXD_0_PIN)
#define AVR32_USART0_TXD_PIN        AVR32_USART0_TXD_0_PIN
#define AVR32_USART0_TXD_FUNCTION   AVR32_USART0_TXD_0_FUNCTION
#endif

#if defined(AVR32_USART1_TXD_0_0_PIN)
#define AVR32_USART1_TXD_PIN        AVR32_USART1_TXD_0_0_PIN
#define AVR32_USART1_TXD_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#elif defined(AVR32_USART1_TXD_0_PIN)
#define AVR32_USART1_TXD_PIN        AVR32_USART1_TXD_0_PIN
#define AVR32_USART1_TXD_FUNCTION   AVR32_USART1_TXD_0_FUNCTION
#endif

#if defined(AVR32_USART2_TXD_0_0_PIN)
#define AVR32_USART2_TXD_PIN        AVR32_USART2_TXD_0_0_PIN
#define AVR32_USART2_TXD_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#elif defined(AVR32_USART2_TXD_0_PIN)
#define AVR32_USART2_TXD_PIN        AVR32_USART2_TXD_0_PIN
#define AVR32_USART2_TXD_FUNCTION   AVR32_USART2_TXD_0_FUNCTION
#endif

#if defined(AVR32_USART3_TXD_0_0_PIN)
#define AVR32_USART3_TXD_PIN        AVR32_USART3_TXD_0_0_PIN
#define AVR32_USART3_TXD_FUNCTION   AVR32_USART3_TXD_0_0_FUNCTION
#elif defined(AVR32_USART3_TXD_0_PIN)
#define AVR32_USART3_TXD_PIN        AVR32_USART3_TXD_0_PIN
#define AVR32_USART3_TXD_FUNCTION   AVR32_USART3_TXD_0_FUNCTION
#endif

/*!
 * \addtogroup xgDevDebugAvr32
 */
/*@{*/

/*!
 * \brief Handle I/O controls for debug device 0.
 *
 * The debug device supports UART_SETSPEED only.
 *
 * \return 0 on success, -1 otherwise.
 */
static int DebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    volatile avr32_usart_t *usart = (avr32_usart_t *) dev->dev_base;

    if (req == UART_SETSPEED) {
        const uint32_t baudrate = (*((uint32_t *) conf));
        const uint32_t pba_hz = NutArchClockGet(NUT_HWCLK_PERIPHERAL_A);
        unsigned int over = (pba_hz >= 16 * baudrate) ? 16 : 8;
        unsigned int cd_fp = ((1 << AVR32_USART_BRGR_FP_SIZE) * pba_hz + (over * baudrate) / 2) / (over * baudrate);
        unsigned int cd = cd_fp >> AVR32_USART_BRGR_FP_SIZE;
        unsigned int fp = cd_fp & ((1 << AVR32_USART_BRGR_FP_SIZE) - 1);

        if (cd < 1 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
            return -1;

        usart->mr = (usart->mr & ~(AVR32_USART_MR_USCLKS_MASK |
                                   AVR32_USART_MR_SYNC_MASK |
                                   AVR32_USART_MR_OVER_MASK)) |
            AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET |
            ((over == 16) ? AVR32_USART_MR_OVER_X16 : AVR32_USART_MR_OVER_X8) << AVR32_USART_MR_OVER_OFFSET;

        usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET | fp << AVR32_USART_BRGR_FP_OFFSET;
        return 0;
    }
    return -1;
}

/*!
 * \brief Initialize debug device 0.
 *
 * \return Always 0.
 */
static int DebugInit(NUTDEVICE * dev)
{
    volatile avr32_usart_t *usart = (avr32_usart_t *) dev->dev_base;

    /* Disable all USART interrupts.
     ** Interrupts needed should be set explicitly on every reset. */
    NutEnterCritical();
    usart->idr = 0xFFFFFFFF;
    usart->csr;
    NutExitCritical();

    /* Reset mode and other registers that could cause unpredictable behavior after reset. */
    usart->mr = 0;
    usart->rtor = 0;
    usart->ttgr = 0;

    /* Shutdown TX and RX (will be re-enabled when setup has successfully completed),
     ** reset status bits and turn off DTR and RTS. */
    usart->cr = AVR32_USART_CR_RSTRX_MASK |
        AVR32_USART_CR_RSTTX_MASK | AVR32_USART_CR_RSTSTA_MASK | AVR32_USART_CR_RSTIT_MASK | AVR32_USART_CR_RSTNACK_MASK |
#ifndef AVR32_USART_CR_DTRDIS_MASK
        AVR32_USART_CR_DTRDIS_MASK |
#endif
        AVR32_USART_CR_RTSDIS_MASK;

    usart->mr |= (8 - 5) << AVR32_USART_MR_CHRL_OFFSET |        /* 8 bit character length */
        AVR32_USART_MR_PAR_NONE << AVR32_USART_MR_PAR_OFFSET |  /* No parity */
        AVR32_USART_MR_CHMODE_NORMAL << AVR32_USART_MR_CHMODE_OFFSET |  /* */
        AVR32_USART_MR_NBSTOP_1 << AVR32_USART_MR_NBSTOP_OFFSET;

    /* Set normal mode. */
    usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) | AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET;

    /* Enable input and output. */
    usart->cr = AVR32_USART_CR_TXEN_MASK;

    return 0;
}

/*!
 * \brief Send a single character to debug device.
 *
 * A newline character will be automatically prepended
 * by a carriage return.
 */
static void DebugPut(CONST NUTDEVICE * dev, char ch)
{
    volatile avr32_usart_t *usart = (avr32_usart_t *) dev->dev_base;

    /* Prepend new line by carriage return */
    if (ch == '\n') {
        DebugPut(dev, '\r');
    }

    /* Wait for TX Ready. */
    while (!(usart->csr & AVR32_USART_CSR_TXRDY_MASK));
    /* Send out character */
    usart->thr = (ch << AVR32_USART_THR_TXCHR_OFFSET) & AVR32_USART_THR_TXCHR_MASK;

}

/*!
 * \brief Send characters to debug device 0.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int DebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int c = len;
    CONST char *cp = buffer;

    while (c--) {
        DebugPut(fp->nf_dev, *cp++);
    }
    return len;
}

/*!
 * \brief Open debug device 0.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *DebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp = (NUTFILE *) (dev->dev_dcb);

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    return fp;
}

/*!
 * \brief Close debug device 0.
 *
 * \return Always 0.
 */
static int DebugClose(NUTFILE * fp)
{
    return 0;
}

#ifdef AVR32_USART0_TXD_PIN
static int Debug0Init(NUTDEVICE * dev)
{
    /* Assign GPIO to USART. */
    gpio_enable_module_pin(AVR32_USART0_TXD_PIN, AVR32_USART0_TXD_FUNCTION);

    pm_enable_module(&AVR32_PM, AVR32_USART0_CLK_PBA);

    return DebugInit(dev);
};

static NUTFILE dbgfile0;

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. */
    (uintptr_t) (&AVR32_USART0),        /*!< Base address, dev_base. */
    0,                          /*!< First interrupt number, dev_irq. */
    0,                          /*!< Interface control block, dev_icb. */
    &dbgfile0,                  /*!< Driver control block, dev_dcb. */
    Debug0Init,                 /*!< Driver initialization routine, dev_init. */
    DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
    0,                          /*!< dev_read. */
    DebugWrite,                 /*!< dev_write. */
    DebugOpen,                  /*!< dev_opem. */
    DebugClose,                 /*!< dev_close. */
    0                           /*!< dev_size. */
};
#endif                          /* AVR32_USART0_TXD_PIN */

#ifdef AVR32_USART1_TXD_PIN
static int Debug1Init(NUTDEVICE * dev)
{
    /* Assign GPIO to USART. */
    gpio_enable_module_pin(AVR32_USART1_TXD_PIN, AVR32_USART1_TXD_FUNCTION);

    pm_enable_module(&AVR32_PM, AVR32_USART1_CLK_PBA);

    return DebugInit(dev);
};

static NUTFILE dbgfile1;

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug1 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. */
    (uintptr_t) (&AVR32_USART1),        /*!< Base address, dev_base. */
    0,                          /*!< First interrupt number, dev_irq. */
    0,                          /*!< Interface control block, dev_icb. */
    &dbgfile1,                  /*!< Driver control block, dev_dcb. */
    Debug1Init,                 /*!< Driver initialization routine, dev_init. */
    DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
    0,                          /*!< dev_read. */
    DebugWrite,                 /*!< dev_write. */
    DebugOpen,                  /*!< dev_opem. */
    DebugClose,                 /*!< dev_close. */
    0                           /*!< dev_size. */
};
#endif                          /* AVR32_USART1_TXD_PIN */

#ifdef AVR32_USART2_TXD_PIN
static int Debug2Init(NUTDEVICE * dev)
{
    /* Assign GPIO to USART. */
    gpio_enable_module_pin(AVR32_USART2_TXD_PIN, AVR32_USART2_TXD_FUNCTION);

    pm_enable_module(&AVR32_PM, AVR32_USART2_CLK_PBA);

    return DebugInit(dev);
};

static NUTFILE dbgfile2;

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug2 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '2', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. */
    (uintptr_t) (&AVR32_USART2),        /*!< Base address, dev_base. */
    0,                          /*!< First interrupt number, dev_irq. */
    0,                          /*!< Interface control block, dev_icb. */
    &dbgfile2,                  /*!< Driver control block, dev_dcb. */
    Debug2Init,                 /*!< Driver initialization routine, dev_init. */
    DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
    0,                          /*!< dev_read. */
    DebugWrite,                 /*!< dev_write. */
    DebugOpen,                  /*!< dev_opem. */
    DebugClose,                 /*!< dev_close. */
    0                           /*!< dev_size. */
};
#endif                          /* AVR32_USART2_TXD_PIN */

#ifdef AVR32_USART3_TXD_PIN
static int Debug3Init(NUTDEVICE * dev)
{
    /* Assign GPIO to USART. */
    gpio_enable_module_pin(AVR32_USART3_TXD_PIN, AVR32_USART3_TXD_FUNCTION);

    pm_enable_module(&AVR32_PM, AVR32_USART3_CLK_PBA);

    return DebugInit(dev);
};

static NUTFILE dbgfile3;

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug3 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '3', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. */
    (uintptr_t) (&AVR32_USART3),        /*!< Base address, dev_base. */
    0,                          /*!< First interrupt number, dev_irq. */
    0,                          /*!< Interface control block, dev_icb. */
    &dbgfile3,                  /*!< Driver control block, dev_dcb. */
    Debug3Init,                 /*!< Driver initialization routine, dev_init. */
    DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
    0,                          /*!< dev_read. */
    DebugWrite,                 /*!< dev_write. */
    DebugOpen,                  /*!< dev_opem. */
    DebugClose,                 /*!< dev_close. */
    0                           /*!< dev_size. */
};

#endif                          /* AVR32_USART3_TXD_PIN */

/*@}*/
