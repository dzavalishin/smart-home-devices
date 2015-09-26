/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Global configuration definitions.
 *
**/

// ------------------------------------------------------------------------
// Configuration (on/off/number) of ins/outs
// ------------------------------------------------------------------------

#define	SERVANT_NPWM	4	// Analogue outputs (PWM) count
#define	SERVANT_NADC	8	// количество аналоговых входов

#define	SERVANT_NDIG	7	// количество цифровых портов (входы)  - bytes
#define	SERVANT_NDIGOUT	0	// количество цифровых портов (выходы) - bytes

#define	SERVANT_DHT11   1       // Have or not DHT11/22

#define	SERVANT_BMP180  1       // Have or not BMP180 pressure/temp meter

// FREQ/PWM duty inputs
// pos 0 is freq for first channel, 1 is duty cycle for first channel
// pos 2 is freq for second channel, 3 is duty cycle for it

#define SERVANT_NFREQ 	0

#define SERVANT_NTEMP   8       // 1-Wire temperature sensors
#define SERVANT_1WMAC   1       // Attempt to get MAC address from 1-wire 2401 chip
#define OW_ONE_BUS      1

// RS232 on dev board
#define SERVANT_TCP_COM0        0
// USB serial on dev board
#define SERVANT_TCP_COM1        0

// Does not fit to code flash mem :(
#define SERVANT_LUA     0       // Have Lua scripting


// ------------------------------------------------------------------------
// Configuration (on/off/number) of components
// ------------------------------------------------------------------------

// SNTP/Syslog kills us :(
#define ENABLE_SNTP 1
#define ENABLE_SYSLOG 0

#define ENABLE_1WIRE ((SERVANT_NTEMP)||(SERVANT_1WMAC))

#define ENABLE_TWI SERVANT_BMP180

// Unused
#define ENABLE_SPI 0
#define PB_SS_PIN (_BV(PB0))


#define ENABLE_HALF_DUPLEX_0    1
#define ENABLE_HALF_DUPLEX_1    1


//#ifdef OW_ONE_BUS
//#define OW_PIN  PB0
//#endif

#ifdef OW_ONE_BUS
#	define N_1W_BUS 1
#else
#	define N_1W_BUS 8
#endif


// ------------------------------------------------------------------------
// Network
// ------------------------------------------------------------------------



/* These values are used if there is no valid configuration in EEPROM. */

#define DEFAULT_MAC   0x02, 0x06, 0x98, 0x00, 0x00, 0x00
#define DEFAULT_IP    "192.168.88.127"
#define DEFAULT_MASK  "255.255.0.0"

// For sntp time setup
#define DEFAULT_TZ    -3

#define DEFAULT_SYSLOGD  "192.168.1.130"
//#define MYSYSLOGD  "syslog."


// ------------------------------------------------------------------------
// Hardware connections
// ------------------------------------------------------------------------

#define LED_EXCLPOS  6 // Port G
#define LED_DDR  DDRG
#define LED_PORT PORTG
#define LED_PIN  PING
#define LED      PING3

#if 0

#define FAIL_LED_EXCLPOS  1
#define FAIL_LED_DDR  DDRB
#define FAIL_LED_PORT PORTB
#define FAIL_LED_PIN  PINB
#define FAIL_LED      PINB6

#endif

#define HALF_DUPLEX0_EXCLPOS 1 // Port B
#define HALF_DUPLEX0_DDR DDRB
#define HALF_DUPLEX0_PORT PORTB
#define HALF_DUPLEX0_PIN PINB6

#define HALF_DUPLEX1_EXCLPOS 1 // Port B
#define HALF_DUPLEX1_DDR DDRB
#define HALF_DUPLEX1_PORT PORTB
#define HALF_DUPLEX1_PIN PINB7

// OneWire - TODO port is hardcoded! def below is unused
#define OW_EXCLPOS  6 // Port G
#define OW_PIN  PG4 // TSC1
#define OW_IN   PING
#define OW_OUT  PORTG
#define OW_DDR  DDRG
#define OW_CONF_DELAYOFFSET 0


// DHT11/22 port
#define DHT_EXCLPOS  1 // Port B
#define DHT_DDR DDRB
#define DHT_PORT PORTB
#define DHT_PIN PINB
#define DHT_INPUTPIN PB4


#define ETHERNET_INTERRUPT_EXCLPOS      4 // Port E
#define ETHERNET_INTERRUPT_PIN 		PE5

#define DATAFLASH_EXCLPOS      		1 // Port B
#define DATAFLASH_SCK_PIN 		PB1
#define DATAFLASH_MOSI_PIN 		PB2
#define DATAFLASH_MISO_PIN 		PB3
#define DATAFLASH_CS_PIN 		PB0


// ------------------------------------------------------------------------
// Hardware definitions
// ------------------------------------------------------------------------



// We have 16 MHz on MMNet101
#define F_CPU     16000000


// количество машинных циклов на один интервал PWM
// период модуляции разделяется на 256 интервалов
// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.
#define	SERVANT_PWM_SPEED	10	
#define	SERVANT_PWM_PORT        PORTD
// PWM bits start at this bit, so now it's PD4..7
#define SERVANT_PWM_BIT         4

#define MODBUS_DEVICE_ID "DZMM00"
extern char modbus_device_id[];

#define DEVICE_NAME "DZ-MMNET-MODBUS"
//#define DEVICE_REV "$Revision: 0 $"
//#define DEVICE_DATE "$Date: $"


extern char *makeDate; // filled in by makefile

//#define HALF_DUPLEX 0


// On 0 we conflict with OS?
#define EEPROM_CFG_BASE 1024

























// ------------------------------------------------------------------------
// CPU freq macros
// ------------------------------------------------------------------------



#if   defined F3686400
#define F_CPU     3686400
#elif defined F7372800
#define F_CPU     7372800
#elif defined F8000000
#define F_CPU     8000000
#elif defined F14745600
#define F_CPU     14745600
#elif defined F16000000
#define F_CPU     16000000
#elif defined F20000000
#define F_CPU     20000000
#endif

#define F_OSC F_CPU
