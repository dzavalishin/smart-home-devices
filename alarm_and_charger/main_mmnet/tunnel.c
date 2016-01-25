/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * TCP/Serial (485) tunneling
 *
**/
#define DEBUG 0

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

// hafduplex control func
#include "io_dig.h"

#include <stdio.h>
#include <string.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/mutex.h>

#include <sys/socket.h>
#include <sys/event.h>

#include <arpa/inet.h>


#include <dev/irqreg.h>

// NB. tx is RS485 transmit and TCP receive, rx is RC485 receive and TCP xmit


#define BUFSZ 512
// Bytes to end of buf to start send
#define RX_THRESHOLD 256

// TODO seems to be ok to have 1/2 of this - 256 b of stack
#define STK 512


#if SERVANT_TUN0 || SERVANT_TUN1

// TODO debug
//
#if DEBUG
#  define DEBUG0(a) printf( "tun%d: %s\n", t->nTunnel, (a) )
#  define DEBUG1i( s, i ) printf( "tun%d: %s %d\n", t->nTunnel, (s), (i) )
#else
#  define DEBUG0(a)
#  define DEBUG1i( s, i )
#endif

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

    HANDLE      rxThreadStarted;
    HANDLE      txThreadStarted;
    //HANDLE      rxThreadStopped; // replace with one ThreadStopped event?
    //HANDLE      txThreadStopped;
    

    char inSend; // We're sending, so ignore all we recv

    char *rxbuf;
    char *rxbuf1; // temp buf
    char *txbuf;

    unsigned int rx_idx;
    unsigned int tx_idx;

    //unsigned int rx_len;
    unsigned int tx_len;

    uint32_t	tx_total_bytes;
    uint32_t	rx_total_bytes;
};

struct tunnel_io tun0 =
{
    .nTunnel = 0,

    .tName = "tun0",
    .rxtname = "Tun0Rx",
    .txtname = "Tun0Tx",

    .tcpPort = 503,
    .set_half_duplex = set_half_duplex0,

};

struct tunnel_io tun1 =
{
    .nTunnel = 1,

    .tName = "tun1",
    .rxtname = "Tun1Rx",
    .txtname = "Tun1Tx",

    .tcpPort = 504,
    .set_half_duplex = set_half_duplex1,

};


THREAD(tunnel_ctl, __arg);
THREAD(tunnel_recv, __arg);
THREAD(tunnel_xmit, __arg);

static void TunUartAvrEnable(uint16_t base);
static void TunAvrUsartSetSpeed(char port, uint32_t rate);

//static void TunTxEmpty(void *arg);
static void TunTxSentAll(void *arg);
static void TunRxComplete(void *arg);
static void TunTxNext(void *arg); // Xmit next byte, have place in data reg

static void TunSendChar(volatile struct tunnel_io *t);
static int TunRxEmpty(volatile struct tunnel_io *t);
static int TunTxClear(volatile struct tunnel_io *t);

static void wait_empty( volatile struct tunnel_io *t );
static void TunUartUDRIE(uint16_t base, char on_off);
static void TunUartTXCIE(uint16_t base, char on_off);

static void init_one_tunnel( struct tunnel_io *t );








void init_tunnels(void)
{

#if SERVANT_TUN0
    if( RT_IO_ENABLED(IO_TUN0) )
    {
        add_exclusion_pin( UART0_EXCLPOS, UART0_TX_PIN );
        add_exclusion_pin( UART0_EXCLPOS, UART0_RX_PIN );
        init_one_tunnel( &tun0 );
    }
#endif
#if SERVANT_TUN1
    if( RT_IO_ENABLED(IO_TUN1) )
    {
        add_exclusion_pin( UART1_EXCLPOS, UART0_TX_PIN );
        add_exclusion_pin( UART1_EXCLPOS, UART0_RX_PIN );
        init_one_tunnel( &tun1 );
    }
#endif
}


static void init_one_tunnel( struct tunnel_io *t )
{
    t->runCount = 0;
    t->sock = 0;
    t->stop = 0;

    DEBUG0("init");

    // Register interrupt handler.
    if(t->nTunnel)
    {
#ifdef UDR1
        if (NutRegisterIrqHandler(&sig_UART1_DATA, TunTxNext, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART1_RECV, TunRxComplete, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART1_TRANS, TunTxSentAll, t))
#endif
            return;
    } else {
        if (NutRegisterIrqHandler(&sig_UART0_DATA, TunTxNext, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART0_RECV, TunRxComplete, t))
            return;
        if (NutRegisterIrqHandler(&sig_UART0_TRANS, TunTxSentAll, t))
            return;
    }

    t->rxbuf = t->rxbuf1 = t->txbuf = 0;

    t->rxbuf = malloc(BUFSZ);
    if( 0 == t->rxbuf )
    {
        DEBUG0("out of mem rx buf");
        goto nomem;
    }

    t->rxbuf1 = malloc(BUFSZ);
    if( 0 == t->rxbuf1 )
    {
        DEBUG0("out of mem rx buf1");
        goto nomem;
    }

    t->txbuf = malloc(BUFSZ);
    if( 0 == t->txbuf )
    {

        DEBUG0("out of mem tx buf");
        goto nomem;

    }

    NutMutexInit( &(t->serialMutex) );

    NutThreadCreate( t->tName, tunnel_ctl, t, STK );
    return;

nomem:
    if( t->rxbuf ) free( t->rxbuf );
    if( t->rxbuf1) free( t->rxbuf1);
    if( t->txbuf ) free( t->txbuf );
}


