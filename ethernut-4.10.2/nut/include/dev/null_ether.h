#ifndef _DEV_NULL_ETHER_H_
#define _DEV_NULL_ETHER_H_

#include <sys/device.h>

/*
 * Available drivers.
 */
extern NUTDEVICE devNullEther;

#ifndef DEV_ETHER
#define DEV_ETHER   devNullEther
#endif

#ifndef devEth0
#define devEth0     devNullEther
#endif

#endif

