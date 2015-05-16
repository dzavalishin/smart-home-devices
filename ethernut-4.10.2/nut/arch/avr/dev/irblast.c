/*
 * Copyright (C) 2007 by Przemyslaw Rudy. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.4  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/08/17 11:16:29  haraldkipp
 * Flush timeout fixed. Thanks to Przemek.
 *
 * Revision 1.1  2007/05/24 07:42:42  haraldkipp
 * New driver from Przemyslaw Rudy sends modulated infrared remote control
 * codes using a simple IR LED. Cool.
 *
 */

#include <compiler.h>
#include <string.h>
#include <dev/irqreg.h>
#include <sys/atom.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/device.h>
#include <arch/timer.h>
#include <dev/irblast.h>

/* This driver sends infrared codes. The hardware pwm feature is used thus output
 * is generated on dedicated pin PB7.
 * Frequency and codes are given in timer OCR value form. For user convenience
 * two helpers are provided, carrier frequency can be given in kHz form (30-50)
 * and code itself is given in carrier frequency periods (1-1000).
 * In short 38 stands for 38kHz, period 100 stands for 100 * (1/38000) = 2.6ms,
 * period 200 stands for 200 * (1/38000) = 5.2ms etc.
 *
 * Example:
 * 1. Driver Init:
 * FILE * irblast_hdl  = 0; 
 * NutRegisterDevice(&devIrblast0, 0, 0);
 * irblast_hdl = fopen("irblast0", "w");
 *
 * 2. Code Trnasmit:
 * #define CODE_LENGTH 4
 * const uint16_t freqCode[CODE_LENGTH] PROGMEM = {100, 200, 200, 200};
 * uint16_t ocrCode[CODE_LENGTH];
 * uint32_t speed;
 * 
 * speed = (uint32_t)IrblastFreq2Ocr(38);
 * _ioctl(_fileno(irblast_hdl), IRBLAST_SETFREQ, &speed);
 * memcpy_P(ocrCode, freqCode, CODE_LENGTH<<1);
 * if(PwmPeriod2Ocr((u_char)38, CODE_LENGTH, ocrCode) == CODE_LENGTH)
 * {
 *    fwrite((uint16_t *)ocrCode, sizeof(uint16_t), CODE_LENGTH, irblast_hdl);
 *    fflush(irblast_hdl);
 * }
 *
 */

typedef struct _IRBLASTDCB IRBLASTDCB;
struct _IRBLASTDCB {
    /* Queue of threads waiting for output buffer empty.
     * Threads are added to this queue when calling IrblastFlush().
     */
    HANDLE dcb_tx_rdy;

    /* Next output index */
    volatile uint8_t if_tx_idx;
    /* Next write index */
    uint8_t if_wr_idx;
    /* Set if transmitter running */
    volatile uint8_t if_tx_act;
    /* Output buffer */
    uint16_t if_tx_buf[256];     // 256*2 = 512 bytes...   
};

static IRBLASTDCB dcb_pwm0;
static NUTFILE file;


/*!
 * \brief Converts carrier frequency in form of kHz to the timer OCR form.
 *
 * \param freqKHz frequency in kHz, range 30-50.
 *
 * \return OCR form of the frequency used by the driver or 0.
 */
uint8_t IrblastFreq2Ocr(uint8_t freqKHz)
{
    uint32_t div;
    uint8_t ocr = 0;

    if ((freqKHz >= 30) && (freqKHz <= 50)) {
        /* prescaler = f/1 */
        div = 2000UL * (uint32_t) freqKHz;
        ocr = (uint8_t) ((NutGetCpuClock() / div) & 0x000000ff) - 1;
    }
    return ocr;
}

/*!
 * \brief Converts ircode from carrier frequency periods form to the timer OCR form.
 *
 * Given array of ir code in form of carrier frequency periods will be converted
 * (in place) to the form of OCR periods used by the driver.
 *
 * \param freqKHz frequency in kHz, range 30-50, for 0 pulse time =10us will be used
 * \param entries ir code length.
 * \param pCode pointer to the array with ir code, each entry in range 1-999.
 *
 * \return number of entries properly converted or -1 on error.
 */
int IrblastPeriod2Ocr(uint8_t freqKHz, int entries, uint16_t * pCode)
{
    uint32_t div, sClk, freq;
    int i = -1;

    if ((freqKHz < 30) && (freqKHz > 50)) {
        freqKHz = 100;          /* use pulse 10us time */
    }

    /* prescaler = f/8 */
    sClk = NutGetCpuClock() / 10UL;
    freq = 800UL * (uint32_t) (freqKHz);

    for (i = 0; i < entries; ++i) {
        if ((pCode[i] == 0) || (pCode[i] > 1000)) {
            return -1;
        }
        div = sClk * (uint32_t) pCode[i];
        div = div / freq;
        pCode[i] = (unsigned int) (div & 0x0000ffff) - 1;
    }
    return i;
}


