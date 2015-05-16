/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.5  2005/02/02 19:59:12  haraldkipp
 * Typo corrected. Compiler failed, if ports were not configured. Due
 * to the completely broken port configuration this error wasn't
 * detected.
 *
 * Revision 1.4  2005/01/22 19:25:14  haraldkipp
 * Changed AVR port configuration names from PORTx to AVRPORTx.
 *
 * Revision 1.3  2004/09/22 08:14:48  haraldkipp
 * Made configurable
 *
 * Revision 1.2  2004/09/08 10:53:25  haraldkipp
 * os/timer.c
 *
 * Revision 1.1.1.1  2003/05/09 14:40:50  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.4  2003/02/04 17:50:54  harald
 * Version 3 released
 *
 * Revision 1.3  2003/01/14 13:34:53  harald
 * *** empty log message ***
 *
 * Revision 1.2  2002/08/11 12:25:38  harald
 * ICC mods
 *
 * Revision 1.1  2002/08/02 14:25:09  harald
 * First check in
 *
 */

/*
 * This header file specifies the hardware port bits. You
 * need to change or replace it, if your hardware differs.
 */
#include <cfg/arch/avr.h>

/*
 * The following header file contains the prototypes of
 * all global functions, which this module provides.
 */
#include <dev/spidigio.h>

/*
 * Determine ports, which had not been explicitly configured.
 */
#ifndef SPIDIGIO_SOUT_BIT
#define SPIDIGIO_SOUT_BIT 5
#define SPIDIGIO_SOUT_AVRPORT AVRPORTD
#define SPIDIGIO_SIN_BIT 6
#define SPIDIGIO_SIN_AVRPORT AVRPORTD
#define SPIDIGIO_SCLK_BIT 7
#define SPIDIGIO_SCLK_AVRPORT AVRPORTD
#define SPIDIGIO_LDI_BIT 7
#define SPIDIGIO_LDI_AVRPORT AVRPORTB
#define SPIDIGIO_LDO_BIT 5
#define SPIDIGIO_LDO_AVRPORT AVRPORTB
#endif

#if (SPIDIGIO_SOUT_AVRPORT == AVRPORTB)
#define SPIDIGIO_SOUT_PORT  PORTB
#define SPIDIGIO_SOUT_DDR   DDRB

#elif (SPIDIGIO_SOUT_AVRPORT == AVRPORTD)
#define SPIDIGIO_SOUT_PORT  PORTD
#define SPIDIGIO_SOUT_DDR   DDRD

#elif (SPIDIGIO_SOUT_AVRPORT == AVRPORTE)
#define SPIDIGIO_SOUT_PORT  PORTE
#define SPIDIGIO_SOUT_DDR   DDRE

#elif (SPIDIGIO_SOUT_AVRPORT == AVRPORTF)
#define SPIDIGIO_SOUT_PORT  PORTF
#define SPIDIGIO_SOUT_DDR   DDRF

#endif

#if (SPIDIGIO_SIN_AVRPORT == AVRPORTB)
#define SPIDIGIO_SIN_PORT   PORTB
#define SPIDIGIO_SIN_PIN    PINB
#define SPIDIGIO_SIN_DDR    DDRB

#elif (SPIDIGIO_SIN_AVRPORT == AVRPORTD)
#define SPIDIGIO_SIN_PORT   PORTD
#define SPIDIGIO_SIN_PIN    PIND
#define SPIDIGIO_SIN_DDR    DDRD

#elif (SPIDIGIO_SIN_AVRPORT == AVRPORTE)
#define SPIDIGIO_SIN_PORT   PORTE
#define SPIDIGIO_SIN_PIN    PINE
#define SPIDIGIO_SIN_DDR    DDRE

#elif (SPIDIGIO_SIN_AVRPORT == AVRPORTF)
#define SPIDIGIO_SIN_PORT   PORTF
#define SPIDIGIO_SIN_PIN    PINF
#define SPIDIGIO_SIN_DDR    DDRF
#endif

#if (SPIDIGIO_SCLK_AVRPORT == AVRPORTB)
#define SPIDIGIO_SCLK_PORT  PORTB
#define SPIDIGIO_SCLK_DDR   DDRB

#elif (SPIDIGIO_SCLK_AVRPORT == AVRPORTD)
#define SPIDIGIO_SCLK_PORT  PORTD
#define SPIDIGIO_SCLK_DDR   DDRD

