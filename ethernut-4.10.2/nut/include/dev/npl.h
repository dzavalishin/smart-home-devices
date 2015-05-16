#ifndef _DEV_NPL_H_
#define _DEV_NPL_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH
 * Copyright (C) 2009 by egnite GmbH
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

#include <dev/irqreg.h>

/*!
 * \file dev/npl.h
 * \brief Nut Programmable Logic Header File
 *
 * \verbatim
 * $Id: npl.h 2817 2009-11-25 17:58:40Z haraldkipp $
 * \endverbatim
 */

/*!
 * \addtogroup xgNpl
 */
/*@{*/

/*!
 * \brief CPLD register base address.
 *
 * On Ethernut 3 the CPLD is selected via NCS2 (GPIO 27), which
 * is configured to 0x21000000 - 0x210FFFFF in the CRT initialization
 * startup file.
 */
#ifndef NPL_BASE
#if defined(ETHERNUT3)
#define NPL_BASE        0x21000000
#else
#define NPL_BASE        0xFF00
#endif
#endif

/*! \brief RS232 command register.
 */
#define NPL_RSCR        _SFR_MEM8(NPL_BASE + 0x00)

#define NPL_RSFON_BIT   0       /*!< \brief Force on bit. */
#define NPL_RSFON       (1 << NPL_RSFON_BIT)  /*!< \brief Force on mask. */
#define NPL_RSFOFF_BIT  1       /*!< \brief Force off bit. */
#define NPL_RSFOFF      (1 << NPL_RSFOFF_BIT)  /*!< \brief Force off mask. */
#define NPL_RSDTR_BIT   2       /*!< \brief DTR handshake bit. */
#define NPL_RSDTR       (1 << NPL_RSDTR_BIT)  /*!< \brief DTR handshake mask. */
#define NPL_RSRTS_BIT   3       /*!< \brief RTS handshake bit. */
#define NPL_RSRTS       (1 << NPL_RSRTS_BIT)  /*!< \brief RTS handshake mask. */
#define NPL_RSUS0E_BIT  5       /*!< \brief USART0 select bit. */
#define NPL_RSUS0E      (1 << NPL_RSUS0E_BIT)  /*!< \brief USART0 select mask. */
#define NPL_RSUS1E_BIT  6       /*!< \brief USART1 select bit. */
#define NPL_RSUS1E      (1 << NPL_RSUS1E_BIT)  /*!< \brief USART1 select mask. */
#define NPL_RSUS1P_BIT  7       /*!< \brief USART1 primary bit. */
#define NPL_RSUS1P      (1 << NPL_RSUS1P_BIT)  /*!< \brief USART1 primary mask. */

/*! \brief Interrupt mask register.
 */
#define NPL_IMR         _SFR_MEM16(NPL_BASE + 0x04)

/*! \brief SPI clock divider register.
 */
#define NPL_SPICTRL     _SFR_MEM8(NPL_BASE + 0x08)

/*! \brief Signal latch register.
 */
#define NPL_SLR         _SFR_MEM16(NPL_BASE + 0x0C)

/*! \brief Signal clear register.
 */
#define NPL_SCR         _SFR_MEM16(NPL_BASE + 0x10)

/*! \brief Status register.
 */
#define NPL_STATUS      _SFR_MEM16(NPL_BASE + 0x10)

