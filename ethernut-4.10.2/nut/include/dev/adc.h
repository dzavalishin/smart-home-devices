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
 * \file include/dev/adc.h
 * \brief Header for AVR Adc driver
 */

/*!
 * \addtogroup xgAvrAdc
 */
 
/*@{*/ 

#ifndef _ADC_H_
#define _ADC_H_

// ENUM declaring possible ADC reference voltages
//   - AVCC = 5V
//   - AREF = External reference
//   - INTERNAL_256 = 2.56V

enum adc_ref_type
{
    AVCC	= 0,
    AREF,
    INTERNAL_256
};

typedef enum adc_ref_type adc_ref_t;

// ENUM declaring possible ADC modes
// FREE_RUNNING:
//    Free-running mode. Samples continuously taken
//    every 13 cycles of ADC clock after
//    ADC_start_conversion() is called.
// SINGLE_CONVERSION:
//    Single-conversion mode. One sample taken every time
//    ADC_start_conversion() is called.

enum adc_mode_type
{
    ADC_OFF,
    FREE_RUNNING,
    SINGLE_CONVERSION
};

typedef enum adc_mode_type adc_mode_t;

// ENUM declaring possible ADC channels

enum adc_channel_type
{
    ADC0=0,
    ADC1=1,
    ADC2=2,
    ADC3=3,
    ADC4=4,
    ADC5=5,
    ADC6=6,
    ADC7=7
};

typedef enum adc_channel_type adc_channel_t;

#define ADC_PRESCALE_DIV2               0x00    ///< 0x01,0x00 -> CPU clk/2
#define ADC_PRESCALE_DIV4               0x02    ///< 0x02 -> CPU clk/4
#define ADC_PRESCALE_DIV8               0x03    ///< 0x03 -> CPU clk/8
#define ADC_PRESCALE_DIV16              0x04    ///< 0x04 -> CPU clk/16
#define ADC_PRESCALE_DIV32              0x05    ///< 0x05 -> CPU clk/32
#define ADC_PRESCALE_DIV64              0x06    ///< 0x06 -> CPU clk/64
#define ADC_PRESCALE_DIV128             0x07    ///< 0x07 -> CPU clk/128


// ADC_Init                       
//                                                    
// This function initializes the ADC based on the
//  #defines in config.h                       
//                                                    
// post: ADC initialized and primed for call to
//       start_conversion

void ADCInit(void);


// ADC_SetRef                      
//                                                    
// Allows setting of reference voltage for ADC.
//
// NOTE: This function stops ADC conversion. One must
//       call ADC_start_conversion to restart the ADC.
//                                                    
// pre: "reference" is a valid ADC reference from the
//       choices given above
// post: ADC conversion stopped and reference voltage
//       set to desired choice

void ADCSetRef(adc_ref_t reference);


// ADC_SetMode                      
//                                                    
// Allows setting of ADC conversion mode: either
// single-conversion or free-running.
//
// NOTE: This function stops ADC conversion. One must
//       call ADC_start_conversion to restart the ADC.
//                                                    
// pre: "mode" is a valid ADC reference from the
//       choices given above
// post: ADC conversion stopped and the ADC mode is
//       set to desired choice

void ADCSetMode(adc_mode_t mode);


// ADC_SetPrescale
//                                                    
// Allows setting of ADC clock prescalar (ADC rate).
// The  ADC rate is given by the system clock rate
// divided by the prescalar value. Possible prescalar
// values range from 2-128 in powers of 2 (2,4,8,etc.)
//
// NOTE: This function stops ADC conversion. One must
//       call ADC_start_conversion to restart the ADC.
//                                                    
// pre: "prescalar" is a valid ADC reference from the
//       choices given above
// post: ADC conversion stopped and ADC prescalar
//       set to desired choice

uint8_t ADCSetPrescale(uint8_t prescalar);


// ADC_SetChannel
//                                                    
// Sets the channel that the ADC reads. The ADC
// may only read from one channel at a time.
//                                                    
// pre: "adc_channel" is a valid ADC reference from the
//       choices given above
// post: ADC conversion stopped and ADC channel
//       set to desired choice

void ADCSetChannel(adc_channel_t adc_channel);


// ADC_BufferFlush
//                                                    
// Flushes the local buffer used to store ADC values
// between conversion and the user's call to ADC_read
//
// NOTE: It is recommended that one calls buffer flush
//       if any changes are made to the ADC's state.
//                                                    
// pre: none
// post: Local ADC buffer has been flushed

void ADCBufferFlush(void);


// ADC_StartConversion
//                                                    
// Begins ADC conversion. If in single-conversion mode,
// this function will only convert one value. If in
// free-running mode, this function will begin
// continuous conversion at the rate set by the
// prescalar (see ADC_set_prescalar).
//
// NOTE: Converted values from the ADC are stored
//       in a local buffer. The user must call
//       ADC_read to obtain these values.
//
// pre:  none
// post: The ADC has started conversion. Completion of
//       any conversions is not guaranteed.

void ADCStartConversion(void);


// ADC_StartLowNoiseConversion
//                                                    
// Set Conversion Mode to SINGLE_CONVERSION, Enters 
// adc sleep mode and wait until conversion interrupt occurs.
// CPU will go to sleep mode!!!
// BE AWARE OF WHAT IS WRITTEN IN THE AVR DATASHEET
//
// NOTE: Converted values from the ADC are stored
//       in a local buffer. The user must call
//       ADC_read to obtain these values.
//
//       Only implemented for avr_gcc. Any other architecture 
//       and compiler will use normal conversion
// pre:  none
// post: The ADC has started conversion. Completion of
//       any conversions is not guaranteed.


void ADCStartLowNoiseConversion(void);

// ADC_StopConversion
//                                                    
// Stops ADC conversion if ADC is in free-running
// mode. Has no effect if ADC is in single-conversion
// mode.
//
// pre:  none
// post: ADC conversion has been stopped.

void ADCStopConversion(void);


// ADC_read
//                                                    
// Reads ADC values from local buffer. Reads one ADC
// conversion value at a time.
//
// return: 0 = OK
//         1 = No ADC value to read. "value" is invalid
//
// pre:  "value" points to a valid variable.
// post: If no errors, one ADC conversion value has
//       been read and placed in the variable pointed
//       to by "value"

uint8_t ADCRead(uint16_t *value);

// ADC_GetMode
// returns current conversion mode

inline adc_mode_t ADCGetMode(void);

#endif
/*@}*/