#elif (SPIDIGIO_SCLK_AVRPORT == AVRPORTE)
#define SPIDIGIO_SCLK_PORT  PORTE
#define SPIDIGIO_SCLK_DDR   DDRE

#elif (SPIDIGIO_SCLK_AVRPORT == AVRPORTF)
#define SPIDIGIO_SCLK_PORT  PORTF
#define SPIDIGIO_SCLK_DDR   DDRF

#endif /* SPIDIGIO_SCLK_AVRPORT */

#if (SPIDIGIO_LDO_AVRPORT == AVRPORTB)
#define SPIDIGIO_LDO_PORT   PORTB
#define SPIDIGIO_LDO_DDR    DDRB

#elif (SPIDIGIO_LDO_AVRPORT == AVRPORTD)
#define SPIDIGIO_LDO_PORT   PORTD
#define SPIDIGIO_LDO_DDR    DDRD

#elif (SPIDIGIO_LDO_AVRPORT == AVRPORTE)
#define SPIDIGIO_LDO_PORT   PORTE
#define SPIDIGIO_LDO_DDR    DDRE

#elif (SPIDIGIO_LDO_AVRPORT == AVRPORTF)
#define SPIDIGIO_LDO_PORT   PORTF
#define SPIDIGIO_LDO_DDR    DDRF

#endif /* SPIDIGIO_LDO_AVRPORT */

#if (SPIDIGIO_LDI_AVRPORT == AVRPORTB)
#define SPIDIGIO_LDI_PORT   PORTB
#define SPIDIGIO_LDI_DDR    DDRB

#elif (SPIDIGIO_LDI_AVRPORT == AVRPORTD)
#define SPIDIGIO_LDI_PORT   PORTD
#define SPIDIGIO_LDI_DDR    DDRD

#elif (SPIDIGIO_LDI_AVRPORT == AVRPORTE)
#define SPIDIGIO_LDI_PORT   PORTE
#define SPIDIGIO_LDI_DDR    DDRE

#elif (SPIDIGIO_LDI_AVRPORT == AVRPORTF)
#define SPIDIGIO_LDI_PORT   PORTF
#define SPIDIGIO_LDI_DDR    DDRF

#endif /* SPIDIGIO_LDI_AVRPORT */

/*!
 * \addtogroup xgSpiDigIo
 */
/*@{*/

static ureg_t us_loops = 1;

/*!
 * \brief Loop for a small number of microseconds.
 *
 * This call will not release the CPU and will not switch to another 
 * thread. It simply executes a number of NOP (no operation) assembly 
 * statements.
 *
 * The routine is quite limited. Depending on the value of the
 * global variable us_loop, which is calculated in SpiDigitalInit(),
 * and based on the CPU clock. On a 14 MHz standard Ethernut its
 * value is 3, allowing a maximum delay of 85 microseconds.
 *
 * \param us Approximate delay in microseconds.
 *
 */
static INLINE void delay_us(ureg_t us)
{
    ureg_t _cnt = us * us_loops;

    while (_cnt--) {
        /*
         * A no-operation assembly statement is used here.
         * Without this statement, the compiler may completely
         * wipe out the loop during optimization.
         */
        _NOP();
    }
}

/*!
 * \brief Toggle shift register clock to perform a shift.
 *
 * Shifting is done on the falling edge of the clock line.
 *
 * The required puls width is 150 ns for the UCN5841 output and only 
 * 25 ns for the SN74HC165 input shift register hardware. However, 
 * longer cables and additional noise filters may increase the required 
 * minimum pulse length. On the reference hardware the rising edge of 
 * the RC filters used is about 3 microseconds.
 */
static INLINE void ShiftDigital(void)
{
    /* Switch clock line low. */
    cbi(SPIDIGIO_SCLK_PORT, SPIDIGIO_SCLK_BIT);
    /* Four microseconds delay. */
    delay_us(4);

    /* Switch clock line back high. */
    sbi(SPIDIGIO_SCLK_PORT, SPIDIGIO_SCLK_BIT);
    /* Four microseconds delay. */
    delay_us(4);
}

/*!
 * \brief Query digital inputs.
 *
 * SpiDigitalInit() must have been called by the application before 
 * calling this function.
 *
 * This routine does not check the validity of the parameter.
 *
 * \param num Number of bits to query, typically 8, 16, 24 or the maximum 
 *            value of 32. This number should exactly match the number of
 *            input pins. If it is lower, only the most significant bits
 *            are returned. However, this may be used by an application
 *            to scan these bits more often with reduced overhead.
 *
 * \return Binary value of the requested inputs. Only the specified number 
 *         of bits are used. Bit 0 is the one, which has been shifted out
 *         last.
 */
