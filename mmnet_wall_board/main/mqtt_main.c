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
#include "mqtt.h"

#include <inttypes.h>
#include <string.h>

#include "libemqtt.h"

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/mutex.h>

#include <netdb.h>
#include <arpa/inet.h>


#define MQTT_READ_TIMEOUT 60


THREAD(mqtt_recv, __arg);

static MUTEX mqttSend;

uint8_t  mqtt_keepalive_timer = 0;

static int need_restart = 0;

#define mqtt_debug 1

static mqtt_broker_handle_t broker;


static int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port);


#warning hardcode IP
//static char *mqtt_host = "192.168.1.141";
static char *mqtt_host = "smart.";
static int mqtt_port = 1883;








void mqtt_start( void )
{
    printf("mqtt_init\n");

    NutMutexInit( &mqttSend );


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
    printf("mqtt_init ok\n");
    mqtt_init_auth(&broker, DEVICE_NAME, "test-test");
    printf("mqtt_init auth\n");

    //init_socket(&broker, mqtt_host, mqtt_port );

    NutThreadCreate("MqttRecv", mqtt_recv, 0, 1024);
    printf("mqtt_init thread started\n");
}



// -----------------------------------------------------------------------
// MQTT to TCP glue
// -----------------------------------------------------------------------

static TCPSOCKET *mqtt_sock;

#define RCVBUFSIZE 1024
static uint8_t packet_buffer[RCVBUFSIZE];

int send_packet(void* socket_info, const void* buf, unsigned int count)
{
    //int fd = *((int*)socket_info);
    //return send(fd, buf, count, 0);
    return NutTcpSend( mqtt_sock, buf, count );
}



static int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
    //int flag = 1;
    int keepalive = 3; // Seconds

#if 0
    uint32_t server_ip = NutDnsGetHostByName( (const unsigned char*)hostname );
    if( server_ip == 0 )
    {
        if(mqtt_debug) printf("[%s] Host not found\n", hostname );
        return -1;
    }
#else
    uint32_t server_ip = inet_addr( "192.168.1.141" );
#endif

    printf("mqtt_init gethost=%s\n", inet_ntoa(server_ip));

    if ((mqtt_sock = NutTcpCreateSocket()) == 0)
    {
        if(mqtt_debug) printf( "Creating socket failed\n" );
        return -1;
    }

printf("mqtt_init sock=%p\n", mqtt_sock );
#warning sock opt
    /*
     // Disable Nagle Algorithm
     if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
     return -2;
     */
    uint32_t to = 20; // sec timeout on send
    int rc = NutTcpSetSockOpt( mqtt_sock, SO_SNDTIMEO, &to, sizeof(to) );
    if( rc ) return -1;


    rc = NutTcpSetSockOpt( mqtt_sock, SO_RCVTIMEO, &to, sizeof(to) );

printf("mqtt_init sock opt, connecting TCP socket\n" );

    rc = NutTcpConnect( mqtt_sock, server_ip, port);
    if(rc < 0)
    {
        printf("mqtt_init error NutTcpConnect  = %d\n", rc );
        return -1;
    }

printf("mqtt_init sock connect\n" );

    // MQTT stuffs
    mqtt_set_alive(broker, keepalive);
    broker->socket_info = (void*)&mqtt_sock;
    broker->send = send_packet;

    return 0;
}




static int read_packet(int timeout)
{
    uint32_t to = timeout;

    int rc = NutTcpSetSockOpt( mqtt_sock, SO_RCVTIMEO, &to, sizeof(to) );
    if( rc ) return -1;

    int total_bytes = 0, bytes_rcvd, packet_length;

    memset(packet_buffer, 0, sizeof(packet_buffer));

    while(total_bytes < 2) // Reading fixed header
    {
         bytes_rcvd = NutTcpReceive(  mqtt_sock, packet_buffer+total_bytes, RCVBUFSIZE-total_bytes );

         if( bytes_rcvd <= 0)
         {
             printf( " re %d ", bytes_rcvd );
             return bytes_rcvd;
         }

        total_bytes += bytes_rcvd; // Keep tally of total bytes
    }

    // TODO var length
    packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length

    while(total_bytes < packet_length) // Reading the packet
    {
         bytes_rcvd = NutTcpReceive(  mqtt_sock, packet_buffer+total_bytes, RCVBUFSIZE-total_bytes );

         if( bytes_rcvd <= 0)
             return -1;

        total_bytes += bytes_rcvd; // Keep tally of total bytes
    }

    return packet_length;
}



uint8_t expect_packet( uint8_t type )
{
    int len = read_packet(MQTT_READ_TIMEOUT);

    if( len < 0)
    {
        if(mqtt_debug) printf( "MQTT: Error(%d) on read packet!\n", len);
        need_restart = 1;
        return -1;
    }

    uint16_t rtype = MQTTParseMessageType( packet_buffer );
    if( rtype == type )
        return 0;

    return -1;
}


