/*
 * Copyright 2009 by egnite GmbH
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
 * Initially the code had been taken from the GPL'ed JTAG-O-MAT project.
 * Its author explicitly confirmed to publish this under BSDL.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * \file dev/jtag_gpio0.c
 * \brief Primary JTAG cable driver.
 *
 * Uses GPIO to control the JTAG signals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/arch/gpio.h>
#include <cfg/progif.h>

#include <sys/timer.h>
#include <stdlib.h>

#include <dev/jtag_gpio.h>

#if defined(ETHERNUT3)
/*
 * We do not have any config settings right now. So I add my
 * Ethernut 3 settings here, which I used for testing the code.
 *
 * - P16 TMS output.
 * - P18 TCK output.
 * - P19 TDO input.
 * - P20 RST output.
 * - P23 TDI output.
 */
#define JTAG0_TDO_PIO_BIT   19
#define JTAG0_TDI_PIO_BIT   23
#define JTAG0_TMS_PIO_BIT   16
#define JTAG0_TCK_PIO_BIT   18
#define JTAG0_CLOCK_RATE    100000
#endif

#if !defined(JTAG0_TDO_PIO_ID) && defined(PIO_ID)
#define JTAG0_TDO_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_TDO_PIO_BIT) && defined(JTAG0_TDO_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_TDO_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE int JTAG0_TDO_IS_ON(void) { return GPIO_GET(JTAG0_TDO_PIO_BIT); }
static INLINE void JTAG0_TDO_SI(void) { GPIO_INPUT(JTAG0_TDO_PIO_BIT); GPIO_ENABLE(JTAG0_TDO_PIO_BIT); }
#else
#define JTAG0_TDO_IS_ON()   (1)
#define JTAG0_TDO_SI()
#endif

#if !defined(JTAG0_TDI_PIO_ID) && defined(PIO_ID)
#define JTAG0_TDI_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_TDI_PIO_BIT) && defined(JTAG0_TDI_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_TDI_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void JTAG0_TDI_LO(void) { GPIO_SET_LO(JTAG0_TDI_PIO_BIT); }
static INLINE void JTAG0_TDI_HI(void) { GPIO_SET_HI(JTAG0_TDI_PIO_BIT); }
static INLINE void JTAG0_TDI_SO(void) { GPIO_OUTPUT(JTAG0_TDI_PIO_BIT); GPIO_ENABLE(JTAG0_TDI_PIO_BIT); }
#else
#define JTAG0_TDI_LO()
#define JTAG0_TDI_HI()
#define JTAG0_TDI_SO()
#endif

#if !defined(JTAG0_TMS_PIO_ID) && defined(PIO_ID)
#define JTAG0_TMS_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_TMS_PIO_BIT) && defined(JTAG0_TMS_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_TMS_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void JTAG0_TMS_LO(void) { GPIO_SET_LO(JTAG0_TMS_PIO_BIT); }
static INLINE void JTAG0_TMS_HI(void) { GPIO_SET_HI(JTAG0_TMS_PIO_BIT); }
static INLINE void JTAG0_TMS_SO(void) { GPIO_OUTPUT(JTAG0_TMS_PIO_BIT); GPIO_ENABLE(JTAG0_TMS_PIO_BIT); }
#else
#define JTAG0_TMS_LO()
#define JTAG0_TMS_HI()
#define JTAG0_TMS_SO()
#endif

#if !defined(JTAG0_TCK_PIO_ID) && defined(PIO_ID)
#define JTAG0_TCK_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_TCK_PIO_BIT) && defined(JTAG0_TCK_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_TCK_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void JTAG0_TCK_LO(void) { GPIO_SET_LO(JTAG0_TCK_PIO_BIT); }
static INLINE void JTAG0_TCK_HI(void) { GPIO_SET_HI(JTAG0_TCK_PIO_BIT); }
static INLINE void JTAG0_TCK_SO(void) { GPIO_OUTPUT(JTAG0_TCK_PIO_BIT); GPIO_ENABLE(JTAG0_TCK_PIO_BIT); }
#else
#define JTAG0_TCK_LO()
#define JTAG0_TCK_HI()
#define JTAG0_TCK_SO()
#endif