/*!
 * \brief Timer1 output compare routine.
 *
 * This interrupt routine is called after when previous timer perios has
 * been finished. It sets new timer period, or finishes counting.
 *
 * \param arg void* cast of NUTDEVICE.
 */
static void IrblastOutComp1CInt(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IRBLASTDCB *dcb = dev->dev_dcb;

    if (dcb->if_tx_idx != dcb->if_wr_idx) {
        /* Set period */
        OCR1A = dcb->if_tx_buf[dcb->if_tx_idx];
        OCR1C = dcb->if_tx_buf[dcb->if_tx_idx];
        ++(dcb->if_tx_idx);
    } else {
        dcb->if_tx_act = 0;

        /* TMR1 output compare A match interrupt disable */
        TIMSK &= ~_BV(OCIE1C);

        TCCR1B &= ~_BV(CS11);   /* stop counting */

        TCCR1A &= ~_BV(COM1C0);
        TCCR1A |= _BV(COM1C1);  /* clrar OC pin on compare */

        TCCR1C = _BV(FOC1C);    /* trigger pin - clear it */

        NutEventPostFromIrq(&dcb->dcb_tx_rdy);
    }
}


/*!
 * \brief Initiate output.
 *
 * This function checks the output buffer for any data. If
 * the buffer contains at least one character, the transmitter
 * is started, if not already running. The function returns
 * immediately, without waiting for the character being
 * completely transmitted. Any remaining characters in the
 * output buffer are transmitted in the background.
 *
 * \param dev Indicates the device.
 *
 * \return 0.
 */
static int IrblastOutput(NUTDEVICE * dev)
{
    IRBLASTDCB *dcb = dev->dev_dcb;

    if ((dcb->if_tx_act == 0) && (dcb->if_tx_idx != dcb->if_wr_idx)) {
        dcb->if_tx_act = 1;

        TCCR1A &= ~_BV(COM1C1);
        TCCR1A |= _BV(COM1C0);  /* toggle OC pin on compare */

        TCCR1C = _BV(FOC1C);    /* trigger pin - toggle it */

        /* Clear Counter register (16bit) */
        TCNT1 = 0;

        /* Set period */
        OCR1A = dcb->if_tx_buf[dcb->if_tx_idx];
        OCR1C = dcb->if_tx_buf[dcb->if_tx_idx];

        ++(dcb->if_tx_idx);

        /* TMR1 output compare A match interrupt enable */
        ETIMSK |= _BV(OCIE1C);

        TCCR1B |= _BV(CS11);    /* start counting f/8 */
    }
    return 0;
}


/*!
 * \brief Wait for output buffer empty.
 *
 * If the output buffer contains any data, the calling
 * thread is suspended until all data has been transmitted.
 *
 * \param dev Indicates the device.
 *
 * \return 0.
 */
static int IrblastFlush(NUTDEVICE * dev)
{
    IRBLASTDCB *dcb = dev->dev_dcb;

    /* Start any pending output */
    IrblastOutput(dev);

    /* Wait until output buffer empty */
    while (dcb->if_tx_idx != dcb->if_wr_idx) {
        NutEventWaitNext(&dcb->dcb_tx_rdy, 100);
    }

    /* TMR1 output compare A match interrupt disable */
    ETIMSK &= ~_BV(OCIE1C);

    TCCR1B &= ~_BV(CS11);       /* stop counting */

    TCCR1A &= ~_BV(COM1C0);
    TCCR1A |= _BV(COM1C1);      /* clrar OC pin on compare */

    TCCR1C = _BV(FOC1C);        /* trigger pin - clear it */

    return 0;
}


/*!
 * \brief Moves data to the output buffer.
 *
 * If the output buffer contains any data, the calling
 * thread is suspended until all data has been transmitted.
 *
 * \param dev Indicates the device.
 * \param buffer data buffer.
 * \param len is length in bytes!.
 * \param pflag 0 - indicates input is ram, !=0 - input is progmem.
 *
 * \return number of bytes sent.
 */
static int IrblastPut(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc = 0;
    IRBLASTDCB *dcb = dev->dev_dcb;
    CONST uint16_t *cp;
    uint16_t ch;

    /* Call without data pointer starts transmission */
    if (buffer == 0) {
        IrblastFlush(dev);
    }

    /* Put data in transmit buffer,
       for us buffer points to table of 'uint16_t' type data */
    cp = buffer;

    /* len is length in bytes, so it must be divided by 2 */
    len >>= 1;

    for (rc = 0; rc < len;) {
        if ((uint8_t) (dcb->if_wr_idx + 1) == dcb->if_tx_idx) {
            IrblastFlush(dev);
        }
        ch = pflg ? PRG_RDB(cp) : *cp;
        dcb->if_tx_buf[dcb->if_wr_idx] = ch;
        ++(dcb->if_wr_idx);
        ++cp;
        ++rc;
    }

    /* multiply result by 2 to return the number of sent bytes */
    return (rc << 1);
}


