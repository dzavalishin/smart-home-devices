/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT glue header.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"

#include "map.h"

#include <inttypes.h>

#include "libemqtt.h"

extern dev_major io_mqtt;


// Send channel status upstream
void mqtt_send_channel( uint8_t state, uint8_t ch );

// Called from MQTT receiver
void mqtt_recv_item( const char *mqtt_name, const char *data );


