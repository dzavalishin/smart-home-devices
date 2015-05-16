#ifndef XSVFEXEC_HOST_H_
#define XSVFEXEC_HOST_H_

/*
 * Copyright (C) 2004-2007 by egnite Software GmbH
 * Copyright (C) 2008 by egnite GmbH
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
 * and Xilinx Application Note XAPP058.
 */

/*
 * $Log$
 * Revision 1.1  2008/10/20 13:10:05  haraldkipp
 * Checked in.
 *
 */

/*!
 * \file host.h
 * \brief Platform header file.
 */

/*!
 * \addtogroup xgXEDefs
 */
/*@{*/

#ifndef XSVFNAME
#if defined(ETHERNUT2)
/*! \brief The XSVF file is expected in the UROM file system. */
#define XSVFNAME    "UROM:enut202.xsvf"
#elif defined(ETHERNUT3)
#define XSVFNAME    "UROM:enut30e.xsvf"
#else
#define XSVFNAME    "cpld/enut202.xsvf"
#endif
#endif /* XSVFNAME */

/*@}*/

/*!
 * \addtogroup xgHost
 */
/*@{*/


/*! \brief Executor version string. */
#define XSVFEXEC_VERSION    "1.2.0"

/* Uncomment to enable debug output */
//#define XSVF_DEBUG


#ifdef ETHERNUT2

#include <sys/types.h>

/*! \brief Set TMS high on Ethernut 2. */
#define SET_TMS()   sbi(PORTF, 5)

/*! \brief Set TMS low on Ethernut 2. */
#define CLR_TMS()   cbi(PORTF, 5)

/*! \brief Set TDI high on Ethernut 2. */
#define SET_TDI()   sbi(PORTF, 6)

/*! \brief Set TDI low on Ethernut 2. */
#define CLR_TDI()   cbi(PORTF, 6)

/*! \brief Set TCK high on Ethernut 2. */
#define SET_TCK()   sbi(PORTF, 4)

/*! \brief Set TCK low on Ethernut 2. */
#define CLR_TCK()   cbi(PORTF, 4)

/*! \brief Get TDO status on Ethernut 2. */
#define GET_TDO()   bit_is_set(PINF, 7)

#elif defined(ETHERNUT3)

#include <sys/types.h>

/*! \brief Set TMS high on Ethernut 3. */
#define SET_TMS() { \
    outr(PIO_OER, _BV(16)); \
    outr(PIO_SODR, _BV(16)); \
    _NOP(); _NOP(); \
}

/*! \brief Set TMS low on Ethernut 3. */
#define CLR_TMS() { \
    outr(PIO_OER, _BV(16)); \
    outr(PIO_CODR, _BV(16)); \
    _NOP(); _NOP(); \
}

/*! \brief Set TDI high on Ethernut 3. */
#define SET_TDI() { \
    outr(PIO_SODR, _BV(23)); \
    _NOP(); _NOP(); \
}

/*! \brief Set TDI low on Ethernut 3. */
#define CLR_TDI() { \
    outr(PIO_CODR, _BV(23)); \
    _NOP(); _NOP(); \
}

/*! \brief Set TCK high on Ethernut 3. */
#define SET_TCK() { \
    outr(PIO_SODR, _BV(18)); \
    _NOP(); _NOP(); \
}

/*! \brief Set TCK low on Ethernut 3. */
#define CLR_TCK() { \
    outr(PIO_OER, _BV(16) | _BV(18) | _BV(23)); \
    outr(PIO_ODR, _BV(19)); \
    outr(PIO_CODR, _BV(18)); \
    _NOP(); _NOP(); \
}

/*! \brief Get TDO status on Ethernut 3. */
#define GET_TDO()   (inr(PIO_PDSR) & _BV(19))

#else

/* Generic routines, do nothing. */
#define SET_TMS()
#define CLR_TMS()
#define SET_TDI()
#define CLR_TDI()
#define SET_TCK()
#define CLR_TCK()

#define GET_TDO()   (0)

#endif

/*! \brief Set TMS high and toggle TCK. */
#define SET_TMS_TCK()   { SET_TMS(); CLR_TCK(); SET_TCK(); }

/*! \brief Set TMS low and toggle TCK. */
#define CLR_TMS_TCK()   { CLR_TMS(); CLR_TCK(); SET_TCK(); }

extern int XsvfInit(void);
extern void XsvfExit(int rc);

extern int XsvfGetError(void);
extern uint8_t XsvfGetCmd(void);
extern uint8_t XsvfGetState(uint8_t state0, uint8_t state1);
extern uint8_t XsvfGetByte(void);
extern short XsvfGetShort(void);
extern long XsvfGetLong(void);
extern int XsvfReadBitString(void *buf, int num);
extern int XsvfSkipComment(void);

extern void XsvfDelay(long usecs);

/*@}*/


#endif
