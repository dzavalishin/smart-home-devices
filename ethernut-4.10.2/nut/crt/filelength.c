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
 * Revision 1.8  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.7  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.6  2004/08/05 06:49:44  drsung
 * Now the correct value for VIRTUALDEVICEs is returned.
 *
 * Revision 1.5  2004/07/30 19:14:09  drsung
 * filelength implemented for VIRTUALDEVICEs using the new
 * global ioctl command IOCTL_GETFILESIZE.
 *
 * Revision 1.4  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.3  2003/07/20 18:24:55  haraldkipp
 * errno support added.
 *
 * Revision 1.2  2003/07/20 17:19:00  haraldkipp
 * Description and result corrected.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:25  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:04  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <errno.h>
#include <sys/device.h>
#include <sys/nutdebug.h>
#include <io.h>

/*!
 * \addtogroup xgCrtLowio
 */
/*@{*/

/*!
 * \brief Return the length of a file.
 * 
 * \param fd   Descriptor of a previously opened file, device or
 *             connected socket.
 *
 * \return Filelength in bytes or -1 in case of an error.
 */
long _filelength(int fd)
{
    NUTFILE *fp = (NUTFILE *) ((uintptr_t) fd);
    NUTDEVICE *dev;
    long l;

    NUTASSERT(fp != NULL);
    dev = fp->nf_dev;
    if (dev == 0) {
        NUTVIRTUALDEVICE *vdv = (NUTVIRTUALDEVICE *) fp;
        if (vdv->vdv_ioctl && vdv->vdv_ioctl(vdv, IOCTL_GETFILESIZE, &l) == 0)
            return l;
        else {
            errno = EBADF;
            return -1;
        }
    }

    if (dev->dev_size == 0) {
        errno = EBADF;
        return -1;
    }

    return (*dev->dev_size) (fp);
}

/*@}*/
