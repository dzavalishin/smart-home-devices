/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Digital I/O.
 *
**/

#ifndef IO_DIG_H
#define IO_DIG_H

#include "defs.h"
#include "map.h"

void 		dio_init(void);


unsigned char   dio_read_port( unsigned char port );
//void	   	dio_write_port( unsigned char port, unsigned char bits );
//void   		dio_set_port_ouput_mask( unsigned char port, unsigned char mask ); // 1 = output
unsigned char   dio_get_port_ouput_mask( unsigned char port ); // 1 = output

unsigned char   dio_read_port_bit( unsigned char port, unsigned char nBit );
//void   		dio_write_port_bit( unsigned char port, unsigned char nBit, unsigned char value );
//void    	dio_set_port_ouput_mask_bit( unsigned char port, unsigned char nBit, unsigned char value ); // 1 = output
unsigned char   dio_get_port_ouput_mask_bit( unsigned char port, unsigned char nBit ); // 1 = output

//void            dio_set_default_output_state( void ); // Used on start and if communications are lost

//void set_half_duplex0( char val );
//void set_half_duplex1( char val );


//void add_exclusion_mask( unsigned char exclPos, unsigned char bitmask );
//#define add_exclusion_pin(p,b) add_exclusion_mask(p,_BV(b))

extern dev_major io_dig;


#define DI_PORT PORTD
#define DI_PIN  PIND
#define DI_DDR  DDRD

#define DO_PORT PORTB
#define DO_PIN  PINB
#define DO_DDR  DDRB

#define DO_LED_MASK 0x0F


#define RELAY_1_BIT 4
#define RELAY_2_BIT 6 	// NB - soldered on 5, resolder

#define DI_1_BIT 6      // Dig in
#define DI_2_BIT 7      // Dig in


extern uint8_t         dio_front_buttons_changed;       // Front panel button in non-menu mode pressed
extern uint8_t         dio_remote_state_changed;        // We've got remote (MQTT broker) state changeg
extern uint8_t         dio_remote_state;		// Remote state as we got from MQTT
extern uint8_t         dio_state;			// Our state as we know



void mqtt_send_channel( uint8_t state, uint8_t ch );









//end
#endif // IO_DIG_H

