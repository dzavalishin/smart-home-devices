#include <sys/heap.h>
#include <sys/socket.h>
#include <stdio.h>

#include "modbus_srv.h"
#include "modbus_reg.h"
#include "modbus.h"

#include "defs.h"
#include "adc.h"

static const int modbus_debug = 0;

// NB! Not reentrant! Both us and modbus can't be reentered, sorry

static TCPSOCKET *modbus_sock;
static HANDLE replySentEvent;


THREAD(ModbusService, arg)
{
    //TCPSOCKET *modbus_sock;
    //FILE *stream;
    //char id = (u_char) ((uptr_t) arg);
    u_char id = 0;

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

        char *bigbuf = malloc( pkt_len + 6 );
        if( bigbuf == 0 ) goto die;

        nread = NutTcpDeviceRead( modbus_sock, bigbuf+6, pkt_len );
        if(modbus_debug) printf("Modbus body %d bytes\n", nread );
        if( (nread != pkt_len) || (nread < 0) ) goto mdie;

        memcpy( bigbuf, hdr, 6 );

        if(modbus_debug) printf("Modbus will process %u bytes\n", pkt_len+6 );
        modbus_process_tcp_pkt( bigbuf, pkt_len+6 );

        NutEventWait(&replySentEvent, 1000); // If modbus didn't reply in 1 sec, lets go anyway.

        free( bigbuf );
        goto again;

    mdie:
        free( bigbuf );
    die:
        NutTcpCloseSocket(modbus_sock);
        if(modbus_debug) printf("MODBUS [%u] Disconnected\n", id);
    }
}


void modbus_send_tcp( unsigned char *modbus_tx_buf, int tx_len )
{
    if(modbus_debug) printf("Modbus reply with %u bytes\n", tx_len );

    //int nread = NutTcpDeviceRead( modbus_sock, hdr, 6 );
    //if( nread != 6 ) goto die;

    //NutTcpDeviceWrite( modbus_sock, modbus_tx_buf, tx_len );
    NutTcpSend( modbus_sock, modbus_tx_buf, tx_len );

    NutEventPostAsync(&replySentEvent);

}

// modbus lib insists - this is for serial mode
void uart_set_baud( int baud ) { (void) baud; }
void modbus_start_tx( void ) {  }











// + 64-79 - 16 x AI (8 provided: 4 real AI, 2 internal, but visible, 2 variable resistors)
#define MB_REG_AI	64
#define MB_COUNT_AI     16


uint8_t modbus_read_register( uint16_t nReg, uint16_t *val )
{
    if(modbus_debug) printf("Modbus reg %u read\n", nReg );

#if SERVANT_NADC > 0
    if( INRANGE( nReg, MB_REG_AI, MB_COUNT_AI ) )
    //if( (nReg >= MB_REG_AI) && (nReg < (MB_REG_AI+MB_COUNT_AI)) )
    {
        unsigned int id = nReg-MB_REG_AI;
        if( id >= SERVANT_NADC ) return 0;
        *val = adc_value[id];
        if(modbus_debug) printf("Modbus replied with %u\n", *val );
        return 1;
    }
#endif

    if(modbus_debug) printf("Modbus read - unknown register %u\n", nReg );
    return 0;
}






// return modbus err no or 0
int modbus_write_register( uint16_t nReg, uint16_t value )
{
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}







