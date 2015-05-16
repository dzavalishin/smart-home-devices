#ifndef _SYS_DEVICE_H_
#define _SYS_DEVICE_H_

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
 * Revision 1.10  2009/01/17 11:26:51  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.9  2009/01/09 17:54:28  haraldkipp
 * Added SPI bus controller for AVR and AT91.
 *
 * Revision 1.8  2008/08/11 07:00:25  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2006/03/16 15:25:34  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.6  2006/01/05 16:45:34  haraldkipp
 * Added a new driver type IFTYP_FS.
 *
 * Revision 1.5  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2004/06/07 15:07:00  olereinhardt
 * Added IFTYP_CAN
 *
 * Revision 1.3  2004/03/18 13:49:00  haraldkipp
 * Deprecated functions removed.
 * IFSTREAM structure taken from ifstream
 * header file.
 *
 * Revision 1.2  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:19  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.18  2003/05/06 17:58:04  harald
 * ATmega128 definitions moved to compiler include
 *
 * Revision 1.17  2003/03/31 14:34:08  harald
 * Added character device
 *
 * Revision 1.16  2003/02/04 18:00:52  harald
 * Version 3 released
 *
 * Revision 1.15  2003/01/14 16:35:04  harald
 * Definitions moved
 *
 * Revision 1.14  2002/11/02 15:17:01  harald
 * Library dependencies moved to compiler.h
 *
 * Revision 1.13  2002/09/15 16:46:28  harald
 * *** empty log message ***
 *
 * Revision 1.12  2002/08/08 17:24:21  harald
 * Using time constants by KU
 *
 * Revision 1.11  2002/07/03 16:45:41  harald
 * Using GCC 3.2
 *
 * Revision 1.10  2002/06/26 17:29:28  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/file.h>

#include <stdint.h>

/*!
 * \file sys/device.h
 * \brief Nut/OS device definitions.
 */

__BEGIN_DECLS

// wait times for emulation and reality
// has to be overworked

#ifndef __EMULATION__
#define WAIT5		5
#define WAIT50		50
#define WAIT100		100
#define WAIT250		250
#define WAIT500		500
#else
#define WAIT5		1
#define WAIT50		5
#define WAIT100		10
#define WAIT250		25
#define WAIT500		50
#endif

/*!
 * \addtogroup xgDevice
 */
/*@{*/

#define IFTYP_RAM       0	/*!< \brief RAM device */
#define IFTYP_ROM       1	/*!< \brief ROM device */
#define IFTYP_STREAM    2	/*!< \brief Stream device */
#define IFTYP_NET       3	/*!< \brief Net device */
#define IFTYP_TCPSOCK	4	/*!< \brief TCP socket */
#define IFTYP_CHAR      5	/*!< \brief Character stream device */
#define IFTYP_CAN       6       /*!< \brief CAN device */
#define IFTYP_BLKIO     7   /*!< \brief Block I/O device */
#define IFTYP_FS       16   /*!< \brief file system device */

/*!
 * \brief Device structure type.
 */
typedef struct _NUTDEVICE NUTDEVICE;

/*!
 * \brief Device structure.
 *
 * Each device driver provides a global variable of this type.
 * Applications use NutRegisterDevice() to bind the device
 * driver to the application code. Except this call, applications
 * refer to device drivers by the name of the device when using 
 * standard C functions like _open() or fopen().
 *
 * More than one device driver may be available for the same
 * hardware device. Typically these drivers provide the same
 * name for the device and applications must not refer to
 * more than one device driver with the same name.
 */
struct _NUTDEVICE {

    /*! 
     * \brief Link to the next device structure. 
     */
    NUTDEVICE *dev_next;

    /*! 
     * \brief Unique device name. 
     */
    char dev_name[9];

    /*! 
     * \brief Type of interface. 
     *
     * May be any of the following:
     * - IFTYP_RAM
     * - IFTYP_ROM
     * - IFTYP_STREAM
     * - IFTYP_NET
     * - IFTYP_TCPSOCK
     * - IFTYP_CHAR
     */
    uint8_t dev_type;