// Control thread
THREAD(tunnel_ctl, __arg)
{
    volatile struct tunnel_io *t = __arg;
    HANDLE rt, tt;

    //DEBUG0("in ctl thread");

    t->set_half_duplex(0);

    TunAvrUsartSetSpeed(t->nTunnel, ee_cfg.tun_baud[t->nTunnel]);
    TunUartAvrEnable(t->nTunnel);

    for(;;)
    {
        // Init and start all

        if((t->sock = NutTcpCreateSocket()) == 0)
        {
            DEBUG0("Creating socket failed");
            NutSleep(5000);
            goto err;
        }

        DEBUG0("Accepting");

        if( NutTcpAccept(t->sock, t->tcpPort) )
        {
            DEBUG0("Accept failed");
            goto err;
        }
        DEBUG0("Accepted");
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

#if 0
        // Races!
        while(t->runCount < 2)
            NutSleep(1000); // wait for threads to start

        //DEBUG0("Seen 2 threads");

        while(t->runCount == 2)
            NutSleep(1000); // wait while communications are going on - some thread will die if error
#else
        //DEBUG0("Wait 4 start");
        NutEventWait( &(t->rxThreadStarted), 1500 ); // Wait for start, no more than 1.5 sec
        NutEventWait( &(t->txThreadStarted), 1500 ); // Wait for start, no more than 1.5 sec

        //DEBUG0("Seen 2 threads");

        while(t->runCount == 2)
            NutSleep(1000); // wait while communications are going on - some thread will die if error

        //NutEventWait( &(t->rxThreadStopped), 0 ); // Wait for stop, forever
        //NutEventWait( &(t->txThreadStopped), 0 ); // Wait for stop, forever
#endif
        goto stop;

    err:
        DEBUG0("died with err");

        NutSleep(2000); // Do not repeat too often

        // Fall through to...
    stop:
        DEBUG0("Stopping");

        // Stop all and deinit
        t->stop = 1;


        while(t->runCount > 0)
            NutSleep(100); // wait for threads to finish

        if(t->sock)
        {
            NutTcpCloseSocket(t->sock);
            t->sock = 0;
        }

        DEBUG0("stopped");
        // Repeat forever
        t->stop = 0;
    }
}


// TCP -> 485
THREAD(tunnel_xmit, __arg)
{
    volatile struct tunnel_io *t = __arg;

//    char buf[BUFSZ];
    //uint32_t tmo5min = 60*1000*5; // 5 min
    //uint32_t tmo5min = 60*1000*5; // 5 min

    t->runCount++;
    NutEventPost(&(t->txThreadStarted));
    DEBUG0("Start tx thread");

    NutThreadSetPriority( 35 ); // Hi prio, but lower than rx thread

    // Timeout, or we will wait forever!
    //NutTcpSetSockOpt( t->sock, SO_RCVTIMEO, &tmo, sizeof(tmo) );

    // TODO do read in 2 steps? 1 byte with long timeout and rest with short?

    while(!t->stop)
    {
        // Get from TCP
        int nread = NutTcpReceive( t->sock, t->txbuf, BUFSZ );
        if( nread == 0 ) // Timeout
            break;
        if( nread < 0 ) // Error
        {
            DEBUG1i("tx tcp err", NutTcpError(t->sock));
            break;
        }

        t->tx_len = nread & 0x7FFF; // positive
        t->tx_idx = 0;

	t->tx_total_bytes += nread;

        DEBUG1i("tx",t->tx_len);


        // Now send to 485 port
        NutMutexLock( (MUTEX *)&(t->serialMutex) );
        t->set_half_duplex(1);
        t->inSend = 1; // Ignore all incoming data

        TunSendChar(t); // Send initial byte

        NutEventWait( &(t->sendDone), 1500 ); // Wait for xmit, no more than 1.5 sec
        //DEBUG1i("tx done 485 sending", t->tx_idx);

        wait_empty( t );
        t->set_half_duplex(0);
        t->inSend = 0;
        NutMutexUnlock( (MUTEX *)&(t->serialMutex) );
    }

    t->runCount--;
    //NutEventPost(&(t->txThreadStopped));
    DEBUG0("End tx thread");
    NutThreadExit();
    for(;;) ; // make compiler happy
}


