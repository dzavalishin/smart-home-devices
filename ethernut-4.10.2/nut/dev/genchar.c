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
 * \file dev/genchar.c
 * \brief Generic character driver template.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/04/07 13:52:52  haraldkipp
 * Generic character driver sample added.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <dev/irqreg.h>
#include <dev/genchar.h>

/*!
 * \addtogroup xgDevGenChar
 */
/*@{*/

/* Sample hardware status port. */
#ifndef GENDEV_SPORT
#define GENDEV_SPORT    0x100
#endif

/* Sample hardware data port. */
#ifndef GENDEV_DPORT
#define GENDEV_DPORT    0x104
#endif

/* Sample hardware interrupt. */
#ifndef GENDEV_SIGNAL
#define GENDEV_SIGNAL   sig_INTERRUPT1
#endif

/*!
 * \brief Device driver's private data structure.
 */
typedef struct {
    HANDLE dcb_rrdy;        /*!< Receiver ready queue. */
    volatile int dcb_rcnt;  /*!< Number of bytes in the receive buffer. */
    uint8_t dcb_rbuff[16];   /*!< Receive buffer. */
    uint32_t dcb_rtimeout;    /*!< Read timeout. */
    HANDLE dcb_trdy;        /*!< Transmitter ready queue. */
    int    dcb_tlen;        /*!< Number of bytes in the transmit buffer. */
    volatile int dcb_tcnt;  /*!< Number of bytes already transmitted. */
    uint8_t dcb_tbuff[16];   /*!< Transmit buffer. */
    uint32_t dcb_ttimeout;    /*!< Write timeout. */
} DEVDCB;

static DEVDCB devdcb;

/*!
 * \brief Driver interrupt entry.
 *
 * If polling is used, this routine is not required, of course.
 */
static void GenCharInterrupt(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *)arg;
    DEVDCB *dcb = dev->dev_dcb;
    uint8_t st = inr(GENDEV_SPORT);

    /* Receive interrupt. */
    if (st) {
        /* Avoid buffer overflow. */
        if (dcb->dcb_rcnt < sizeof(dcb->dcb_rbuff)) {
            /* Get byte from device and increment receive counter. */
            dcb->dcb_rbuff[dcb->dcb_rcnt] = inr(GENDEV_DPORT);
            dcb->dcb_rcnt++;
        }
        /* Send event on first character. */
        if (dcb->dcb_rcnt == 1) {
            NutEventPostFromIrq(&dcb->dcb_rrdy);
        }
    }

    /* Transmit interrupt. */
    else {
        if (dcb->dcb_tcnt < dcb->dcb_tlen) {
            /* Send byte to device and increment transmit counter. */
            outr(GENDEV_DPORT, dcb->dcb_tbuff[dcb->dcb_tcnt]);
            dcb->dcb_tcnt++;
        }
        /* Transmit buffer empty, send event. */
        else {
            NutEventPostFromIrq(&dcb->dcb_trdy);
            /* Optionally disable further transmit interrupts. */
        }
    }
}

/*!
 * \brief Handle device I/O controls.
 *
 * This function is called by the ioctl() function of the C runtime
 * library. Applications use the ioctl() to control device specific 
 * functions.
 *
 * \param dev Pointer to the device information structure.
 * \param req Requested control function. Possible values are defined
 *            in the device driver's header file.
 * \return 0 on success or -1 if the function fails or is not supported.
 */
