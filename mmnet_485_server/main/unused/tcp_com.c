/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Modbus/TCP to serial convertor.
 *
**/

#if SERVANT_TCP_COM0 || SERVANT_TCP_COM1

#include "defs.h"
#include "modbus.h"

#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

#define MAX_RTU_PKT     1024

struct tcp_com_param
{
    FILE *com_port;
    int tcp_port;
};

static const int modbus_debug = 0;

// NB! Not reentrant! Both us and modbus can't be reentered, sorry



THREAD(ModbusTcpCom, _arg)
{
    struct tcp_com_param *arg = _arg;
    TCPSOCKET *modbus_sock;

    // Now loop endless for connections.
    for (;;) {

        if ((modbus_sock = NutTcpCreateSocket()) == 0)
        {
            if(modbus_debug) printf("[%u] Creating socket failed\n", id);
            NutSleep(5000);
            continue;
        }

        // Listen on port 502. This call will block until we get a connection from a client.
        NutTcpAccept(modbus_sock, 502);
        if(modbus_debug) printf("Modbus [%u] Connected, %u bytes free\n", id, NutHeapAvailable());

    again:
        // Wait until at least 4 kByte of free RAM is available. This will keep the client connected in low memory situations.
        while (NutHeapAvailable() < 4096)
        {
            if(modbus_debug) printf("Modbus [%u] Low mem\n", id);
            NutSleep(1000);
        }

        unsigned char hdr[6];
        int nread = NutTcpDeviceRead( modbus_sock, hdr, 6 );
        if(modbus_debug) printf("Modbus got hdr %d bytes\n", nread );

        if( (nread != 6) || (nread < 0) ) goto die;

        unsigned int pkt_len = 0;
        modbus_parse_tcp_hdr( hdr, &pkt_len );
        if(modbus_debug) printf("Modbus TCP header states %d bytes len\n", pkt_len );

        if( (pkt_len < 2) || (pkt_len > 1024) ) goto die;

        unsigned char *bigbuf = malloc( pkt_len + 6 );
        if( bigbuf == 0 ) goto die;

        nread = NutTcpDeviceRead( modbus_sock, bigbuf+6, pkt_len );
        if(modbus_debug) printf("Modbus body %d bytes\n", nread );
        if( (nread != pkt_len) || (nread < 0) ) goto mdie;

        memcpy( bigbuf, hdr, 6 );

        if(modbus_debug) printf("Modbus will process %u bytes\n", pkt_len+6 );

        int rt_len = modbus_tcp_2_rtu( bigbuf, pkt_len+6 );

        // TODO
        nread = fwrite( bigbuf, 1, rt_len, arg.com_port );
        if( nread != rt_len )
        {
            if(modbus_debug) printf("Serial wrote %u bytes, must be %u\n", nread, rt_len );
            goto mdie;
        }

        void *recv_rt_pkt;
        int recv_rt_len = modbus_rtu_recv( arg.com_port, &recv_rt_pkt, &recv_rt_len );
        if( (recv_rt_len <= 0) || (recv_rt_len > MAX_RTU_PKT) )
        {
            if(modbus_debug) printf("Serial recv fail %d\n", recv_rt_len );
            goto mdie;
        }

        {
            char recv_tcp_pkt[recv_rt_len+6];
            int tcp_len = modbus_tcp_2_rtu( recv_rt_pkt, recv_rt_len, recv_tcp_pkt, recv_rt_len+6 );

            free( recv_tcp_pkt ); recv_tcp_pkt = 0;

            if(modbus_debug) printf("Modbus reply with %u bytes\n", tcp_len );

            NutTcpSend( modbus_sock, recv_tcp_pkt, tcp_len );
        }



        free( bigbuf );
        goto again;

    mdie:
        free( bigbuf );
    die:
        NutTcpCloseSocket(modbus_sock);
        if(modbus_debug) printf("MODBUS [%u] Disconnected\n", id);
    }
}







void init_tcp_com(void)
{

#if SERVANT_TCP_COM0
    static struct tcp_com_param arg0 = { 0, 503 };

    arg0.com_port = fopen("uart0", "rwb" );

    if( arg0.com_port != 0 )
        NutThreadCreate("TcpCom0", long_init, &arg0, 1500+MAX_RTU_PKT );
#endif

#if SERVANT_TCP_COM1
    static struct tcp_com_param arg1 = { 0, 504 };

    arg1.com_port = fopen("uart1", "rwb" );

    if( arg0.com_port != 0 )
        NutThreadCreate("TcpCom1", long_init, &arg1, 1500+MAX_RTU_PKT );
#endif

}





#endif // SERVANT_TCP_COM0 || SERVANT_TCP_COM1

