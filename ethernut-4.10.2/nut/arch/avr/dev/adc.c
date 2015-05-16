/*
 * Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@kernelconcepts.de>,
 *                       Kernelconcepts http://www.kernelconcepts.de
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

/*!
 * \file dev/adc.c
 * \brief AVR adc driver
 */


/*
 * $Log$
 * Revision 1.8  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2006/01/25 12:51:40  freckle
 * added explicit off mode
 *
 * Revision 1.6  2006/01/11 16:34:44  freckle
 * ADCInit can be called multiple times (makes life easier)
 *
 * Revision 1.5  2005/10/24 17:59:55  haraldkipp
 * Fixes for ATmega103
 *
 * Revision 1.4  2005/08/25 16:23:42  olereinhardt
 * Removed compute intensive % and replaced by an &
 *
 * Revision 1.3  2005/08/19 21:52:43  freckle
 * use 8-bit buffer pointers, removed critical section from ADCRead
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2005/06/13 12:00:09  olereinhardt
 * Removed unnecessary NutExitCritical
 *
 * Revision 1.2  2005/03/04 11:42:05  olereinhardt
 * Added function void ADCStartLowNoiseConversion(void)
 * This function enters sleep mode!!! Be shure to read the AVR datasheet before using this function
 *
 * Revision 1.1  2004/08/02 10:05:25  olereinhardt
 * First checkin. ADC driver for ATMega128 / GCC
 *
 */


/*!
 * \addtogroup xgAvrAdc
 */

/*@{*/

/* Not ported. */
#ifdef __GNUC__

#include <string.h>
#include <avr/sleep.h>
#include <sys/heap.h>
#include <sys/atom.h>
#include <sys/nutconfig.h>

#include <dev/irqreg.h>
#include <dev/adc.h>

#ifndef ADC_INITIAL_CHANNEL
#define ADC_INITIAL_CHANNEL ADC0
#endif

#ifndef ADC_INITIAL_REF
#define ADC_INITIAL_REF AVCC
#endif

#ifndef ADC_INITIAL_MODE
#define ADC_INITIAL_MODE SINGLE_CONVERSION
#endif

#ifndef ADC_INITIAL_PRESCALE
#define ADC_INITIAL_PRESCALE ADC_PRESCALE_DIV64
#endif

#define ADC_BUF_SIZE 16 // this may only be a power of two

#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
uint8_t adc_sleep_mode = SLEEP_MODE_ADC;

/* AT90CAN128 uses a different register to enter sleep mode */
#if defined(SMCR)
#define AVR_SLEEP_CTRL_REG    SMCR
#else
#define AVR_SLEEP_CTRL_REG    MCUCR
#endif
#endif



/********** DRIVER GLOBALS **********/
adc_mode_t current_mode = ADC_OFF;

// The buffers are FIFO buffers implemented as char
// arrays with head and tail pointers.

// All read and write functions return a 0 for
// success and a 1 for fail, if a successfull read,
// the character is placed into *read.

// buffer: [data1 data2 ... datalast head tail]

#define _adc_buf_head ADC_BUF_SIZE
#define _adc_buf_tail (ADC_BUF_SIZE+1)

uint16_t *ADC_buffer = NULL;

inline int ADCBufRead(uint16_t * buf, uint16_t * read)
{
    uint8_t tail, head;
    tail = buf[_adc_buf_tail];
    head = buf[_adc_buf_head];
    if (head != tail) {
        *read = buf[tail];
        buf[_adc_buf_tail] = (tail + 1) & (ADC_BUF_SIZE-1);
        return 0;
    }
    return 1;
}

inline int ADCBufWrite(uint16_t * buf, uint16_t * write)
{
    uint8_t tail, head;
    tail = buf[_adc_buf_tail];
    head = buf[_adc_buf_head];
    if ((head + 1) % ADC_BUF_SIZE != tail) {
        buf[head] = *write;
        buf[_adc_buf_head] = (head + 1) & (ADC_BUF_SIZE-1);
        return 0;
    }
    return 1;
}

void ADCBufInit(uint16_t * buf)
{
    buf[_adc_buf_head] = 0;
    buf[_adc_buf_tail] = 0;
}

static void ADCInterrupt(void *arg)
{
    uint16_t ADC_value;

    ADC_value = inw(ADCW);

    if (ADCBufWrite(ADC_buffer, &ADC_value) != 0) {
        // Send error message
    }

}

