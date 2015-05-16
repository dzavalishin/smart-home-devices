/*
 * Copyright (C) 2001-2005 by EmbeddedIT, 
 * Ole Reinhardt <ole.reinhardt@embedded-it.de> All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.4  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:03  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/12/09 22:13:08  olereinhardt
 * Added cvs log tag
 *
 */

#include <arch/arm.h>
#include <dev/irqreg.h>

#include <sys/event.h>
#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include <dev/irqreg.h>
#include <dev/at91_adc.h>

/*!
 * \addtogroup xgNutArchArmAt91Adc
 */
/*@{*/

#ifndef AT91_ADC_INITIAL_MODE
#define AT91_ADC_INITIAL_MODE SINGLE_CONVERSION
#endif

#ifndef AT91_ADC_INITIAL_PRESCALE
#define AT91_ADC_INITIAL_PRESCALE 55
#endif

#define AT91_ADC_BUF_SIZE 16 // this may only be a power of two

#define _adc_buf_head AT91_ADC_BUF_SIZE
#define _adc_buf_tail AT91_ADC_BUF_SIZE+1

uint16_t **ADC_Buffer = NULL;

/*!
 * \brief Reads data from the adc buffer
 *
 * \param channel  Specifies the channel to read data from
 * \param read     Variable to store the data in
 * \return 0: data read succesfully, 1: no data available
 */

int ADCBufRead(uint16_t channel, uint16_t * read)
{
    uint16_t tail, head;
    tail = ADC_Buffer[channel][_adc_buf_tail];
    head = ADC_Buffer[channel][_adc_buf_head];
    if (head != tail) {
        *read = ADC_Buffer[channel][tail];
        ADC_Buffer[channel][_adc_buf_tail] = (tail + 1) & (AT91_ADC_BUF_SIZE - 1);
        return 0;
    }
    return 1;
}

/* Store data in the buffer, called from interrupt */

static inline int ADCBufWrite(uint16_t channel, uint16_t write)
{
    uint16_t tail, head;
    tail = ADC_Buffer[channel][_adc_buf_tail];
    head = ADC_Buffer[channel][_adc_buf_head];
    if (((head + 1) & (AT91_ADC_BUF_SIZE - 1)) != tail) {
        ADC_Buffer[channel][head] = write;
        ADC_Buffer[channel][_adc_buf_head] = (head + 1) & (AT91_ADC_BUF_SIZE - 1);
        return 0;
    }
    return 1;
}

/*!
 * \brief Sets the data aquisition mode for the adc
 *
 * \param mode  Mode to set
 */

void ADCSetMode(TADCMode mode) 
{
    uint32_t regval;
    
    regval = inr(ADC_MR);
    regval &= ~ADC_SLEEP;
    switch (mode) {
        case ADC_OFF:
            regval &= ~ADC_TRGEN;
            regval |= ADC_SLEEP;
            break;
        case SINGLE_CONVERSION:
            regval &= ~ADC_TRGEN;
            break;
        case FREE_RUNNING_T0:
            regval &= ~ADC_TRGSEL;
            regval |= ADC_TRGEN | ADC_TRGSEL_TIOA0;
            break;     
        case FREE_RUNNING_T1:
            regval &= ~ADC_TRGSEL;
            regval |= ADC_TRGEN | ADC_TRGSEL_TIOA1;
            break;
        case FREE_RUNNING_T2:
            regval &= ~ADC_TRGSEL;
            regval |= ADC_TRGEN | ADC_TRGSEL_TIOA2;
        break;
        case FREE_RUNNING_EXT:
            regval &= ~ADC_TRGSEL;
            regval |= ADC_TRGEN | ADC_TRGSEL_EXT;
            break;
    }
    outr(ADC_MR, regval);
}

/*!
 * \brief Enable a channel used to sample when conversion started
 *
 * \param channel  Specifies the channel to enable
 */

void ADCEnableChannel(TADCChannel channel) 
{
    uint32_t     adc_chsr;
    register int idx;
    register int max = channel;

    outr(ADC_CHER, _BV(channel));
    adc_chsr = inr(ADC_CHSR);

    /* Search the highest numbered channel which is enabled */
    for (idx = 0; idx < 8; idx ++) {
        if (adc_chsr & _BV(idx)) max = idx;
    }

    /* Disable EOC for all channels */
    outr(ADC_IDR, 0x000000FF);
    /* Enable EOC for highest numbered channel */
    outr(ADC_IER, _BV(max));
}

/*!
 * \brief Disable a channel.
 *
 * \param channel  Specifies the channel to disable
 */

void ADCDisableChannel(TADCChannel channel) 
{
    outr(ADC_CHDR, _BV(channel));
    outr(ADC_IDR,  _BV(channel));
}

/*!
 * \brief Set the prescaler for the adc
 *
 * \param prescale  Prescaler value 0-128
 */

void ADCSetPrescale(uint32_t prescale)
{
    if (prescale > 128) prescale = 128;

    prescale = (prescale / 2) - 1;
    outr(ADC_MR, ((inr(ADC_MR) & ~(ADC_PRESCAL | ADC_STARTUP | ADC_SHTIM)) | 
                (prescale << ADC_PRESCAL_LSB) | ADC_STARTUP | ADC_SHTIM));     // set maximum sample & hold and startup time
}

/*!
 * \brief Start conversion
 */


void ADCStartConversion(void)
{
    outr(ADC_CR, ADC_START);
}

/*
 * ADC interrupt handler.
 */

static void ADCInterrupt(void *arg)
{
    register uint32_t adcsr = inr(ADC_SR) & inr(ADC_CHSR);
    uint16_t ADC_Value;        
    uint16_t channel;

    for (channel = 0; channel < ADC_MAX_CHANNEL; channel ++) {
        if (adcsr & _BV(channel)) {
            ADC_Value = inr(ADC_CDR(channel));
            if (ADCBufWrite(channel, ADC_Value) != 0) {
                // Send error message
            }
        }
    }
}

/*!
 * \brief Initialize the adc to the configured default values and enable interrupt
 */

void ADCInit(void)
{
    int channel;
    
    /* Only init once */
    if (ADC_Buffer) return;

    /* Enable clock int PMC and reset ADC */
    outr(PMC_PCER, _BV(ADC_ID));              // Enable ADC clock in PMC
    outr(ADC_CR, ADC_SWRST);                  // Reset bus
    outr(ADC_CR, 0x00);
    
    /* Basic configuration: Disable all channels and set mode and prescaler */
    outr(ADC_CHDR, ADC_CH0 | ADC_CH1 | ADC_CH2 | ADC_CH3 | ADC_CH4 | ADC_CH5 | ADC_CH6 | ADC_CH7);
    ADCSetMode(AT91_ADC_INITIAL_MODE);
    ADCSetPrescale(AT91_ADC_INITIAL_PRESCALE);

    /* Init adc buffers. One for every channel as we can sample all by automatic sequence */
    ADC_Buffer = NutHeapAlloc(sizeof(uint16_t *) * ADC_MAX_CHANNEL);
    for (channel = 0; channel < ADC_MAX_CHANNEL; channel ++) {
        ADC_Buffer[channel] = NutHeapAlloc(sizeof(uint16_t) * AT91_ADC_BUF_SIZE + 2);
        ADC_Buffer[channel][_adc_buf_head] = 0;
        ADC_Buffer[channel][_adc_buf_tail] = 0;        
    }

    if (NutRegisterIrqHandler(&sig_ADC, ADCInterrupt, NULL)) {
        // We do not free buffer as this would cost ROM and is not likely
        return;
    }
    NutIrqEnable(&sig_ADC);
}

/*@}*/
