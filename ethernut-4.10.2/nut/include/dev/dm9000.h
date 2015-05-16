#ifndef _DEV_DM9000_H_
#define _DEV_DM9000_H_

#include <sys/device.h>
#include <net/if_var.h>

/*
 * Available drivers.
 */
extern NUTDEVICE devDm9000;

#ifndef DEV_ETHER
#define DEV_ETHER   devDm9000
#endif

#ifndef devEth0
#define devEth0     devDm9000
#endif

#endif