#if !defined(JTAG0_NSRST_PIO_ID) && defined(PIO_ID)
#define JTAG0_NSRST_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_NSRST_PIO_BIT) && defined(JTAG0_NSRST_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_NSRST_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void JTAG0_NSRST_LO(void) { GPIO_SET_LO(JTAG0_NSRST_PIO_BIT); }
static INLINE void JTAG0_NSRST_HI(void) { GPIO_SET_HI(JTAG0_NSRST_PIO_BIT); }
static INLINE void JTAG0_NSRST_SO(void) { GPIO_OPENDRAIN(JTAG0_NSRST_PIO_BIT); GPIO_OUTPUT(JTAG0_NSRST_PIO_BIT); GPIO_ENABLE(JTAG0_NSRST_PIO_BIT); }
#else
#define JTAG0_NSRST_LO()
#define JTAG0_NSRST_HI()
#define JTAG0_NSRST_SO()
#endif

#if !defined(JTAG0_NTRST_PIO_ID) && defined(PIO_ID)
#define JTAG0_NTRST_PIO_ID    PIO_ID
#endif
#if defined(JTAG0_NTRST_PIO_BIT) && defined(JTAG0_NTRST_PIO_ID)
#undef GPIO_ID
#define GPIO_ID JTAG0_NTRST_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void JTAG0_NTRST_LO(void) { GPIO_SET_LO(JTAG0_NTRST_PIO_BIT); }
static INLINE void JTAG0_NTRST_HI(void) { GPIO_SET_HI(JTAG0_NTRST_PIO_BIT); }
static INLINE void JTAG0_NTRST_SO(void) { GPIO_OPENDRAIN(JTAG0_NTRST_PIO_BIT); GPIO_OUTPUT(JTAG0_NTRST_PIO_BIT); GPIO_ENABLE(JTAG0_NTRST_PIO_BIT); }
#else
#define JTAG0_NTRST_LO()
#define JTAG0_NTRST_HI()
#define JTAG0_NTRST_SO()
#endif

#if defined(JTAG0_CLOCK_RATE) && (JTAG0_CLOCK_RATE <= 500000)
#define JTAG0_CLOCK_DELAY()     NutMicroDelay(1000000 / JTAG0_CLOCK_RATE / 2)
#else
#define JTAG0_CLOCK_DELAY()
#endif

typedef struct {
    uint8_t cib_buf[256];
    int cib_bidx;
    int cib_bbit;
} CABLE_INFO;

/*!
 * \brief Initialize the cable driver.
 *
 * \return Pointer to a cable specific information structure, which must
 *         be passed to all other routines of this driver.
 */
static void *JtagCable0Open(void)
{
    CABLE_INFO *cib;

    cib = calloc(1, sizeof(CABLE_INFO));
    if (cib) {
#if defined(MCU_AT91) && defined(JTAG0_TDO_PIO_ID)
#if defined(PS_PCER)
        outr(PS_PCER, _BV(JTAG0_TDO_PIO_ID));
#elif defined(PMC_PCER)
        outr(PMC_PCER, _BV(JTAG0_TDO_PIO_ID));
#endif
#endif
        JTAG0_TDO_SI();
        JTAG0_TDI_SO();
        JTAG0_TMS_SO();
        JTAG0_TCK_SO();
        JTAG0_NSRST_SO();
        JTAG0_NTRST_SO();
    }
    return (void *)cib;
}

/*!
 * \brief Flush all buffered state changes.
 *
 * \param cbl Pointer to the cable information structure, obtained by 
 *            a previous call to JtagCable0Open().
 */
static void JtagCable0TmsFlush(void *cbl)
{
    CABLE_INFO *cib = (CABLE_INFO *)cbl;
    uint_fast8_t tms;
    int i;
    int j;
    int n;

    n = cib->cib_bidx + (cib->cib_bbit == 0);
    for (i = 0; i < n; i++) {
        tms = cib->cib_buf[i];
        for (j = 8; j--;) {
            JTAG0_TCK_LO();
            if (tms & 1) {
                JTAG0_TMS_HI();
            } else {
                JTAG0_TMS_LO();
            }
            JTAG0_CLOCK_DELAY();
            tms >>= 1;
            JTAG0_TCK_HI();
            JTAG0_CLOCK_DELAY();
        }
    }
    if (cib->cib_bbit) {
        tms = cib->cib_buf[cib->cib_bidx];
        for (j = cib->cib_bbit; j--;) {
            JTAG0_TCK_LO();
            if (tms & 1) {
                JTAG0_TMS_HI();
            } else {
                JTAG0_TMS_LO();
            }
            JTAG0_CLOCK_DELAY();
            tms >>= 1;
            JTAG0_TCK_HI();
            JTAG0_CLOCK_DELAY();
        }
        cib->cib_bbit = 0;
    }
    cib->cib_bidx = 0;
}