void ADCInit()
{
    if (ADC_buffer) return;

    ADCSetChannel(ADC_INITIAL_CHANNEL);
    ADCSetRef(ADC_INITIAL_REF);
    ADCSetMode(ADC_INITIAL_MODE);
    ADCSetPrescale(ADC_INITIAL_PRESCALE);

    ADC_buffer = NutHeapAlloc(sizeof(uint16_t) * (ADC_BUF_SIZE + 2));
    ADCBufInit(ADC_buffer);

    if (NutRegisterIrqHandler(&sig_ADC, ADCInterrupt, NULL)) {
        // MR 2006-01-11: we do not free buffer as this would cost ROM and is not likely
        return;
    }
    // Enable ADC
    sbi(ADCSR, ADEN);

    // Enable ADC interrupt
    sbi(ADCSR, ADIE);
}

void ADCSetRef(adc_ref_t reference)
{
    ADCStopConversion();

#ifdef __AVR_ENHANCED__
    switch (reference) {
    case AVCC:
        cbi(ADMUX, REFS1);
        sbi(ADMUX, REFS0);
        break;
    case AREF:
        cbi(ADMUX, REFS1);
        cbi(ADMUX, REFS0);
        break;
    case INTERNAL_256:
        sbi(ADMUX, REFS1);
        sbi(ADMUX, REFS0);
        break;
    }
#endif /* __AVR_ENHANCED__ */

}

void ADCSetMode(adc_mode_t mode)
{
    ADCStopConversion();

    switch (mode) {
    case FREE_RUNNING:
        sbi(ADCSR, ADFR);
        break;
    case SINGLE_CONVERSION:
        cbi(ADCSR, ADFR);
        break;
    case ADC_OFF:
    		break;
    }
  	current_mode = mode;
}

uint8_t ADCSetPrescale(uint8_t prescalar)
{

    ADCStopConversion();

    if (prescalar > 128) {
        prescalar = 128;
    }

    switch (prescalar) {
    case ADC_PRESCALE_DIV2:
        cbi(ADCSR, ADPS2);
        cbi(ADCSR, ADPS1);
        cbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV4:
        cbi(ADCSR, ADPS2);
        sbi(ADCSR, ADPS1);
        cbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV8:
        cbi(ADCSR, ADPS2);
        sbi(ADCSR, ADPS1);
        sbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV16:
        sbi(ADCSR, ADPS2);
        cbi(ADCSR, ADPS1);
        cbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV32:
        sbi(ADCSR, ADPS2);
        cbi(ADCSR, ADPS1);
        sbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV64:
        sbi(ADCSR, ADPS2);
        sbi(ADCSR, ADPS1);
        cbi(ADCSR, ADPS0);
        break;
    case ADC_PRESCALE_DIV128:
        sbi(ADCSR, ADPS2);
        sbi(ADCSR, ADPS1);
        sbi(ADCSR, ADPS0);
        break;

    default:
        return 1;
        break;
    }

    return 0;
}

void ADCSetChannel(adc_channel_t adc_channel)
{
    uint8_t current_admux;

    current_admux = inb(ADMUX) & 0xF8;

    outb(ADMUX, (current_admux | adc_channel));
}

void ADCBufferFlush(void)
{
    ADCBufInit(ADC_buffer);
}

void ADCStartConversion()
{
    sbi(ADCSR, ADSC);
}

void ADCStartLowNoiseConversion()
{
    ADCSetMode(SINGLE_CONVERSION);

#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
    {
        uint8_t sleep_mode = AVR_SLEEP_CTRL_REG & _SLEEP_MODE_MASK;
        set_sleep_mode(adc_sleep_mode);
        /* Note:  avr-libc has a sleep_mode() function, but it's broken for
        AT90CAN128 with avr-libc version earlier than 1.2 */
        AVR_SLEEP_CTRL_REG |= _BV(SE);
        __asm__ __volatile__ ("sleep" "\n\t" :: );
        AVR_SLEEP_CTRL_REG &= ~_BV(SE);
        set_sleep_mode(sleep_mode);
    }
#else
    sbi(ADCSR, ADSC);
#endif    
}

void ADCStopConversion()
{
    if (current_mode != FREE_RUNNING) {
        // Send warning message
        return;
    }
// Terminate and restart the ADC 
// When restarted, start_conversion needs to be
// called again
    cbi(ADCSR, ADEN);
    cbi(ADCSR, ADSC);
    sbi(ADCSR, ADEN);
}

uint8_t ADCRead(uint16_t * value)
{
    return ADCBufRead(ADC_buffer, value);
}

inline adc_mode_t ADCGetMode(void)
{
    return (current_mode);
}
#endif
/*@}*/
