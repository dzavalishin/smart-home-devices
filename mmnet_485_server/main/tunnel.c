/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * TCP/Serial (485) tunneling
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

// hafduplex control func
#include "io_dig.h"

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/mutex.h>

#include <sys/socket.h>
#include <sys/event.h>

#include <arpa/inet.h>

#define BUFSZ 512
#define STK 1024


#if SERVANT_TUN0 || SERVANT_TUN1

// TODO debug
#define DEBUG0(a)


struct tunnel_io
{
    int nTunnel;

    char *tName;
    char *rxtname;
    char *txtname;

    int tcpPort;
    void (*set_half_duplex)(char state);

    char stop;
    char runCount;

    MUTEX       serialMutex;
    TCPSOCKET   *sock;

    char *rxbuf;
    char *txbuf;
};

struct tunnel_io tun0 =
{
    .nTunnel = 0,

    .tName = "tun0",
    .rxtname = "Tun0Rx",
    .rxtname = "Tun0Tx",

    .tcpPort = 503,
    .set_half_duplex = set_half_duplex0,

    .stop = 0,

};

struct tunnel_io tun1 =
{
    .nTunnel = 1,

    .tName = "tun1",
    .rxtname = "Tun1Rx",
    .rxtname = "Tun1Tx",

    .tcpPort = 504,
    .set_half_duplex = set_half_duplex1,

    .stop = 0,
};


THREAD(tunnel_ctl, __arg);
THREAD(tunnel_recv, __arg);
THREAD(tunnel_xmit, __arg);


static void init_one_tunnel( struct tunnel_io *t );

void init_tunnels(void)
{

    if( RT_IO_ENABLED(IO_TUN0) )
    {
        init_one_tunnel( &tun0 );
    }

    if( RT_IO_ENABLED(IO_TUN1) )
    {
        init_one_tunnel( &tun1 );
    }


}


static void init_one_tunnel( struct tunnel_io *t )
{
    t->runCount = 0;
    t->sock = 0;

    t->rxbuf = malloc(BUFSZ);
    if( 0 == t->rxbuf )
    {
        DEBUG0("out of mem rx buf");
        return;
    }

    t->txbuf = malloc(BUFSZ);
    if( 0 == t->txbuf )
    {
        free(t->rxbuf);
        DEBUG0("out of mem tx buf");
        return;
    }

    NutMutexInit( &(t->serialMutex) );

    NutThreadCreate( t->tName, tunnel_ctl, t, 512);
}


// Control thread
THREAD(tunnel_ctl, __arg)
{
    volatile struct tunnel_io *t = __arg;

    HANDLE rt, tt;

    t->set_half_duplex(0);

    for(;;)
    {
        // Init and start all

        if((t->sock = NutTcpCreateSocket()) == 0)
        {
            DEBUG0("Creating socket failed\n");
            NutSleep(5000);
            goto err;
        }


        if( NutTcpAccept(t->sock, 502) )
        {
            DEBUG0("Accept failed\n");
            goto err;
        }
        DEBUG0("Accepted\n");
        //(modbus_debug) printf("Modbus [%u] Connected, %u bytes free\n", id, NutHeapAvailable());

        // Timeout, or we will wait forever!
        {
            uint32_t tmo = ((uint32_t)60)*1000*5; // 5 min
            NutTcpSetSockOpt( t->sock, SO_SNDTIMEO, &tmo, sizeof(tmo) );
            NutTcpSetSockOpt( t->sock, SO_RCVTIMEO, &tmo, sizeof(tmo) );
        }



        rt = NutThreadCreate( t->rxtname, tunnel_recv, (void *)t, STK);
        if( 0 == rt ) goto err;

        tt = NutThreadCreate( t->txtname, tunnel_xmit, (void *)t, STK);
        if( 0 == tt ) goto err;


        while(t->runCount < 2)
            NutSleep(1000); // wait for threads to start

        while(t->runCount == 2)
            NutSleep(1000); // wait while communications are going on - some thread will die if error

        goto stop;

    err:
        DEBUG0("died\n");
        // TODO mark err
        NutSleep(1000); // Do not repeat too often

        // Fall through to...
    stop:
        // Stop all and deinit
        t->stop = 1;


        while(t->runCount > 0)
            NutSleep(100); // wait for threads to finish

        if(t->sock)
        {
            NutTcpCloseSocket(t->sock);
            t->sock = 0;
        }

        // Repeat forever
    }
}


// TCP -> 485
THREAD(tunnel_recv, __arg)
{
    volatile struct tunnel_io *t = __arg;

    char buf[BUFSZ];
    //uint32_t tmo5min = 60*1000*5; // 5 min
    //uint32_t tmo5min = 60*1000*5; // 5 min

    t->runCount++;

    // Timeout, or we will wait forever!
    //NutTcpSetSockOpt( t->sock, SO_RCVTIMEO, &tmo, sizeof(tmo) );

    // TODO do read in 2 steps? 1 byte with long timeout and rest with short?

    while(!t->stop)
    {
        // Get from TCP
        int nread = NutTcpDeviceRead( t->sock, t->rxbuf, sizeof(buf) );

        // Now send to 485 port
        NutMutexLock( &(t->serialMutex) );
        t->set_half_duplex(1);

        t->set_half_duplex(0);
        NutMutexUnlock( &(t->serialMutex) );
    }

    t->runCount--;

    NutThreadExit();
    for(;;) ; // make compiler happy
}


// 485 -> TCP
THREAD(tunnel_xmit, __arg)
{
    volatile struct tunnel_io *t = __arg;

    //char buf[BUFSZ];
    int tx_len = 0;

    t->runCount++;

    while(!t->stop)
    {
        // Recv data from 485 port
        NutMutexLock( &(t->serialMutex) );
        t->set_half_duplex(0);

        NutMutexUnlock( &(t->serialMutex) );

        // Now send to TCP
        NutTcpSend( t->sock, t->txbuf, tx_len );
    }

    t->runCount--;

    NutThreadExit();
    for(;;) ; // make compiler happy
}










#endif // SERVANT_TUN0 || SERVANT_TUN1