static int GenCharIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    DEVDCB *dcb = dev->dev_dcb;
    uint32_t *lvp = (uint32_t *) conf;

    switch (req) {
    case DEV_SETREADTIMEOUT:
        /* Set read timeout. */
        dcb->dcb_rtimeout = *lvp;
        break;
    case DEV_GETREADTIMEOUT:
        /* Query read timeout. */
        *lvp = dcb->dcb_rtimeout;
        break;

    case DEV_SETWRITETIMEOUT:
        /* Set write timeout. */
        dcb->dcb_ttimeout = *lvp;
        break;
    case DEV_GETWRITETIMEOUT:
        /* Query write timeout. */
        *lvp = dcb->dcb_ttimeout;
        break;

        /* Add attional ioctl functions here. */

    default:
        /* Unknown function. */
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Initialize the device.
 *
 * This function is called by NutRegisterDevice(), using the 
 * _NUTDEVICE::dev_init entry.
 *
 * \param dev Pointer to the device information structure.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GenCharInit(NUTDEVICE * dev)
{
    /* Add hardware initialization here. */

    /* Register interrupt handler, if required. */
    if (NutRegisterIrqHandler(&GENDEV_SIGNAL, GenCharInterrupt, dev)) {
        return -1;
    }
    /* Set interrupt mode and enable interrupts. */
    NutIrqSetMode(&GENDEV_SIGNAL, NUT_IRQMODE_LOWLEVEL);
    NutIrqEnable(&GENDEV_SIGNAL);

    return 0;
}

/*! 
 * \brief Read data from the device. 
 *
 * This function is called by the low level input routines of the 
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_read 
 * entry.
 *
 * The function may block the calling thread until at least one
 * character has been received or a timeout occurs.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a 
 *               previous call to GenCharOpen().
 * \param buffer Pointer to the buffer that receives the data. If zero,
 *               then all characters in the input buffer will be
 *               removed.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes read, which may be less than the number
 *         of bytes specified. A return value of -1 indicates an error,
 *         while zero is returned in case of a timeout.
 */
static int GenCharRead(NUTFILE * fp, void *buffer, int size)
{
    int rc;
    int i;
    NUTDEVICE *dev = fp->nf_dev;
    DEVDCB *dcb = dev->dev_dcb;

    /* Call without data pointer discards receive buffer. */
    if (buffer == 0) {
        /* Atomic access to the receive counter. */
        NutIrqDisable(&GENDEV_SIGNAL);
        dcb->dcb_rcnt = 0;
        NutIrqEnable(&GENDEV_SIGNAL);

        return 0;
    }

    /*
     * Wait until at least one character is buffered or until a read
     * timeout occured.
     */
    for (;;) {
        /* Atomic access to the receive counter. */
        NutIrqDisable(&GENDEV_SIGNAL);
        rc = dcb->dcb_rcnt;
        NutIrqEnable(&GENDEV_SIGNAL);
        if (rc) {
            break;
        }

        if (NutEventWait(&dcb->dcb_rrdy, dcb->dcb_rtimeout)) {
            return 0; /* Timeout. */
        }
    }

    if (rc > size) {
        rc = size;
    }

    if (rc) {
        memcpy(buffer, dcb->dcb_rbuff, rc);
        /* 
         * This sample driver simply moves remaining bytes to the front 
         * of the buffer. A more sophisticated driver may use a circular 
         * buffer.
         */
        NutIrqDisable(&GENDEV_SIGNAL);
        dcb->dcb_rcnt -= rc;
        for (i = 0; i < dcb->dcb_rcnt; i++) {
            dcb->dcb_rbuff[i] = dcb->dcb_rbuff[rc + i];
        }
        NutIrqEnable(&GENDEV_SIGNAL);
    }
    return rc;
}

/*!
 * \brief Write data to the device.
 *
 * This function is called by the low level output routines of the 
 * \ref xrCrtLowio "C runtime library", using the 
 * \ref _NUTDEVICE::dev_write entry.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a _NUTFILE structure, obtained by a previous 
 *               call to GenCharOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. With some devices this may be 
 *         less than the number of bytes specified if a timeout occured. 
 *         A return value of -1 indicates an error.
 */
static int GenCharWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int rc = 0;
    int cnt;
    int pend;
    CONST char *cp = buffer;
    NUTDEVICE *dev = fp->nf_dev;
    DEVDCB *dcb = dev->dev_dcb;

    while (rc < len) {

        /*
         * This sample driver waits on each output until all characters 
         * had been tranmitted. A more sophisticated driver may add
         * new characters as soon as there is room left in the tranmit
         * buffer.
         */
        for (;;) {
            /* Atomic access to the transmit counter. */
            NutIrqDisable(&GENDEV_SIGNAL);
            pend = dcb->dcb_tlen - dcb->dcb_tcnt;
            NutIrqEnable(&GENDEV_SIGNAL);
            
            if (pend == 0) {
                break; /* All characters tranmitted. */
            }
            if (NutEventWait(&dcb->dcb_trdy, dcb->dcb_ttimeout)) {
                return rc; /* Timeout. */
            }
        }

        /* Call without data pointer flushes the buffer. */
        if (buffer == 0) {
            return 0;
        }

        /* Determine the number of bytes left to transmit. */
        cnt = len - rc;
        if (cnt > sizeof(dcb->dcb_tbuff)) {
            cnt = sizeof(dcb->dcb_tbuff);
        }

        /* Fill the tranmit buffer. */
        NutIrqDisable(&GENDEV_SIGNAL);
        dcb->dcb_tcnt = cnt;
        memcpy(dcb->dcb_tbuff, cp + rc, cnt);
        /* Add code here to enable tranmit interrupts. */
        NutIrqEnable(&GENDEV_SIGNAL);
        rc += cnt;
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
/*!
 * \brief Write program data to the device.
 *
 * Similar to GenCharWrite() except that the data is located in program 
 * memory.
 *
 * This function is called by the low level output routines of the 
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_write_P 
 * entry, which is available on Harvard architectures only.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a NUTFILE structure, obtained by a previous 
 *               call to UsartOpen().
 * \param buffer Pointer to the data in program space to be written.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. With some devices this may be 
 *         less than the number of bytes specified if a timeout occured. 
 *         A return value of -1 indicates an error.
 */
int GenCharWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    /* Our sample driver doesn't support this. */
    return -1;
}
#endif

/*!
 * \brief Open the device.
 *
 * This function is called by the low level open routine of the C runtime 
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev  Pointer to the NUTDEVICE structure.
 * \param name Character device drivers usually ignore this paramter.
 * \param mode Operation mode. Any of the following values may be or-ed:
 * - \ref _O_BINARY
 * - \ref _O_RDONLY
 * - \ref _O_WRONLY
 * \param acc  Access type, usually ignored by Nut/OS drivers.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF if
 *         the open failed.
 */
static NUTFILE *GenCharOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp = (NUTFILE *) (dev->dev_dcb);

    if ((fp = malloc(sizeof(NUTFILE))) == 0) {
        return NUTFILE_EOF; /* No memory. */
    }

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    return fp;
}

