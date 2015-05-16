/*
 * Copyright (C) 2000-2004 by ETH Zurich
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
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * unix_eeprom.c - avr eeprom functions for unix emulation
 *
 * 2004.08.05 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */

/**
\brief Provides the avr eeprom api. It uses the file 'btnode_eerpom.bin' to store
 its content.
*/

/* avoid stdio nut wrapper */
#define NO_STDIO_NUT_WRAPPER

#include <stdio.h>
#include <sys/types.h>
// eeprom image
static FILE *eepromFile = 0;

// asserts eepromFile open and positions read/write pointer
void unix_eeprom_acces(const void *addr)
{
    if (eepromFile == 0) {
        // create file if it doenst exist
        eepromFile = fopen("eeprom.bin", "a");
        if (eepromFile)
            fclose(eepromFile);
        // open file for read & write
        eepromFile = fopen("eeprom.bin", "r+");
        if (eepromFile == 0) {
            printf("ERROR: Open 'eeprom.bin' failed\n\r");
        }
    }
    if (eepromFile) {
        fseek(eepromFile, (long) addr, SEEK_SET);
    }
}

 /** \ingroup unix_eeprom
    write a byte \c val to EEPROM address \c addr */
void eeprom_write_byte(unsigned char *addr, unsigned char val)
{
    unix_eeprom_acces(addr);
    fwrite(&val, 1, 1, eepromFile);
    fflush(eepromFile);
}

/** \ingroup unix_eeprom
    read a block of \c n bytes from EEPROM address \c addr to
    \c buf */
void eeprom_read_block(void *buf, const void *addr, size_t n)
{
    uint16_t count;
    unix_eeprom_acces(addr);
    count = fread(buf, 1, n, eepromFile);

    // fill missing values
    while (count < n) {
        ((uint8_t *) buf)[count++] = 0xff;
    }
}

/** \ingroup unix_eeprom
    read one byte from EEPROM address \c addr */
uint8_t eeprom_read_byte(const unsigned char *addr)
{
    uint8_t result;
    uint16_t count;
    unix_eeprom_acces(addr);
    count = fread(&result, 1, 1, eepromFile);
    if (count != 1)
        return -1;
    return result;
}

/** \ingroup unix_eeprom
    read one 16-bit word (little endian) from EEPROM address \c addr */
uint16_t eeprom_read_word(const unsigned short *addr)
{
    uint16_t result;
    uint16_t count;
    unix_eeprom_acces(addr);
    count = fread(&result, 1, 2, eepromFile);
    if (count != 2)
        return -1;
    return result;
}

/*!
 * \brief Load data from AVR EEPROM.
 *
 * \return Allways 0.
 */
int NutNvMemLoad(unsigned int addr, void *buff, size_t siz)
{
    eeprom_read_block (buff, (void *)addr, siz);
    return 0;
}

/*!
 * \brief Save data in AVR EEPROM.
 *
 * \return Allways 0.
 */
int NutNvMemSave(unsigned int addr, const void *buff, size_t len)
{
    uint8_t *cp;
    size_t i;

    for (cp = (uint8_t *) buff, i = 0; i < len; cp++, i++) {
        if (eeprom_read_byte((uint8_t *) (addr + i)) != *cp) {
            eeprom_write_byte((uint8_t *) (addr + i), *cp);
        }
    }
    return 0;
}
