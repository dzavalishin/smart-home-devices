/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT/UDP glue header.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
//#include "map.h"

#include <inttypes.h>

#include "mqtt_udp.h"



void mqtt_udp_start( void );


// Send channel status upstream
void mqtt_udp_send_channel( uint8_t state, uint8_t ch );

// Called from MQTT/UDP recv loop
void mqtt_udp_recv_item( const char *mqtt_name, const char *data );


// Called from remote config code
int rconfig_rw_callback( int pos, int write );



extern uint8_t  mqtt_keepalive_timer; // updated from each_second()
extern uint32_t  mqtt_io_count;




