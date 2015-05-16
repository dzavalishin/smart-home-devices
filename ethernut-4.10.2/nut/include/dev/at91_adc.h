/*
 * Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@embedded-it.de>,
 *                       Kernelconcepts http://www.embedded-it.de
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
 * Revision 1.4  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/12/09 22:12:05  olereinhardt
 * Added cvs log tag
 *
 */


/*!
 * \file include/dev/at91_adc.h
 * \brief Header for AT91 Adc driver
 */

/*@{*/ 

#ifndef _AT91_ADC_H_
#define _AT91_ADC_H_

/*!
 * \enum  adc_moded_type dev/at91_adc.h
 * \brief enum declaring possible ADC modes
 *
 * ADC_OFF:
 *    Switch off adc and enable slep mode
 * FREE_RUNNING_x:
 *    This is a pseudo free running mode as we don't want to use a 
 *    dedicated timer (0..3) for this purpose we retrigger sampling in 
 *    the interrupt handler. Starting when ADC_start_conversion() is called.
 * SINGLE_CONVERSION:
 *    Single-conversion mode. One sample taken every time
 *    ADC_start_conversion() is called.
 *
 */

typedef enum adc_mode_type
{
    ADC_OFF,
    FREE_RUNNING_T0,
    FREE_RUNNING_T1,
    FREE_RUNNING_T2,
    FREE_RUNNING_EXT,
    SINGLE_CONVERSION
} TADCMode;


/*!
 * \enum  adc_channel_type dev/at91_adc.h
 * \brief enum declaring possible ADC channels
 */

typedef enum adc_channel_type
{
    ADC0=0,
    ADC1=1,
    ADC2=2,
    ADC3=3,
    ADC4=4,
    ADC5=5,
    ADC6=6,
    ADC7=7,
    ADC_MAX_CHANNEL = 8
} TADCChannel;

/* Function prototypes */

void ADCInit(void);


// ADCStartConversion
//                                                    
// Begins ADC conversion. The conversion will process all 
// enabled channels one after the other.
//
// NOTE: Converted values from the ADC are stored
//       in a local buffer. The user must call
//       ADC_read to obtain these values.
//
// pre:  none
// post: The ADC has started conversion. Completion of
//       any conversions is not guaranteed.

void ADCStartConversion(void);


// ADCSetPrescale
//                                                    
// Allows setting of ADC clock prescalar (ADC rate).
// The  ADC rate is given by the system clock rate
// divided by the prescalar value. Possible prescalar
// values range from 2-128
//
// pre: "prescalar" is a valid ADC reference from the
//       choices given above
// post: ADC prescalar set to desired choice

void ADCSetPrescale(uint32_t prescale);


// ADCEnableChannel
// ADCDisableChannel
//
// Enables/disables a channel to be sampled on the next conversion
//
// pre: none
// post: Channel is selected / deselected. Next conversion will respect these settings

void ADCDisableChannel(TADCChannel channel);
void ADCEnableChannel(TADCChannel channel);


// ADCSetMode
//
// Possible values: 
//    - ADC_OFF
//    - SINGLE_CONVERSION
//    - FREE_RUNNING_T0
//    - FREE_RUNNING_T1
//    - FREE_RUNNING_T2
//      These depend on a timer t0 / t1 / t2 
//    - FREE_RUNNING_EXT
//      External trigger
//
// pre: none
// post: Set adc conversion to the selected value. 

void ADCSetMode(TADCMode mode); 

// AFCBufRead
//
// Reads the next sampled value of the given channel from the buffer.
// 
// pre: Sample completed
// post: Value will be removed from buffer

int ADCBufRead(uint16_t channel, uint16_t * read);

#endif
/*@}*/
