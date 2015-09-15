#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "defs.h"

char hexdigit( unsigned char d );


void delay_sec(void);
void delay_halfsec(void);
void delay_qsec(void);   // 1/4 sec
void delay_esec(void);   // 1/8 sec
void delay_thsec(void);  // 1/16 sec


void led_ddr_init(void);

void flash_led(uint8_t count);
void flash_led_once(void);
void flash_led_long(void);

//void turn_led_on(void);
//void turn_led_off(void);

#define LED_HI (LED_PORT |= _BV(LED))
#define LED_LOW (LED_PORT &= ~_BV(LED))

#define LED_INVERTED 1

#if LED_INVERTED
#define LED_ON LED_LOW
#define LED_OFF LED_HI
#else
#define LED_ON LED_HI
#define LED_OFF LED_LOW
#endif




#define DEBUG_PUTS(msg) { static const prog_char s[] = msg;  puts_P(s); }

