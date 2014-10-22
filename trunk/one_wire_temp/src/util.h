#include <inttypes.h>

char hexdigit( unsigned char d );
void hex_encode( unsigned char *dest, unsigned char src );

uint8_t nibble_from_hex( uint8_t h );
uint8_t fromhex( unsigned char *src );




void delay_sec();
void delay_halfsec();
void delay_qsec(); // 1/4 sec
void delay_esec(); // 1/8 sec

void flash_led(uint8_t count);
void flash_led_once();
void flash_led_long();

void turn_led_on(void);
void turn_led_off(void);


void flash_led2(uint8_t count);
void flash_led2_once();

void turn_led2_on(void);
void turn_led2_off(void);


#define flash_led1 flash_led
#define flash_led1_once flash_led_once

#define turn_led1_on turn_led_on
#define turn_led1_off turn_led_off


// returns immediately, timer turns off
void led1_timed( uint16_t msec );
void led2_timed( uint16_t msec );





#define debug_puts lcd_puts
#define debug_putc lcd_putc

void lcd_putc(char c);
void lcd_puts(char*str);
void lcd_puti(int i);
void lcd_puthex( const char *b, int len);

//void lcd_set_cursor(int line,int pos);

void lcd_gotoxy(int x, int y);
void lcd_clear();

void lcd_put_temp( uint16_t temp );

