#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "modbus.h"
#include "modbus_reg.h"
//#include "defs.h"
//#include "util.h"
//#include "uart.h"
//#include "temperature.h"

#include "hex_util.h"

// TODO address in response! 2 hex bytes

/*

$AAP1   !AA             Set proto
$AAP1 - switch to modbus
$AAP	!AASC           Reads the Protocol

#** - sync sampling broadcast cmd '#**[CHKSUM](CR)', read with '$AA4', -> '>AAS(Data)[CHKSUM](CR) ' S = 1 if read firts time
~** - host heartbeat

$AAF    !AA(Data)       Reads the Firmware Version
$AAM    !AA(Data)       Reads the Module Name

~AAO(Name)      !AA     Sets the Module Name

$AA2    !AANNTTCCFF     Reads the Module Configuration
%AANNTTCCFF     !AA     Set Module Configuration

NN - hex new address
TT - set type, ?
CC - &0x3F=baud 03=1200...07=19200 08=38400..57600..  0A=115200..230400..460800..921600
FF - format


error responce ?AA


$AAEM   !AAXXXXXXXXXXXX MAC addr - ROM ID?!
$AAEI IP
$AAES subnet
$AAEG gateway
$AAEP port

$AAEP33 set port 33


*/


#if HAVE_DCON

static uint16_t dcon_read_int( unsigned char *cp, uint16_t len );

static void dcon_send_all_ins(void);
static void dcon_send_in(uint16_t nIn);


// Protocol
void dcon_make_pkt( char prefix, uint8_t *data );


static void dcon_send_error(void);
static void dcon_send_ack(void);


static uint8_t dcon_checksum(unsigned char *p, uint8_t len)
{
    uint8_t res = 0;
    while( len-- )
        res += *p++;

    return res;
}

uint8_t is_dcon_frame( uint8_t *cp, uint8_t len )
{
    if( cp[len-1] != 0x0D )
        return 0;

    switch( *cp )
    {
    case '$':
    case '#':
    case '%':
    case '@':
    case '*':
        break;

    default:
        return 0;
    }

    uint8_t recv_crc = fromhex( cp+len-3 );

    uint8_t calc_crc = dcon_checksum( cp, len - 3 );

    if( calc_crc != recv_crc )
    {
        modbus_crc_cnt++;
        return 0;
    }

    return 1;
}


void dcon_process_pkt( uint8_t *cp, uint8_t len )
{
    uint8_t node_addr = fromhex( cp+1 );
    uint8_t cmd_prefix = cp[0];

    // TODO ** is broadcast too
    if( (node_addr != modbus_our_address) && (node_addr != 0) )
        return;

    cp += 3; // skip prefix & addr
    len -= 3;

    len -= 3; // chk and 0x0D at end

    switch( cmd_prefix )
    {

    case '#':
        if( len == 0 )
            dcon_send_all_ins();
        else if( len == 1 )
        {
            uint8_t nIn = nibble_from_hex( *cp );
            dcon_send_in(nIn);
        }
        else
        {
            // analogue out - just integer
            uint8_t nOut = nibble_from_hex( *cp );
            uint16_t data = dcon_read_int( cp+1, len-1 );
            // TODO aout
        }
        return;

    case '@':
        { // dig outs
            if( len == 0 ) // read dig in
            {
                dcon_make_pkt( '>', "00" ); // TODO di
                return;
            }
            else
            {
                uint8_t dout = fromhex( cp ); // TODO do
                dcon_make_pkt( '>', 0 );
                return;
            }
        }
        break;

    case '$':
        { // $aaB
            //            if( (len == 1) && (*cp == 'B') )
            //                dcon_send_channel_errors();

            // $aaF - fw ver
            if( (len == 1) && (*cp == 'F') )
            {
                dcon_make_pkt( '!', FW_VER_STRING ); // TODO get ver through modbus
                return;
            }

            // $aaM - module name
            if( (len == 1) && (*cp == 'M') )
            {
                dcon_make_pkt( '!', DEVICE_NAME );
                return;
            }

            // $aaP - proto info
            if( (len == 1) && (*cp == 'P') )
            {
                dcon_make_pkt( '!', "10" ); // RTU
                return;
            }
        }
        break;

    case '~':
        {
            // $aaO - set name
            if( (len > 1) && (len < 17) && (*cp == 'O') )
            {
                char name[17];
                strlcpy( name, cp+1, len-1 );
                // TODO dev name, eeprom, modbus, menu
                dcon_send_ack();
                return;
            }
        }
        break;

    }

    dcon_send_error();

}


