#ifndef UTIL_H
#define UTIL_H

#include <inttypes.h>
/*
#if 1

#include "uart.h"
#define DEBUG_PUTS(msg) { static const prog_char s[] = msg;  uart_puts_p(s); }
#define DEBUG_PUTX(i) uart_puthex_byte(i)
#else

#include "lcd.h"
#define DEBUG_PUTS(msg) { static const prog_char s[] = msg;  lcd_puts_p(s); }
#define DEBUG_PUTX(i) lcd_puthex_byte(i)
#endif
*/

char hexdigit( unsigned char d );

//void delay_ms(unsigned char ms);


void delay_sec();
void delay_halfsec();
void delay_qsec(); // 1/4 sec
void delay_esec(); // 1/8 sec

void flash_led(uint8_t count);
void flash_led_once();
void flash_led_long();

void signal_led();

# if 0
void turn_led_on(void);
void turn_led_off(void);

void __inline__ turn_led_on()
{
    LED_PORT |= _BV(LED);
    //LED_PORT &= ~_BV(LED);
}


void __inline__ turn_led_off()
{
    //LED_PORT |= _BV(LED);
    LED_PORT &= ~_BV(LED);
}
#else

#define turn_led_on()  (LED_PORT |= _BV(LED))
#define turn_led_off() (LED_PORT &= ~_BV(LED))

#endif

#endif // UTIL_H

