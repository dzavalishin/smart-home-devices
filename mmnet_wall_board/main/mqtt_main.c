/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT glue code.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "io_dig.h"

#include <inttypes.h>
#include <string.h>

#include "libemqtt.h"

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>





#define mqtt_debug 1

static mqtt_broker_handle_t broker;


static int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port);



void mqtt_send_item( const char *mqtt_name, const char *data )
{

}






static void mqtt_init_dev( dev_major* d )
{
    (void) d;
    printf("mqtt_init_dev\n");
    //timer1_init();

    char clientName[64];

    sprintf( clientName, "%s/%02x.%02x.%02x.%02x.%02x.%02x", DEVICE_NAME,
             ee_cfg.mac_addr[0],
             ee_cfg.mac_addr[1],
             ee_cfg.mac_addr[2],
             ee_cfg.mac_addr[3],
             ee_cfg.mac_addr[4],
             ee_cfg.mac_addr[5]
           );

    mqtt_init(&broker, clientName );
    mqtt_init_auth(&broker, DEVICE_NAME, "test-test");
#warning hardcode IP
    init_socket(&broker, "192.168.1.141", 1883);

}
static void mqtt_start_dev( dev_major* d )
{
    (void) d;
    printf("mqtt_start_dev\n");
    //timer1_start();
}

//static void mqtt_stop_dev( dev_major* d ) { (void) d;  } // TODO


dev_major io_mqtt =
{
    .name = "mqtt",

    .init = mqtt_init_dev,
    .start = mqtt_start_dev,
    .stop = 0, // TODO
    .timer = 0, //mqtt_timer,

    .to_string = 0,
    .from_string = 0,

    .minor_count = 0,
    .subdev = 0,
};


// -----------------------------------------------------------------------
// MQTT to TCP glue
// -----------------------------------------------------------------------

static TCPSOCKET *mqtt_sock;


int send_packet(void* socket_info, const void* buf, unsigned int count)
{
    //int fd = *((int*)socket_info);
    //return send(fd, buf, count, 0);
    return NutTcpSend( mqtt_sock, buf, count );
}

static int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	int keepalive = 3; // Seconds

        uint32_t server_ip = NutDnsGetHostByName( hostname );
        if( server_ip == 0 )
        {
            if(mqtt_debug) printf("[%s] Host not found\n", hostname );
            return -1;
        }

        if ((mqtt_sock = NutTcpCreateSocket()) == 0)
        {
            if(mqtt_debug) printf( "Creating socket failed\n" );
            return -1;
        }


#warning sock opt
/*
	// Disable Nagle Algorithm
	if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;
*/


        if(NutTcpConnect( mqtt_sock, server_ip, port) < 0)
            return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&mqtt_sock;
	broker->send = send_packet;

	return 0;
}


