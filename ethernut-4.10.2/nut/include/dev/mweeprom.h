#ifndef _MWEEPROM_H_
#define _MWEEPROM_H_

/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.2  2004/03/19 07:46:35  jdubiec
 * Make this file independent on sys/types.h due to compilation problems.
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*
 * Microwire EEPROM routines compatible with avr-gcc API which Nut/OS uses.
 * Based on Yousuke FURUSAWA's code:
 * http://www.open.esys.tsukuba.ac.jp/~yousuke/software/h8/microwire/index.jp.html
 *
 * Jan Dubiec <jdx@slackware.pl>
 *
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern unsigned char eeprom_read_byte(const unsigned char *addr);
    extern unsigned short eeprom_read_word(const unsigned short *addr);
    extern void eeprom_read_block(void *buf, const void *addr, size_t n);
    extern void eeprom_write_byte(unsigned char *addr, unsigned char val);
    extern void eeprom_write_word(unsigned short *addr, unsigned short val);
    extern void eeprom_write_block(const void *buf, void *addr, size_t n);
    extern void eeprom_fill_all(const unsigned char c);
    extern void eeprom_erase(unsigned char *addr);
    extern void eeprom_erase_all(void);

#ifdef __cplusplus
}
#endif
#endif                          /* #ifndef _MWEEPROM_H_ */
