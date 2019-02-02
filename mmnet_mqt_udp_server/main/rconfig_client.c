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
#include "runtime_cfg.h"


static int rconfig_rw_callback( int pos, int write );


// -----------------------------------------------------------------------
//
// Use remote config item list (rconfig.client.c) as
// channel number / topic name map. Item index for
// topic is a local channel number
//
// DO NOT MOVE items with MQ_CFG_KIND_TOPIC, their position
// is local channel number.
//
// -----------------------------------------------------------------------


// Will be parameter of mqtt_udp_rconfig_client_init()
mqtt_udp_rconfig_item_t rconfig_list[] =
{
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 1 topic", "topic/sw1", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 2 topic", "topic/sw2", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 3 topic", "topic/sw3", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 4 topic", "topic/sw4", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 4 topic", "topic/di0", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_TOPIC, "Switch 4 topic", "topic/di1", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, MQ_CFG_KIND_OTHER, "Switch 4 topic", "net/mac",   { .s = 0 } },
};


// Will be parameter of mqtt_udp_rconfig_client_init()
int rconfig_list_size = sizeof(rconfig_list) / sizeof(mqtt_udp_rconfig_item_t);



void init_rconfig( void )
{
    char mac_string[12+1];

    unsigned char *mp = ee_cfg.mac_addr;
    int i;
    for( i = 0; i < 6; i++ )
    {
        sprintf( mac_string+(i*2), "%02X", mp[i] );
    }

    mac_string[sizeof(mac_string)-1] = 0;

    int rc = mqtt_udp_rconfig_client_init( mac_string, rconfig_rw_callback, rconfig_list, rconfig_list_size );
    if( rc ) printf("rconfig init failed, %d\n", rc );
}



// -----------------------------------------------------------------------
//
// Topic to index and back
//
// -----------------------------------------------------------------------



/**
 *
 * @brief Find config item number (position in array) by string value.
 *
 * Used to find io channel number by topic name. Remote config item
 * supposed to contain topic name.
 *
 * It is supposed that item index is equal to io channel nubmer.
 * Usually it means that topic related items are at the beginning
 * of item array and their position in array is important.
 *
 * @param in topic Topic name to find in item _value_.
 *
 * @param in kind Expected kind og the item, sanity check.
 *
 * @return Item position in array or -1 if not found.
 *
**/
int rconfig_find_by_string_value( const char *search, mqtt_udp_rconfig_inetm_kind_t kind )
{
    int i;
    for( i = 0; i < rconfig_list_size; i++ )
    {
        if( rconfig_list[i].type != MQ_CFG_TYPE_STRING )
            continue;

        if( rconfig_list[i].kind != kind )
            continue;

        if( 0 == strcmp( rconfig_list[i].value.s, search ) )
            return i;
    }

    return -1;
}

/**
 *
 * @brief Get config item string by item number (position in array).
 *
 * Used to find topic for io channel by channel number.
 * supposed to contain topic name
 *
 * It is supposed that item index is equal to io channel nubmer.
 * Usually it means that topic related items are at the beginning
 * of item array and their position in array is important.
 *
 * @todo Convert other types to string?
 *
 * @param in pos Position in array.
 *
 * @param in kind Expected kind og the item, sanity check.
 *
 * @return Item _value_ (string) or 0 if intem type is not string.
 *
**/
const char * rconfig_get_string_by_item_index( int pos, mqtt_udp_rconfig_inetm_kind_t kind )
{
    if( rconfig_list[pos].type != MQ_CFG_TYPE_STRING )
    {
        mqtt_udp_global_error_handler( MQ_Err_Invalid, 0, "string_by_item_index !str", 0 );
        return 0;
    }

    if( rconfig_list[pos].kind != kind )
    {
        mqtt_udp_global_error_handler( MQ_Err_Invalid, 0, "string_by_item_index !kind", 0 );
        return 0;
    }

    return rconfig_list[pos].value.s;
}




// -----------------------------------------------------------------------
//
// Callback to connect to host code
//
// -----------------------------------------------------------------------


// Must read from local storage or write to local storage
// config item at pos
static int rconfig_rw_callback( int pos, int write )
{
    printf("asked to %s item %d\n", write ? "save" : "load", pos );

    if( (pos < 0) || (pos >= rconfig_list_size) ) return -1; // TODO error

    if( write )
    {
        printf("new val = '%s'\n", rconfig_list[pos].value.s );
        if( pos < EEPROM_CFG_N_TOPICS )
        {
            strlcpy( ee_cfg.topics[pos], rconfig_list[pos].value.s, sizeof( ee_cfg.topics[pos] ) );
            int rc = runtime_cfg_eeprom_write(); // TODO do write from thread with timeout to combine writes
            if( rc ) printf("eeprom wr err %d\n", rc );
        }
        return 0;
    }
    else
    {
#if 1
        if( pos < EEPROM_CFG_N_TOPICS )
            mqtt_udp_rconfig_set_string( pos, ee_cfg.topics[pos] );
#else
        char name[] = "Ch0";
        name[2] = '0' + pos;
        //printf("return '%s'\n", name );
        mqtt_udp_rconfig_set_string( pos, name );
#endif
        return 0;
    }
}



