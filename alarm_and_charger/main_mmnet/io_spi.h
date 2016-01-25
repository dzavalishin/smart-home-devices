/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * SPI IO.
 *
**/

#include "defs.h"
#include "dev_map.h"


extern dev_major io_spi;

//
// ss   - slave select number
//
// cmd1. cmd2 - bytes to send
//
//
//

uint16_t spi_send( unsigned char ss, unsigned char cmd1, unsigned char cmd2 );

extern uint8_t spi_do0;
extern uint8_t spi_do1;

#define N_SPI_SLAVE_PWM 10
extern uint8_t spi_slave_pwm[N_SPI_SLAVE_PWM];

// ----------------------------------------------------------------------
// IO map
// ----------------------------------------------------------------------
//
// Map of SPI devices.
//
// ----------------------------------------------------------------------

// 2 serial registers connevted to 1602 LCD
// 1st byte - control bits, 2nd - data byte
#define SPI_SS_LCD      0       


// PWM child, attiny2313 based
// 1st byte - register number, 2nd - data, register number is PWM out number 0-4, 4 is USART pseudo-pwm
#define SPI_SS_2313_A   2
#define SPI_SS_2313_B   3

// General data outputs, 1st byte - ? 595 register, 2nd byte - ? TPIC595 LED/relay driver
#define SPI_SS_DO       4

// General data inputs, NB!, - needs clock with SS turned off to load data to register
#define SPI_SS_DI       5




