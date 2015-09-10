#include "hex_util.h"


// --------------------------------------------------------------------------
// Formatting
// --------------------------------------------------------------------------

char hexdigit( unsigned char d )
{
    d &= 0xF;

    if( d < 0xa )
        return '0' + d;

    return 'A'+d-0xa;
}

void hex_encode( unsigned char *dest, unsigned char src )
{
    dest[0] = hexdigit( (src) >> 4 );
    dest[1] = hexdigit( src );
}





uint8_t nibble_from_hex( uint8_t h )
{
    if( h >= '0' && h <= '9' ) return h - '0';
    if( h >= 'a' && h <= 'f' ) return h - 'a' + 10;
    if( h >= 'A' && h <= 'F' ) return h - 'A' + 10;

    return 0;
}

uint8_t fromhex( unsigned char *src )
{
    return ( nibble_from_hex( src[0] ) << 4 ) | nibble_from_hex( src[1] );
}


