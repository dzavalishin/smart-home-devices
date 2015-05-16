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
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.2  2004/03/19 07:46:23  jdubiec
 * Make this file independent on sys/types.h due to compilation problems.
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
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

#include <h83068f.h>
#include <dev/mweeprom.h>

#define EEPROM_SET()         PADDR = 0x0e
#define EEPROM_CS            PADR.BIT.B3
#define EEPROM_SK            PADR.BIT.B2
#define EEPROM_DI            PADR.BIT.B1
#define EEPROM_DO            PADR.BIT.B0

#define EEPROM_HIGH             1
#define EEPROM_LOW              0

#define EEPROM_MASK(x,y)        ((x & y) ? EEPROM_HIGH : EEPROM_LOW)

#define EEPROM_WORD             8       /* 93LC66A : 1 word = 8 bit */


static const unsigned short bitmask[16] = {
    0x0001,                     /* 0000 0000 0000 0001 */
    0x0002,                     /* 0000 0000 0000 0010 */
    0x0004,                     /* 0000 0000 0000 0100 */
    0x0008,                     /* 0000 0000 0000 1000 */
    0x0010,                     /* 0000 0000 0001 0000 */
    0x0020,                     /* 0000 0000 0010 0000 */
    0x0040,                     /* 0000 0000 0100 0000 */
    0x0080,                     /* 0000 0000 1000 0000 */
    0x0100,                     /* 0000 0001 0000 0000 */
    0x0200,                     /* 0000 0010 0000 0000 */
    0x0400,                     /* 0000 0100 0000 0000 */
    0x0800,                     /* 0000 1000 0000 0000 */
    0x1000,                     /* 0001 0000 0000 0000 */
    0x2000,                     /* 0010 0000 0000 0000 */
    0x4000,                     /* 0100 0000 0000 0000 */
    0x8000                      /* 1000 0000 0000 0000 */
};

/* Delay for about 450 ns
   This function depends on crystal frequency; for 22,1184MHz crystal "nop"
   execution time is 90,42 ns
*/
inline static void microdelay(void)
{
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
} static void eeprom_enable(void)
{
    EEPROM_CS = EEPROM_HIGH;
    microdelay();
}

static void eeprom_disable(void)
{
    EEPROM_CS = EEPROM_LOW;
    microdelay();
}

static unsigned char eeprom_pulse(unsigned char di)
{
    unsigned char buf;

    EEPROM_DI = di;
    EEPROM_SK = EEPROM_HIGH;
    microdelay();
    buf = EEPROM_DO;
    EEPROM_SK = EEPROM_LOW;
    EEPROM_DI = EEPROM_LOW;
    microdelay();

    return buf;
}

unsigned char eeprom_read_byte(const unsigned char *addr)
{
    signed char i;
    unsigned char c;
    unsigned short cmd;

    eeprom_enable();

    cmd = 0x0c00 | (size_t) addr;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    c = 0;
    for (i = 7; i >= 0; --i)
        c += ((unsigned char) eeprom_pulse(EEPROM_LOW) << i);

    eeprom_disable();

    return c;
}

void eeprom_read_block(void *buf, const void *addr, size_t n)
{
    unsigned char *p, *d;

    p = (unsigned char *) addr;
    d = (unsigned char *) buf;

    while (n-- > 0)
        *(d++) = eeprom_read_byte(p++);
}

unsigned short eeprom_read_word(const unsigned short *addr)
{
    unsigned short i;

    eeprom_read_block((void *) &i, (void *) addr, sizeof(unsigned short));

    return i;
}

static void eeprom_write_enable(void)
{
    signed char i;
    unsigned short cmd;

    eeprom_enable();

    cmd = 0x0980;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    eeprom_disable();
}

static void eeprom_write_disable(void)
{
    signed char i;
    unsigned short cmd;

    eeprom_enable();

    cmd = 0x0800;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    eeprom_disable();
}

static void eeprom_generic_write(unsigned char *addr, unsigned char val)
{
    signed char i;
    unsigned short cmd;

    eeprom_enable();

    cmd = 0x0a00 | (size_t) addr;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    for (i = 7; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(val, bitmask[i]));

    eeprom_disable();

    eeprom_enable();
    while (EEPROM_DO == 0);
    eeprom_disable();
}

void eeprom_write_byte(unsigned char *addr, unsigned char val)
{
    eeprom_write_enable();

    eeprom_generic_write(addr, val);

    eeprom_write_disable();
}

void eeprom_write_block(const void *buf, void *addr, size_t n)
{
    unsigned char *p, *s;

    p = (unsigned char *) addr;
    s = (unsigned char *) buf;

    eeprom_write_enable();

    while (n-- > 0)
        eeprom_generic_write(p++, *(s++));

    eeprom_write_disable();
}

void eeprom_write_word(unsigned short *addr, unsigned short val)
{
    eeprom_write_block((void *) addr, (void *) &val, sizeof(unsigned short));
}

void eeprom_fill_all(const unsigned char c)
{
    signed char i;
    unsigned short cmd;

    eeprom_write_enable();

    eeprom_enable();

    cmd = 0x0880;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    for (i = 7; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(c, bitmask[i]));

    eeprom_disable();

    eeprom_enable();
    while (EEPROM_DO == 0);
    eeprom_disable();

    eeprom_write_disable();
}

void eeprom_erase(unsigned char *addr)
{
    signed char i;
    unsigned short cmd;

    eeprom_write_enable();

    eeprom_enable();

    cmd = 0x0e00 | (size_t) addr;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    eeprom_disable();

    eeprom_enable();
    while (EEPROM_DO == 0);
    eeprom_disable();

    eeprom_write_disable();
}


void eeprom_erase_all(void)
{
    signed char i;
    unsigned short cmd;

    eeprom_write_enable();

    eeprom_enable();

    cmd = 0x0900;
    for (i = 11; i >= 0; --i)
        eeprom_pulse(EEPROM_MASK(cmd, bitmask[i]));

    eeprom_disable();

    eeprom_enable();
    while (EEPROM_DO == 0);
    eeprom_disable();

    eeprom_write_disable();
}
