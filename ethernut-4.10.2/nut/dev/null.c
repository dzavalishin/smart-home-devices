/*
 * Copyright (C) 2000-2004 by ETH Zurich
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
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/* @file null.c - a nut/os null device driver
 * @brief A /dev/null device driver
 * This can be useful if your application might write unwanted output to stdout.
 * With this deveice you can redirect stdout to the nullDev which discards any output
 * Any IOCTL will result in OK, and any read will give back zero read bytes
 *
 * 2005.06.21 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */

#include <compiler.h>
#include <stdlib.h>

#include <sys/file.h>
#include <sys/device.h>

/*!
 * \addtogroup xgDevice
 */
/*@{*/

/*!
 * \brief Open UnixDev
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *NullOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *nf;

    nf = malloc(sizeof(NUTFILE));

    // enter data
    nf->nf_next = 0;
    nf->nf_dev = dev;

    return nf;
}


/*!
 * \brief Blocking write bytes to file
 *
 * \return Number of characters sent.
 */
static int NullWrite(NUTFILE * nf, CONST void *buffer, int len)
{
    return len;
}
#ifdef __HARVARD_ARCH__
static int NullWriteP(NUTFILE * nf, PGM_P buffer, int len)
{
    return len;
}
#endif



 
/*!
 * \brief Read bytes from file
 *
 * \return Number of characters read.
 */
static int NullRead(NUTFILE * nf, void *buffer, int len)
{
    // test for read len. len == 0 => flush fd
    if (len == 0){
        return 0;
    }

	// otherwise also just return 0 bytes without blocking
    return 0;
}

/*! 
 * \brief Close ...
 *
 * \return Always 0.
 */
static int NullClose(NUTFILE * nf)
{
	if (nf)
		free (nf);
    return 0;
}

/*!
 * \brief Perform control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. We do return ok for any function
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 */
int NullIOCTL(NUTDEVICE * dev, int req, void *conf)
{
    return 0;
}

/* ======================= Devices ======================== */
/*!
 * \brief Null device information structure.
 */
NUTDEVICE devNull = {
    0,                          /*!< Pointer to next device. */
    {'n', 'u', 'l', 'l', 0, 0, 0, 0, 0},
                                /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */

    0,                          /*!< Driver control block. */
    0,                          /*!< Driver initialization routine. */
    
    NullIOCTL,                  /*!< Driver specific control function. */
    NullRead,
    NullWrite,
#ifdef __HARVARD_ARCH__
    NullWriteP,
#endif
    NullOpen,
    NullClose,
    0
};


/*@}*/
