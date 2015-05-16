/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * \file dev/cy2239x.c
 * \brief Routines for the Cypress CY22393/4/5 clock chips.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/01/23 17:32:11  haraldkipp
 * Avoid hang-ups caused by debug leftovers.
 *
 * Revision 1.2  2006/01/05 16:51:06  haraldkipp
 * Several new functions added to query and modify the clock settings.
 *
 * Revision 1.1  2005/10/24 10:21:57  haraldkipp
 * Initial check in.
 *
 *
 * \endverbatim
 */

#include <sys/event.h>
#include <dev/twif.h>
#include <dev/cy2239x.h>

/*!
 * \addtogroup xgCy2239x
 */
/*@{*/

/*!
 * \brief I2C address.
 */
#ifndef I2C_SLA_PLL
#define I2C_SLA_PLL     0x69
#endif

/*!
 * \brief Base frequency.
 *
 * Frequency of the attached crystal. On Ethernut 3 a 25 MHz crystal is used.
 */
#ifndef NUT_PLL_FREF
#define NUT_PLL_FREF   25000000UL
#endif

/*!
 * \brief Calculate PLL frequency.
 *
 * \parm reg Points to an array with the three PLL registers.
 *
 * \return Frequency in Hertz.
 */
static uint32_t PllFreq(uint8_t * reg)
{
    uint32_t p;
    uint32_t pt;
    uint32_t qt;

    /* The 11-bit P value is stored in two registers. */
    p = (uint32_t) (reg[2] & 0x03) << 8 | reg[1];
    /* Calculate Pt = (2 x (P + 3)) + PO. */
    pt = 2 * (p + 3) + ((reg[2] >> 2) & 1);
    /* Calculate Qt = Q + 2. */
    qt = reg[0] + 2;
    /* Calculate Fpll = Fref x (Pt / Qt) */
    return (((NUT_PLL_FREF * 10UL + 5UL) / qt) * pt) / 10UL;
}

/*!
 * \brief Get the PLL connected to the specified output.
 *
 * \param clk Specifies the output. 0 = ClkA, 1 = ClkB etc.
 *
 * \return 0 for the reference clock, 1 for PLL1, 2 for PLL2 or 3 for PLL3.
 *         In case of an error -1 is returned.
 *
 * The following code fragment stores the number of the PLL connected
 * to Clock Output C in the variable pll.
 * \code
 * int pll;
 * pll = Cy2239xGetPll(CY2239X_CLKC);
 * \endcode
 */
int Cy2239xGetPll(int clk)
{
    int rc = -1;
    uint8_t loc = 0x0E;
    uint8_t reg;

    /* ClkE is fixed to PLL1. */
    if (clk == CY2239X_CLKE) {
        rc = 1;
    } 
    /* Register 0x0E contains the PLL index for ClkA-ClkD. */
    else if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, &reg, 1, NUT_WAIT_INFINITE) == 1) {
        rc = (reg >> (2 * clk)) & 0x03;
    }
    return rc;
}

/*!
 * \brief Connect a specified PLL to a specified output.
 *
 * \note Use with great care. This function may destroy your hardware.
 *
 * \param clk Specifies the output. 0 = ClkA, 1 = ClkB etc.
 * \param pll 1 - 3 for PLL1 - PLL3 resp. or 0 to select the reference 
 *            clock.
 *
 * \return 0 on success, -1 otherwise.
 *
 * The following code can be used to connect PLL3 to Clock Output B.
 * \code
 * if (Cy2239xSetPll(CY2239X_CLKB, CY2239X_PLL3)) {
 *     printf("Failed to select PLL3 for Clock B\n");
 * }
 * \endcode
 */
int Cy2239xSetPll(int clk, int pll)
{
    uint8_t reg[2];
    uint8_t msk = 0x03;

    /* ClkE is fixed to PLL1. */
    if (clk >= CY2239X_CLKE) {
        if (pll != CY2239X_PLL1) {
            return -1;
        }
        return 0;
    }

    if ((pll | msk) == msk) {

        /* Register 0x0E contains the PLL index for ClkA-ClkD. */
        reg[0] = 0x0E;
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, NUT_WAIT_INFINITE) == 1) {
            clk <<= 1;
            reg[1] &= ~(msk << clk);
            reg[1] |= pll << clk;
            TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, NUT_WAIT_INFINITE);
            return 0;
        }
    }
    return -1;
}

