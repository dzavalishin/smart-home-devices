#include <inttypes.h>

char hexdigit( unsigned char d );
void hex_encode( unsigned char *dest, unsigned char src );

uint8_t nibble_from_hex( uint8_t h );
uint8_t fromhex( unsigned char *src );

