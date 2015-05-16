/*
 * Copyright (C) 2002 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 * -
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by egnite Software GmbH
 *    and its contributors.
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
 * Revision 1.12  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.11  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2007/07/17 18:32:27  haraldkipp
 * Fixed bug #1369171, memory leak in NutPppClose(). Thanks to Sergey Danilov.
 *
 * Revision 1.9  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.8  2006/03/29 01:23:52  olereinhardt
 *  Signednes of strings
 *
 * Revision 1.7  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.6  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.5  2004/03/14 10:12:29  haraldkipp
 * Bugfix, failed to compile
 *
 * Revision 1.4  2004/03/08 11:15:32  haraldkipp
 * HDLC functions moved to async HDLC driver.
 *
 * Revision 1.3  2003/08/14 15:21:01  haraldkipp
 * Bugfix, allow HDLC flag to mark end _and_ begin
 *
 * Revision 1.2  2003/08/05 20:05:11  haraldkipp
 * DNS removed from interface
 *
 * Revision 1.1.1.1  2003/05/09 14:40:48  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:33:50  harald
 * PPP hack for simple UART support, functions reordered.
 *
 * Revision 1.1  2003/03/31 14:53:08  harald
 * Prepare release 3.1
 *
 */

#include <cfg/os.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

#include <net/if_var.h>
#include <net/ppp.h>

#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/thread.h>

#include <dev/ppp.h>
#include <netinet/ppp_fsm.h>
#include <dev/ahdlc.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgPPP
 */
/*@{*/

static PPPDCB dcb_ppp;


/*
 * Pass reads to the physical driver for now.
 */
static int NutPppRead(NUTFILE * fp, void *buffer, int size)
{
    return _read(((PPPDCB *) (fp->nf_dev->dev_dcb))->dcb_fd, buffer, size);
}

/*
 * Pass writes to the physical driver for now.
 */
static int NutPppWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return _write(((PPPDCB *) (fp->nf_dev->dev_dcb))->dcb_fd, buffer, len);
}

/*
 * Pass writes to the physical driver for now.
 */
#ifdef __HARVARD_ARCH__
static int NutPppWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return _write_P(((PPPDCB *) (fp->nf_dev->dev_dcb))->dcb_fd, buffer, len);
}
#endif

/*!
 * \brief Perform PPP control functions.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - LCP_OPEN
 *             - LCP_CLOSE
 *             - LCP_LOWERUP
 *             - LCP_LOWERDOWN
 *             Any other function will be passed to the physical driver.
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 */
static int NutPppIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;

    switch (req) {
    case LCP_OPEN:
        LcpOpen(dev);
        break;

    case LCP_CLOSE:
        LcpClose(dev);
        break;

    case LCP_LOWERUP:
        LcpLowerUp(dev);
        break;

    case LCP_LOWERDOWN:
        LcpLowerDown(dev);
        break;

    default:
        rc = _ioctl(((PPPDCB *) (dev->dev_dcb))->dcb_fd, req, conf);
        break;
    }
    return rc;
}

/*
 * \brief Enable the link layer to come up.
 *
 * The underlying hardware driver should have established a physical 
 * connection before calling this function.
 *
 * \param name Physical device name optionally followed by username 
 *             and password, each separated by a slash.
 *
 */
