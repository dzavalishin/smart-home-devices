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
//        m->name = name;
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

    // TODO
    //snprintf( ret, len, "%s%d", name, minor );
    // snprintf doesnt work

    sprintf( ret, "%s%d", name, minor );

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
    //snprintf( out, out_size, "%d", val );

    // TODO
    // snprintf doesnt work

    if( out_size < 5 ) return -1;

    sprintf( out, "%d", val );

    return 0;
}


void
dev_init_subdev_getset( dev_major *dev, minor_from_string_f from, minor_to_string_f to )
{
    uint8_t i;

    for( i = 0; i < dev->minor_count; i++ )
    {
        dev_minor *m = dev->subdev + i;

        m->to_string = to;
        m->from_string = from;
    }
}



// -----------------------------------------------------------------------
// Global from/to string (data read/write)
// -----------------------------------------------------------------------

dev_minor *  	dev_get_minor_by_name( const char *name )
{
    uint8_t i,j;
    dev_major *	dev;
    dev_minor * ret = 0;

    for( i = 0; i < n_major_total; i++ )
    {
        dev = devices[i];

        if( (0 == dev->init) || (0 == dev->start) )
            continue;

        if( !dev->started )
            continue;

        for( j = 0; j < dev->minor_count; j++ )
        {
            char *subname = dev->subdev[j].name;
            if( (0 == subname) || strcmp( name, subname ) )
                continue;
            return dev->subdev+j;
        }
        
    }

    return ret;

}


int8_t
dev_global_from_string( const char *name, const char *value )          // 0 - success
{
    dev_minor *m = dev_get_minor_by_name( name );

    if( m == 0 ) return -1;

    if( 0 == m->from_string ) return -1;

    return m->from_string( m, value );
}

int8_t
dev_global_to_string( const char *name, char *out, uint8_t out_size )	// 0 - success
{
    dev_minor *m = dev_get_minor_by_name( name );

    if( m == 0 ) return -1;

    if( 0 == m->to_string ) return -1;

    return m->to_string( m, out, out_size );
}