/*!
 * \brief Get the current divider value of the specified output.
 *
 * \param clk   Specifies the output. 0 = ClkA, 1 = ClkB etc.
 * \param fctrl Frequency control input status, only valid for clock A 
 *              and B. Otherwise this parameter is ignored. For 
 *              Ethernut 3 the inputs S0 and S1 are high and S2 is 
 *              connected to the CPLD. Thus, this value is either 3 if 
 *              control input S2 is low or 7 if S2 is set high (default).
 *
 * \return Clock's divider value. 0 means, that the output is switched off.
 */
int Cy2239xGetDivider(int clk, int fctrl)
{
    int rc = -1;
    int idx;
    uint8_t loc;
    uint8_t reg;

    /*
     * Clock E has a simple divider only.
     */
    if (clk == CY2239X_CLKE) {
        /* The two lower bits of register 0x0F define the Clock E divider. */
        loc = 0x0F;
        if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, &reg, 1, NUT_WAIT_INFINITE) == 1) {
            rc = reg & 3;
            if (rc == 1) {
                rc = 4;
            }
        }
    }
    else {
        /*
         * Clock A and B have two dividers, which are indirectly selected 
         * by the frequency control inputs.
         */
        if (clk <= CY2239X_CLKB) {
            /* Read the DivSel bit. */
            loc = 0x42 + fctrl * 3;
            if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, &reg, 1, NUT_WAIT_INFINITE) != 1) {
                return -1;
            }
            idx = clk * 2 + (reg >> 7);
        }
        else {
            idx = clk + 2;
        }
        loc = 0x08 + idx;
        if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, &reg, 1, NUT_WAIT_INFINITE) == 1) {
            rc = reg & 0x7F;
        }
    }
    return rc;
}

/*!
 * \brief Set the divider value of the specified output.
 *
 * Clock A through D provide a 7-bit output divider, while the Clock E
 * divider is fixed to 0 (off), 2, 3 or 4.
 *
 * Changing the divider value of an active output may cause a glitch on 
 * the output.
 *
 * \note Use with great care. This function may destroy your hardware.
 *
 * \param clk  Specifies the output. 0 = ClkA, 1 = ClkB etc.
 * \param sel  Divider select for Clock A and B, either 0 or 1. For other 
 *             outputs this parameter is ignored.
 * \param val  New divider value to set. A value of zero powers down the 
 *             divider and forces the output to three-state.
 *
 * \return 0 on success, -1 otherwise.
 */
int Cy2239xSetDivider(int clk, int sel, int val)
{
    uint8_t reg[2];

    /* Clock E has a simple divider only. */
    if (clk == CY2239X_CLKE) {
        if (val == 0 || (val >= 2 && val <= 4)) {
            if (val == 4) {
                val = 1;
            }
            /* The two lower bits of register 0x0F define the Clock E divider. */
            reg[0] = 0x0F;
            if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, NUT_WAIT_INFINITE) == 1) {
                reg[1] &= ~0x03;
                reg[1] |= (uint8_t) val;
                TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, NUT_WAIT_INFINITE);
                return 0;
            }
        }
        return -1;
    }

    if (val > 0 && val < 128) {
        /* Clock A and B have two selectable dividers. */
        if (clk <= CY2239X_CLKB) {
            reg[0] = 0x08 + clk * 2 + sel;
        }
        else {
            reg[0] = 0x08 + clk + 2;
        }
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, NUT_WAIT_INFINITE) == 1) {
            reg[1] &= ~0x7F;
            reg[1] |= (uint8_t) val;
            TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, NUT_WAIT_INFINITE);
            return 0;
        }
    }
    return -1;
}

/*!
 * \brief Enable or disable a specified PLL.
 *
 * \param pll   Set to 1 - 3 for PLL1 - PLL3 resp.
 * \param fctrl Frequency control input status, only used for PLL1. 
 *              Otherwise this parameter is ignored. For Ethernut 3 the 
 *              inputs S0 and S1 are high and S2 is connected to the 
 *              CPLD. Thus, this value is either 3 if control input S2 
 *              is low or 7 if S2 is set to high.
 * \param ena   0 will disable the PLL, 1 will enable it. Any other value
 *              may be used to query the current status.
 *
 * \return 0 if the PLL had been previously disabled, 1 if it had been
 *         enabled or -1 in case of an error.
 *
 * The following code shuts down PLL2 and Clock Output A. On Ethernut 3
 * this will remove the clock from the Ethernet Controller.
 * \code
 * if (Cy2239xSetDivider(CY2239X_CLKA, 1, 0)) {
 *     printf("Failed to disable ClkA Divider 1\n");
 * }
 * if (Cy2239xPllEnable(CY2239X_PLL2, 7, 0)) {
 *     printf("Failed to disable PLL2\n");
 * }
 * \endcode
 */