// send packet in modbus_tx_buf of length tx_len
// bytes 1 & 2 in buf must be empty (space char) to fill in our address
void dcon_send_pkt()
{
    uint8_t lrc = dcon_checksum( modbus_tx_buf, tx_len );

    uint8_t lrc_cr[4];

    hex_encode( modbus_tx_buf+1, modbus_our_address ); // Put in our address

    hex_encode( lrc_cr, lrc );

    lrc_cr[2] = 0x0D;
    lrc_cr[3] = 0;

    strlcat( (char *)modbus_tx_buf, (const char *)lrc_cr, MODBUS_MAX_TX_PKT );

    tx_len += 3;

    modbus_start_tx();
    modbus_event_cnt++; // wrong - must not count exception packets?
}


void dcon_make_pkt( char prefix, uint8_t *data )
{
    modbus_tx_buf[0] = prefix;

    if( data )
        strlcpy( (char *)modbus_tx_buf+3, (const char *)data, MODBUS_MAX_TX_PKT-6 ); // 4 bytes for prefix, addr*2, ... lrc*2, cr

    tx_len = strlen( (const char *)modbus_tx_buf );

    dcon_send_pkt();
}


void dcon_send_error(void)
{
    modbus_exceptions_cnt++;
    dcon_make_pkt( '?', 0 );
}

void dcon_send_ack(void)
{
    dcon_make_pkt( '!', 0 );
}


#define MAX_DCON_INT 10

static uint16_t dcon_read_int( unsigned char *cp, uint16_t len )
{
    char buf[MAX_DCON_INT+1];

    if( len > MAX_DCON_INT ) len = MAX_DCON_INT; // wrong, but, well, don't use 10 char ints!

    strncpy( (char *)buf, (const char *)cp, len );
    buf[len] = 0;

    return atoi( buf );
}


void dcon_send_all_ins(void)
{
    //int left = MODBUS_MAX_TX_PKT-4; // prefix,  2bytes lrc, cr
    char *cp = (char *)modbus_tx_buf;

    uint8_t i;

    *cp++ = '>';
    *cp++ = ' '; // for addr
    *cp++ = ' ';
    *cp = 0;

    int last_good_len = 0;

    for( i = 0; i < 64; i++ )
    {
        int pos = find_pos_by_persistent_id( i );

        if( (pos < 0) || (pos >= nTempSensors) )
        {
            //strlcat( cp, "+?", MODBUS_MAX_TX_PKT-4 );
            strlcat( cp, "-999.90", MODBUS_MAX_TX_PKT-6 );
        }
        else
        {
            char buf[MAX_DCON_INT*2];
            itoa( currTemperature[pos], buf, 10 );

            strlcat( cp, "+", MODBUS_MAX_TX_PKT-6 );
            strlcat( cp, buf, MODBUS_MAX_TX_PKT-6 );

            last_good_len = strlen(cp);
        }
    }

    // Cut off tail of unknown ones
    //if( last_good_len > 0 )        cp[last_good_len] = 0;

    tx_len = strlen( (const char *)modbus_tx_buf );

    dcon_send_pkt();
}


void dcon_send_in(uint16_t nIn)
{
    uint16_t val;

    uint8_t ok = modbus_read_register( nIn, &val );

    char buf[MAX_DCON_INT*2];

    if(ok)
        itoa( val, buf, 10 );
    else
    {
        buf[0] = '?';
        buf[1] = 0;
    }

    dcon_send_pkt('>', buf);
}


#endif // HAVE_DCON
