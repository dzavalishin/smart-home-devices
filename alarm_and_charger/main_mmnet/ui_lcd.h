/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * UI: LCD funcs.
 *
**/

#include <inttypes.h>


void lcd_init(void);


void lcd_putc( char c );
void lcd_puts( char* str );
void lcd_puti( int i );

void lcd_puthex( const char *bp, int cnt );

void lcd_put_temp( uint16_t temp );


void lcd_gotoxy( int pos, int line );
void lcd_clear( void );
