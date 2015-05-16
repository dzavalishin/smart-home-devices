#ifndef _DEV_DM9000E_H_
#define _DEV_DM9000E_H_

#include <sys/device.h>
#include <net/if_var.h>

/*
 * Available drivers.
 */
extern NUTDEVICE devDM9000E;

#ifndef DEV_ETHER
#define DEV_ETHER   devDM9000E
#endif

#ifndef devEth0
#define devEth0   devDM9000E
#endif

#endif

