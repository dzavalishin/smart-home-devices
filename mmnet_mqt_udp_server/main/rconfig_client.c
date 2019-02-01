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




// Will be parameter of mqtt_udp_rconfig_client_init()
mqtt_udp_rconfig_item_t rconfig_list[] =
{
    { MQ_CFG_TYPE_STRING, "Channel 1 topic", "Ch1Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 2 topic", "Ch2Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 3 topic", "Ch3Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Channel 4 topic", "Ch4Topic", { .s = 0 } },
};


// Will be parameter of mqtt_udp_rconfig_client_init()
int rconfig_list_size = sizeof(rconfig_list) / sizeof(mqtt_udp_rconfig_item_t);


// TODO kill me
int rconfig_rw_callback( int pos, int write );



//static const char *topic_prefix = "$SYS/%s/conf/"; // put MAC address in %s
static char topic_prefix[24];
static int topic_prefix_len = 0;

static int rconfig_listener( struct mqtt_udp_pkt *pkt );

static void rconfig_send_topic_list( void );
static void rconfig_send_topic_by_pos( int pos );

static int rconfig_find_by_topic( const char *topic );
static void rconfig_read_all( void );

static int find_by_full_topic( const char *topic );


static char *rconfig_mac_address_string = 0;


//static int (*user_rw_callback)( int pos, int write );
mqtt_udp_rconfig_rw_callback user_rw_callback;



void mqtt_udp_rconfig_client_init(char *mac_address_string, mqtt_udp_rconfig_rw_callback cb )
{
    printf( "RConfig client init with mac '%s'\n", mac_address_string );

    //user_rw_callback = rconfig_rw_callback; // will be parameter
    user_rw_callback = cb;

    rconfig_mac_address_string = mac_address_string;

    sprintf( topic_prefix, "$SYS/%s/conf/", rconfig_mac_address_string );
    topic_prefix_len = strlen( topic_prefix );


    mqtt_udp_add_packet_listener( rconfig_listener );

    /*
    int rc = mqtt_udp_rconfig_set_string( 0, "Ch1" );
    if( rc ) printf("cant ch1 %d\n", rc );

    mqtt_udp_rconfig_set_string( 1, "Ch2" );
    mqtt_udp_rconfig_set_string( 2, "Ch3" );
    mqtt_udp_rconfig_set_string( 3, "Ch4" );
    */
    rconfig_read_all();
    rconfig_send_topic_list();
}




int mqtt_udp_rconfig_set_string( int pos, char *string )
{
    if( (pos < 0) || (pos >= rconfig_list_size) ) return -1; // TODO error

    if( rconfig_list[pos].type != MQ_CFG_TYPE_STRING ) return -2; // TODO error

    int slen = strnlen( string, PKT_BUF_SIZE );

    mqtt_udp_rconfig_item_t *item = rconfig_list + pos;

    //if( 0 == item->value.s ) return -3;

    if( item->value.s ) free( item->value.s );
    item->value.s = 0;

    item->value.s = malloc( slen );

    if( 0 == item->value.s ) return -4;

    strcpy( item->value.s, string );
    return 0;
}


#define SYS_WILD "$SYS/#"

static int rconfig_listener( struct mqtt_udp_pkt *pkt )
{
    //printf("rconf\n");

    // Got request
    if( pkt->ptype == PTYPE_SUBSCRIBE )
    {
        if( 0 == strcmp( pkt->topic, SYS_WILD ) ) { rconfig_send_topic_list(); return 0; }

        int pos = find_by_full_topic( pkt->topic );
        if( pos < 0 ) return 0;
        printf("rconf got subscribe '%s' pos = %d\n", pkt->topic, pos );

        rconfig_send_topic_by_pos( pos );
    }

    // Got data
    if( pkt->ptype == PTYPE_PUBLISH )
    {
        int pos = find_by_full_topic( pkt->topic );
        if( pos < 0 ) return 0;
        printf("rconf set '%s'='%s' pos = %d\n", pkt->topic, pkt->value, pos );

        int rc = mqtt_udp_rconfig_set_string( pos, pkt->value );

        rc = user_rw_callback( pos, 1 ); // Ask user to write item to local storage and use it

    }

    return 0;
}


static int find_by_full_topic( const char *topic )
{
    //printf("topic  '%s'\n", topic );
    //printf("prefix '%s'\n", topic_prefix );

    if( strncmp( topic_prefix, topic, topic_prefix_len ) ) return -1;

    const char *suffix = topic + topic_prefix_len;

    return rconfig_find_by_topic( suffix );
}






static void rconfig_send_topic_by_pos( int pos )
{

    printf("rconfig_send_topic_list %d: ", pos );

    // TODO do more
    if( rconfig_list[pos].type != MQ_CFG_TYPE_STRING )
        return;

    const char *subtopic = rconfig_list[pos].topic;

    char topic[80];

    //snprintf( topic, sizeof(topic)-1, "$SYS/%s/conf/%s", rconfig_mac_address_string, subtopic );
    sprintf( topic, "$SYS/%s/conf/%s", rconfig_mac_address_string, subtopic );

    char *val = rconfig_list[pos].value.s;

    if( val == 0 ) val = "";

    mqtt_udp_send_publish( topic, val );
    printf("'%s'='%s'\n", topic, val );
}



static void rconfig_send_topic_list( void )
{
    int i;
    for( i = 0; i < rconfig_list_size; i++ )
        rconfig_send_topic_by_pos( i );
}








// return -1 if not found
static int rconfig_find_by_topic( const char *topic )
{
    int i;
    for( i = 0; i < rconfig_list_size; i++ )
    {
        if( 0 == strcmp( rconfig_list[i].topic, topic ) )
            return i;
    }

    return -1;
}



static void rconfig_read_all( void )
{
    int i;
    for( i = 0; i < rconfig_list_size; i++ )
    {
        user_rw_callback( i, 0 ); // Ask user to read item from local storage
    }
}



// -----------------------------------------------------------------------
//
// Callback to connect to host code
//
// -----------------------------------------------------------------------


// Must read from local storage or write to local storage
// config item at pos
int rconfig_rw_callback( int pos, int write )
{
    printf("asked to %s item %d\n", write ? "save" : "load", pos );


    if( write )
    {
        return 0;
    }
    else
    {
        char *name = "Ch0";
        name[2] = '0' + pos;
        mqtt_udp_rconfig_set_string( pos, name );
        return 0;
    }
}



