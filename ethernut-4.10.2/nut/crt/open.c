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
 * Revision 1.7  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.6  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/03/16 15:25:20  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/08/05 20:01:55  haraldkipp
 * Typing errors corrected
 *
 * Revision 1.1.1.1  2003/05/09 14:40:30  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:08  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <errno.h>
#include <sys/device.h>
#include <sys/nutdebug.h>

/*!
 * \addtogroup xgCrtLowio
 */
/*@{*/


/*!
 * \brief Open a file.
 *
 * \param name The name of a registered device, optionally followed by a
 *             colon and a filename.
 * \param mode Operation mode. May be any of the following:
 * - _O_APPEND Always write at the end. 
 * - _O_BINARY Raw mode.
 * - _O_CREAT Create file if it does not exist. 
 * - _O_EXCL Open only if it does not exist. 
 * - _O_RDONLY Read only. 
 * - _O_RDWR Read and write. 
 * - _O_TEXT End of line translation. 
 * - _O_TRUNC Truncate file if it exists. 
 * - _O_WRONLY Write only.
 *
 * \return File descriptor for the opened file or -1 to indicate an error.
 */
int _open(CONST char *name, int mode)
{
    NUTDEVICE *dev;
    char dev_name[9];
    uint_fast8_t nidx;
    CONST char *nptr = name;

    NUTASSERT(name != NULL);

    /*
     * Extract device name.
     */
    for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++) {
        dev_name[nidx] = *nptr++;
    }
    dev_name[nidx] = 0;

    /*
     * Get device structure of registered device. In later releases we
     * try to open a file on a root device.
     */
    if ((dev = NutDeviceLookup(dev_name)) == 0) {
        errno = ENOENT;
        return -1;
    }

    /*
     * We should check, if the mode flags are device conformant.
     */

    /*
     * If a device name was specified, open this device. Otherwise open
     * a file on the device.
     */
    if (*nptr++ != ':')
        nptr = 0;
    return (int) ((uintptr_t) ((*dev->dev_open) (dev, nptr, mode, 0)));
}

/*@}*/
