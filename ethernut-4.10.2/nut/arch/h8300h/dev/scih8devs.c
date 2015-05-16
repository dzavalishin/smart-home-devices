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
 * Revision 1.2  2004/03/18 14:06:52  haraldkipp
 * Deprecated header file replaced
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <sys/file.h>
#include <sys/device.h>
#include <dev/scih8.h>

/*!
 * \addtogroup xgUartDev
 */
/*@{*/


static UARTDCB dcb_sci0;
static IFSTREAM ifs_sci0;

static UARTDCB dcb_sci1;
static IFSTREAM ifs_sci1;

static UARTDCB dcb_sci2;
static IFSTREAM ifs_sci2;

__BEGIN_DECLS
/* SCI functions prototypes which are defined in scih8.c */
extern int SciH8Init(NUTDEVICE * dev);
extern int SciH8IOCtl(NUTDEVICE * dev, int req, void *conf);
extern int SciH8Input(NUTDEVICE * dev);
extern int SciH8Output(NUTDEVICE * dev);
extern int SciH8Flush(NUTDEVICE * dev);

extern int SciH8GetRaw(u_char * cp);
extern int SciH8PutRaw(u_char ch);

extern int SciH8Read(NUTFILE * fp, void *buffer, int size);
extern int SciH8Write(NUTFILE * fp, CONST void *buffer, int len);
extern NUTFILE *SciH8Open(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern int SciH8Close(NUTFILE * fp);

__END_DECLS
/*!
 * \brief SCI0 Device information structure.
 */
    NUTDEVICE devSci0 = {
    0,                          /*!< Pointer to next device. */
    {
    's', 'c', 'i', '0', 0, 0, 0, 0, 0}, /*!< Unique device name. */
        IFTYP_STREAM,           /*!< Type of device. */
        0,                      /*!< Base address. */
        0,                      /*!< First interrupt number. */
        &ifs_sci0,              /*!< Interface control block. */
        &dcb_sci0,              /*!< Driver control block. */
        SciH8Init,              /*!< Driver initialization routine. */
        SciH8IOCtl,             /*!< Driver specific control function. */
        SciH8Read, SciH8Write,
        /* SciH8Write_P */
SciH8Open, SciH8Close, 0};

/*!
 * \brief SCI1 Device information structure.
 */
NUTDEVICE devSci1 = {
    0,                          /*!< Pointer to next device. */
    {'s', 'c', 'i', '1', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    1,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    &ifs_sci1,                  /*!< Interface control block. */
    &dcb_sci1,                  /*!< Driver control block. */
    SciH8Init,                  /*!< Driver initialization routine. */
    SciH8IOCtl,                 /*!< Driver specific control function. */
    SciH8Read,
    SciH8Write,
    /* SciH8Write_P */
    SciH8Open,
    SciH8Close,
    0
};

/*!
 * \brief SCI2 Device information structure.
 */
NUTDEVICE devSci2 = {
    0,                          /*!< Pointer to next device. */
    {'s', 'c', 'i', '2', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    2,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    &ifs_sci2,                  /*!< Interface control block. */
    &dcb_sci2,                  /*!< Driver control block. */
    SciH8Init,                  /*!< Driver initialization routine. */
    SciH8IOCtl,                 /*!< Driver specific control function. */
    SciH8Read,
    SciH8Write,
    /* SciH8Write_P */
    SciH8Open,
    SciH8Close,
    0
};

/*@}*/
