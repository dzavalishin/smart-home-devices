/*
 * Copyright 2010-2011 by egnite GmbH
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
 */

/*
 * \file arch/arm/board/ethernut5.c
 * \brief Ethernut 5 board initialization.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <arch/arm.h>

#ifndef PMM_RST_BASE
/* Power management reset port. */
#define PMM_RST_BASE    PIOB_BASE
#endif

#ifndef PMM_RST_PIN
/* Power management reset pin. */
#define PMM_RST_PIN     8
#endif


/*! \name Power Management Registers */
/*@{*/
/*! \brief Version register. */
#define PWRMAN_REG_VERS         0       
/*! \brief Feature status register. */
#define PWRMAN_REG_STA          1       
/*! \brief Feature enable register. */
#define PWRMAN_REG_ENA          2       
/*! \brief Feature disable register. */
#define PWRMAN_REG_DIS          3       
/*! \brief Board temperature register. */
#define PWRMAN_REG_TEMP         4       
/*! \brief Auxiliary input voltage register. */
#define PWRMAN_REG_VAUX         6       
/*! \brief LED blinking timer register. */
#define PWRMAN_REG_LEDCTL       8       
/*@}*/

/* \name Feature flags */
/*@{*/
/*! \brief 1.8V and 3.3V supply. */
#define PWRMAN_BOARD    0x01
/*! \brief VBUS input at device connector. */
#define PWRMAN_VBIN     0x02    
/*! \brief VBUS output at host connector. */
#define PWRMAN_VBOUT    0x04    
/*! \brief Memory card supply. */
#define PWRMAN_MMC      0x08    
/*! \brief RS-232 driver shutdown. */
#define PWRMAN_RS232    0x10    
/*! \brief Ethernet clock enable. */
#define PWRMAN_ETHCLK   0x20    
/*! \brief Ethernet PHY reset. */
#define PWRMAN_ETHRST   0x40    
/*! \brief RTC wake-up. */
#define PWRMAN_WAKEUP   0x80    
/*@}*/

/*!
 * \brief Delay loop.
 *
 * Delay by executing a given number of loops, roughly 5ns each.
 *
 * \param n Number of microseconds.
 */
static void BootLoopDelay(int n)
{
    while (n--) {
        _NOP();
    }
}

/*!
 * \brief Microsecond delay.
 *
 * \param us Number of microseconds.
 */
static void BootMicroDelay(int us)
{
    while (us--) {
        BootLoopDelay(200);
    }
}

/*!
 * \brief Millisecond delay.
 *
 * We are running prior to Nut/OS timer initialization and cannot use 
 * NutSleep, not even NutDelay.
 *
 * \param ms Number of milliseconds.
 */
static void BootMilliDelay(int ms)
{
    while (ms--) {
        BootMicroDelay(1000);
    }
}

/*!
 * \brief Initialize the power management interface.
 */
static void PmmInit(void)
{
#if defined(PMM_RST_BASE) && defined(PMM_RST_PIN)
    /* Activate the power management reset pin. */
    outr(PMM_RST_BASE + PIO_SODR_OFF, _BV(PMM_RST_PIN));
    outr(PMM_RST_BASE + PIO_PER_OFF, _BV(PMM_RST_PIN));
    outr(PMM_RST_BASE + PIO_OER_OFF, _BV(PMM_RST_PIN));
    BootMilliDelay(1);
    /* Deactivate the reset. */
    outr(PMM_RST_BASE + PIO_CODR_OFF, _BV(PMM_RST_PIN));
    BootMilliDelay(100);
#endif
    /* Set peripheral lines for TWD and TWCK. */
    outr(PIOA_ASR, _BV(PA23_TWD_A) | _BV(PA24_TWCK_A));
    outr(PIOA_PDR, _BV(PA23_TWD_A) | _BV(PA24_TWCK_A));
    /* Switch TWI lines to open drain. */
    outr(PIOA_MDER, _BV(PA23_TWD_A) | _BV(PA24_TWCK_A));
    /* Enable TWI clock. */
    outr(PMC_PCER, _BV(TWI_ID));
    /* Disable interrupts and reset the interface. */
    outr(TWI_IDR, 0xFFFFFFFF);
    outr(TWI_CR, TWI_SWRST);
    /* Switch to master mode. */
    outr(TWI_CR, TWI_MSEN | TWI_SVDIS);
    /* Set transfer rate. */
    outr(TWI_CWGR, (7 << TWI_CKDIV_LSB) | (128 << TWI_CHDIV_LSB) | (128 << TWI_CLDIV_LSB));
}

/*!
 * \brief Write value to an 8-bit power management register.
 *
 * \param reg PMM register to write to.
 * \param val Value to write.
 *
 * \return 0 on success, -1 otherwise.
 */
static int PmmWriteReg(unsigned int reg, unsigned int val)
{
    volatile int tmo;

    outr(TWI_MMR, 0x22 << TWI_DADR_LSB);
    outr(TWI_CR, TWI_START);
    outr(TWI_THR, reg);
    for (tmo = 0; (inr(TWI_SR) & TWI_TXRDY) == 0; tmo++) {
        if (tmo > 100000) {
            return -1;
        }
    }
    outr(TWI_CR, TWI_STOP);
    outr(TWI_THR, val);
    for (tmo = 0; (inr(TWI_SR) & TWI_TXCOMP) == 0; tmo++) {
        if (tmo > 100000) {
            return -1;
        }
    }
    return 0;
}

#if 0
/* For an unknown reason the system hangs in NutTimer(!) processing
   when trying to read the version. Something is somewhere
   mysteriously broken. Stack? CPU initialization? Keep this code
   for reference. */
static int PmmReadReg(unsigned int reg, unsigned char *val)
{
    unsigned long sr;
    volatile unsigned int tmo;

    outr(TWI_IADRR, reg);
    outr(TWI_MMR, 0x22 << TWI_DADR_LSB | TWI_IADRSZ_1BYTE | TWI_MREAD);
    outr(TWI_CR, TWI_START | TWI_STOP);
    for (tmo = 0; ((sr = inr(TWI_SR)) & TWI_RXRDY) == 0; tmo++) {
        if (tmo > 100000) {
            return -1;
        }
    }
    if (sr & TWI_NACK) {
        return -1;
    }
    *val = inb(TWI_RHR);
    return 0;
}
#endif

/*!
 * \brief Ethernet PHY hardware reset.
 */
static void PmmPhyReset(void)
{
    /* Enable PIO pull-ups at PHY mode strap pins. */
    outr(PIOA_ODR, _BV(14) | _BV(15) | _BV(17));
    outr(PIOA_PUER, _BV(14) | _BV(15) | _BV(17));
    outr(PIOA_PER, _BV(14) | _BV(15) | _BV(17));

    /* Enable PIO at PHY address 0 strap pin. */
    outr(PIOA_ODR, _BV(18));
    outr(PIOA_PUDR, _BV(18));
    outr(PIOA_PER, _BV(18));

    BootMilliDelay(10);
    PmmWriteReg(PWRMAN_REG_ENA, PWRMAN_ETHRST | PWRMAN_ETHCLK);
    BootMilliDelay(1);
    PmmWriteReg(PWRMAN_REG_DIS, PWRMAN_ETHRST);
    BootMilliDelay(10);
}

/*!
 * \brief Early Ethernut 5 hardware initialization.
 */
void NutBoardInit(void)
{
    PmmInit();
    PmmPhyReset();
}