/*!
 * \brief Release the cable driver.
 *
 * Any pending state change sequence will be processed before 
 * releasing the cable.
 *
 * \param cbl Pointer to the cable information structure, obtained by 
 *            a previous call to JtagCable0Open().
 */
static void JtagCable0Close(void *cbl)
{
    JtagCable0TmsFlush(cbl);
    free(cbl);
}

/*!
 * \brief Activate or deactivate the target reset line.
 *
 * Any pending state change sequence will be processed before 
 * activating the reset line.
 *
 * \param cbl Pointer to the cable information structure, obtained by 
 *            a previous call to JtagCable0Open().
 * \param on  If not zero, the reset line will be activated (pulled
 *            low). Otherwise the target reset will be deactivated.
 */
static void JtagCable0TargetReset(void *cbl, uint_fast8_t on)
{
    if (on) {
        JtagCable0TmsFlush(cbl);
        JTAG0_NSRST_LO();
    } else {
        JTAG0_NSRST_HI();
    }
}

/*!
 * \brief Output a sequence of state changes.
 *
 * State changes may not be done immediately, but stored in a buffer.
 * Call JtagCable0TmsFlush() to execute the buffered state changes.
 *
 * \param cbl Pointer to the cable information structure, obtained by 
 *            a previous call to JtagCable0Open().
 * \param tms TMS sequence, bit 0 is transfered first. 
 * \param len Number of state changes. Maximum is 8.
 */
static void JtagCable0TmsPut(void *cbl, uint_fast8_t tms, uint_fast8_t len)
{
    CABLE_INFO *cib = (CABLE_INFO *)cbl;
    int bbit;
    uint_fast8_t byte;

    bbit = cib->cib_bbit;
    while (len) {
        if (bbit) {
            byte = cib->cib_buf[cib->cib_bidx];
        } else {
            byte = 0;
        }
        while (len && bbit < 8) {
            if (tms & 1) {
                byte |= 1 << bbit;
            }
            tms >>= 1;
            bbit++;
            len--;
        }
        cib->cib_buf[cib->cib_bidx] = (uint8_t)byte;
        if (bbit >= 8) {
            cib->cib_bidx++;
            if (cib->cib_bidx >= sizeof(cib->cib_buf)) {
                JtagCable0TmsFlush(cbl);
            }
            bbit = 0;
        }
    }
    cib->cib_bbit = bbit;
}

/*!
 * \brief Read input bitstream from TDO and write output bitstream to TDI.
 *
 * Before starting the transfer, any pending state change sequence will
 * be flushed.
 *
 * \param cbl Pointer to the cable information structure, obtained by 
 *            a previous call to JtagCable0Open().
 * \param rbuf Points to the buffer that receives the read data. May be 
 *             NULL if the caller is not interested.
 * \param wbuf Points to the buffer that contains the write data. If 
 *             NULL, all outgoing bits are zero.
 * \param len  Number of bits to read and/or write.
 * \param last Indicates the last data transfer. If set, then TMS
 *             will be set high on the last bit and the TAP controller 
 *             will leave the stable state.
 */
static void JtagCable0TransferData(void *cbl, uint8_t *rbuf, CONST uint8_t *wbuf, uint32_t len, uint_fast8_t last)
{
    JtagCable0TmsFlush(cbl);

    while (len) {
        uint8_t mask = 1;
        uint8_t val = 0;

        if (wbuf) {
            val = *wbuf++;
        }
    
        while (len && mask) {
            JTAG0_TCK_LO();
            if (val & mask) {
                JTAG0_TDI_HI();
            } else {
                JTAG0_TDI_LO();
            }
            if (--len || !last) {
                JTAG0_TMS_LO();
            } else {
                JTAG0_TMS_HI();
            }
            JTAG0_CLOCK_DELAY();
            JTAG0_TCK_HI();
            JTAG0_CLOCK_DELAY();
            if (JTAG0_TDO_IS_ON()) {
                val |= mask;
            } else {
                val &= ~mask;
            }
            mask <<= 1;
        }

        if (rbuf) {
            *rbuf++ = val;
        }
    }
}

/*!
 * \brief GPIO cable driver structure.
 *
 * Pass a pointer to this structure when calling TapOpen() to associate 
 * the tap controller with this cable.
 */
JTAG_CABLE jtag_gpio0 = {
    JtagCable0Open, /* cable_open */
    JtagCable0Close, /* cable_close */
    JtagCable0TargetReset, /* cable_reset_target */
    JtagCable0TmsPut, /* cable_tms_put */
    JtagCable0TmsFlush, /* cable_tms_flush */
    JtagCable0TransferData /* cable_transfer_data */
};
