/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Device map. Impl.
 *
**/


#include "dev_map.h"

#include <stdio.h>


#include <stdlib.h>
#include <string.h>

static char *make_subdev_name( const char *name, int minor );



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
        m->name = make_subdev_name( name, i );

        m->dev = dev;

        m->io_count = 0;
        m->err_count = 0;

        m->to_string = 0;
        m->from_string = 0;
    }

    dev->minor_count = n_minor;

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






static char *make_subdev_name( const char *name, int minor )
{
    size_t len = strlen(name) + 1;

    len++;

    if( minor > 9 ) len++;
    if( minor > 99 ) minor = -1; // too much

    char *ret = malloc( len );

    snprintf( ret, len, "%s%d", name, minor );

    return ret;
}


uint8_t dev_count_devices( dev_major *devices[], uint8_t n_major )
{
    uint8_t i;
    dev_major *	dev;
    uint8_t ret = 0;

    for( i = 0; i < n_major; i++ )
    {
        dev = devices[i];

        if( (0 == dev->init) || (0 == dev->start) )
            continue;

        if( !dev->started )
            continue;

        ret += dev->minor_count;
    }

    return ret;
}



int8_t
dev_uint16_to_string( struct dev_minor *sub, char *out, uint8_t out_size, uint16_t val )
{
    snprintf( out, out_size, "%d", val );
    return 0;
}