/*! 
 * \brief Close the device.
 *
 * This function is called by the low level close routine of the C runtime 
 * library, using the _NUTDEVICE::dev_close entry.
 *
 * \param fp Pointer to a _NUTFILE structure, obtained by a previous call
 *           to GenCharOpen().
 *
 * \return 0 on success or -1 otherwise.
 */
static int GenCharClose(NUTFILE * fp)
{
    /* We may optionally flush our output and discard our input buffers. */

    /* Release all resources which we allocated in the open function. */
    if (fp) {
        free(fp);
    }
    return 0;
}

/*!
 * \brief Retrieves the number of characters in input buffer.
 *
 * This function is called by the low level size routine of the C runtime 
 * library, using the _NUTDEVICE::dev_size entry.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a 
 *               previous call to UsartOpen().
 *
 * \return The number of bytes currently stored in input buffer.
 */
long GenCharSize (NUTFILE *fp)
{
    long rc;
    NUTDEVICE *dev = fp->nf_dev;
    DEVDCB *dcb = dev->dev_dcb;

    /* Atomic access to the receive buffer counter. */
    NutIrqDisable(&GENDEV_SIGNAL);
    rc = dcb->dcb_rcnt;
    NutIrqEnable(&GENDEV_SIGNAL);

    return rc;
}

/*!
 * \brief Device information structure.
 */
