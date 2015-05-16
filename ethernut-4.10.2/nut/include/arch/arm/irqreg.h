#ifndef _DEV_IRQREG_ARM_H_
#define _DEV_IRQREG_ARM_H_

/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.19  2010/12/15 21:46:28  ve2yag
 * Add PWM interrupt support for AT91SAM7S and SE.
 *
 * Revision 1.18  2010/12/15 13:22:28  ve2yag
 * Add support for all AT91SAM7S and SE family.
 *
 * Revision 1.17  2008/10/03 11:31:28  haraldkipp
 * Added TWI support for the AT91SAM9260.
 *
 * Revision 1.16  2008/08/06 12:51:11  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.15  2008/02/15 17:00:08  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.14  2008/01/31 09:22:32  haraldkipp
 * Added first version of platform independent GPIO routines. Consider the
 * AVR version untested.
 *
 * Revision 1.13  2007/12/09 21:29:33  olereinhardt
 * Added adc support
 *
 * Revision 1.12  2007/10/04 20:27:58  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.11  2007/09/06 19:46:47  olereinhardt
 * Added interrupt definitions for SPI/TWI
 *
 * Revision 1.10  2007/02/15 16:29:23  haraldkipp
 * Support for system controller interrupts added.
 *
 * Revision 1.9  2006/09/29 12:34:59  haraldkipp
 * Basic AT91 SPI support added.
 *
 * Revision 1.8  2006/09/08 16:47:49  haraldkipp
 * For some reason the SSC driver for SAM7X had not been included.
 *
 * Revision 1.7  2006/09/05 12:33:45  haraldkipp
 * SSC interrupt handler added.
 *
 * Revision 1.6  2006/08/31 19:04:08  haraldkipp
 * Added support for the AT91SAM9260 and Atmel's AT91SAM9260 Evaluation Kit.
 *
 * Revision 1.5  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 * Revision 1.4  2006/06/28 17:22:34  haraldkipp
 * Make it compile for AT91SAM7X256.
 *
 * Revision 1.3  2006/02/23 15:33:59  haraldkipp
 * Support for Philips LPC2xxx Family and LPC-E2294 Board from Olimex added.
 * Many thanks to Michael Fischer for this port.
 *
 * Revision 1.2  2005/10/24 10:26:21  haraldkipp
 * AT91 handlers added.
 *
 * Revision 1.1  2005/07/26 18:35:09  haraldkipp
 * First check in
 *
 * Revision 1.3  2005/04/05 17:52:40  haraldkipp
 * Much better implementation of GBA interrupt registration.
 *
 * Revision 1.2  2004/09/08 10:52:31  haraldkipp
 * Tyou's support for the SAMSUNG S3C45
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 *
 */

#ifdef S3C4510B
#include "s3c4510b_irqreg.h"

#elif defined(MCU_GBA)

extern IRQ_HANDLER sig_VBLANK;
extern IRQ_HANDLER sig_HBLANK;
extern IRQ_HANDLER sig_VCOUNT;
extern IRQ_HANDLER sig_TMR0;
extern IRQ_HANDLER sig_TMR1;
extern IRQ_HANDLER sig_TMR2;
extern IRQ_HANDLER sig_TMR3;
extern IRQ_HANDLER sig_SIO;
extern IRQ_HANDLER sig_DMA0;
extern IRQ_HANDLER sig_DMA1;
extern IRQ_HANDLER sig_DMA2;
extern IRQ_HANDLER sig_DMA3;
extern IRQ_HANDLER sig_KEYPAD;
extern IRQ_HANDLER sig_GAMEPAK;

extern void InitIrqHandler(void);

#elif defined(MCU_AT91R40008)

extern IRQ_HANDLER sig_FIQ;
extern IRQ_HANDLER sig_SWIRQ;
extern IRQ_HANDLER sig_UART0;
extern IRQ_HANDLER sig_UART1;
extern IRQ_HANDLER sig_TC0;
extern IRQ_HANDLER sig_TC1;
extern IRQ_HANDLER sig_TC2;
extern IRQ_HANDLER sig_WDI;
extern IRQ_HANDLER sig_PIO;
extern IRQ_HANDLER sig_INTERRUPT0;
extern IRQ_HANDLER sig_INTERRUPT1;
extern IRQ_HANDLER sig_INTERRUPT2;

#elif defined(MCU_LPC2XXX)

extern IRQ_HANDLER sig_UART0;
extern IRQ_HANDLER sig_UART1;
extern IRQ_HANDLER sig_TC0;
extern IRQ_HANDLER sig_TC1;
extern IRQ_HANDLER sig_WDI;
extern IRQ_HANDLER sig_INTERRUPT0;
extern IRQ_HANDLER sig_INTERRUPT1;
extern IRQ_HANDLER sig_INTERRUPT2;
extern IRQ_HANDLER sig_INTERRUPT2;

#elif defined(MCU_AT91SAM7X)

