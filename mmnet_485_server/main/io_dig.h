#include "defs.h"

void 		dio_init(void);


unsigned char   dio_read_port( unsigned char port );
void	   	dio_write_port( unsigned char port, unsigned char bits );
void   		dio_set_port_ouput_mask( unsigned char port, unsigned char mask ); // 1 = output
unsigned char   dio_get_port_ouput_mask( unsigned char port ); // 1 = output

unsigned char   dio_read_port_bit( unsigned char port, unsigned char nBit );
void   		dio_write_port_bit( unsigned char port, unsigned char nBit, unsigned char value );
void    	dio_set_port_ouput_mask_bit( unsigned char port, unsigned char nBit, unsigned char value ); // 1 = output
unsigned char   dio_get_port_ouput_mask_bit( unsigned char port, unsigned char nBit ); // 1 = output


void set_half_duplex0( char val );
void set_half_duplex1( char val );


void add_exclusion_mask( unsigned char exclPos, unsigned char bitmask );
#define add_exclusion_pin(p,b) add_exclusion_mask(p,_BV(b))



//end
