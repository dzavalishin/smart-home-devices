#include <inttypes.h>

void lcd_init( void );

void lcd_putc( char c );
void lcd_puts( const char* str );
void lcd_puti( int i );
void lcd_putx( char c );

void lcd_puthex( const char *b, int len );

// fixed *100
void lcd_putf( int f );

//void lcd_set_cursor(int line,int pos);

void lcd_gotoxy(int x, int y);
void lcd_clear( void );

void lcd_put_temp( uint16_t temp );