static NUTFILE *NutPppOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp;
    uint8_t i;
    char *cp;
    char *sp;
    char pdn[9];
    PPPDCB *dcb = dev->dev_dcb;

    /* Clear our device control block. */
    memset(dcb, 0, sizeof(PPPDCB));

    /* Get the first part of the name, it specifies the physical device. */
    for (cp = (char *) name, i = 0; *cp && *cp != '/' && i < sizeof(pdn) - 1; i++) {
        pdn[i] = *cp++;
    }
    pdn[i] = 0;

    /* Open the pysical device. */
    if ((dcb->dcb_fd = _open(pdn, _O_RDWR | _O_BINARY)) == -1) {
        return NUTFILE_EOF;
    }

    /*
     * Allocate a file structure to return.
     */
    if ((fp = NutHeapAlloc(sizeof(NUTFILE))) == 0) {
        return NUTFILE_EOF;
    }
    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    /*
     * Extract user name and password and store it in our device control
     * block. It will be used later by the authentication layer.
     */
    if (*cp == '/') {
        for (sp = ++cp, i = 0; *sp && *sp != '/'; sp++, i++);
        if (i) {
            dcb->dcb_user = NutHeapAlloc(i + 1);
            for (sp = (char*)dcb->dcb_user; *cp && *cp != '/';)
                *sp++ = *cp++;
            *sp = 0;
        }
        if (*cp == '/') {
            for (sp = ++cp, i = 0; *sp && *sp != '/'; sp++, i++);
            if (i) {
                dcb->dcb_pass = NutHeapAlloc(i + 1);
                for (sp = (char*)dcb->dcb_pass; *cp && *cp != '/';)
                    *sp++ = *cp++;
                *sp = 0;
            }
        }
    }

    /* Enable all layers to come up. */
    IpcpOpen(dev);

    return fp;
}


/*
 * Start closing connection.
 */
static int NutPppClose(NUTFILE * fp)
{
    PPPDCB *dcb = fp->nf_dev->dev_dcb;

    IpcpClose(fp->nf_dev);
    _close(dcb->dcb_fd);

    if (dcb->dcb_user)
        NutHeapFree(dcb->dcb_user);
    if (dcb->dcb_pass)
        NutHeapFree(dcb->dcb_pass);
    NutHeapFree(fp);

    return 0;
}

/*!
 * \brief Initialize the PPP device.
 *
 * This routine is called during device registration and initializes the 
 * PPP state machine.
 *
 * \param dev  Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NutPppInit(NUTDEVICE * dev)
{
    return NutPppInitStateMachine(dev);
}

/*!
 * \brief Network interface information structure.
 */
IFNET ifn_ppp = {
    IFT_PPP,                    /*!< \brief Interface type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0}
    ,                           /*!< \brief Hardware net address. */
    0,                          /*!< \brief IP address. */
    0,                          /*!< \brief Remote IP address for point to point. */
    0,                          /*!< \brief IP network mask. */
    PPP_MRU,                    /*!< \brief Maximum size of a transmission unit. */
    0,                          /*!< \brief Packet identifier. */
    0,                          /*!< \brief Linked list of arp entries. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutPppInput,                /*!< \brief Routine to pass received data to, if_recv(). */
    0,                          /*!< \brief Dynamically attached driver output routine, if_send(). */
    NutPppOutput                /*!< \brief Media output routine, if_output(). */
};

/*!
 * \brief Device information structure.
 *
 * This is a virtual device driver has no underlying hardware
 * and must not be registered. It will be initialized when the
 * application calls NutNetIfConfig().
 */
NUTDEVICE devPpp = {
    0,                          /* Pointer to next device, dev_next. */
    {'p', 'p', 'p', 0, 0, 0, 0, 0, 0}
    ,                           /* Unique device name, dev_name. */
    IFTYP_NET,                  /* Type of device, dev_type. */
    0,                          /* Base address, dev_base. */
    0,                          /* First interrupt number, dev_irq. */
    &ifn_ppp,                   /* Interface control block, dev_icb. */
    &dcb_ppp,                   /* Driver control block, dev_dcb. */
    NutPppInit,                 /* Driver initialization routine, dev_init(). */
    NutPppIOCtl,                /* Driver specific control function, dev_ioctl(). */
    NutPppRead,                 /* Read from device, dev_read. */
    NutPppWrite,                /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutPppWrite_P,              /* Write data from program space to device, dev_write_P. */
#endif
    NutPppOpen,                 /* Open a device or file, dev_open. */
    NutPppClose,                /* Close a device or file, dev_close. */
    0                           /* Request file size. */
};

/*@}*/
