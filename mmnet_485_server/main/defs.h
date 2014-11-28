#define TEST_PWM 0




//definitions

#if 1

// ------------------------------------------------------------------------
// MMnet101
// ------------------------------------------------------------------------

// We have 16 MHz on MMNet101
#define F_CPU     16000000

#define LED_EXCLPOS  6
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

#define ALARM_OUTPUTS_USED 0

#if ALARM_OUTPUTS_USED
// TODO - WRONG!
#define	SERVANT_ALARM_PIN	0	//PB0
#define	SERVANT_WARN_PIN	1	//PB1

#define SERVANT_ALARM_PORT      PORTB
#define SERVANT_WARN_PORT       PORTB

#define SERVANT_ALARM_EXCLPOS   1
#define SERVANT_WARN_EXCLPOS    1

#endif


//#define	SERVANT_NPWM	4	// количество аналоговых выходов -- PWM
#define	SERVANT_NPWM	0	// количество аналоговых выходов -- PWM

#define	SERVANT_NADC	0	// количество АЦП
#define	SERVANT_NDIG	0	// количество цифровых портов (входы)
#define	SERVANT_NDIGOUT	0	// количество цифровых портов - вывод



// FREQ/PWM duty inputs
// pos 0 is freq for first channel, 1 is duty cycle for first channel
// pos 2 is freq for second channel, 3 is duty cycle for it

#define SERVANT_NFREQ 0


#define	SERVANT_PWM_SPEED	10	// количество машинных циклов на один интервал PWM
						// период модуляции разделяется на 256 интервалов
						// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.

#define DEVICE_NAME "DZ128A-MMNET"

#define N_TEMPERATURE_IN 0
//#define N_TEMPERATURE_IN 0

#define ENABLE_SPI 0

// 1-wire child processor
#define ENABLE_CH1W 0

#define HALF_DUPLEX 0

// OneWire
#define OW_EXCLPOS  6 // Port G
#define OW_PIN  PG4 // TSC1
#define OW_IN   PING
#define OW_OUT  PORTG
#define OW_DDR  DDRG
#define OW_CONF_DELAYOFFSET 0














#else


#ifdef __AVR_ATmega128__

// ------------------------------------------------------------------------
// Atmega 128
// ------------------------------------------------------------------------


#define ALARM_OUTPUTS_USED 1

#if ALARM_OUTPUTS_USED
// TODO - WRONG!
#define	SERVANT_ALARM_PIN	0	//PB0
#define	SERVANT_WARN_PIN	1	//PB1

#define SERVANT_ALARM_PORT      PORTB
#define SERVANT_WARN_PORT       PORTB

#define SERVANT_ALARM_EXCLPOS   1
#define SERVANT_WARN_EXCLPOS    1

#endif


#define	SERVANT_NPWM	4	// количество аналоговых выходов -- PWM

#define	SERVANT_NADC	8	// количество АЦП
#define	SERVANT_NDIG	7	// количество цифровых портов

#define	SERVANT_PWM_SPEED	10	// количество машинных циклов на один интервал PWM
						// период модуляции разделяется на 256 интервалов
						// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.

#define DEVICE_NAME "DZ128A-USB"

#define N_TEMPERATURE_IN 8


#define ENABLE_SPI 1

#define HALF_DUPLEX 0

// OneWire
#define OW_EXCLPOS  6 // Port G
#define OW_PIN  PG2
#define OW_IN   PING
#define OW_OUT  PORTG
#define OW_DDR  DDRG
#define OW_CONF_DELAYOFFSET 0





#else

// ------------------------------------------------------------------------
// Atmega 8
// ------------------------------------------------------------------------






#define ALARM_OUTPUTS_USED 0

#if ALARM_OUTPUTS_USED
// TODO - WRONG!
#define	SERVANT_ALARM_PIN	0	//PB0
#define	SERVANT_WARN_PIN	1	//PB1

#define SERVANT_ALARM_PORT      PORTB
#define SERVANT_WARN_PORT       PORTB

#define SERVANT_ALARM_EXCLPOS   1
#define SERVANT_WARN_EXCLPOS    1

#endif


//#define	SERVANT_NPWM	4	// количество аналоговых выходов -- PWM
#define	SERVANT_NPWM	0	// количество аналоговых выходов -- PWM

#define	SERVANT_NADC	6	// количество АЦП
#define	SERVANT_NDIG	7	// количество цифровых портов - ввод
#define	SERVANT_NDIGOUT	7	// количество цифровых портов - вывод

#define	SERVANT_PWM_SPEED	10	// количество машинных циклов на один интервал PWM
						// период модуляции разделяется на 256 интервалов
						// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.


// NB! TODO - disabling temp code stops hardware serial from working too.

//#define N_TEMPERATURE_IN 8
#define N_TEMPERATURE_IN 2


#define ENABLE_SPI 0

#define DEVICE_NAME "DZ8A-485"



#define HALF_DUPLEX 1
#define HALF_DUPLEX_EXCLPOS 3 // port d
#define HALF_DUPLEX_PORT PORTD
#define HALF_DUPLEX_PIN PD6
#define HALF_DUPLEX_DDR DDRD

// OneWire
#define OW_EXCLPOS  1 // Port B
#define OW_PIN  PB1
#define OW_IN   PINB
#define OW_OUT  PORTB
#define OW_DDR  DDRB
#define OW_CONF_DELAYOFFSET 0




#endif

#endif



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
