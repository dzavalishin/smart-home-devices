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
 * \file arch/avr/dev/eeprom.c
 * \brief AVR On-Chip EEPROM support.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.2  2006/01/23 19:50:02  haraldkipp
 * Wrong parameters in ImageCraft call corrected.
 *
 * Revision 1.1  2006/01/23 17:27:08  haraldkipp
 * AVR specific routines for EEPROM access.
 *
 *
 * \endverbatim
 */

#if defined(__IMAGECRAFT__)
#include <eeprom.h>
#elif defined(__GNUC__)
#include <avr/eeprom.h>
#endif

#include <dev/nvmem.h>

#include <stdint.h>

/*!
 * \addtogroup xgArchAvrDevEeprom
 */
/*@{*/

/*!
 * \brief Load data from AVR EEPROM.
 *
 * \return Allways 0.
 */
int OnChipNvMemLoad(unsigned int addr, void *buff, size_t siz)
{
#if defined(__IMAGECRAFT__)
    EEPROMReadBytes((int)addr, buff, siz);
#elif defined(__GNUC__)
    eeprom_read_block (buff, (void *)addr, siz);
#endif
    return 0;
}

/*!
 * \brief Save data in AVR EEPROM.
 *
 * \return Allways 0.
 */
int OnChipNvMemSave(unsigned int addr, CONST void *buff, size_t len)
{
    uint8_t *cp;
    size_t i;

    for (cp = (uint8_t *) buff, i = 0; i < len; cp++, i++) {
#if defined(__IMAGECRAFT__)
        if (EEPROMread((int) (addr + i)) != *cp) {
            EEPROMwrite((int) (addr + i), *cp);
        }
#elif defined(__GNUC__)
        if (eeprom_read_byte((uint8_t *) (addr + i)) != *cp) {
            eeprom_write_byte((uint8_t *) (addr + i), *cp);
        }
#endif
    }
    return 0;
}

/*@}*/
