/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * BMP180 pressure and temperature meter.
 *
**/

#include "defs.h"
#include <inttypes.h>

#if SERVANT_BMP180

/*
uint8_t bmp180ReadPressure( int32_t* pressure );
int bmp180ReadTemp( int16_t *temperature );
*/

int bmp180_calibration( void );

int bmp180_getdata( void );


extern int32_t bmp180_temperature;
extern int32_t bmp180_pressure;
extern uint16_t bmp180_pressure_mmHg;

// Unconverted data from sensor
//extern int16_t bmp180_temperature_raw;
//extern int32_t bmp180_pressure_raw;

#endif // SERVANT_BMP180

