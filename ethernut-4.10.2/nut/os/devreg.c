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
 * Revision 1.6  2009/01/17 11:26:52  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 07:00:33  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2004/09/08 10:24:26  haraldkipp
 * RAMSTART is too platform dependant
 *
 * Revision 1.3  2004/03/18 13:42:50  haraldkipp
 * Deprecated header file removed
 *
 * Revision 1.2  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:47  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.3  2003/04/21 17:06:40  harald
 * Device init moved to registration
 *
 * Revision 1.2  2003/02/04 18:15:56  harald
 * Version 3 released
 *
 * Revision 1.1  2003/01/17 18:55:24  harald
 * Device routines splitted
 *
 */

#include <string.h>
#include <sys/nutdebug.h>
#include <dev/uart.h>

/*!
 * \addtogroup xgDevice
 */
/*@{*/

/*!
 * \brief Linked list of all registered devices.
 */
NUTDEVICE *nutDeviceList = 0;

/*!
 * \brief Find device entry by name.
 *
 * \param  name Unique device name.
 *
 * \return Pointer to the ::NUTDEVICE structure.
 */
NUTDEVICE *NutDeviceLookup(CONST char *name)
{
    NUTDEVICE *dev;

    NUTASSERT(name != NULL);
    for (dev = nutDeviceList; dev; dev = dev->dev_next)
        if (strcmp(dev->dev_name, name) == 0)
            break;

    return dev;
}

/*!
 * \brief Register and initialize a device.
 *
 * Initializes the device and adds it to the system device list.
 * Applications should call this function during initialization
 * for each device they intend to use.
 *
 * \param dev  Pointer to the ::NUTDEVICE structure, which is
 *             provided by the device driver. This structure
 *             contains a hardware device name, which must be 
 *             unique among all registered devices. Drivers may 
 *             operate in a different mode using the same hardware,
 *             like interrupt driven or polling UART drivers.
 *             Only one of those drivers can be registered, because
 *             they specify the same hardware device name.
 * \param base Hardware base address of this device. Set to 0,
 *             if the device driver has a fixed hardware address.
 * \param irq  Hardware interrupt used by this device. Set to 0,
 *             if the device driver doesn't support configurable
 *             interupts.
 * \return 0 if the device has been registered for the first time
 *         and initialization was successful.
 *         The function returns -1 if any device with the same name
 *         had been registered previously, if the ::NUTDEVICE 
 *         structure is invalid or if the device initialization
 *         failed.
 */
int NutRegisterDevice(NUTDEVICE * dev, uintptr_t base, uint8_t irq)
{
    int rc = -1;

    NUTASSERT(dev != NULL);

    if (base)
        dev->dev_base = base;
    if (irq)
        dev->dev_irq = irq;

    if (NutDeviceLookup(dev->dev_name) == 0) {
        if(dev->dev_init == 0 || (*dev->dev_init)(dev) == 0) {
            dev->dev_next = nutDeviceList;
            nutDeviceList = dev;
            rc = 0;
        }
    }
    return rc;
}

/*@}*/
