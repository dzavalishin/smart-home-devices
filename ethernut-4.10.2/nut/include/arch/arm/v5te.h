#ifndef _ARCH_ARM_V5TE_H_
#define _ARCH_ARM_V5TE_H_
/*
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

/*!
 * \file arch/arm/v5te.h
 * \brief ARM9 specific features.
 *
 * \verbatim
 * $Id: v5te.h 2475 2009-02-17 09:35:05Z haraldkipp $
 * \endverbatim
 */

/*! \brief Enables MMU. */
#define CP15_CR1_M_BIT      0
/*! \brief Enables alignment fault. */
#define CP15_CR1_A_BIT      1
/*! \brief Enables data cache. */
#define CP15_CR1_C_BIT      2
/*! \brief Enables big endian operation. */
#define CP15_CR1_B_BIT      7
/*! \brief Enables system protection. */
#define CP15_CR1_S_BIT      8
/*! \brief Enables ROM protection. */
#define CP15_CR1_R_BIT      9
/*! \brief Enables instruction cache. */
#define CP15_CR1_I_BIT      12
/*! \brief Enables high exception vectors. */
#define CP15_CR1_V_BIT      13
/*! \brief Enables round robin replacement strategy. */
#define CP15_CR1_RR_BIT     14
/*! \brief Enables MMU. */
#define CP15_CR1_L4_BIT     15

#endif
