/*
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.10  2009/01/16 17:03:02  haraldkipp
 * Configurable host name length. The *nix conditional is
 * no longer required as this will be handled in the nvmem
 * routines. NutLoadConfig will now set the virgin host
 * name, if no valid configuration is available. Cookie
 * and virgin host name are configurable too, but disabled
 * in the Configurator until we fixed the string value
 * problem. You may use UserConf.mk instead.
 *
 * Revision 1.9  2006/05/25 09:18:28  haraldkipp
 * API documentation updated and corrected.
 *
 * Revision 1.8  2006/01/23 19:50:48  haraldkipp
 * Dummy NVMEM configuration for GBA added.
 *
 * Revision 1.7  2006/01/23 17:26:17  haraldkipp
 * Platform independant routines added, which provide generic access to
 * non-volatile memory.
 *
 * Revision 1.6  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.5  2004/09/08 10:24:34  haraldkipp
 * No EEPROM support for AT91
 *
 * Revision 1.4  2004/04/07 12:13:58  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.3  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2004/03/03 17:42:19  drsung
 * Bugfix in NutSaveConfig. Write only to eeprom if actual byte in
 * eeprom differs from byte to write.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:46  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 18:17:07  harald
 * Version 3 released
 *
 */

#include <cfg/arch.h>
#include <sys/nutdebug.h>
#include <sys/confos.h>
#include <dev/nvmem.h>

#include <string.h>

/*!
 * \addtogroup xgConfOs
 */
/*@{*/

/*!
 * \brief Global system configuration structure.
 *
 * Contains the current system configuration. Nut/OS will load
 * this structure from non-volatile memory during initialization.
 */
CONFOS confos;

/*!
 * \brief Load Nut/OS configuration from non-volatile memory.
 *
 * This routine is automatically called during system initialization.
 * It tries to read the OS configuration structure \ref CONFOS from
 * non-volatile memory, starting at /ref CONFOS_EE_OFFSET.
 *
 * The routine may fail, if 
 * - non-volatile memory is not supported,
 * - non-volatile memory contains invalid data,
 * - the configuration structure has changed.
 *
 * \return 0 if OK, -1 on failures, in which case the hostname will
 *         be set to \ref CONFOS_VIRGIN_HOSTNAME.
 */
int NutLoadConfig(void)
{
    /* Sanity check. */
    NUTASSERT(sizeof(CONFOS) <= 255);

    if (NutNvMemLoad(CONFOS_EE_OFFSET, &confos, sizeof(CONFOS)) == 0 /* If loaded... */
        && confos.size == sizeof(CONFOS) /* ...check magic size and cookie. */
        && memcmp(confos.magic, CONFOS_EE_MAGIC, sizeof(CONFOS_EE_MAGIC) - 1) == 0) {
            return 0;
    }

    /* No valid configuration, set virgin hostname. */
    NUTASSERT(sizeof(confos.hostname) > strlen(CONFOS_VIRGIN_HOSTNAME));
    strcpy(confos.hostname, CONFOS_VIRGIN_HOSTNAME);

    return -1;
}

/*!
 * \brief Save Nut/OS configuration in non-volatile memory.
 *
 * Since Nut/OS version 4.7.5, this routine is no longer called automatically 
 * during system initialization. Thus, if NutLoadConfig() fails, then the 
 * non-volatile memory remains invalid and NutLoadConfig() will again fail 
 * during the next system start.
 *
 * \return 0 if OK, -1 on failures.
 */
int NutSaveConfig(void)
{
    /* Sanity checks. */
    NUTASSERT(sizeof(CONFOS) <= 255);
    NUTASSERT(strlen(confos.hostname) <= MAX_HOSTNAME_LEN);

    /* Update the magic part. */
    confos.size = sizeof(CONFOS);
    memcpy(confos.magic, CONFOS_EE_MAGIC, sizeof(CONFOS_EE_MAGIC) - 1);

    return NutNvMemSave(CONFOS_EE_OFFSET, &confos, sizeof(CONFOS));
}

/*@}*/
