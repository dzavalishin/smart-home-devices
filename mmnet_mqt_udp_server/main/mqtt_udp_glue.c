/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * MQTT/UDP glue code
 *
**/



#include "mqtt_udp_glue.h"
#include "mqtt_udp.h"
#include "mqtt_udp_defs.h"

#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/event.h>

#include <string.h>
#include <stdio.h>

#if ENABLE_MQTT_UDP


#define DEBUG 0




int mqtt_udp_incoming_pkt( struct mqtt_udp_pkt *o )
{
    if( o->ptype != PTYPE_PUBLISH ) return;

    printf( "got pkt from %d.%d.%d.%d",
            //o->pflags, o->pkt_id,
            (int)(0xFF & (o->from_ip >> 24)),
            (int)(0xFF & (o->from_ip >> 16)),
            (int)(0xFF & (o->from_ip >> 8)),
            (int)(0xFF & (o->from_ip))
          );

    if( o->topic_len > 0 )
        printf(" topic '%s'", o->topic );

    if( o->value_len > 0 )
        printf(" = '%s'", o->value );

    printf( "\n");
    return 0;
}



THREAD( mqtt_udp_recv_thread, __arg )
{
    printf("Listen to MQTT/UDP\n");

    while(1)
    {
        int rc = mqtt_udp_recv_loop( mqtt_udp_incoming_pkt );
        if( rc )
            mqtt_udp_global_error_handler( MQ_Err_Other, rc, "recv_loop error", 0 );
    }
}


void mqtt_udp_start( void )
{
    mqtt_udp_set_throttle( 0 ); // Turn off for a while
    NutThreadCreate("MQTT/UDP Recv", mqtt_udp_recv_thread, 0, 3640);
}


// Send channel status upstream
void mqtt_udp_send_channel( uint8_t state, uint8_t ch )
{
    char topic[] = "mmnet/01/dio0";
    //topic[12] = ch + '0';

    char val[4+10];
    //snprintf( val, sizeof(val) - 1, "%d", state );
    sprintf( val, "%d", state );

    printf("publish '%s'='%s'\n", topic, val );
    int rc = mqtt_udp_send_publish( topic, val );
    if( rc )        printf("publish err=%d", rc);
}



// -----------------------------------------------------------------------
//
// Time
//
// -----------------------------------------------------------------------



uint64_t mqtt_udp_arch_get_time_msec()
{
    return NutGetMillis();
}

void  mqtt_udp_arch_sleep_msec( uint32_t msec )
{
    NutSleep( msec );
}


// -----------------------------------------------------------------------
//
// UDP
//
// -----------------------------------------------------------------------







int mqtt_udp_socket(void)
{
#if DEBUG
    printf("MQTT/UDP mk socket \n");
#endif

    UDPSOCKET *fd = NutUdpCreateSocket( MQTT_PORT );


    if( fd == 0 )
    {
        mqtt_udp_global_error_handler( MQ_Err_Establish, (int)fd, "socket creation error", "" );
        return 0;
    }

    /*
    {
        int broadcast=1;
        setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    }

    {
        int enable = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

        //if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
       	//    error("setsockopt(SO_REUSEADDR) failed");
    }
    */

    uint16_t sz = PKT_BUF_SIZE;
    NutUdpSetSockOpt( fd, SO_RCVBUF, &sz, sizeof(uint16_t) );

    return (int) fd;
}


int mqtt_udp_bind( int fd )
{
#if DEBUG
    printf("MQTT/UDP bind \n");
#endif
    return 0;
}



int mqtt_udp_close_fd( int fd ) 
{
    return NutUdpDestroySocket( (UDPSOCKET *) fd );
}






int mqtt_udp_recv_pkt( int fd, char *buf, size_t buflen, int *src_ip_addr )
{

#if DEBUG
    printf("MQTT/UDP recv pkt ");
#endif

    memset( buf, 0, buflen );

    uint32_t netorder_src_addr;
    uint16_t netorder_src_port;

    int rc = NutUdpReceiveFrom( (UDPSOCKET *) fd, &netorder_src_addr, &netorder_src_port, buf, buflen, NUT_WAIT_INFINITE );

    if( src_ip_addr ) *src_ip_addr = ntohl( netorder_src_addr );

#if DEBUG
    printf(" recv pkt ok\n");
#endif

    return rc;
}





int mqtt_udp_send_pkt( int fd, char *data, size_t len )
{
#if DEBUG
    printf("MQTT/UDP broadcast pkt\n");
#endif

    int rc = NutUdpSendTo( (UDPSOCKET *) fd, 0xFFFFFFFF, htons( MQTT_PORT ), data, len );

#if DEBUG
    printf("MQTT/UDP broadcast rc = %d  len = %d\n", rc, len );
#endif

    return rc ? -5 : 0; // -5 = EIO
}


int mqtt_udp_send_pkt_addr( int fd, char *data, size_t len, int ip_addr )
{
#if DEBUG
    printf("MQTT/UDP sent pkt to\n");
#endif

    int rc = NutUdpSendTo( (UDPSOCKET *) fd, htonl( ip_addr ), htons( MQTT_PORT ), data, len );

    return rc ? -5 : 0; // -5 = EIO
}



#endif // ENABLE_MQTT_UDP

