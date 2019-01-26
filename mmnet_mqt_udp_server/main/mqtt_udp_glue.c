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



#if 0
int mqtt_udp_parse_any_pkt( const char *pkt, size_t plen, int from_ip, process_pkt callback )
{
    printf("MQTT/UDP got pkt\n");
}
#endif


THREAD( mqtt_udp_recv_thread, __arg )
{
    printf("Listen to MQTT/UDP\n");

    while(1)
    {
        int rc = mqtt_udp_recv_loop( mqtt_udp_dump_any_pkt );
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
    printf("MQTT/UDP mk socket \n");

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
    printf("MQTT/UDP bind \n");
    /*
    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(srcaddr));

    srcaddr.sin_family = AF_INET;
#ifdef __MINGW32__
    srcaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#else
    srcaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    srcaddr.sin_port = htons(MQTT_PORT);

    return bind(fd, (struct sockaddr *) &srcaddr, sizeof(srcaddr));
/ *
    if (bind(fd, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    */
    return 0;
}



int mqtt_udp_close_fd( int fd ) 
{
    return NutUdpDestroySocket( (UDPSOCKET *) fd );
}






int mqtt_udp_recv_pkt( int fd, char *buf, size_t buflen, int *src_ip_addr )
{

    /*
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = inet_addr(IP);
    //addr.sin_port = htons(MQTT_PORT);

    //int slen = sizeof(addr);
    socklen_t slen = sizeof(addr);

    //memset(buf, 0, sizeof(buf));
    memset( buf, 0, buflen );

    int rc = recvfrom(fd, buf, buflen, 0, (struct sockaddr *) &addr, &slen);

    if( src_ip_addr ) *src_ip_addr = ntohl( addr.sin_addr.s_addr );

    return rc;
    */

    printf("MQTT/UDP recv pkt ");

    memset( buf, 0, buflen );

    uint32_t netorder_src_addr;
    uint16_t netorder_src_port;

    int rc = NutUdpReceiveFrom( (UDPSOCKET *) fd, &netorder_src_addr, &netorder_src_port, buf, buflen, NUT_WAIT_INFINITE );

    if( src_ip_addr ) *src_ip_addr = ntohl( netorder_src_addr );

    printf(" recv pkt ok\n");

    return rc;
}





int mqtt_udp_send_pkt( int fd, char *data, size_t len )
{
    /*
    struct sockaddr_in addr;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    //Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( MQTT_PORT );
    serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    addr_size = sizeof serverAddr;

    mqtt_udp_throttle(); // Speed limit

    ssize_t rc = sendto( fd, data, len, 0, (struct sockaddr *)&serverAddr, addr_size);

    return (rc != len) ? EIO : 0;
    */
    printf("MQTT/UDP broadcast pkt\n");

    int rc = NutUdpSendTo( (UDPSOCKET *) fd, 0xFFFFFFFF, htons( MQTT_PORT ), data, len );
    printf("MQTT/UDP broadcast rc = %d  len = %d\n", rc, len );

    //return (rc != len) ? -5 : 0; // -5 = EIO
    return rc ? -5 : 0; // -5 = EIO
}


int mqtt_udp_send_pkt_addr( int fd, char *data, size_t len, int ip_addr )
{
    /*
    struct sockaddr_in addr;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( MQTT_PORT );
    serverAddr.sin_addr.s_addr = ip_addr;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    addr_size = sizeof serverAddr;

    ssize_t rc = sendto( fd, data, len, 0, (struct sockaddr *)&serverAddr, addr_size);

    return (rc != len) ? EIO : 0;
    */
    printf("MQTT/UDP sent pkt to\n");


    int rc = NutUdpSendTo( (UDPSOCKET *) fd, htonl( ip_addr ), htons( MQTT_PORT ), data, len );
    //return (rc != len) ? -5 : 0; // -5 = EIO
    return rc ? -5 : 0; // -5 = EIO
}



#endif // ENABLE_MQTT_UDP

