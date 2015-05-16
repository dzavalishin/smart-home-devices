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
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <dev/debug.h>

/*!
 * \addtogroup xgNutArchH8300hDevDebug
 */
/*@{*/

#include <sys/device.h>
#include <sys/file.h>

#include <h83068f.h>

#define SSR_TDRE        0x80
#define SSR_RDRF        0x40
#define SSR_ORER        0x20
#define SSR_FER         0x10
#define SSR_PER         0x08
#define SSR_TEND        0x04

static NUTFILE dbgfile;

/*
 * Returns apropriate H8/300H SCI structure pointer.
 */
#define GET_SCI(p) ( ((p) == 0) ? &SCI0 : ( ((p) == 1) ? &SCI1 : &SCI2 ) )

/*!
 * \brief Handle I/O controls for debug device.
 *
 * The debug device doesn't support any.
 *
 * \return Always -1.
 */
static int DebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    return -1;
}

/*!
 * \brief Initialize debug device.
 *
 * Simply enable the device. Baudrate divisor set to 7 for
 * 115.2 kBaud at 22.1184 MHz.
 *
 * \return Always 0.
 */
static int DebugInit(NUTDEVICE * dev)
{
    unsigned short i;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    /* serial port initialisation */
    sci->SCR.BYTE = 0x00;
    sci->SMR.BYTE = 0x00;
//    sci->BRR = 0x47;          /* 9600 bps with 22.1184MHz crystal */
    sci->BRR = 0x05;            /* 115200 bps with 22.1184MHz crystal */
    for (i = 0; i < 500; i++);  /* wait for a while */
    sci->SCR.BYTE = 0x30;
    sci->SSR.BYTE &= 0x84;

    return 0;
}

/*!
 * \brief Send a single character to debug device.
 *
 * A carriage return character will be automatically appended 
 * to any linefeed.
 */
static void DebugPut(volatile struct st_sci *sci, char ch)
{
    while (!(sci->SSR.BYTE & SSR_TDRE));        /* Wait until TDRE = 1 */

    sci->TDR = ch;              /* Set transmit data */
    sci->SSR.BYTE &= ~SSR_TDRE;
    if (ch == '\n')
        DebugPut(sci, '\r');
}

/*!
 * \brief Read a single character from debug device.
 *
 * This function waits until a character is received.
 */
//static int DebugGet(volatile struct st_sci* sci)
//{
//    unsigned char c;

    /* clear any detected errors */
//    sci->SSR.BYTE &= ~(SSR_ORER | SSR_FER | SSR_PER); /* Clear error bits */

    /* wait for a byte */
//    while(!( sci->SSR.BYTE & SSR_RDRF));

    /* got one-- return it */
//    c = sci->RDR;
//    sci->SSR.BYTE &= ~SSR_RDRF;

//    return (int) c;
//}

/*!
 * \brief Read a single character from debug device.
 *
 * Non blocking version of DebugGet(). If I/O operation is pending
 * this function returns -1.
 */
static int DebugGetNB(volatile struct st_sci *sci)
{
    unsigned char c;

    /* clear any detected errors */
    if ((sci->SSR.BYTE & (SSR_ORER | SSR_FER | SSR_PER))) {
        sci->SSR.BYTE &= ~(SSR_ORER | SSR_FER | SSR_PER);       /* Clear error bits */
        return -1;
    }

    /* wait for a byte */
    if (!(sci->SSR.BYTE & SSR_RDRF))
        return -1;

    /* got one-- return it */
    c = sci->RDR;
    sci->SSR.BYTE &= ~SSR_RDRF;

    return (int) c;
}

/*!
 * \brief Read one character from debug device.
 *
 * This function doesn't wait for a character. If I/O operation is pending,
 * it returns -1
 * \return Number of characters read.
 */
static int DebugRead(NUTFILE * fp, void *buffer, int len)
{
    int c, l = 0;
    char *cp = buffer;
    volatile struct st_sci *sci = GET_SCI(fp->nf_dev->dev_base);

    while (l < len) {
        c = DebugGetNB(sci);
        if (c == -1) {
            *cp++ = '\0';
            return 0;
        }
        cp[l++] = (unsigned char) c;
    }

    return l;
}

/*!
 * \brief Send characters to debug device.
 *
 * A carriage return character will be automatically appended 
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int DebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int c = len;
    CONST char *cp = buffer;
    volatile struct st_sci *sci = GET_SCI(fp->nf_dev->dev_base);

    while (c--)
        DebugPut(sci, *cp++);
    return len;
}

/*!
 * \brief Open debug device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *DebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    dbgfile.nf_next = 0;
    dbgfile.nf_dev = dev;
    dbgfile.nf_fcb = 0;

    return &dbgfile;
}

/*! 
 * \brief Close debug device.
 *
 * \return Always 0.
 */
static int DebugClose(NUTFILE * fp)
{
    return 0;
}

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device. */
    {'s', 'c', 'i', '0', 'd', 'b', 'g', 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    0,                          /*!< Driver control block. */
    DebugInit,                  /*!< Driver initialization routine. */
    DebugIOCtl,                 /*!< Driver specific control function. */
    DebugRead,
    DebugWrite,
    /* Write from program space data to device. */
    DebugOpen,
    DebugClose,
    0
};

NUTDEVICE devDebug1 = {
    0,                          /*!< Pointer to next device. */
    {'s', 'c', 'i', '1', 'd', 'b', 'g', 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    1,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    0,                          /*!< Driver control block. */
    DebugInit,                  /*!< Driver initialization routine. */
    DebugIOCtl,                 /*!< Driver specific control function. */
    DebugRead,
    DebugWrite,
    /* Write from program space data to device. */
    DebugOpen,
    DebugClose,
    0
};

NUTDEVICE devDebug2 = {
    0,                          /*!< Pointer to next device. */
    {'s', 'c', 'i', '2', 'd', 'b', 'g', 0, 0}
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    2,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    0,                          /*!< Driver control block. */
    DebugInit,                  /*!< Driver initialization routine. */
    DebugIOCtl,                 /*!< Driver specific control function. */
    DebugRead,
    DebugWrite,
    /* Write from program space data to device. */
    DebugOpen,
    DebugClose,
    0
};

/*@}*/
