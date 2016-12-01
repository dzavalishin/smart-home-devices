/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Device map. Impl.
 *
**/


#include "map.h"
#include <stdlib.h>

int8_t init_subdev( dev_major *dev, uint8_t n_minor, const char *name )
{
    uint8_t i;

    // for free in case of error
    //for( i = 0; i < n_minor; i++ )
    //  dev->subdev[i] = 0;

    dev->subdev = malloc( n_minor * sizeof (dev_minor) );
    if( 0 == dev->subdev ) goto free;

    for( i = 0; i < n_minor; i++ )
    {
        dev_minor *m = dev->subdev+i;

        m->number = i;
        m->name = 0; // TODO make name

        m->dev = dev;

        m->io_count = 0;
        m->err_count = 0;

        m->to_string = 0;
        m->from_string = 0;
    }

    return 0;

free:
    //for( i = 0; i < n_minor; i++ )
    //    if(dev->subdev[i]) free( dev->subdev[i] );

    free( dev->subdev );

    return -1;
}


int8_t init_dev( dev_major *dev, uint8_t n_minor, const char *name )
{
    if( n_minor == 0 ) n_minor = 1;

    dev->subdev = malloc( n_minor * sizeof(dev_minor *) );
    if( dev->subdev == 0 )
        return -1;


    dev->name = name;
    dev->minor_count = n_minor;

    dev->to_string = 0;
    dev->from_string = 0;

    dev->init = 0;
    dev->start = 0;
    dev->stop = 0;
    dev->timer = 0;

    return init_subdev( dev, n_minor, name );
}

dev_major *new_dev( uint8_t n_minor, const char *name )
{
    dev_major *dev = malloc( sizeof (dev_major) );
    if( dev == 0 ) return 0;

    if( !init_dev( dev, n_minor, name ) )
        return dev;

    free( dev );
    return 0;
}



