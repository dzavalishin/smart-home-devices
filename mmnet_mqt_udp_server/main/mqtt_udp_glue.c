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


void mqtt_udp_start( void )
{
    //mqtt_udp_set_throttle( 0 ); // We have no
}


// Send channel status upstream
void mqtt_udp_send_channel( uint8_t state, uint8_t ch )
{
    char topic[] = "mmnet/01/dio0";
    topic[12] = ch + '0';

    char val[4];
    snprintf( val, sizeof(val) - 1, "%d", state );

//    int rc = mqtt_udp_send_publish( topic, val );
//    if( rc )        printf("publish err=%d", rc);
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

    memset( buf, 0, buflen );

    uint32_t netorder_src_addr;
    uint16_t netorder_src_port;

    int rc = NutUdpReceiveFrom( (UDPSOCKET *) fd, &netorder_src_addr, &netorder_src_port, buf, buflen, NUT_WAIT_INFINITE );

    if( src_ip_addr ) *src_ip_addr = ntohl( netorder_src_addr );

    return rc;
}





int mqtt_udp_send_pkt( int fd, char *data, size_t len )
{
    /*
    struct sockaddr_in addr;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*Configure settings in address struct* /
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( MQTT_PORT );
    serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    addr_size = sizeof serverAddr;

    mqtt_udp_throttle(); // Speed limit

    ssize_t rc = sendto( fd, data, len, 0, (struct sockaddr *)&serverAddr, addr_size);

    return (rc != len) ? EIO : 0;
    */

    int rc = NutUdpSendTo( (UDPSOCKET *) fd, 0xFFFFFFFF, htons( MQTT_PORT ), data, len );
    return (rc != len) ? -5 : 0; // -5 = EIO
}


int mqtt_udp_send_pkt_addr( int fd, char *data, size_t len, int ip_addr )
{
    /*
    struct sockaddr_in addr;

    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*Configure settings in address struct* /
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( MQTT_PORT );
    serverAddr.sin_addr.s_addr = ip_addr;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    addr_size = sizeof serverAddr;

    ssize_t rc = sendto( fd, data, len, 0, (struct sockaddr *)&serverAddr, addr_size);

    return (rc != len) ? EIO : 0;
    */


    int rc = NutUdpSendTo( (UDPSOCKET *) fd, htonl( ip_addr ), htons( MQTT_PORT ), data, len );
    return (rc != len) ? -5 : 0; // -5 = EIO
}



#endif // ENABLE_MQTT_UDP

