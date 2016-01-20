//definitions


#define DEVICE_NAME "SPI-PWM-2313"


/*
#define HALF_DUPLEX 1

#define HALF_DUPLEX_PORT PORTD
#define HALF_DUPLEX_DDR DDRD
#define HALF_DUPLEX_PIN PIND4
*/




#ifdef __AVR_ATtiny2313__

//#define LED_EXCLPOS  1
#define LED_DDR  DDRD
#define LED_PORT PORTD
//#define LED_PIN  PIND
#define LED      PIND6

#endif




// We have 8 MHz on Atmega8 and tiny2313
#define F_CPU     8000000
#define F_OSC F_CPU
