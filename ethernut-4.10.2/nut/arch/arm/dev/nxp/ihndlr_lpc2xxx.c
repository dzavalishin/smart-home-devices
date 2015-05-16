/****************************************************************************
*  This file is part of the Ethernut port for the LPC2XXX
*
*  Copyright (c) 2005 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*
*  History:
*
*  24.09.05  mifi   First Version
*                   The CrossWorks for ARM toolchain will be used. Some of 
*                   this functions looks similar like my ARM uC/OS port for 
*                   CrossWorks because I have done it before (AN-1711B).
*                   For some bit definition and information, take a look
*                   in the LPC2119/2129/2194/2292/2294 user manual 2004 May 03.
****************************************************************************/
#define __IHNDLR_LPC2XXX_C__

#include <arch/arm/lpc2xxx.h>
#include <stdio.h>

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/
/*
 * Take a look in the LPC2294 user manual (2004 May 03) page 52
 */
#define UNDEFINED_INSTRUCTION_VECTOR_ADDR   (*(uint32_t *)0x00000004L)   
#define SOFTWARE_INTERRUPT_VECTOR_ADDR      (*(uint32_t *)0x00000008L)
#define PREFETCH_ABORT_VECTOR_ADDR          (*(uint32_t *)0x0000000CL)
#define DATA_ABORT_VECTOR_ADDR              (*(uint32_t *)0x00000010L)
#define IRQ_VECTOR_ADDR                     (*(uint32_t *)0x00000018L)
#define FIQ_VECTOR_ADDR                     (*(uint32_t *)0x0000001CL)

#define DATA_ABORT_ISR_ADDR                 (*(uint32_t *)0x00000030L)
#define IRQ_ISR_ADDR                        (*(uint32_t *)0x00000038L)
#define FIQ_ISR_ADDR                        (*(uint32_t *)0x0000003CL)

typedef  void (*FNCPTR)(void);

/*=========================================================================*/
/*  DEFINE: Definition of all local Data                                   */
/*=========================================================================*/

/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/
/***************************************************************************/
/*  VICInit                                                                */
/*                                                                         */
/*  This function will "init" the VIC.                                     */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
static void VICInit (void)
{
   /*
    * Disable ALL interrupts
    */
   VICIntEnClr = 0xFFFFFFFF;

   /* 
    * VIC register can be accessed in
    * User or privileged mode.
    */
   VICProtection = 0;
   
   /*
    * All interrupts assigned to IRQ category
    */
   VICIntSelect = 0;
} /* VICInit */

/***************************************************************************/
/*  ABORTHandler                                                           */
/*                                                                         */
/*  This is the ABORT handler.                                             */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
static void ABORTHandler(void) __attribute__ ((interrupt ("IRQ")));
static void ABORTHandler (void)
{ 
  /*
   * The next lines are only used for debugging purpose
   */
  volatile uint32_t AbortCounter = 0;    
  
  AbortCounter++;  
} /* ABORTHandler */

/***************************************************************************/
/*  IRQHandler                                                             */
/*                                                                         */
/*  This is the IRQ handler. If an IRQ occurred, the address of the        */ 
/*  ISR can be read from the VICVectAddr.                                  */
/*                                                                         */
/*  1. Read the address of the IRQHandler to execute from VICVectAddr      */
/*  2. But check if the address is NOT 0.                                  */
/*  3. Jump to the IRQHandler                                              */
/*  4. Read VICVectAddr again to check if more interrupts are available    */
/*     to execute.                                                         */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
static void IRQHandler(void) __attribute__ ((interrupt ("IRQ")));
static void IRQHandler (void)
{
  FNCPTR  fncptr;

  fncptr = (FNCPTR)VICVectAddr;     /* 1 */
  while (fncptr != (FNCPTR)0) {     /* 2 */
    (*fncptr)();                    /* 3 */
    fncptr = (FNCPTR)VICVectAddr;   /* 4 */
  }
} /* IRQHandler */

/***************************************************************************/
/*  FIQHandler                                                             */
/*                                                                         */
/*  This is the FIQ handler. If an FIQ occurred, the address of the        */ 
/*  ISR can be read from the VICVectAddr                                   */
/*                                                                         */
/*  1. Read the address of the IRQHandler to execute from VICVectAddr      */
/*  2. But check if the address is NOT 0.                                  */
/*  3. Jump to the IRQHandler                                              */
/*  4. Read VICVectAddr again to check if more interrupts are available    */
/*     to execute.                                                         */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
static void FIQHandler(void) __attribute__ ((interrupt ("FIQ")));
static void FIQHandler (void)
{
  FNCPTR  fncptr;

  fncptr = (FNCPTR)VICVectAddr;     /* 1 */
  while (fncptr != (FNCPTR)0) {     /* 2 */
    (*fncptr)();                    /* 3 */
    fncptr = (FNCPTR)VICVectAddr;   /* 4 */
  }
} /* FIQHandler */

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
/***************************************************************************/
/*  InitIrqHandler                                                         */
/*                                                                         */
/*  We will map the ARM interrupt vectors to the RAM.                      */
/*  With this trick, the interrupt latency is a less than from flash.      */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void InitIrqHandler (void)
{
  /*
   * User RAM Mode. Interrupt vecors are re-mapped to Static RAM.
   */
  MEMMAP = 2;
  
  /*
   * I got the information how to setup the interrupt
   * handler from the uC/OS port for the ARM. Take a look
   * in the application note AN-1014 from micrium.
   * (www.micrium.com)
   */
  
  /*
   * Set IRQHandler
   */
  IRQ_VECTOR_ADDR = 0xE59FF018;           /* LDR PC,[PC,#0x18] instruction */
  IRQ_ISR_ADDR    = (uint32_t)IRQHandler;   /* IRQ exception vector address  */

  /*
   * Set FIQHandler
   */
  FIQ_VECTOR_ADDR = 0xE59FF018;           /* LDR PC,[PC,#0x18] instruction */
  FIQ_ISR_ADDR    = (uint32_t)FIQHandler;   /* FIQ exception vector address  */

  /*
   * We does not need the next interrupt sources in the moment,
   * therefore jump to itself.
   */
  UNDEFINED_INSTRUCTION_VECTOR_ADDR = 0xEAFFFFFE;
  SOFTWARE_INTERRUPT_VECTOR_ADDR    = 0xEAFFFFFE;
  PREFETCH_ABORT_VECTOR_ADDR        = 0xEAFFFFFE;

  /*
   * In case we must find an ABORT error,
   * enable the next lines and set a breakpoint
   * in ABORTHandler.
   */  
#if 1
  DATA_ABORT_VECTOR_ADDR = 0xE59FF018;
  DATA_ABORT_ISR_ADDR    = (uint32_t)ABORTHandler;
#endif  

  /*
   * Init the Vectored Interrupt Controller (VIC)
   */
  VICInit();
} /* InitIrqHandler */

/*** EOF ***/