static uint8_t subscribe( const char *topic )
{
    uint16_t msg_id, msg_id_rcv;

    mqtt_subscribe(&broker, topic, &msg_id);

    uint8_t rc = expect_packet( MQTT_MSG_SUBACK );
    if(rc)
    {
        if(mqtt_debug) printf( "MQTT sub: got no reply %d\n", rc  );
        return -1;
    }

    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);

    if(msg_id != msg_id_rcv)
    {
        if(mqtt_debug) printf( "MQTT sub: %d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv);
        need_restart = 1;
        return -3;
    }

    printf("subscribe %s done\n", topic );

    return 0;
}

static uint8_t publish( const char *mqtt_name, const char *data )
{
    uint16_t msg_id, msg_id_rcv;
    // >>>>> PUBLISH QoS 1

    mqtt_publish_with_qos(&broker, mqtt_name, data, 0, 1, &msg_id);

    // <<<<< PUBACK

    uint8_t rc = expect_packet( MQTT_MSG_PUBACK );
    if(rc)
    {
        if(mqtt_debug) printf( "MQTT sub: got no reply %d\n", rc  );
        return -1;
    }

    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
    if(msg_id != msg_id_rcv)
    {
        if(mqtt_debug) printf( "MQTT pub %d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv );
        need_restart = 1;
        return -3;
    }

    return 0;
}


static volatile int mqtt_send_flag = 0;
static volatile const char *mqtt_send_mqtt_name;
static volatile const char *mqtt_send_data;


THREAD(mqtt_recv, __arg)
{
    (void) __arg;
    int rc;
    int read_err_cnt = 0;

    need_restart = 1;

    while(1) // to satisfy no return
    {
        NutSleep( 1 ); // Protect from DOS attack :)

        // TODO reconnect
        if( need_restart )
        {
            NutSleep( 1000 );
printf("mqtt_init thread close socket\n");

            if( mqtt_sock != 0 )
            {
                rc = NutTcpCloseSocket( mqtt_sock );
                if( rc < 0 )
                {
                    if(mqtt_debug) printf( "MQTT: Error(%d) on close!\n", rc );
                }
                mqtt_sock = 0;
            }
printf("mqtt_init thread init socket\n");
            rc = init_socket( &broker, mqtt_host, mqtt_port );
            if( rc < 0)
            {
                if(mqtt_debug) printf( "MQTT: Error(%d) on reconnect!\n", rc );
                continue;
            }

            if(mqtt_debug) printf("Connected socket, proto connect\n" );

            mqtt_connect(&broker);
            rc = expect_packet( MQTT_MSG_CONNACK );
            if( rc < 0)
            {
                printf( "MQTT: Connect failure (%d) on reconnect!\n", rc );
                continue;
            }

            if(mqtt_debug) printf("Got reply\n" );

            if(packet_buffer[3] != 0x00)
            {
                if(mqtt_debug) printf("CONNACK failed: %d!\n", packet_buffer[3] );
                continue;
            }

            printf("Connected, subscribe\n" );

#warning todo subscribe list
            if( subscribe( "/aa" ) ) continue;

            need_restart = 0;
        }


        // Process send
        NutMutexLock( &mqttSend );
        if( mqtt_send_flag )
        {
            publish( (const char *)mqtt_send_mqtt_name, (const char *)mqtt_send_data );
            mqtt_send_flag = 0;
        }
        NutMutexUnlock( &mqttSend );
        if( need_restart ) continue;


        if( mqtt_keepalive_timer-- <= 0 )
        {
            mqtt_keepalive_timer = 2;
            mqtt_ping( &broker );
        }

        // Process recv

        int len = read_packet(MQTT_READ_TIMEOUT);

        if( len <= 0 )
	{
            read_err_cnt++;

            if( len == 0 ) // seems like no data recvd/timeout
                NutSleep(50);

            if( (read_err_cnt > 1000) || (len < 0) )
            {
                printf( "MQTT: too many read errors (%d), reconnect\n", read_err_cnt );
                need_restart = 1;
                read_err_cnt = 0;
            }
            continue;
        }

        uint16_t type = MQTTParseMessageType( packet_buffer );
        printf( "MQTT: got pkt type=%d\n", type );

        if( type == MQTT_MSG_PUBLISH)
        {
#warning overrun?
            uint8_t topic[255], msg[1000];
            uint16_t len;

            len = mqtt_parse_pub_topic(packet_buffer, topic);
            topic[len] = '\0'; // for printf

            len = mqtt_parse_publish_msg(packet_buffer, msg);
            msg[len] = '\0'; // for printf

            if(mqtt_debug) printf("%s %s\n", topic, msg);


            mqtt_recv_item( (const char *)topic, (const char *)msg );

        }
    }
}



// Done in main mqtt thread
void mqtt_send_item( const char *mqtt_name, const char *data )
{
    while(1)
    {
        NutMutexLock( &mqttSend );
        if( !mqtt_send_flag )
        {
            mqtt_send_flag = 1;
            mqtt_send_mqtt_name = mqtt_name;
            mqtt_send_data = data;
            NutMutexUnlock( &mqttSend );
            break;
        }
        NutMutexUnlock( &mqttSend );
    }

    // Other thread uses our args, wait for it to finish using

    while( mqtt_send_flag )
    {
        NutSleep(2);
    }

}



