/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * LCD: SPI data exchange queue.
 *
**/

#include <inttypes.h>



uint8_t 	lcd_q_empty( void );
uint8_t		lcd_q_full( void );

//
// structure is XXXX XXXC DDDD DDDD
//
// D - data
// C - register select
//

void 		lcd_q_put( uint16_t data );
uint16_t 	lcd_q_get( void );

static inline void	lcd_q_put_data( uint8_t data ) { lcd_q_put( data ); }
static inline void	lcd_q_put_ctrl( uint8_t control ) { lcd_q_put( control | 0x0100 ); }


