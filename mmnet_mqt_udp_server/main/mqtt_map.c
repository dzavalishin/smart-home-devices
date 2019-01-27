/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT/UDP to local ids mapping.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_dig.h"

#include "mqtt_udp.h"
#include "mqtt_udp_glue.h"

#include <inttypes.h>
#include <string.h>

#include <ctype.h>


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





void mqtt_udp_send_channel( uint8_t state, uint8_t ch )
{
    if( !network_started ) return;

    int8_t pos = pos_by_id( ch );

    if( pos < 0 )
    {
        printf("no ch %d", ch);
        return;
    }

    struct mqtt_io *mp = mqm+pos;

    //char data[20];
    //sprintf( data, "%d", state );
    char *data = state ? "ON" : "OFF";

    char buf[80];

    strlcpy( buf, ee_cfg.topic_prefix, sizeof(buf) );
    //strlcat( buf, "/", sizeof(buf) );
    strlcat( buf, mp->mqtt_name, sizeof(buf) );

    //printf( "channel %d send %d item %s\n", ch, state, buf );

    mqtt_io_count++;

    //mqtt_send_item( buf, data );
    printf("publish '%s'='%s'\n", buf, data );
    int rc = mqtt_udp_send_publish( buf, data );
    if( rc )        printf("publish err=%d", rc);
}




// Called from MQTT receiver
void mqtt_udp_recv_item( const char *mqtt_name, const char *data )
{
    int plen = strlen( ee_cfg.topic_prefix );

    if( plen && strncmp( mqtt_name, ee_cfg.topic_prefix, plen ) )
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

    int state = 0;

    if( isdigit( *data ) )
        state = atoi( data );
    else
        state = !strcasecmp( data, "ON" );


    uint8_t ch = mp->local_id;

    // Now put to local data structure

    if( ch >= (8*sizeof(dio_remote_state)) )
    {
        printf("ch %d >= 8", ch);
        return;
    }

    uint32_t mask = 1 << ch;

    dio_remote_state_changed |= mask;
    dio_remote_state &= ~mask;

    if( state )
        dio_remote_state |= mask;
}


// -----------------------------------------------------------------------
// Subscription
// -----------------------------------------------------------------------



/*
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

*/