    /*! 
     * \brief Hardware base address.
     *
     * Will be set by calling NutRegisterDevice(). On some device 
     * drivers this address may be fixed.
     */
    uintptr_t dev_base;

    /*! \brief Interrupt registration number. 
     *
     * Will be set by calling NutRegisterDevice(). On some device 
     * drivers the interrupt may be fixed.
     */
    uint8_t dev_irq;

    /*! \brief Interface control block.
     *
     * With stream devices, this points to the IFSTREAM structure and 
     * with network devices this is a pointer to the IFNET structure.
     */
    void *dev_icb;

    /*! 
     * \brief Driver control block.
     *
     * Points to a device specific information block.
     */
    void *dev_dcb;

    /*! 
     * \brief Driver initialization routine. 
     *
     * This routine is called during device registration.
     */
    int (*dev_init) (NUTDEVICE *);

    /*! 
     * \brief Driver control function.
     *
     * Used to modify or query device specific settings.
     */
    int (*dev_ioctl) (NUTDEVICE *, int, void *);

    /*! 
     * \brief Read from device. 
     */
    int (*dev_read) (NUTFILE *, void *, int);

    /*! 
     * \brief Write to device. 
     */
    int (*dev_write) (NUTFILE *, CONST void *, int);

    /*! 
     * \brief Write to device. 
     */
#ifdef __HARVARD_ARCH__
    int (*dev_write_P) (NUTFILE *, PGM_P, int);
#endif

    /*! 
     * \brief Open a device or file. 
     */
    NUTFILE * (*dev_open) (NUTDEVICE *, CONST char *, int, int);

    /*! 
     * \brief Close a device or file. 
     */
    int (*dev_close) (NUTFILE *);

    /*! 
     * \brief Request file size. 
     */
    long (*dev_size) (NUTFILE *);

};

/*!
 * \brief Device structure type.
 */
typedef struct _NUTVIRTUALDEVICE NUTVIRTUALDEVICE;

/*!
 * \brief Virtual device structure.
 */
struct _NUTVIRTUALDEVICE {
    NUTVIRTUALDEVICE *vdv_next;
    NUTVIRTUALDEVICE *vdv_zero;
    uint8_t vdv_type;
    int (*vdv_read) (void *, void *, int);
    int (*vdv_write) (void *, CONST void *, int);
#ifdef __HARVARD_ARCH__
    int (*vdv_write_P) (void *, PGM_P, int);
#endif
    int (*vdv_ioctl) (void *, int, void *);
};

/*!
 * \brief Stream interface type.
 */
typedef struct _IFSTREAM IFSTREAM;

/*!
 * \brief Stream interface information structure.
 *
 * Deprecated structure. Device drivers should use
 * the device control block.
 */
struct _IFSTREAM {
    int  (*if_input)(NUTDEVICE *);  /*!< \brief Wait for input. */
    int  (*if_output)(NUTDEVICE *); /*!< \brief Initiate output. */
    int  (*if_flush)(NUTDEVICE *);  /*!< \brief Wait until output buffer empty. */
    volatile uint8_t if_rx_idx;      /*!< \brief Next input index. */
    uint8_t if_rd_idx;               /*!< \brief Next read index. */
    volatile uint8_t if_tx_idx;      /*!< \brief Next output index. */
    uint8_t if_wr_idx;               /*!< \brief Next write index. */
    volatile uint8_t if_tx_act;      /*!< \brief Set if transmitter running. */
    uint8_t if_last_eol;             /*!< \brief Last end of line character read. */
    uint8_t if_rx_buf[256];          /*!< \brief Input buffer. */
    uint8_t if_tx_buf[256];          /*!< \brief Output buffer. */
};

/*@}*/


extern NUTDEVICE *nutDeviceList;

extern int NutRegisterDevice(NUTDEVICE * dev, uintptr_t base, uint8_t irq);
extern NUTDEVICE *NutDeviceLookup(CONST char *name);

__END_DECLS

#endif
