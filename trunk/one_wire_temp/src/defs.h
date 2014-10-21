#ifndef DEFS_H
#define DEFS_H

#include "freq_defs.h"

// ------------------------------------------------------------------------
//
// Platform definitions
//
// Device supposed to be Atmega 32
//
// ------------------------------------------------------------------------

#define DEVICE_NAME "DZ1W-485"
#define FW_VER_STRING "0.10"

#define USE_WDOG 0




// ------------------------------------------------------------------------
// Number and existance of IO
// ------------------------------------------------------------------------


#define OW_SERIAL_ID    0       // Read unique serial ID from DS2401 chip
#define	SERVANT_NPWM	0	// No of analogue outputs -- PWM
#define	SERVANT_NADC	8	// No of ADC
#define	SERVANT_NCNT	0	// Max no of counters
#define SERVANT_NDI     0       // Max no of dig ins
#define N_TEMPERATURE_IN 32     // Max no of 1-wire temperature sensors


// Tacts per pwm cycle
// PWM freq = CPU clock/256/SERVANT_PWM_SPEED

//#define	SERVANT_PWM_SPEED	10


#if OW_SERIAL_ID && !N_TEMPERATURE_IN
#	warning OW_SERIAL_ID wont work without N_TEMPERATURE_IN
#endif




// ------------------------------------------------------------------------
// Serial IO defs
// ------------------------------------------------------------------------

//#define BAUD 9600
//#define BAUD 38400

#define DEFAULT_UART_BAUD 38400

#define HALF_DUPLEX 1
#define HALF_DUPLEX_PORT PORTD
#define HALF_DUPLEX_PIN PD4
#define HALF_DUPLEX_DDR DDRD



#define MODBUS_DEFAULT_ADDRESS 16

#define HAVE_MODBUS_ASCII 1
#define HAVE_MODBUS_RTU 1
#define HAVE_DCON 0


// ------------------------------------------------------------------------
// OneWire
// ------------------------------------------------------------------------

//#define OW_ONE_BUS


#ifdef OW_ONE_BUS
#define OW_PIN  PB0
#endif


#define OW_IN   PINB
#define OW_OUT  PORTB
#define OW_DDR  DDRB
//#define OW_CONF_DELAYOFFSET 2
//#define OW_CONF_DELAYOFFSET 4



// ------------------------------------------------------------------------
// Onboard LEDs are connected to pins PD2 & PD3
// ------------------------------------------------------------------------


#define LED1_DDR  DDRD
#define LED1_PORT PORTD
#define LED1_PIN  PIND
#define LED1      PIND2

#define LED2_DDR  DDRD
#define LED2_PORT PORTD
#define LED2_PIN  PIND
#define LED2      PIND3


#endif // DEFS_H