#define NPL_RSCTS_BIT       0                       /*!< \brief RS232 CTS interrupt bit. */
#define NPL_RSCTS           (1 << NPL_RSCTS_BIT)    /*!< \brief RS232 CTS interrupt mask. */
#define NPL_RSDSR_BIT       1                       /*!< \brief RS232 DSR interrupt bit. */
#define NPL_RSDSR           (1 << NPL_RSDSR_BIT)    /*!< \brief RS232 DSR interrupt mask. */
#define NPL_RSDCD_BIT       2                       /*!< \brief RS232 DCD interrupt bit. */
#define NPL_RSDCD           (1 << NPL_RSDCD_BIT)    /*!< \brief RS232 DCD interrupt mask. */
#define NPL_RSRI_BIT        3                       /*!< \brief RS232 RI interrupt bit. */
#define NPL_RSRI            (1 << NPL_RSRI_BIT)     /*!< \brief RS232 RI interrupt mask. */
#define NPL_RTCALARM_BIT    4                       /*!< \brief RTC alarm interrupt bit. */
#define NPL_RTCALARM        (1 << NPL_RTCALARM_BIT) /*!< \brief RTC alarm interrupt mask. */
#define NPL_LANWAKEUP_BIT   5                       /*!< \brief NIC wakeup interrupt bit. */
#define NPL_LANWAKEUP       (1 << NPL_LANWAKEUP_BIT)/*!< \brief NIC wakeup interrupt mask. */
#define NPL_FMBUSY_BIT      6                       /*!< \brief FLASH ready interrupt bit. */
#define NPL_FMBUSY          (1 << NPL_FMBUSY_BIT)   /*!< \brief FLASH ready interrupt mask. */
#define NPL_MMCREADY_BIT    7                       /*!< \brief MMC shift register ready bit. */
#define NPL_MMCREADY        (1 << NPL_MMCREADY_BIT) /*!< \brief MMC shift register ready mask. */
#define NPL_RSINVAL_BIT     8                       /*!< \brief RS232 invalid interrupt bit. */
#define NPL_RSINVAL         (1 << NPL_RSINVAL_BIT)  /*!< \brief RS232 invalid interrupt mask. */
#define NPL_NRSINVAL_BIT    9                       /*!< \brief RS232 valid interrupt bit. */
#define NPL_NRSINVAL        (1 << NPL_NRSINVAL_BIT) /*!< \brief RS232 valid interrupt mask. */
#define NPL_MMCD_BIT        10                      /*!< \brief MMC insert interrupt bit. */
#define NPL_MMCD            (1 << NPL_MMCD_BIT)     /*!< \brief MMC insert interrupt mask. */
#define NPL_NMMCD_BIT       11                      /*!< \brief MMC remove interrupt bit. */
#define NPL_NMMCD           (1 << NPL_NMMCD_BIT)    /*!< \brief MMC remove interrupt mask. */

/*! \brief MMC data register.
 */
#define NPL_MMCDR       _SFR_MEM8(NPL_BASE + 0x14)

/*! \brief External enable register.
 */
#define NPL_XER         _SFR_MEM8(NPL_BASE + 0x18)

#define NPL_MMCS        0x0001  /*!< \brief MMC select. */
#define NPL_PANCS       0x0002  /*!< \brief Panel select. */
#define NPL_USRLED      0x0004  /*!< \brief User LED. */
#define NPL_NPCS0       0x0008  /*!< \brief DataFlash CS. */

/*! \brief Version identifier register.
 */
#define NPL_VIDR        _SFR_MEM8(NPL_BASE + 0x1C)


extern IRQ_HANDLER sig_RSCTS;
extern IRQ_HANDLER sig_RSDSR;
extern IRQ_HANDLER sig_RSDCD;
extern IRQ_HANDLER sig_RSRI;
extern IRQ_HANDLER sig_RTCALARM;
extern IRQ_HANDLER sig_LANWAKEUP;
extern IRQ_HANDLER sig_FMBUSY;
extern IRQ_HANDLER sig_MMCREADY;
extern IRQ_HANDLER sig_RSINVAL;
extern IRQ_HANDLER sig_NRSINVAL;
extern IRQ_HANDLER sig_MMCD;
extern IRQ_HANDLER sig_NMMCD;

__BEGIN_DECLS
/* Function prototypes. */
extern int NplRegisterIrqHandler(IRQ_HANDLER * irq, void (*handler) (void *), void *arg);
extern int NplIrqEnable(IRQ_HANDLER * irq);
extern int NplIrqDisable(IRQ_HANDLER * irq);
__END_DECLS
/* */
/*@}*/
#endif