int Cy2239xPllEnable(int pll, int fctrl, int ena)
{
    int rc = -1;
    uint8_t reg[2];

    if (pll) {
        if (pll == CY2239X_PLL1) {
            /* PLL1 can be switched by the external control inputs. */
            reg[0] = 0x42 + fctrl * 3;
        }
        else if (pll == CY2239X_PLL2) {
            reg[0] = 0x13;
        }
        else if (pll == CY2239X_PLL3) {
            reg[0] = 0x16;
        }

        /* Set bit 6 of the third PLL register. */
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 1, NUT_WAIT_INFINITE) == 1) {
            rc = (reg[1] & 0x40) != 0;
            if (ena == 1) {
                reg[1] |= 0x40;
            }
            else if (ena == 0) {
                reg[1] &= ~0x40;
            }
            else {
                return rc;
            }
            TwMasterTransact(I2C_SLA_PLL, reg, 2, 0, 0, NUT_WAIT_INFINITE);
        }
    }
    return rc;
}

/*!
 * \brief Get the current frequency of a specified PLL.
 *
 * \param pll   1 - 3 for PLL1 - PLL3 resp. or 0 to query the reference 
 *              clock.
 * \param fctrl Frequency control input status, only used for PLL1. 
 *              Otherwise this parameter is ignored. For Ethernut 3 the 
 *              inputs S0 and S1 are high and S2 is connected to the 
 *              CPLD. Thus, this value is either 3 if control input S2 
 *              is low or 7 if S2 is set to high.
 *
 * \return Frequency in Hertz.
 *
 * Use the following code to retrieve the reference clock:
 * \code
 * uint32_t fref;
 *
 * fref = Cy2239xPllGetFreq(CY2239X_REF, 7);
 * \endcode
 */
uint32_t Cy2239xPllGetFreq(int pll, int fctrl)
{
    uint32_t rc = NUT_PLL_FREF;
    uint8_t loc;
    uint8_t reg[3];

    if (pll) {
        if (pll == CY2239X_PLL1) {
            /* PLL1 can be switched by the external control inputs. */
            loc = 0x40 + fctrl * 3;
        }
        else if (pll == CY2239X_PLL2) {
            loc = 0x11;
        }
        else if (pll == CY2239X_PLL3) {
            loc = 0x14;
        }
        /* Read the three PLL specific registers. */
        if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, reg, 3, NUT_WAIT_INFINITE) != 3) {
            rc = 0;
        }
        else {
            rc = PllFreq(reg);
        }
    }
    return rc;
}

/*!
 * \brief Set the frequency of a specified PLL.
 *
 * If the PLL is enabled, it will be temporarily disabled.
 *
 * The resulting frequency can calculated by
 * \code
 * F = Fref * (2 * (Pval + 3) + Poff) / (Qval + 2)
 * \endcode
 *
 * It is recommended to use Cypress' CyClocksRT utility for calculating 
 * the best parameter values.
 * 
 * \note Use with great care. This function may destroy your hardware.
 *
 * \param pll   1 - 3 for PLL1 - PLL3 resp.
 * \param fctrl Frequency control input status, only valid for clock A 
 *              and B. Otherwise this parameter is ignored. For 
 *              Ethernut 3 the inputs S0 and S1 are high and S2 is 
 *              connected to the CPLD. Thus, this value is either 3 if 
 *              control input S2 is low or 7 if S2 is set to high.
 * \param pval  10-bit multiplier.
 * \param poff  1-bit offset.
 * \param qval  8-bit divider.
 * \param fval  2-bit loop filter value. This value corresponds to the
 *              multiplier value and guarantess the PLL stability.
 *
 * \return 0 on success, -1 otherwise.
 */
int Cy2239xPllSetFreq(int pll, int fctrl, unsigned int pval, unsigned int poff, unsigned int qval, unsigned int fval)
{
    uint8_t reg[4];
    int ena;

    if (pll) {
        /* Determine the location of the PLL specific registers. */
        if (pll == CY2239X_PLL1) {
            reg[0] = 0x40 + fctrl * 3;
        }
        else if (pll == CY2239X_PLL2) {
            reg[0] = 0x11;
        }
        else if (pll == CY2239X_PLL3) {
            reg[0] = 0x14;
        }

        /* Read the three PLL specific registers. */
        if (TwMasterTransact(I2C_SLA_PLL, reg, 1, &reg[1], 3, NUT_WAIT_INFINITE) == 3) {
            /* The register is updated immediately. Disable the PLL to avoid
             * out of bounds settings. */
            if ((ena = Cy2239xPllEnable(pll, fctrl, 0)) >= 0) {
                reg[1] = (uint8_t) qval;
                reg[2] = (uint8_t) pval;
                reg[3] &= 0x80; /* Clear all except the divider select. */
                reg[3] |= (uint8_t)(pval >> 8) & 0x03;
                reg[3] |= (poff & 1) << 2;
                reg[3] |= (fval & 7) << 3;
                TwMasterTransact(I2C_SLA_PLL, reg, 4, 0, 0, NUT_WAIT_INFINITE);
                Cy2239xPllEnable(pll, fctrl, ena);
            }
            return 0;
        }
    }
    return -1;
}

