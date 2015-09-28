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


#include <dev/irqreg.h>



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

    HANDLE      txEmpty;
    HANDLE      sendDone;
    HANDLE      recvDone;
    HANDLE      rxGotSome;

    char inSend; // We're sending, so ignore all we recv

    char *rxbuf;
    char *txbuf;

    int rx_idx;
    int tx_idx;

    int rx_len;
    int tx_len;
};

struct tunnel_io tun0 =
{
    .nTunnel = 0,

    .tName = "tun0",
    .rxtname = "Tun0Rx",
    .rxtname = "Tun0Tx",

    .tcpPort = 503,
    .set_half_duplex = set_half_duplex0,

};

struct tunnel_io tun1 =
{
    .nTunnel = 1,

    .tName = "tun1",
    .rxtname = "Tun1Rx",
    .rxtname = "Tun1Tx",

    .tcpPort = 504,
    .set_half_duplex = set_half_duplex1,

};


THREAD(tunnel_ctl, __arg);
THREAD(tunnel_recv, __arg);
THREAD(tunnel_xmit, __arg);

static void TunUartAvrEnable(uint16_t base);


static void TunTxEmpty(void *arg);
static void TunTxComplete(void *arg);
static void TunRxComplete(void *arg);

static void TunSendChar(volatile struct tunnel_io *t);
static int TunRxEmpty(volatile struct tunnel_io *t);
static int TunTxClear(volatile struct tunnel_io *t);

static void wait_empty( volatile struct tunnel_io *t );
static void TunUartUDRIE(uint16_t base, char on_off);

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
    t->stop = 0;

    // Register interrupt handler.
    if(t->nTunnel)
    {
#ifdef UDR1
        if (NutRegisterIrqHandler(&sig_UART1_DATA, TunTxEmpty, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART1_RECV, TunRxComplete, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART1_TRANS, TunTxComplete, t))
#endif
            return;
    } else {
        if (NutRegisterIrqHandler(&sig_UART0_DATA, TunTxEmpty, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART0_RECV, TunRxComplete, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART0_TRANS, TunTxComplete, t))
            return;
    }

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

    TunUartAvrEnable(t->nTunnel);

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
        //int nread = NutTcpDeviceRead( t->sock, t->rxbuf, sizeof(buf) );
        int nread = NutTcpReceive( t->sock, t->rxbuf, sizeof(buf) );
        if( nread == 0 ) // Timeout
            break;
        if( nread < 0 ) // Error
        {
            break;
        }

        t->tx_len = nread & 0x7FFF; // positive
        t->tx_idx = 0;

        // Now send to 485 port
        NutMutexLock( &(t->serialMutex) );
        t->set_half_duplex(1);
        t->inSend = 1; // Ignore all incoming data

        TunSendChar(t); // Send initial byte

        NutEventWait( &(t->sendDone), 500 ); // Wait for xmit, no more than 0.5 sec

        wait_empty( t );
        t->set_half_duplex(0);
        t->inSend = 0;
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
    //int tx_len = 0;

    t->runCount++;

    while(!t->stop)
    {
        while( NutEventWait( &(t->rxGotSome), 500 ) && !TunRxEmpty(t) ) // Wait forever for some data to come
        {
            // Timeout - check if we have to die
            if(t->stop) goto die;
        }

        // Recv data from 485 port
        NutMutexLock( &(t->serialMutex) );
        //t->set_half_duplex(0); // Make sure we recv - actually pointless

        int rx_idx1, rx_idx2;
        do {
            rx_idx1 = t->rx_idx;
            NutEventWait( &(t->recvDone), 2 ); // Wait for end of data
            rx_idx2 = t->rx_idx;
        } while( rx_idx1 != rx_idx2 ); // Got some? Re-wait!

        NutEventBroadcast(&(t->rxGotSome)); // Clear signaled state after getting all recvd data
        NutMutexUnlock( &(t->serialMutex) );

        // Flush prev TCP send, make sure there is free place in buffer and our actual
        // data will be sent in separate datagram
        NutTcpDeviceWrite( t->sock, t->rxbuf, 0 );

        // Now send to TCP
        NutTcpDeviceWrite( t->sock, t->rxbuf, t->rx_idx );
        t->rx_idx = 0;
    }

die:
    t->runCount--;

    NutThreadExit();
    for(;;) ; // make compiler happy
}


// Put as many bytes as possible to send buffer
static void TunSendChar(volatile struct tunnel_io *t)
{
    di(); // Prevent tx clear inerrupts as we put bytes
    while(TunTxClear(t))
    {
        if( t->tx_idx >= t->tx_len )
            break;

        // Extract char befor touching UDR, prevent races
        char c = t->txbuf[t->tx_idx++];

#ifdef UDR1
        if(t->nTunnel)
            UDR1 = c;
        else
#endif
            UDR = c;
    }
    ei(); // Ok to interrupt now
}

// Interrupt
static void TunTxComplete(void *arg)
{
    volatile struct tunnel_io *t = arg;

    if( t->tx_idx >= t->tx_len )
    {
        NutEventPostFromIrq(&(t->sendDone));
        return;
    }

    TunSendChar(t);
}


// Interrupt
static void TunRxComplete(void *arg)
{
    volatile struct tunnel_io *t = arg;

    if( !t->inSend )
        NutEventPostFromIrq( &(t->rxGotSome) ); // Start recv process

    if( t->rx_idx >= BUFSZ )
    {
        //NutEventPostFromIrq(&(t->sendDone)); // did it byte ago
        return;
    }

    char c;

#ifdef UDR1
    if(t->nTunnel)
        c = UDR1;
    else
#endif
        c = UDR;

    if( t->inSend )
        return; // Drop it

    t->rxbuf[t->rx_idx++] = c;

    if( t->rx_idx >= t->rx_len )
    {
        NutEventPostFromIrq(&(t->recvDone));
        return;
    }

}

// Interrupt
static void TunTxEmpty(void *arg)
{
    volatile struct tunnel_io *t = arg;
    TunUartUDRIE(t->nTunnel, 0);
    NutEventPostFromIrq(&(t->txEmpty));
}

static void wait_empty( volatile struct tunnel_io *t )
{
    TunUartUDRIE(t->nTunnel, 1);
    NutEventWait( &(t->txEmpty), 10 ); // No more than 10 msec to send it all
}




/*
 * Carefully enable UART functions.
 */
static void TunUartAvrEnable(uint16_t base)
{
    //NutEnterCritical();
#ifdef UCSR1B
    if (base)
        UCSR1B = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    else
#endif
        UCR = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    //NutExitCritical();
}


static void TunUartUDRIE(uint16_t base, char on_off)
{
    if( on_off )
    {
#ifdef UCSR1B
        if (base)
            UCSR1B |= BV(UDRIE);
        else
#endif
            UCR |= BV(UDRIE);
    }
    else
    {
#ifdef UCSR1B
        if (base)
            UCSR1B &= ~BV(UDRIE);
        else
#endif
            UCR &= ~BV(UDRIE);
    }
}


static int TunRxEmpty(volatile struct tunnel_io *t)
{
    uint8_t us;

#ifdef UDR1
    if (t->nTunnel)
        us = inb(UCSR1A);
    else
#endif
        us = inb(USR);

    return us & RXC;
}

// Can send one more byte
static int TunTxClear(volatile struct tunnel_io *t)
{
    uint8_t us;

#ifdef UDR1
    if (t->nTunnel)
        us = inb(UCSR1A);
    else
#endif
        us = inb(USR);

    return us & TXC;
}


#endif // SERVANT_TUN0 || SERVANT_TUN1

