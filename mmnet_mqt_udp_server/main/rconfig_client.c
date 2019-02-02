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



// Will be parameter of mqtt_udp_rconfig_client_init()
mqtt_udp_rconfig_item_t rconfig_list[] =
{
    { MQ_CFG_TYPE_STRING, "Switch 1 topic", "Sw1Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Switch 2 topic", "Sw2Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Switch 3 topic", "Sw3Topic", { .s = 0 } },
    { MQ_CFG_TYPE_STRING, "Switch 4 topic", "Sw4Topic", { .s = 0 } },
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