/*!
 * \brief Get the frequency of a specified output.
 *
 * On Ethernut 3 Clock A provides the Ethernet Controller clock and 
 * Clock C is used as the CPU clock.
 *
 * PLL1 provides two configurations, selectable by the external input 
 * S2, which might be controlled by the Ethernut 3 CPLD. However, NPL 
 * version 2 doesn't allow to modify the S2 line, but holds it at a
 * fixed high level.
 *
 * \param clk   Specifies the output. 0 = ClkA, 1 = ClkB etc.
 * \param fctrl Frequency control input status, only valid for clock A 
 *              and B. Otherwise this parameter is ignored. For 
 *              Ethernut 3 the inputs S0 and S1 are high and S2 is 
 *              connected to the CPLD. Thus, this value is either 3 if 
 *              control input S2 is low or 7 if S2 is set to high.
 *
 * \return Frequency in Hertz. 0 is returned if the clock output is
 *         disabled or if an error occured.
 *
 * The following code can be used to query the main CPU clock on the
 * Ethernut 3 board.
 * \code
 * uint32_t fcpu;
 *
 * fcpu = Cy2239xGetFreq(CY2239X_CLKC, 7);
 * \endcode
 */
uint32_t Cy2239xGetFreq(int clk, int fctrl)
{
    uint32_t rc;
    uint32_t d;
    uint8_t loc;
    uint8_t reg;
    uint8_t clk_reg[8];
    uint8_t pll_reg[3];
    int pll;

    /* Read clock registers 0x08 - 0x0F. */
    loc = 0x08;
    if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, clk_reg, 8, NUT_WAIT_INFINITE) != 8) {
        return 0;
    }

    /* 
     * Get the PLL index for the specified output divider. Index 0 
     * specifies the reference clock, while 1 to 3 specify PLL1 to 
     * PLL3 resp.
     */
    if (clk == CY2239X_CLKE) {
        /* ClkE is fixed to PLL1. */
        pll = CY2239X_PLL1;
    } else {
        /* Register 0x0E contains the PLL index for ClkA-ClkD. */
        pll = (clk_reg[6] >> (2 * clk)) & 0x03;
    }

    /*
     * We got the PLL index. Now let's determine the PLL frequency.
     */
    if (pll == CY2239X_REF) {
        /* Index 0 means reference clock. */
        rc = NUT_PLL_FREF;
    } else {
        if (pll == CY2239X_PLL1) {
            /* PLL1 can be switched by the external control inputs. */
            loc = 0x40 + fctrl * 3;
        }
        else if (pll == CY2239X_PLL2) {
            loc = 0x11;
        }
        else if (pll == CY2239X_PLL3) {
            loc = 0x14;
        }
        /* Read the three PLL specific registers. */
        if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, pll_reg, 3, NUT_WAIT_INFINITE) != 3) {
            return 0;
        }
        rc = PllFreq(pll_reg);
    }

    /*
     * At this point we got the divider input frequency. Now we retrieve 
     * the divider value.
     */
    if (clk <= CY2239X_CLKB) {
        /* Clock A and B provide two dividers, selected by the frequency
         * control input. */
        if (pll == CY2239X_PLL1) {
            /* For PLL1 we already read the register. */
            reg = pll_reg[2];
        }
        else {
            /* For PLL2 and PLL3 we read the register now. */
            loc = 0x42 + fctrl * 3;
            if (TwMasterTransact(I2C_SLA_PLL, &loc, 1, &reg, 1, NUT_WAIT_INFINITE) != 1) {
                return 0;
            }
        }
        d = clk_reg[clk * 2 + (reg >> 7)] & 0x7F;
    }
    else if (clk == CY2239X_CLKE) {
        /* Get divider for Clock E. */
        d = clk_reg[7] & 3;
        if (d == 1) {
            d = 4;
        }
    }
    else {
        /* Get divider for Clock C and D. */
        d = clk_reg[clk + 2] & 0x7F;
    }

    /*
     * Finally divide the input frequency. A divider value of zero means
     * that the output is switched off.
     */
    if (d) {
        rc /= d;
    } else {
        rc = 0;
    }
    return rc;
}

/*@}*/
