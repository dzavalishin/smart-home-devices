/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Device map.
 *
**/

#ifndef MAP_H
#define MAP_H


#include "defs.h"
#include <inttypes.h>

struct dev_major;

struct dev_minor
{
    uint8_t     number;
    char *	name;

    struct dev_major *dev;

    char *      (*to_string)( struct dev_minor *sub );
    int16_t     (*from_string)( struct dev_minor *sub, char *);         // 0 - success

    uint16_t    io_count;
    uint16_t    err_count;
};

typedef struct dev_minor dev_minor;


struct dev_major
{
    const char *name;

    void        (*init)(struct dev_major *dev);     // Called on system init
    uint8_t     (*start)(struct dev_major *dev);    // Called after all inits if dev is enabled, returns 0 if ok
    void        (*stop)(struct dev_major *dev);     // Called if dev is disabled runtime
    void        (*timer)(struct dev_major *dev);    // Called once a second

    char *      (*to_string)( struct dev_major *dev );
    int16_t     (*from_string)( struct dev_major *dev, char *);         // 0 - success

    uint16_t    minor_count; // Number of sub-devices found
    struct dev_minor *subdev;

    uint8_t     started;
};

typedef struct dev_major dev_major;


dev_major *new_dev( uint8_t n_minor, const char *name );
int8_t init_dev( dev_major *dev, uint8_t n_minor, const char *name );
int8_t init_subdev( dev_major *dev, uint8_t n_minor, const char *name );


void timer_regular_devices(void); // Must be called once a second, calls *timer for all devs

// TODO call on manual reboot
void stop_regular_devices(void);


#endif // MAP_H

