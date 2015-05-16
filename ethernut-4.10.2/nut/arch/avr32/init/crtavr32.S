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
 *
 * Portions Copyright Atmel Corporation, see the following note. 
 */ 

/* This file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AVR32UC C runtime startup file.
 *
 * This file has been built from the Newlib crt0.S.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32UC devices can be used.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#include <avr32/io.h>

// Include NutOS header files
#include <arch/avr32.h>
#include <cfg/clock.h>
#include <cfg/memory.h>

#ifndef PLL_DIV_VAL
#define PLL_DIV_VAL 1
#endif

#ifndef PLL_MUL_VAL
#define PLL_MUL_VAL 0
#endif

//! @{
//! \verbatim

  // Moves efficiently an immediate value of up to 32 bits into a register.
  .macro  mov.w   rd, imm
    .if ((-(1 << (21 - 1))) <= \imm) && (\imm <= ((1 << (21 - 1)) - 1))
      mov     \rd, \imm
#if __AVR32_UC__ >= 2
    .elseif !(\imm & 0x0000FFFF)
      movh    \rd, HI(\imm)
#endif
    .else
      mov     \rd, LO(\imm)
      orh     \rd, HI(\imm)
    .endif
  .endm
  
  // Set a peripheral mux.
  .macro GpioSetPinFunction pin, value
    mov.w r0, (AVR32_GPIO_ADDRESS+(0x100 * (\pin >> 5)))
    eor r1, r1
    sbr r1, (\pin & 0x1f)
    .if \value == 0
      st.w r0[AVR32_GPIO_PMR1C], r1
      st.w r0[AVR32_GPIO_PMR0C], r1
    .elseif \value == 1
      st.w r0[AVR32_GPIO_PMR1C], r1
      st.w r0[AVR32_GPIO_PMR0S], r1
    .elseif \value == 2    
      st.w r0[AVR32_GPIO_PMR1S], r1
      st.w r0[AVR32_GPIO_PMR0C], r1
    .else
      st.w r0[AVR32_GPIO_PMR1S], r1
      st.w r0[AVR32_GPIO_PMR0S], r1
    .endif
   st.w r0[AVR32_GPIO_GPERC], r1
  .endm


  // This must be linked @ 0x80000000 if it is to be run upon reset.
  .section  .reset, "ax", @progbits


  .global _start
  .type _start, @function
_start:
  // Jump to the C runtime startup routine.
  lda.w   pc, _stext


  // _stext is placed outside the .reset section so that the program entry point
  // can be changed without affecting the C runtime startup.
  .section  .text._stext, "ax", @progbits


  .global _stext
  .type _stext, @function
_stext:

  /*
   * Disable Watchdog
   */
#define WDT_KEY1 (AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET)
#define WDT_KEY2 ((~AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET) & AVR32_WDT_CTRL_KEY_MASK)
  mov.w r0, (AVR32_WDT_ADDRESS)
  ld.w  r1, r0[AVR32_WDT_CTRL]
  cbr   r1, AVR32_WDT_CTRL_EN_OFFSET
  mov   r2, r1
  mov.w r3, WDT_KEY1
  mov.w r4, WDT_KEY2
  or    r1, r3
  or    r2, r4		
  st.w  r0[AVR32_WDT_CTRL], r1
  st.w  r0[AVR32_WDT_CTRL], r2

  /*
   * Enable SDRAM interface, if configured.
   */
#ifdef NUTMEM_SDRAM_BASE
  // start of with pins driven / needed by EBI
  
  // Enable data pins
  GpioSetPinFunction AVR32_EBI_DATA_0_PIN, AVR32_EBI_DATA_0_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_1_PIN, AVR32_EBI_DATA_1_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_2_PIN, AVR32_EBI_DATA_2_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_3_PIN, AVR32_EBI_DATA_3_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_4_PIN, AVR32_EBI_DATA_4_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_5_PIN, AVR32_EBI_DATA_5_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_6_PIN, AVR32_EBI_DATA_6_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_7_PIN, AVR32_EBI_DATA_7_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_8_PIN, AVR32_EBI_DATA_8_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_9_PIN, AVR32_EBI_DATA_9_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_10_PIN, AVR32_EBI_DATA_10_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_11_PIN, AVR32_EBI_DATA_11_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_12_PIN, AVR32_EBI_DATA_12_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_13_PIN, AVR32_EBI_DATA_13_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_14_PIN, AVR32_EBI_DATA_14_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_15_PIN, AVR32_EBI_DATA_15_FUNCTION
