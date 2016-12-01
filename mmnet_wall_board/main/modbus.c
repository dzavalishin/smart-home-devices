/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Modbus/TCP interface to general Modbus library.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"

#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>


#include "modbus_srv.h"
#include "modbus_reg.h"
#include <modbus.h>

#include "io_adc.h"
#include "io_pwm.h"
#include "io_dig.h"
#include "io_temp.h"
#include "io_dht.h"
#include "io_bmp180.h"

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

        // Timeout, or we will wait forever!
        {
            uint32_t tmo = ((uint32_t)60)*1000*1; // 1 min
            NutTcpSetSockOpt( modbus_sock, SO_SNDTIMEO, &tmo, sizeof(tmo) );
            NutTcpSetSockOpt( modbus_sock, SO_RCVTIMEO, &tmo, sizeof(tmo) );
        }

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










#define READ_ONE(__id_, __val_ ) do {  if( nReg == (__id_) ) { *val = (__val_); return 1; } } while(0)

char modbus_device_id[] = MODBUS_DEVICE_ID;

uint8_t modbus_read_register( uint16_t nReg, uint16_t *val )
{
    if(modbus_debug) printf("Modbus reg %u read\n", nReg );

    // Configuration

    READ_ONE( MB_REG_HWCONF_DI, 32 ); // Max DI, bits
    READ_ONE( MB_REG_HWCONF_DO, 32 ); // Max DO, bits
    READ_ONE( MB_REG_HWCONF_AI, SERVANT_NADC + (SERVANT_DHT11 ? 2 : 0) + (SERVANT_BMP180 ? 2 : 0));
    READ_ONE( MB_REG_HWCONF_AO, SERVANT_NPWM ); // Max AO
    READ_ONE( MB_REG_HWCONF_CN, 0 ); // Max counters
    READ_ONE( MB_REG_HWCONF_TS, SERVANT_NTEMP ); // Actual temp sensorts count
#if SERVANT_NTEMP > 0
    READ_ONE( MB_REG_HWCONF_TA, nTempSensors ); // Actual temp sensorts count
#else
    READ_ONE( MB_REG_HWCONF_TA, 0 ); // Actual temp sensorts count
#endif
    // Id

    READ_ONE( MB_REG_ID+0, *((uint16_t *)modbus_device_id+0) ); /* 'DZ' */
    READ_ONE( MB_REG_ID+1, *((uint16_t *)modbus_device_id+0) ); /* 'DZ' */
    READ_ONE( MB_REG_ID+2, *((uint16_t *)modbus_device_id+1) );
    READ_ONE( MB_REG_ID+3, *((uint16_t *)modbus_device_id+2) );

#if SERVANT_1WMAC
    if( INRANGE( nReg, MB_REG_ROMID, MB_COUNT_ROMID ) )
    {
        *val = *((uint16_t*)serialNumber+nReg-MB_REG_ROMID);
        return 1;
    }
#endif

    // Digital inputs

    //READ_ONE( MB_REG_IO+0, (((uint16_t)dio_read_port( 3 )) << 8 ) || dio_read_port( 1 ) ); // Port D, Port B
    //READ_ONE( MB_REG_IO+1, (((uint16_t)dio_read_port( 5 )) << 8 ) || dio_read_port( 4 ) ); // Port F, Port E
    READ_ONE( MB_REG_IO+0, dio_read_port( 1 ) ); // Port B
    READ_ONE( MB_REG_IO+1, dio_read_port( 3 ) ); // Port D
    READ_ONE( MB_REG_IO+2, dio_read_port( 4 ) ); // Port E
    READ_ONE( MB_REG_IO+3, dio_read_port( 5 ) ); // Port F

    // + 64-79 - 16 x AI
    // 0-7 - DAC
    // 8 - DHT11 humidity
    // 9 - DHT11 temp
    // 10 - BMP180 pressure
    // 11 - BMP180 temp

    if( INRANGE( nReg, MB_REG_AI, MB_COUNT_AI ) )
    {
        unsigned int id = nReg-MB_REG_AI;

#if SERVANT_DHT11
        if( id == 8 ) { *val = dht_humidity; return 1; }
        if( id == 9 ) { *val = dht_temperature; return 1; }
#endif
#if SERVANT_BMP180
        if( id == 10 ) { *val = bmp180_pressure_mmHg; return 1; } // mm Hg *10
        if( id == 11 ) { *val = (uint16_t)bmp180_temperature; return 1; }
#endif
#if SERVANT_NADC > 0
        if( id >= SERVANT_NADC ) return 0;
        *val = adc_value[id];
//        if(modbus_debug) printf("Modbus replied with %u\n", *val );
        return 1;
    }
#endif

#if SERVANT_NTEMP > 0
    // unmapped - returns temp in unknown order
    if( INRANGE( nReg, MB_REG_TEMP_DIRECT, SERVANT_NTEMP ) )
    {
        unsigned int id = nReg-MB_REG_TEMP_DIRECT;
        if( id >= SERVANT_NTEMP ) return 0;
        *val = currTemperature[id];
        return 1;
    }

    // mapped - returns temp in order set in web setup
    if( INRANGE( nReg, MB_REG_TEMP, MAX_OW_MAP ) )
    {
        unsigned int id = nReg-MB_REG_TEMP;
        if( id >= MAX_OW_MAP ) return 0;

        uint8_t p = ow_id_map[id];

        if( p >= SERVANT_NTEMP ) return 0;

        *val = currTemperature[p];

        return 1;
    }

#endif

    if(modbus_debug) printf("Modbus read - unknown register %u\n", nReg );
    return 0;
}




#define WRITE_ONE(__id_, __code_ ) do {  if( nReg == (__id_) ) { __code_ ; return 0; } } while(0)


// return modbus err no or 0
int modbus_write_register( uint16_t nReg, uint16_t value )
{

    WRITE_ONE( MB_REG_IO+0, dio_write_port( 1, value ) );
    WRITE_ONE( MB_REG_IO+1, dio_write_port( 3, value ) );
    WRITE_ONE( MB_REG_IO+2, dio_write_port( 4, value ) );
    WRITE_ONE( MB_REG_IO+3, dio_write_port( 5, value ) );

#if SERVANT_NPWM > 0
    if( INRANGE( nReg, MB_REG_AO, MB_COUNT_AO ) )
    {
        unsigned int id = nReg-MB_REG_AO;

        if( id >= SERVANT_NPWM ) goto err;
        set_an( (unsigned char)id, (unsigned char) value);
        return 0;
    }
#endif

err:
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}