/*!
 * \brief Moves data from ram to the output buffer.
 *
 * \param fp file pointer.
 * \param buffer data buffer.
 * \param len is length in bytes!.
 *
 * \return number of bytes sent.
 */
static int IrblastWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return IrblastPut(fp->nf_dev, buffer, len, 0);
}

/*!
 * \brief Moves data from progmem to the output buffer.
 *
 * \param fp file pointer.
 * \param buffer data buffer.
 * \param len is length in bytes!.
 *
 * \return number of bytes sent.
 */
static int IrblastWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return IrblastPut(fp->nf_dev, (CONST char *) buffer, len, 1);
}

/*!
 * \brief Asynchronous control interface.
 *
 * \param dev Indicates the device.
 * \param req request type.
 * \param conf request parameter.
 *
 * \return 0-success, -1-error.
 */
static int IrblastIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    uint8_t *usp = (uint8_t *) conf;

    switch (req) {
    case IRBLAST_SETFREQ:
        if (*usp == 0) {
            /* disable compare modulator hardware functionality and timer at all */
            TCCR2 &= ~(_BV(WGM21) | _BV(COM20));
        } else {
            OCR2 = *usp;

            /* Clear Counter register (8bit) */
            TCNT2 = 0;

            /* enable compare modulator hardware functionality and timer itself in CTC mode */
            TCCR2 |= _BV(WGM21) | _BV(COM20);
        }
        break;

    case IRBLAST_GETFREQ:
        *usp = OCR2;
        break;

    default:
        return -1;
    }
    return 0;
}

/*!
 * \brief Driver Open Funtion.
 *
 * \param dev Indicates the device.
 * \param name unused.
 * \param mode unused.
 * \param acc unused.
 *
 * \return 0-success, -1-error.
 */
static NUTFILE *IrblastOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    file.nf_next = 0;
    file.nf_dev = dev;
    file.nf_fcb = 0;
    return &file;
}

/*!
 * \brief Driver Close Funtion.
 *
 * \param fp file pointer.
 *
 * \return 0.
 */
static int IrblastClose(NUTFILE * fp)
{
    return 0;
}

/*!
 * \brief Timer1 Initialization.
 *
 * Timer 1 counts ir periods. 
 */
static void IrblastTmr1Init(void)
{
    /* TMR1 runs in CTC mode */
    TCCR1A &= ~(_BV(COM1C1) | _BV(COM1C0) | _BV(WGM11) | _BV(WGM10));   /* CTC mode */
    TCCR1A |= _BV(COM1C1);      /* clrar OC pin on compare */

    TCCR1B &= ~(_BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS11) | _BV(CS10));   /* f = off */
    TCCR1B |= _BV(WGM12);       /* CTC */

    TCCR1C = _BV(FOC1C);        /* trigger pin - clear it */

    /* TMR1 output compare A match interrupt disable */
    ETIMSK &= ~_BV(OCIE1C);
}

/*!
 * \brief Timer2 Initialization.
 *
 * Timer 1 serves ir carrier frequency. 
 */
static void IrblastTmr2Init(void)
{
    /* TMR2 is off - must be started with ioctl call */
    TCCR2 = _BV(CS20);          /* f=clk/1 - do not enable compare modulator hardware functionality */
}


/*!
 * \brief Driver Initialization Funtion.
 *
 * \param dev Indicates the device.
 *
 * \return 0-success, -1-error.
 */
static int IrblastInit(NUTDEVICE * dev)
{
    IRBLASTDCB *dcb = dev->dev_dcb;

    /* Initialize Driver Control Block */
    memset(dcb, 0, sizeof(IRBLASTDCB));

    /* Register interrupt handlers */
    if (NutRegisterIrqHandler(&sig_OUTPUT_COMPARE1C, IrblastOutComp1CInt, dev))
        return -1;

    /* Init Timer2 - carrier frequency generator */
    IrblastTmr2Init();

    /* Init Timer1 - AM modulation of Timer3 output pin */
    IrblastTmr1Init();

    /* This pin is used by hardware mux */
    sbi(DDRB, PORTB7);

    return 0;
}



NUTDEVICE devIrblast0 = {
    0,                          /* Pointer to next dev */
    {'i', 'r', 'b', 'l', 'a', 's', 't', '0', 0}
    ,                           /* Unique device name */
    IFTYP_STREAM,               /* Type of dev, IFTYP_CHAR ? */
    0,                          /* Base address */
    0,                          /* First irq # */
    0,                          /* I/f control block */
    &dcb_pwm0,                  /* Dev control block */
    IrblastInit,
    IrblastIOCtl,
    0,
    IrblastWrite,
    IrblastWrite_P,
    IrblastOpen,
    IrblastClose,
    0
};
