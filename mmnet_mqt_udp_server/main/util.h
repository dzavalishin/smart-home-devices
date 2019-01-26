#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <io.h>

// log levels - move here?
#include <sys/syslog.h>


#include "defs.h"

#define push_cli() uint8_t __save_SREG = SREG; cli();
#define pop_sti() SREG = __save_SREG;


void lcd_status_line(const char *msg);



void log_init( void );
void log_syslog_init( void );

#if ENABLE_LOGBUF
void log_puts( const char *data );
#else
#  define log_puts(...)
#endif

#if ENABLE_SYSLOG

#if 0
void syslog(int pri, const char *fmt, ...);
void vsyslog(int pri, const char *fmt, va_list ap);

#else

void mk_syslog_header(int pri);
void log_put( const char *data, uint16_t len );


#define syslog( __pri, __fmt, ... ) do {\
    mk_syslog_header(__pri); 					\
    char ___buf[128];                                           \
    if(__fmt)                                                   \
    {                                                           \
        int len = snprintf( ___buf, sizeof(___buf) - 1, __fmt, __VA_ARGS__ ); \
        if( len > 0 )                                           \
            log_put( ___buf, len );                             \
    }                                                           \
    } while(0)

#endif


#else

#define syslog( __pri, __fmt, ... ) printf( __fmt "\n", __VA_ARGS__ )

#endif




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

void fail_led(void); // debug


#define LED_HI (LED_PORT |= _BV(LED))
#define LED_LOW (LED_PORT &= ~_BV(LED))

#ifdef FAIL_LED_EXCLPOS
#  define FAIL_LED_HI (FAIL_LED_PORT |= _BV(FAIL_LED))
#  define FAIL_LED_LOW (FAIL_LED_PORT &= ~_BV(FAIL_LED))
#else
#  define FAIL_LED_HI 
#  define FAIL_LED_LOW 
#endif

#define LED_INVERTED 1

#if LED_INVERTED
#define LED_ON LED_LOW
#define LED_OFF LED_HI
#define FAIL_LED_ON FAIL_LED_LOW
#define FAIL_LED_OFF FAIL_LED_HI
#else
#define LED_ON LED_HI
#define LED_OFF LED_LOW
#define FAIL_LED_ON FAIL_LED_HI
#define FAIL_LED_OFF FAIL_LED_LOW
#endif




#define DEBUG_PUTS(msg) { static const prog_char s[] = msg;  puts_P(s); }

