/*
 * Copyright (C) 2008-2011 by egnite GmbH
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
 * \file dev/spi_at45d_info.c
 * \brief Atmel AT45D SPI Flash types.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <dev/at45d.h>

/*!
 * \addtogroup xgSpiInfoAt45d
 */
/*@{*/

/*! \brief Parameter table of known DataFlash types. */
AT45D_INFO at45d_info[] = {
    { 8,  512,  256, 0x0D}, /* AT45DB011B  128kB  256B/pg */
    { 9,  512,  264, 0x0C}, /* AT45DB011B  128kB  264B/pg */
    { 8, 1025,  256, 0x15}, /* AT45DB021B  256kB  256B/pg */
    { 9, 1025,  264, 0x14}, /* AT45DB021B  256kB  264B/pg */
    { 8, 2048,  256, 0x1D}, /* AT45DB041B  512kB  256B/pg */
    { 9, 2048,  264, 0x1C}, /* AT45DB041B  512kB  264B/pg */
    { 8, 4096,  256, 0x25}, /* AT45DB081B    1MB  256B/pg */
    { 9, 4096,  264, 0x24}, /* AT45DB081B    1MB  264B/pg */
    { 9, 4096,  512, 0x2D}, /* AT45DB0161B   2MB  512B/pg */
    {10, 4096,  528, 0x2C}, /* AT45DB0161B   2MB  528B/pg */
    { 9, 8192,  512, 0x35}, /* AT45DB0321B   4MB  512B/pg */
    {10, 8192,  528, 0x34}, /* AT45DB0321B   4MB  528B/pg */
    {10, 8192, 1024, 0x39}, /* AT45DB0642    8MB 1024B/pg */
    {11, 8192, 1056, 0x38}, /* AT45DB0642    8MB 1056B/pg */
    {10, 8192, 1024, 0x3D}, /* AT45DB0642D   8MB 1024B/pg */
    {11, 8192, 1056, 0x3C}  /* AT45DB0642D   8MB 1056B/pg */
};

/*! \brief Number of known Dataflash types. */
uint_fast8_t at45d_known_types = sizeof(at45d_info) / sizeof(AT45D_INFO);

/*@}*/