uint32_t SpiDigitalGet(ureg_t num)
{
    uint32_t bits = 0;

    cbi(SPIDIGIO_SOUT_PORT, SPIDIGIO_SOUT_BIT);

    /*
     * Toggle the input strobe. The shift register will latch
     * the current value at the parallel inputs into the shift
     * register.
     */
    sbi(SPIDIGIO_LDI_PORT, SPIDIGIO_LDI_BIT);
    delay_us(4);
    cbi(SPIDIGIO_LDI_PORT, SPIDIGIO_LDI_BIT);
    delay_us(4);

    /* Loop for the specified number of bits. */
    while (num--) {
        /* Shift the resulting value first. */
        bits <<= 1;

        /*
         * The shift register's serial output pin is connected to 
         * the port input pin. If this input is high, set the least 
         * significant bit of the resulting value. Otherwise leave 
         * it zero.
         */
        if (bit_is_set(SPIDIGIO_SIN_PIN, SPIDIGIO_SIN_BIT)) {
            bits |= 1;
        }

        /* 
         * This will toggle the clock line, presenting the next bit
         * at the shift register's serial output pin.
         */
        ShiftDigital();
    }
    return bits;
}

/*!
 * \brief Set digital outputs.
 *
 * SpiDigitalInit() must have been called by the application before 
 * calling this function.
 *
 * This routine does not check the validity of any parameter.
 *
 * \param num  Number of bits to set, typically 8, 16, 24 or 32, which is
 *             the maximum. The number must not be lower than the number 
 *             of shift register output bits.
 * \param bits The bit value to set. Only the number of bits specified are
 *             used, of which the most significant bit is shifted in first.
 */
void SpiDigitalSet(ureg_t num, uint32_t bits)
{
    uint32_t mask;

    /* Nothing to do, if the number of bits is zero. */
    if (num) {
        /*
         * Create the bit mask of the most significant bit. Note the UL 
         * specifier. Most compilers will use integers by default, when 
         * calculating of the right side. They do not consider the left
         * side. In our case this would create unexpected results, if 
         * integers are 16 bit only.
         */
        mask = 1UL << (num - 1);

        /* Loop for the specified number of bits. */
        while (num--) {

            /*
             * The shift register input is connected to the CPU output.
             * If the currently masked bit is set, then set the CPU
             * output pin to high level. Otherwise set the output
             * pin to low.
             */
            if (bits & mask) {
                /* Set bit instruction. */
                sbi(SPIDIGIO_SOUT_PORT, SPIDIGIO_SOUT_BIT);
            } 
            else {
                /* Clear bit instruction. */
                cbi(SPIDIGIO_SOUT_PORT, SPIDIGIO_SOUT_BIT);
            }

            /* Let the value get settled. */
            delay_us(4);

            /* Toggle the shift register clock line. */
            ShiftDigital();

            /* Left shift the mask by one. */
            mask >>= 1;
        }

        /*
         * Toggle the output strobe line. The shift register will latch
         * the shifted value and present it at its parallel output pins.
         */
        cbi(SPIDIGIO_LDO_PORT, SPIDIGIO_LDO_BIT);
        delay_us(4);
        sbi(SPIDIGIO_LDO_PORT, SPIDIGIO_LDO_BIT);
    }
}

/*!
 * \brief Return the number of shifts required to get an expected bit 
 * value at the input.
 *
 * When calling this function, it is assumed, that the shift register is 
 * already filled with the complement of the input bit.
 *
 * If the search mode is zero, then the function will return the number 
 * of shifts until the bit value at the input appears at the output. 
 * This can be used to detect the total size of the shift register.
 *
 * If the search mode is set to one, then the input strobe will be 
 * toggled before shifting and the function returns the number of shifts 
 * until the first unmodified bit appears.  This can be used to detect 
 * the number of inputs. This method requires, that the parallel shift 
 * register inputs do not change during shifting. If they do change, then
 * the resulting number will be lower than expected. The routine may be 
 * called several times to compensate this problem.
 *
 * \param num   Total number of shifts, should be 8, 16, 24 or 32.
 * \param bit   Input bit, either 0 or 1.
 * \param smode Search mode.
 *
 * \return The number of shifts.
 */
