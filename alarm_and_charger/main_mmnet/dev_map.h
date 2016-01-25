/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Device map.
 *
**/

#ifndef MAP_H
#define MAP_H


#include "defs.h"
#include <inttypes.h>

struct dev_major;
struct dev_minor;

typedef    int8_t      (*minor_to_string_f)( struct dev_minor *sub, char *out, uint8_t out_size );
typedef    int8_t      (*minor_from_string_f)( struct dev_minor *sub, char *);



struct dev_minor
{
    uint8_t     number;
    char *	name;

    struct dev_major *dev;

    int8_t      (*to_string)( struct dev_minor *sub, char *out, uint8_t out_size );     // 0 - success
    int8_t      (*from_string)( struct dev_minor *sub, char *);         		// 0 - success

    uint16_t    io_count;
    uint16_t    err_count;
};

typedef struct dev_minor dev_minor;


struct dev_major
{
    const char *name;

    int8_t      (*init)(struct dev_major *dev);     // Called on system init, returns 0 on success
    int8_t      (*start)(struct dev_major *dev);    // Called after all inits if dev is enabled, returns 0 on success
    void        (*stop)(struct dev_major *dev);     // Called if dev is disabled runtime
    void        (*timer)(struct dev_major *dev);    // Called once a second

    //char *      (*to_string)( struct dev_major *dev );
    int8_t      (*to_string)( struct dev_minor *sub, char *out, uint8_t out_size );     // 0 - success
    int8_t      (*from_string)( struct dev_major *dev, char *);         		// 0 - success

    uint16_t    minor_count; // Number of sub-devices found
    struct dev_minor *subdev;

    uint8_t     started;
};

typedef struct dev_major dev_major;


dev_major *new_dev( uint8_t n_minor, const char *name );
int8_t init_dev( dev_major *dev, uint8_t n_minor, const char *name );
int8_t init_subdev( dev_major *dev, uint8_t n_minor, const char *name );

// set all subdev from/to string func
void 	dev_init_subdev_getset( dev_major *dev, minor_from_string_f, minor_to_string_f );


uint8_t dev_count_devices( dev_major *devices[], uint8_t n_major );
dev_minor *dev_get_minor( uint8_t n_minor ); // get minor by global index (counting through all the minors)

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

int8_t      dev_uint16_to_string( struct dev_minor *sub, char *out, uint8_t out_size, uint16_t val );


// -----------------------------------------------------------------------
// Global state
// -----------------------------------------------------------------------



extern uint8_t n_major_total;
extern uint8_t n_minor_total;








#endif // MAP_H

