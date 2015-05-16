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
 * Revision 1.5  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.4  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/12/15 19:23:09  haraldkipp
 * We need more dox on this
 *
 * Revision 1.1.1.1  2003/05/09 14:40:30  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:07  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <sys/nutdebug.h>
#include <sys/device.h>

/*!
 * \addtogroup xgCrtLowio
 */
/*@{*/

/*!
 * \brief Perform device specific control functions.
 *
 * Check the specific device driver for a list of supported control 
 * functions.
 *
 * \param fd   Descriptor of a previously opened device or
 *             connected socket.
 * \param cmd  Requested control function.
 * \param data Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 */
int _ioctl(int fd, int cmd, void *data)
{
    NUTFILE *fp = (NUTFILE *) ((uintptr_t) fd);
    NUTDEVICE *dev;

    NUTASSERT(fp != NULL);
    dev = fp->nf_dev;
    if (dev == 0) {
        NUTVIRTUALDEVICE *vdv = (NUTVIRTUALDEVICE *) fp;
        return (*vdv->vdv_ioctl) (vdv, cmd, data);
    }
    return (*dev->dev_ioctl) (dev, cmd, data);
}

/*@}*/
