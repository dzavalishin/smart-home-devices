/**
 *
 * MQTT/UDP project
 *
 * https://github.com/dzavalishin/mqtt_udp
 * 
 * Copyright (C) 2017-2019 Dmitry Zavalishin, dz@dz.ru
 *
 *
 * Remote configuratio client code.
 *
 * This is a 'passive' remote config scenario: IoT device
 * keeps configuration in a local storage, but configuration
 * can be modified remotely via MQTT/UDP requests.
 *
**/



#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mqtt_udp.h"


typedef enum {
    MQ_CFG_TYPE_BOOL,
    MQ_CFG_TYPE_STRING,
    MQ_CFG_TYPE_INT32,
} mqtt_udp_config_item_type_t;

typedef union {
    int32_t     b;
    char **     s; // will reallocate, must be malloc'ed
    int32_t     i;
    void *      o; // other
} mqtt_udp_config_item_value_t;

typedef struct {
    mqtt_udp_config_item_type_t         type;
    const char *                        name;
    const char *                        topic; // MQTT/UDP topic name for this config parameter
    mqtt_udp_config_item_value_t        value; // Current or default value

} mqtt_udp_config_item_t;


// Will be parameter of mqtt_udp_rconfig_client_init()
mqtt_udp_config_item_t rconfig_list[] =
{
    { MQ_CFG_TYPE_STRING, "Channel 1 topic", "Ch1Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 2 topic", "Ch2Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 3 topic", "Ch3Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 4 topic", "Ch4Topic", { .s = 0 } },
};


// Will be parameter of mqtt_udp_rconfig_client_init()
int rconfig_list_size = sizeof(rconfig_list) / sizeof(mqtt_udp_config_item_t);




//static const char *topic_prefix = "$SYS/%s/conf/"; // put MAC address in %s


static int rconfig_listener( struct mqtt_udp_pkt *pkt );
static void rconfig_send_topic_list( void );

static char *rconfig_mac_address_string = 0;

void mqtt_udp_rconfig_client_init(char *mac_address_string)
{
    printf( "RConfig client init with mac '%s'\n", mac_address_string );

    rconfig_mac_address_string = mac_address_string;

    //mqtt_udp_add_packet_listener( rconfig_listener );

    mqtt_udp_rconfig_set_string( 0, "Ch1" );
    mqtt_udp_rconfig_set_string( 0, "Ch2" );
    mqtt_udp_rconfig_set_string( 0, "Ch3" );
    mqtt_udp_rconfig_set_string( 0, "Ch4" );

    rconfig_send_topic_list();
}


int mqtt_udp_rconfig_set_string( int pos, char *string )
{
    if( (pos < 0) || (pos >= rconfig_list_size) ) return -1; // TODO error

    if( rconfig_list[pos].type != MQ_CFG_TYPE_STRING ) return -1; // TODO error

    int slen = strnlen( string, PKT_BUF_SIZE );

    mqtt_udp_config_item_t *item = rconfig_list + pos;

    if( 0 == item->value.s ) return -1;

    if( *item->value.s ) free( *item->value.s );
    *item->value.s = 0;

    *item->value.s = malloc( slen );

    if( 0 == *item->value.s ) return -1;

    strcpy( *item->value.s, string );
    return 0;
}


#define SYS_WILD "$SYS/#"

static int rconfig_listener( struct mqtt_udp_pkt *pkt )
{
    // Got request
    if( pkt->ptype == PTYPE_SUBSCRIBE )
    {
        if( 0 == strcmp( pkt->topic, SYS_WILD ) ) { rconfig_send_topic_list(); return 0; }
    }

    // Got data
    if( pkt->ptype == PTYPE_PUBLISH )
    {
    }

    return 0;
}







static void rconfig_send_topic_list( void )
{
    int i;
    for( i = 0; i < rconfig_list_size; i++ )
    {
        printf("rconfig_send_topic_list %d: ", i );
        // TODO do more
        if( rconfig_list[i].type != MQ_CFG_TYPE_STRING )
            continue;

        const char *subtopic = rconfig_list[i].topic;

        char topic[80];

        //snprintf( topic, sizeof(topic)-1, "$SYS/%s/conf/%s", rconfig_mac_address_string, subtopic );
        sprintf( topic, "$SYS/%s/conf/%s", rconfig_mac_address_string, subtopic );

        char *val = 0;

        if( rconfig_list[i].value.s != 0 )
            val = *rconfig_list[i].value.s;

        if( val == 0 ) val = "";

        mqtt_udp_send_publish( topic, val );
        printf("'%s'='%s'\n", topic, val );
    }

}
















