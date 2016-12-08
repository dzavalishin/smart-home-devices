/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT to local ids mapping.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "io_dig.h"

#include "mqtt.h"

#include <inttypes.h>
#include <string.h>



struct mqtt_io
{
    const char *mqtt_name;      // MQTT item name
    uint8_t	local_id;       // Our integer ID
};


struct mqtt_io  mqm[] =
{
    { "Light5A",        0 },
    { "Light5B",        1 },
    { "Light1A",        2 },
    { "Light1B",        3 },
};


#define N_MQM ( sizeof(mqm) / sizeof(struct mqtt_io) )

static int8_t pos_by_id( uint8_t id )
{
    uint8_t i;
    for( i = 0; i < N_MQM; i++ )
    {
        if( mqm[i].local_id == id ) return i;
    }

    return -1;
}

static int8_t pos_by_name( const char *name )
{
    uint8_t i;
    for( i = 0; i < N_MQM; i++ )
    {
        if( strcmp( mqm[i].mqtt_name, name) == 0 ) return i;
    }

    return -1;
}





void mqtt_send_channel( uint8_t state, uint8_t ch )
{
    int8_t pos = pos_by_id( ch );

    if( pos < 0 )
    {
        printf("no ch %d", ch);
        return;
    }

    struct mqtt_io *mp = mqm+pos;

    char data[20];
    sprintf( data, "%d", state );

    char buf[80];

    strlcpy( buf, ee_cfg.topic_prefix, sizeof(buf) );
    strlcat( buf, "/", sizeof(buf) );
    strlcat( buf, mp->mqtt_name, sizeof(buf) );

    mqtt_send_item( buf, data );
}


// Called from MQTT receiver
void mqtt_recv_item( const char *mqtt_name, const char *data )
{
    int plen = strlen( ee_cfg.topic_prefix );

    if( strncmp( mqtt_name, ee_cfg.topic_prefix, plen ) )
    {
        printf("No prefix (%s) on incoming item (%s)", ee_cfg.topic_prefix, mqtt_name );
        return;
    }

    mqtt_name += plen;

    while( *mqtt_name == '/' )
        mqtt_name++;

    int8_t pos = pos_by_name( mqtt_name );

    if( pos < 0 )
    {
        printf("no item %s", mqtt_name );
        return;
    }

    struct mqtt_io *mp = mqm+pos;

    int state = atoi( data );
    uint8_t ch = mp->local_id;

    // Now put to local data structure

    if( ch >= 8 )
    {
        printf("ch %d >= 8", ch);
        return;
    }

    uint8_t mask = 1 << ch;

    dio_remote_state_changed |= mask;
    dio_remote_state &= ~mask;

    if( state )
        dio_remote_state |= mask;
}


// -----------------------------------------------------------------------
// Subscription
// -----------------------------------------------------------------------




uint8_t subscribe_all( void )
{
    uint8_t i;
    for( i = 0; i < N_MQM; i++ )
    {
        char buf[80];

        strlcpy( buf, ee_cfg.topic_prefix, sizeof(buf) );
        strlcat( buf, "/", sizeof(buf) );
        strlcat( buf, mqm[i].mqtt_name, sizeof(buf) );

        subscribe( buf );
    }

    return 0;
}


