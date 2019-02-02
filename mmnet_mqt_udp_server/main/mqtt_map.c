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


#define DEBUG_NET 0

#define RCONF 1


// -----------------------------------------------------------------------
//
// Use remote config item list (rconfig.client.c) as
// channel number / topic name map. Item index for
// topic is a local channel number
//
//
// -----------------------------------------------------------------------



#if !RCONF
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

#endif


// -----------------------------------------------------------------------
//
// Local event to network
//
// -----------------------------------------------------------------------


void mqtt_udp_send_channel( uint8_t state, uint8_t ch )
{
    if( !network_started ) return;

#if RCONF
    const char *topic = rconfig_get_string_by_item_index( ch, MQ_CFG_KIND_TOPIC );
    if( topic == 0 )
    {
#if DEBUG_NET
        printf("no ch %d\n", ch);
#endif
        return;
    }
#else
    int8_t pos = pos_by_id( ch );

    if( pos < 0 )
    {
        printf("no ch %d\n", ch);
        return;
    }

    struct mqtt_io *mp = mqm+pos;
    const char *topic = mp->mqtt_name;
#endif

    //char data[20];
    //sprintf( data, "%d", state );
    char *data = state ? "ON" : "OFF";

    char buf[80];

    strlcpy( buf, ee_cfg.topic_prefix, sizeof(buf) );
    //strlcat( buf, "/", sizeof(buf) );
    strlcat( buf, topic, sizeof(buf) );

    //printf( "channel %d send %d item %s\n", ch, state, buf );

    mqtt_io_count++;

#if DEBUG_NET
    printf("publish '%s'='%s'\n", buf, data );
#endif

    int rc = mqtt_udp_send_publish( buf, data );
    if( rc )
        printf("publish err=%d\n", rc);
}


// -----------------------------------------------------------------------
//
// Incoming network message
//
// -----------------------------------------------------------------------


/// Called from MQTT receiver @todo use .opaque
void mqtt_udp_recv_item( const char *mqtt_name, const char *data )
{
    int plen = strlen( ee_cfg.topic_prefix );

    //printf("." );
    //printf("incoming item '%s'\n", mqtt_name );

    if( plen && strncmp( mqtt_name, ee_cfg.topic_prefix, plen ) )
    {
        //printf("No prefix (%s) on incoming item (%s)", ee_cfg.topic_prefix, mqtt_name );
        return;
    }

    mqtt_name += plen;

    while( *mqtt_name == '/' )
        mqtt_name++;

#if RCONF
    int8_t pos = rconfig_find_by_string_value( mqtt_name, MQ_CFG_KIND_TOPIC );
#else
    int8_t pos = pos_by_name( mqtt_name );
#endif
    if( pos < 0 )
    {
        //printf("no item %s\n", mqtt_name );
        return;
    }

#if DEBUG_NET
    printf("our item '%s'\n", mqtt_name );
#endif

    int state = 0;

    if( isdigit( *data ) )
        state = atoi( data );
    else
        state = !strcasecmp( data, "ON" );


#if RCONF
    uint8_t ch = pos;
#else
    struct mqtt_io *mp = mqm+pos;
    uint8_t ch = mp->local_id;
#endif

    // Now put to local data structure

    if( ch >= (8*sizeof(dio_remote_state)) )
    {
        //printf("ch %d >= 8", ch);
        return;
    }

    uint32_t mask = 1 << ch;

    dio_remote_state_changed |= mask;
    dio_remote_state &= ~mask;

    if( state )
        dio_remote_state |= mask;
}