#if NUTMEM_SDRAM_DBW >= 32
  GpioSetPinFunction AVR32_EBI_DATA_16_PIN, AVR32_EBI_DATA_16_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_17_PIN, AVR32_EBI_DATA_17_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_18_PIN, AVR32_EBI_DATA_18_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_19_PIN, AVR32_EBI_DATA_19_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_20_PIN, AVR32_EBI_DATA_20_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_21_PIN, AVR32_EBI_DATA_21_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_22_PIN, AVR32_EBI_DATA_22_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_23_PIN, AVR32_EBI_DATA_23_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_24_PIN, AVR32_EBI_DATA_24_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_25_PIN, AVR32_EBI_DATA_25_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_26_PIN, AVR32_EBI_DATA_26_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_27_PIN, AVR32_EBI_DATA_27_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_28_PIN, AVR32_EBI_DATA_28_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_29_PIN, AVR32_EBI_DATA_29_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_30_PIN, AVR32_EBI_DATA_30_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_31_PIN, AVR32_EBI_DATA_31_FUNCTION
  GpioSetPinFunction AVR32_EBI_DATA_32_PIN, AVR32_EBI_DATA_32_FUNCTION
#endif

  // Enable row/column address pins.
  GpioSetPinFunction AVR32_EBI_ADDR_2_PIN, AVR32_EBI_ADDR_2_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_3_PIN, AVR32_EBI_ADDR_3_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_4_PIN, AVR32_EBI_ADDR_4_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_5_PIN, AVR32_EBI_ADDR_5_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_6_PIN, AVR32_EBI_ADDR_6_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_7_PIN, AVR32_EBI_ADDR_7_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_8_PIN, AVR32_EBI_ADDR_8_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_9_PIN, AVR32_EBI_ADDR_9_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_10_PIN, AVR32_EBI_ADDR_10_FUNCTION
  GpioSetPinFunction AVR32_EBI_ADDR_11_PIN, AVR32_EBI_ADDR_11_FUNCTION
  GpioSetPinFunction AVR32_EBI_SDA10_0_PIN, AVR32_EBI_SDA10_0_FUNCTION
#if NUTMEM_SDRAM_ROWBITS >= 12
  GpioSetPinFunction AVR32_EBI_ADDR_13_PIN, AVR32_EBI_ADDR_13_FUNCTION
# if NUTMEM_SDRAM_ROWBITS >= 12
  GpioSetPinFunction AVR32_EBI_ADDR_14_PIN, AVR32_EBI_ADDR_14_FUNCTION
# endif
#endif

   // Enable bank address pins.
   GpioSetPinFunction AVR32_EBI_ADDR_16_PIN, AVR32_EBI_ADDR_16_FUNCTION 
#if NUTMEM_SDRAM_BANKS >= 2
   GpioSetPinFunction AVR32_EBI_ADDR_17_PIN, AVR32_EBI_ADDR_17_FUNCTION
#endif

   // Enable data mask pins.
   GpioSetPinFunction AVR32_EBI_ADDR_0_PIN, AVR32_EBI_ADDR_0_FUNCTION
   GpioSetPinFunction AVR32_EBI_NWE1_0_PIN, AVR32_EBI_NWE1_0_FUNCTION
#if NUTMEM_SDRAM_DBW >= 32
   GpioSetPinFunction AVR32_EBI_ADDR_1_PIN, AVR32_EBI_ADDR_1_FUNCTION
   GpioSetPinFunction AVR32_EBI_NWE3_0_PIN, AVR32_EBI_NWE3_0_FUNCTION
#endif

   // Enable control pins.
   GpioSetPinFunction AVR32_EBI_SDWE_0_PIN, AVR32_EBI_SDWE_0_FUNCTION
   GpioSetPinFunction AVR32_EBI_CAS_0_PIN, AVR32_EBI_CAS_0_FUNCTION
   GpioSetPinFunction AVR32_EBI_RAS_0_PIN, AVR32_EBI_RAS_0_FUNCTION
   GpioSetPinFunction AVR32_EBI_NCS_1_PIN, AVR32_EBI_NCS_1_FUNCTION

   // Enable clock-related pins.
   GpioSetPinFunction AVR32_EBI_SDCK_0_PIN, AVR32_EBI_SDCK_0_FUNCTION
   GpioSetPinFunction AVR32_EBI_SDCKE_0_PIN, AVR32_EBI_SDCKE_0_FUNCTION
   
   // Set R0 to HMatrix SFR5 base address
   mov.w r0, (AVR32_HMATRIX_ADDRESS+AVR32_HMATRIX_SFR0+(4*AVR32_EBI_HMATRIX_NR))

   ld.w  r1, r0
   sbr   r1, AVR32_EBI_SDRAM_CS	// Enable SDRAM mode for AVR32_EBI_SDRAM_CS.
   st.w  r0, r1					// Special Function Register AVR32_EBI_HMATRIX_NR

   // Configure the SDRAM Controller with SDRAM setup and timing information.
   // All timings below are rounded up because they are minimal values.

   // Frequency of the HSB in Mhz rounded up. This is the same as the CPU Clock