static ureg_t CountDigitalShifts(ureg_t num, ureg_t bit, ureg_t smode)
{
    ureg_t i;
    ureg_t rc = 0;

    /*
     * Toggle input strobe if we are searching the last modified bit.
     * Input lines are latched on the falling edge.
     */
    if (smode) {
        sbi(SPIDIGIO_LDI_PORT, SPIDIGIO_LDI_BIT);
        delay_us(4);
        cbi(SPIDIGIO_LDI_PORT, SPIDIGIO_LDI_BIT);
    }

    /*
     * Set the shift register input.
     */
    if (bit) {
        sbi(SPIDIGIO_SOUT_PORT, SPIDIGIO_SOUT_BIT);
    } else {
        cbi(SPIDIGIO_SOUT_PORT, SPIDIGIO_SOUT_BIT);
    }
    delay_us(4);

    /*
     * Do the requested number of shifts and watch the requested bit 
     * position.
     */
    for (i = 0; i < num; i++) {
        if (bit_is_set(SPIDIGIO_SIN_PIN, SPIDIGIO_SIN_BIT)) {
            if (bit) {
                if (smode) {
                    rc = i + 1;
                } else if (rc == 0) {
                    rc = i + 1;
                }
            }
        } else {
            if (bit == 0) {
                if (smode) {
                    rc = i + 1;
                } else if (rc == 0) {
                    rc = i + 1;
                }
            }
        }
        ShiftDigital();
    }
    return rc;
}

/*!
 * \brief Initialize the digital I/O shift register interface.
 *
 * This routine must be called before using the interface.
 *
 * Automatically detects the number of digital inputs and outputs. However,
 * the method used is limited and may fail on fast changing inputs. The
 * application should check the result for plausibility. It is save to
 * call the routine more than once.
 *
 * \param inputs  Pointer to an 8-bit value, where the number of 
 *                detected inputs will be stored.
 * \param outputs Pointer to an 8-bit value, where the number of 
 *                detected outputs will be stored.
 */
void SpiDigitalInit(ureg_t * inputs, ureg_t * outputs)
{
    ureg_t total = 0;
    ureg_t i;
    ureg_t cnt;

    /*
     * Determine the delay loop count based on the CPU clock.
     */
    if ((us_loops = (NutGetCpuClock() + 500000UL) / 4000000UL) < 1) {
        us_loops = 1;
    }

    /*
     * Set serial data output line direction.
     */
    sbi(SPIDIGIO_SOUT_DDR, SPIDIGIO_SOUT_BIT);

    /*
     * Set serial data input line direction and enable pullup.
     */
    sbi(SPIDIGIO_SIN_PORT, SPIDIGIO_SIN_BIT);
    cbi(SPIDIGIO_SIN_DDR, SPIDIGIO_SIN_BIT);

    /*
     * Clock. Input data is shifted on the falling, output data on the 
     * rising edge.
     */
    sbi(SPIDIGIO_SCLK_PORT, SPIDIGIO_SCLK_BIT);
    sbi(SPIDIGIO_SCLK_DDR, SPIDIGIO_SCLK_BIT);

    /*
     * UCN5841 output strobe. Shift register data appears on the output 
     * pins as long as this line is held high.
     */
    sbi(SPIDIGIO_LDO_PORT, SPIDIGIO_LDO_BIT);
    sbi(SPIDIGIO_LDO_DDR, SPIDIGIO_LDO_BIT);

    /*
     * SN74HC165 input strobe. Data at input pins is latched in the shift 
     * register on the falling edge.
     */
    cbi(SPIDIGIO_LDI_PORT, SPIDIGIO_LDI_BIT);
    sbi(SPIDIGIO_LDI_DDR, SPIDIGIO_LDI_BIT);

    /*
     * Fill the shift register with zeros. Then shift in ones until the 
     * first appears on the output. This gives us the total size plus one 
     * of the shift register.
     */
    CountDigitalShifts(32, 0, 0);
    total = CountDigitalShifts(32, 1, 0) - 1;

    /*
     * Determine the number of inputs. We do this five times for zeros 
     * and ones and take the maximum count. This way we compensate 
     * changing inputs while counting.
     */
    *inputs = 0;
    for (i = 0; i < 5; i++) {
        if ((cnt = CountDigitalShifts(total, 0, 1)) > *inputs) {
            *inputs = cnt;
        }
        if ((cnt = CountDigitalShifts(total, 1, 1)) > *inputs) {
            *inputs = cnt;
        }
    }
    *outputs = total - *inputs;
}


/*@}*/