NUTDEVICE devGenChar = {
    /*! \brief dev_next points to next device.
     *
     * This is used by Nut/OS to create a linked list of all registered 
     * devices. Must be a NULL pointer initially.
     */
    0,

    /*! \brief dev_name specifies the unique device name.
     *
     * Applications use this name to open a device.
     *
     * Each registered device must have a unique name. However, if more 
     * than one driver is available for the same, all drivers may use
     * the same name. This way Nut/OS can make sure, that only one
     * driver is loaded for that device.
     */
    {'g', 'e', 'n', 'c', 'h', 'a', 'r', 0, 0},

    /*! \brief dev_type secifies the device type.
     *
     * In general it's up to the device driver to use this entry.
     * Nut/OS ignores this value, while Nut/Net checks for IFTYP_NET.
     *
     * This attribute had been used by old Nut/OS versions to determine
     * between polling (IFTYP_CHAR) and buffering (IFTYP_STREAM) drivers.
     * Since Nut/OS 3, this distinction makes no sense anymore. Since
     * Nut/OS 4, block device drivers make use of this type to automatically
     * detect a registered high level file system driver (IFTYP_FS).
     *
     * It might be possible, that later Nut/OS versions will check the
     * type. So character device drivers should set it to IFTYP_CHAR
     * or IFTYP_STREAM.
     */
    IFTYP_CHAR,

    /*! \brief dev_base specifies the hardware base address.
     *
     * NutRegisterDevice() will set this value, if the calling application 
     * passed an address not equal zero. It's up to the device driver
     * to use this value. Though, for performance reasons most drivers
     * ignore this address and use hardcoded values, typically configured
     * by the Configurator during system build.
     *
     * Drivers may freely use this entry for other purposes.
     */
    0,

    /*! \brief dev_irq specifies first interrupt number.
     *
     * NutRegisterDevice() will set this value, if the calling application 
     * passed an interrupt number not equal zero. It's up to the device 
     * driver to use this value. Though, most drivers ignore it and use 
     * hardcoded interrupts, typically configured by the Configurator 
     * during system build.
     *
     * Drivers may freely use this entry for other purposes.
     */
    0,

    /*! \brief dev_icb points to the interface control block.
     *
     * Drivers may set this pointer to an internal structure during 
     * initialization.
     *
     * Character device drivers typically do not use this entry. Network 
     * or file system drivers may use it to link drivers of different 
     * levels or to store hardware independent values.
     *
     * Drivers may freely use this entry for other purposes.
     */
    0,

    /*! \brief dev_dcb points to the driver control block.
     *
     * Drivers will set this pointer to an internal structure during 
     * initialization in order to store local data like pointers
     * to buffers or status and mode informations. The structure
     * may be either statically or dynamically allocated.
     *
     * Drivers may freely use this entry for other purposes.
     */
    &devdcb,

    /*! \brief dev_init initializes the driver and the device hardware.
     *
     * Points to the driver's initialization routine and is called by 
     * NutRegisterDevice(). Drivers, which do not require any initialization
     * may set this entry to NULL.
     */
    GenCharInit,

    /*! \brief dev_ioctl implements driver specific control function.
     *
     * Currently this entry is required, because the C runtime doesn't
     * check the pointer value for NULL.
     */
    GenCharIOCtl,

    /*! \brief dev_read receives data from the device.
     *
     * Will be NULL for write-only devices.
     */
    GenCharRead,

    /*! \brief dev_write sends data to the device.
     *
     * Will be NULL for read-only devices.
     */
    GenCharWrite,

#ifdef __HARVARD_ARCH__
    /*! \brief dev_write_P sends program space data to the device.
     *
     * This entry is available on Harvard architectures (AVR) only.
     *
     * Will be NULL for read-only devices.
     */
    GenCharWrite_P,
#endif

    /*! \brief dev_open opens a device.
     *
     */
    GenCharOpen,

    /*! \brief dev_close, close a device.
     *
     */
    GenCharClose,

    /*! \brief dev_size queries device size information.
     *
     * This had been initially used by early file system drivers to query 
     * the size of a previously opened file.
     *
     * Some character device drivers return the number of bytes currently
     * available in the input buffer.
     */
    GenCharSize
};


/*@}*/
