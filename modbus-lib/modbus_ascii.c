#include <inttypes.h>
#include <string.h>

//#include "defs.h"
//#include "util.h"
#include "modbus.h"
#include "hex_util.h"

#if HAVE_MODBUS_ASCII



uint8_t is_modbus_ascii_frame( unsigned char *cp, uint16_t len)
{
    return (cp[len-1] == 0x0A) && (cp[len-2] == 0x0D) && (*cp == ':');
}


uint8_t modbus_ascii_lrc( unsigned char *cp, uint16_t len )
{
    uint8_t sum = 0;

    while( len-- )
    {
        sum += *cp++;
    }

    return (uint8_t) -((int8_t)sum);

}

uint8_t check_modbus_ascii_lrc( unsigned char * cp, uint16_t len )
{
    uint8_t calc = modbus_ascii_lrc( cp, len-1 );
    return calc == cp[len-1];
}


uint8_t modbus_ascii_decode( unsigned char *cp, uint16_t len )
{
    // Eat all controls at end
    while(  cp[len-1] < ' ' )
        len--;

    uint8_t i;

    for( i = 0; i < len/2; i++ )
    {
        cp[i] = fromhex( cp+1+(i*2) );
    }

    return len/2;
}




uint8_t modbus_ascii_encode( unsigned char *cp, uint16_t len )
{
    unsigned char tmp[ len*2];

    if( len*2 + 5 > MODBUS_MAX_TX_PKT ) return 0;

    uint8_t lrc = modbus_ascii_lrc(cp,len);

    uint8_t i;

    for( i = 0; i < len; i++ )
    {
        hex_encode( tmp + i*2, cp[i] );
    }

    strncpy( (char *)cp + 1, (const char *)tmp, len*2 );

    cp[0] = ':';

    hex_encode( cp + len*2 + 1, lrc );

    cp[len*2+3] = 0x0D;
    cp[len*2+4] = 0x0A;

    return len * 2 + 5;
}


#endif // HAVE_MODBUS_ASCII