// 485 -> TCP
THREAD(tunnel_recv, __arg)
{
    volatile struct tunnel_io *t = __arg;

    //char buf[BUFSZ];
    int tx_len = 0;

    t->runCount++;
    DEBUG0("Start rx thread");
    NutEventPost(&(t->rxThreadStarted));

    NutThreadSetPriority( 32 ); // Hi prio

    while(!t->stop)
    {
        //DEBUG0("rx wait 4 data");
        while( NutEventWait( &(t->rxGotSome), 500 ) && !TunRxEmpty(t) ) // Wait forever for some data to come
        {
            // Timeout - check if we have to die
            if(t->stop) goto die;
        }

        //DEBUG0("rx lock mutex");
        // Recv data from 485 port
        NutMutexLock( (MUTEX *)&(t->serialMutex) );
        t->set_half_duplex(0); // Make sure we recv - actually pointless

        //DEBUG0("rx loop recv");

        int rx_idx1, rx_idx2 = t->rx_idx;
        NutSleep( 20 ); // Give 485 some time to recv data

        do {
            rx_idx1 = rx_idx2;
            NutEventWait( &(t->recvDone), 40 ); // Wait for end of data
            //NutSleep( 40 ); // Give 485 some time to recv data
            rx_idx2 = t->rx_idx;

            // Near the end of buf, go send!
            if( t->rx_idx >= (BUFSZ-RX_THRESHOLD) )
                break;

        } while( rx_idx1 != rx_idx2 ); // Got some since last wait? Re-wait!

        //DEBUG0("rx loop recv ok");
        NutEventBroadcast(&(t->rxGotSome)); // Clear signaled state after getting all recvd data
        NutMutexUnlock( (MUTEX *)&(t->serialMutex) );

        {
            tx_len = t->rx_idx;
            t->rx_total_bytes += tx_len;
            DEBUG1i("rx", tx_len);
            cli();
            memcpy( t->rxbuf1, t->rxbuf, tx_len );
            t->rx_idx = 0;
            sei();
        }

        // Flush prev TCP send, make sure there is free place in buffer and our actual
        // data will be sent in separate datagram
        //NutTcpDeviceWrite( t->sock, t->rxbuf1, 0 );
        //DEBUG0("rx flushed");
    again:
        ;
        // Now send to TCP
        int sent = NutTcpDeviceWrite( t->sock, t->rxbuf1, tx_len );
        //DEBUG1i("rx sent", sent);

        if( sent < 0 ) break; // error

        if( sent < tx_len )
        {
            int done = tx_len - sent;

            memmove( t->rxbuf1, t->rxbuf1+done, tx_len-done );
            tx_len -= done;
            goto again;
        }

        NutTcpDeviceWrite( t->sock, t->rxbuf1, 0 );
        //DEBUG0("rx flushed");

        //t->rx_idx = 0;
    }

die:
    t->runCount--;
    //NutEventPost(&(t->rxThreadStopped));
    DEBUG0("End rx thread");
    NutThreadExit();
    for(;;) ; // make compiler happy
}


// Put as many bytes as possible to send buffer
static void TunSendChar(volatile struct tunnel_io *t)
{
    cli(); // Prevent tx clear inerrupts as we put bytes

    //DEBUG1i("485 tx char idx", t->tx_idx);
    //DEBUG1i("485 tx char len", t->tx_len);

    while(TunTxClear(t))
    {
        if( t->tx_idx >= t->tx_len )
            break;

        TunUartUDRIE(t->nTunnel, 1); // Enable data reg empty interrupt to fill data

        // Extract char before touching UDR, prevent races
        char c = t->txbuf[t->tx_idx++];

#ifdef UDR1
        if(t->nTunnel)
            UDR1 = c;
        else
#endif
            UDR = c;
    }
    sei(); // Ok to interrupt now
}

// Interrupt - send next char
static void TunTxNext(void *arg)
{
    volatile struct tunnel_io *t = arg;

    if( t->tx_idx >= t->tx_len )
    {
        TunUartUDRIE(t->nTunnel, 0);
        NutEventPostFromIrq(&(t->sendDone));
        return;
    }

    //TunUartUDRIE(t->nTunnel, 1);
    TunSendChar(t);
}


// Interrupt
static void TunRxComplete(void *arg)
{
    volatile struct tunnel_io *t = arg;

    if( !t->inSend )
        NutEventPostFromIrq( &(t->rxGotSome) ); // Start recv process

    char c;

#ifdef UDR1
    if(t->nTunnel)
        c = UDR1;
    else
#endif
        c = UDR;

    if( t->rx_idx >= BUFSZ )
        return;

    if( t->inSend )
        return; // Drop it

    t->rxbuf[t->rx_idx++] = c;

    // 10 bytes left? Time to send data!
    if( t->rx_idx >= (BUFSZ-RX_THRESHOLD) )
    {
        NutEventPostFromIrq(&(t->recvDone));
    }

}