extern IRQ_HANDLER sig_FIQ;
extern IRQ_HANDLER sig_SYS;
extern IRQ_HANDLER sig_UART0;
extern IRQ_HANDLER sig_UART1;
extern IRQ_HANDLER sig_TC0;
extern IRQ_HANDLER sig_TC1;
extern IRQ_HANDLER sig_TC2;
extern IRQ_HANDLER sig_INTERRUPT0;
extern IRQ_HANDLER sig_INTERRUPT1;
extern IRQ_HANDLER sig_EMAC;
extern IRQ_HANDLER sig_PIOA;
extern IRQ_HANDLER sig_PIOB;
extern IRQ_HANDLER sig_PIOC;
extern IRQ_HANDLER sig_SWIRQ;
extern IRQ_HANDLER sig_SSC;
extern IRQ_HANDLER sig_SPI0;
extern IRQ_HANDLER sig_SPI1;
extern IRQ_HANDLER sig_TWI;
extern IRQ_HANDLER sig_ADC;

/*
 * Registered system interrupt handler information structure.
 */
typedef struct {
    void *sir_arg;
    void (*sir_handler) (void *);
    int sir_enabled;
} SYSIRQ_HANDLER;

extern SYSIRQ_HANDLER syssig_DBGU;
extern SYSIRQ_HANDLER syssig_MC;
extern SYSIRQ_HANDLER syssig_PIT;
extern SYSIRQ_HANDLER syssig_PMC;
extern SYSIRQ_HANDLER syssig_RSTC;
extern SYSIRQ_HANDLER syssig_RTT;
extern SYSIRQ_HANDLER syssig_WDT;

extern int NutRegisterSysIrqHandler(SYSIRQ_HANDLER * sysirq, void (*handler) (void *), void *arg);
extern int NutSysIrqEnable(SYSIRQ_HANDLER * sysirq);
extern int NutSysIrqDisable(SYSIRQ_HANDLER * sysirq);

#elif defined(MCU_AT91SAM7S) || defined(MCU_AT91SAM7SE)

extern IRQ_HANDLER sig_FIQ;
extern IRQ_HANDLER sig_SYS;
extern IRQ_HANDLER sig_UART0;
extern IRQ_HANDLER sig_UART1;
extern IRQ_HANDLER sig_TC0;
extern IRQ_HANDLER sig_TC1;
extern IRQ_HANDLER sig_TC2;
extern IRQ_HANDLER sig_PWMC;
extern IRQ_HANDLER sig_INTERRUPT0;
extern IRQ_HANDLER sig_INTERRUPT1;
extern IRQ_HANDLER sig_PIOA;
extern IRQ_HANDLER sig_PIOB;
extern IRQ_HANDLER sig_PIOC;
extern IRQ_HANDLER sig_SWIRQ;
extern IRQ_HANDLER sig_SSC;
extern IRQ_HANDLER sig_SPI0;
extern IRQ_HANDLER sig_TWI;

/*
 * Registered system interrupt handler information structure.
 */
typedef struct {
    void *sir_arg;
    void (*sir_handler) (void *);
    int sir_enabled;
} SYSIRQ_HANDLER;

extern SYSIRQ_HANDLER syssig_DBGU;
extern SYSIRQ_HANDLER syssig_MC;
extern SYSIRQ_HANDLER syssig_PIT;
extern SYSIRQ_HANDLER syssig_PMC;
extern SYSIRQ_HANDLER syssig_RSTC;
extern SYSIRQ_HANDLER syssig_RTT;
extern SYSIRQ_HANDLER syssig_WDT;

extern int NutRegisterSysIrqHandler(SYSIRQ_HANDLER * sysirq, void (*handler) (void *), void *arg);
extern int NutSysIrqEnable(SYSIRQ_HANDLER * sysirq);
extern int NutSysIrqDisable(SYSIRQ_HANDLER * sysirq);

#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512) || defined(MCU_AT91SAM9G45)

extern IRQ_HANDLER sig_FIQ;
extern IRQ_HANDLER sig_UART0;
extern IRQ_HANDLER sig_UART1;
extern IRQ_HANDLER sig_TC0;
extern IRQ_HANDLER sig_TC1;
extern IRQ_HANDLER sig_TC2;
extern IRQ_HANDLER sig_INTERRUPT0;
extern IRQ_HANDLER sig_INTERRUPT1;
extern IRQ_HANDLER sig_EMAC;
extern IRQ_HANDLER sig_PIOA;
extern IRQ_HANDLER sig_PIOB;
extern IRQ_HANDLER sig_PIOC;
extern IRQ_HANDLER sig_SWIRQ;
extern IRQ_HANDLER sig_SSC;
extern IRQ_HANDLER sig_SPI0;
extern IRQ_HANDLER sig_SPI1;
extern IRQ_HANDLER sig_TWI;

/*
 * Registered system interrupt handler information structure.
 */
typedef struct {
    void *sir_arg;
    void (*sir_handler) (void *);
    int sir_enabled;
} SYSIRQ_HANDLER;

extern SYSIRQ_HANDLER syssig_DBGU;
extern SYSIRQ_HANDLER syssig_MC;
extern SYSIRQ_HANDLER syssig_PIT;
extern SYSIRQ_HANDLER syssig_PMC;
extern SYSIRQ_HANDLER syssig_RSTC;
extern SYSIRQ_HANDLER syssig_RTT;
extern SYSIRQ_HANDLER syssig_WDT;

extern int NutRegisterSysIrqHandler(SYSIRQ_HANDLER * sysirq, void (*handler) (void *), void *arg);
extern int NutSysIrqEnable(SYSIRQ_HANDLER * sysirq);
extern int NutSysIrqDisable(SYSIRQ_HANDLER * sysirq);

#else
#warning "No MCU defined"
#endif

#endif