#define HSB_MHZ_UP ((((PLL_MUL_VAL+1)/PLL_DIV_VAL*OSC0_VAL) + 999999) / 1000000)
   // Frequency of the HSB in Mhz rounded down.
#define HSB_MHZ_DOWN (((PLL_MUL_VAL+1)/PLL_DIV_VAL*OSC0_VAL) / 1000000)
#define SDRAM_CR_VALUE \
      ((( NUTMEM_SDRAM_COLBITS                  -    8) << AVR32_SDRAMC_CR_NC_OFFSET  ) & AVR32_SDRAMC_CR_NC_MASK  ) | \
      ((( NUTMEM_SDRAM_ROWBITS                  -   11) << AVR32_SDRAMC_CR_NR_OFFSET  ) & AVR32_SDRAMC_CR_NR_MASK  ) | \
      ((( NUTMEM_SDRAM_BANKS                    -    1) << AVR32_SDRAMC_CR_NB_OFFSET  ) & AVR32_SDRAMC_CR_NB_MASK  ) | \
      ((  NUTMEM_SDRAM_CASLAT                           << AVR32_SDRAMC_CR_CAS_OFFSET ) & AVR32_SDRAMC_CR_CAS_MASK ) | \
      ((( NUTMEM_SDRAM_DBW                      >>   4) << AVR32_SDRAMC_CR_DBW_OFFSET ) & AVR32_SDRAMC_CR_DBW_MASK ) | \
      ((((NUTMEM_SDRAM_TWR  * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TWR_OFFSET ) & AVR32_SDRAMC_CR_TWR_MASK ) | \
      ((((NUTMEM_SDRAM_TRC  * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TRC_OFFSET ) & AVR32_SDRAMC_CR_TRC_MASK ) | \
      ((((NUTMEM_SDRAM_TRP  * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TRP_OFFSET ) & AVR32_SDRAMC_CR_TRP_MASK ) | \
      ((((NUTMEM_SDRAM_TRCD * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TRCD_OFFSET) & AVR32_SDRAMC_CR_TRCD_MASK) | \
      ((((NUTMEM_SDRAM_TRAS * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TRAS_OFFSET) & AVR32_SDRAMC_CR_TRAS_MASK) | \
      ((((NUTMEM_SDRAM_TXSR * HSB_MHZ_UP + 999) / 1000) << AVR32_SDRAMC_CR_TXSR_OFFSET) & AVR32_SDRAMC_CR_TXSR_MASK)
  mov.w r1, SDRAM_CR_VALUE

  mov.w r0, (AVR32_SDRAMC_ADDRESS)		
  st.w  r0[AVR32_SDRAMC_CR], r1

  //  AVR32_SDRAMC.mr = AVR32_SDRAMC_MR_MODE_NOP;
  mov.w r1, AVR32_SDRAMC_MR_MODE_NOP					// Mode Register value = NOP
  st.w  r0[AVR32_SDRAMC_MR], r1
  ld.w  r1, r0[AVR32_SDRAMC_MR]						// Cause a stall and flush the value.

  mov.w r0, NUTMEM_SDRAM_BASE		// SDRAM start address
  st.w  r0, r1					// write to DRAM to trigger the NOP

  // Wait during the SDRAM stable-clock initialization delay.
  mov.w r12, HSB_MHZ_UP*200		// delay 200u
  call sdramc_delay

  // Issue a PRECHARGE ALL command to the SDRAM.
  mov.w r0, (AVR32_SDRAMC_ADDRESS)	// Mode Register
  mov.w r1, (AVR32_SDRAMC_MR_MODE_BANKS_PRECHARGE)	// All banks precharge
  st.w  r0[AVR32_SDRAMC_MR], r1
  ld.w  r1, r0[AVR32_SDRAMC_MR]						// Cause a stall and flush the value.

  mov.w r0, NUTMEM_SDRAM_BASE		// SDRAM start address
  st.w  r0, r1					// write to DRAM to trigger precharge command
  
  // Wait during the SDRAM TRP delay.
  mov.w r12, HSB_MHZ_UP*NUTMEM_SDRAM_TRP
  call sdramc_delay

  // Issue initialization AUTO REFRESH commands to the SDRAM.
  mov.w r0, (AVR32_SDRAMC_ADDRESS)
  mov.w r1, (AVR32_SDRAMC_MR_MODE_AUTO_REFRESH)
  st.w  r0[AVR32_SDRAMC_MR], r1
  ld.w  r1, r0[AVR32_SDRAMC_MR]						// Cause a stall and flush the value.

  mov.w r12, HSB_MHZ_UP*NUTMEM_SDRAM_TRC
  mov.w r0, NUTMEM_SDRAM_BASE	// SDRAM start address
  st.w  r0, r1				// write to DRAM to trigger auto refresh command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command
  call sdramc_delay
  st.w  r0, r1				// write to DRAM to trigger precharge command

  // Issue a LOAD MODE REGISTER command to the SDRAM.
  // This configures the SDRAM with the following parameters in the mode register:
  //  - bits 0 to 2: burst length: 1 (000b);
  //  - bit 3: burst type: sequential (0b);
  //  - bits 4 to 6: CAS latency: AVR32_SDRAMC.CR.cas;
  //  - bits 7 to 8: operating mode: standard operation (00b);
  //  - bit 9: write burst mode: programmed burst length (0b);
  //  - all other bits: reserved: 0b.
  mov.w r0, (AVR32_SDRAMC_ADDRESS)		
  mov.w r1, (AVR32_SDRAMC_MR_MODE_LOAD_MODE)
  st.w  r0[AVR32_SDRAMC_MR], r1

  mov.w r0, NUTMEM_SDRAM_BASE		// SDRAM start address
  st.w  r0, r1					// write to DRAM to trigger "load mode register" command

  mov.w r12, HSB_MHZ_UP*NUTMEM_SDRAM_TMRD
  call sdramc_delay

  // Switch the SDRAM Controller to normal mode.
  mov.w r0, (AVR32_SDRAMC_ADDRESS)
  mov.w r1, (AVR32_SDRAMC_MR_MODE_NORMAL)
  st.w  r0[AVR32_SDRAMC_MR], r1
  ld.w  r1, r0[AVR32_SDRAMC_MR]						// Cause a stall and flush the value.
  
  mov.w r0, NUTMEM_SDRAM_BASE		// SDRAM start address
  st.w  r0, r1					// write to DRAM to trigger "load mode register" command

  // Write the refresh period into the SDRAMC Refresh Timer Register.
  // TR is rounded down because it is a maximal value.
  mov.w r0, (AVR32_SDRAMC_ADDRESS)	// Refresh Timer Register (TR)
  mov.w r1, (NUTMEM_SDRAM_TR * HSB_MHZ_DOWN) / 1000
  st.w  r0[AVR32_SDRAMC_MR], r1
  ld.w  r1, r0[AVR32_SDRAMC_MR]
   
#endif // NUTMEM_SDRAM_BASE

  // Set initial stack pointer.
  lda.w   sp, _estack

  // Set up EVBA so interrupts can be enabled.
  lda.w   r0, _evba
  mtsr    AVR32_EVBA, r0

  // Enable the exception processing.
  csrf    AVR32_SR_EM_OFFSET

  // Load initialized data having a global lifetime from the data LMA.
  lda.w   r0, _data
  lda.w   r1, _edata
  cp      r0, r1
  brhs    idata_load_loop_end
  lda.w   r2, _data_lma
idata_load_loop:
  ld.d    r4, r2++
  st.d    r0++, r4
  cp      r0, r1
  brlo    idata_load_loop
idata_load_loop_end:

  // Clear uninitialized data having a global lifetime in the blank static storage section.
  lda.w   r0, __bss_start
  lda.w   r1, _end
  cp      r0, r1
  brhs    udata_clear_loop_end
  mov     r2, 0
  mov     r3, 0
udata_clear_loop:
  st.d    r0++, r2
  cp      r0, r1
  brlo    udata_clear_loop
udata_clear_loop_end:

#ifdef CONFIG_FRAME_POINTER
  // Safety: Set the default "return" @ to the exit routine address.
  lda.w   lr, exit
#endif

  // Start the show.
  lda.w   pc, NutInit


#ifdef NUTMEM_SDRAM_BASE
  // sdramc_delay
  // r12 = Number of HSB clock cycles to wait.
  // Warning: This call clobbers r10 and r11
  .global sdramc_delay
  .type sdramc_delay, @function
sdramc_delay:
  // r10 = startCycle, r11 = endCycle
  mfsr	r10, AVR32_COUNT
  add		r11, r10, r12
  cp.w	r10, r11
  brls	.LCOUNTNOTWRAPPED

.LCOUNTWRAPPED:
  // while (AVR32_COUNT > endCycle);
  mfsr	r10, AVR32_COUNT
  cp.w    r10, r11
  brhi	.LCOUNTWRAPPED
.LCOUNTNOTWRAPPED:
  mfsr	r10, AVR32_COUNT
  cp.w    r10, r11
  brlo	.LCOUNTNOTWRAPPED
  
  mov pc, lr
  .size	sdramc_delay, .-sdramc_delay
#endif

//! \endverbatim
//! @}

#include "exception.S"
