/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT/UDP glue code
 *
**/



#include "mqtt_udp_glue.h"
#include <sys/timer.h>


void mqtt_udp_start( void )
{
    //mqtt_udp_set_throttle( 0 ); // We have no
}


// Send channel status upstream
void mqtt_udp_send_channel( uint8_t state, uint8_t ch )
{
}




uint64_t mqtt_udp_arch_get_time_msec()
{
    return NutGetMillis();
}

void  mqtt_udp_arch_sleep_msec( uint32_t msec )
{
    NutSleep( msec );
}