// Interrupt
static void TunTxSentAll(void *arg)
{
    volatile struct tunnel_io *t = arg;
    //TunUartUDRIE(t->nTunnel, 0);
    TunUartTXCIE(t->nTunnel, 0);
    NutEventPostFromIrq(&(t->txEmpty));
}

static void wait_empty( volatile struct tunnel_io *t )
{
    TunUartTXCIE(t->nTunnel, 1);
    //TunUartUDRIE(t->nTunnel, 1);
    NutEventWait( &(t->txEmpty), 10 ); // No more than 10 msec to send it all
}




/*
 * Carefully enable UART functions.
 */
static void TunUartAvrEnable(uint16_t base)
{
    if (base)
    {
        add_exclusion_pin( UART1_EXCLPOS, UART0_TX_PIN );
        add_exclusion_pin( UART1_EXCLPOS, UART0_RX_PIN );

        //DDRD |= _BV(UART1_TX_PIN);
        DDRD &= ~_BV(UART1_TX_PIN);
        DDRD &= ~_BV(UART1_RX_PIN);

        UCSR1C = 0x6; // no parity, one stop, 8 bit
        UCSR1A = 0x00;
        UCSR1B = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    }
    else
    {
        add_exclusion_pin( UART0_EXCLPOS, UART0_TX_PIN );
        add_exclusion_pin( UART0_EXCLPOS, UART0_RX_PIN );

        //DDRE |= _BV(UART0_TX_PIN);
        DDRE &= ~_BV(UART0_TX_PIN);
        DDRE &= ~_BV(UART0_RX_PIN);

        UCSR0C = 0x6; // no parity, one stop, 8 bit
        UCSR0A = 0x00;
        UCSR0B = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    }
}


static void TunUartUDRIE(uint16_t base, char on_off)
{
    if( on_off )
    {
        if (base)
            UCSR1B |= BV(UDRIE);
        else
            UCR |= BV(UDRIE);
    }
    else
    {
        if (base)
            UCSR1B &= ~BV(UDRIE);
        else
            UCR &= ~BV(UDRIE);
    }
}

static void TunUartTXCIE(uint16_t base, char on_off)
{
    if( on_off )
    {
        if (base)
            UCSR1B |= BV(TXCIE);
        else
            UCR |= BV(TXCIE);
    }
    else
    {
        if (base)
            UCSR1B &= ~BV(TXCIE);
        else
            UCR &= ~BV(TXCIE);
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

    return us & (1<<RXC);
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

    return us & (1<<UDRE);
}


// From NutOS code
static void TunAvrUsartSetSpeed(char port, uint32_t rate)
{
    uint16_t sv;

    // Modified Robert Hildebrand's refined calculation.
#ifdef __AVR_ENHANCED__
    if( port )
    {
        if (bit_is_clear(UCSR1C, UMSEL)) {
            if (bit_is_set(UCSR1A, U2X)) {
                rate <<= 2;
            } else {
                rate <<= 3;
            }
        }
    }
    else
    {
        if (bit_is_clear(UCSR0C, UMSEL)) {
            if (bit_is_set(UCSR0A, U2X)) {
                rate <<= 2;
            } else {
                rate <<= 3;
            }
        }
    }
#else
    rate <<= 3;
#endif
    sv = (uint16_t) ((NutGetCpuClock() / rate + 1UL) / 2UL) - 1;

    if( port )
    {
        outb(UBRR1L, (uint8_t) sv);
#ifdef __AVR_ENHANCED__
        outb(UBRR1H, (uint8_t) (sv >> 8));
#endif
    } else {
        outb(UBRR0L, (uint8_t) sv);
#ifdef __AVR_ENHANCED__
        outb(UBRR0H, (uint8_t) (sv >> 8));
#endif
    }
}


void get_tunnel_stats( uint8_t nTunnel, uint32_t *tx_total, uint32_t *rx_total, uint8_t *active  )
{
    struct tunnel_io *t = 0;
    switch( nTunnel )
    {
#if SERVANT_TUN0
    case 0:        t = &tun0; break;
#endif
#if SERVANT_TUN1
    case 1:        t = &tun1; break;
#endif
    }

    if( t == 0 )
    {
        *rx_total = *tx_total = 0;
        *active = 0;
    }
    else
    {
        *rx_total = t->rx_total_bytes;
        *tx_total = t->tx_total_bytes;
        *active = (t->runCount > 0) ? 1 : 0;
    }
}




#endif // SERVANT_TUN0 || SERVANT_TUN1

