#ifndef _CFG_CLOCK_H_
#define _CFG_CLOCK_H_

/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * \file cfg/clock.h
 * \brief Nut/OS Clock Configuration.
 *
 * This file is reserved for kernel related configurations when building 
 * the system in the source tree without using the Configurator.
 *
 * When creating a new build tree with the Configurator, then it will
 * be replaced by a file with the same name, which is located in the
 * build tree.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.1  2006/01/05 16:32:00  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

/* Default configuration for Ethernut 3. */
#if 0

/*! \brief PLL Clock Output that is used for the CPU Clock. */
#ifndef NUT_PLL_CPUCLK
#define NUT_PLL_CPUCLK 3
#endif

/*! \brief PLL Clock Output that is used for the Ethernet Controller Clock. */
#ifndef NUT_PLL_ETHCLK
#define NUT_PLL_ETHCLK 1
#endif

/*! \brief PLL Clock Output that is fed to the GCK1 Pin of the CPLD. */
#ifndef NUT_PLL_NPLCLK1
#define NUT_PLL_NPLCLK1 2
#endif

/*! \brief PLL Clock Output that is fed to the GCK3 Pin of the CPLD. */
#ifndef NUT_PLL_NPLCLK3
#define NUT_PLL_NPLCLK3 4
#endif

#endif /* Ethernut 3 */

#endif
