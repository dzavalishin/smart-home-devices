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
*                   The CrossWorks for ARM toolchain will be used.
****************************************************************************/
#ifndef __LPC2XXX_H__
#define __LPC2XXX_H__

/*
 * Include the header file for the lpc22xx, from CrossWorks
 */
#include <targets/lpc22xx.h>

/*
 * For the Debug macro we must include cross_studio_io.h
 */
#define Debug             debug_printf

#define VIC_WDT           0
#define VIC_TIMER0        4
#define VIC_TIMER1        5
#define VIC_UART0         6
#define VIC_UART1         7
#define VIC_PWM0          8
#define VIC_I2C           9
#define VIC_SPI0          10
#define VIC_SPI1          11
#define VIC_PLL           12
#define VIC_RTC           13
#define VIC_EINT0         14
#define VIC_EINT1         15
#define VIC_EINT2         16
#define VIC_EINT3         17

#define IRQ_ENTRY()
#define IRQ_EXIT()

/*!
 * \brief Case insensitive string comparisions.
 *
 * Not supported by CrossWorks and temporarly redirected to
 * the case sensitive routines.
 *
 */
#define strcasecmp(s1, s2)      strcmp(s1, s2)
#define strncasecmp(s1, s2, n)  strncmp(s1, s2, n)

#endif /* __LPC2XXX_H__ */
/*** EOF ***/

