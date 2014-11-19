//#################################################################################
//	Library to use BMP180 with ATMEL Atmega family
//#################################################################################

#ifndef _ATMEGA_BMP180_LIB_H_
#define _ATMEGA_BMP180_LIB_H_

#include <avr/io.h>
#include <util/delay.h> 
#include <stdio.h>
//#include <util/twi.h>
#include <math.h>

//----------------------------------------
#define OSS 3
//----------------------------------------

#define BMP180_R 0xEF
#define BMP180_W 0xEE



uint8_t bmp180ReadPressure( int32_t* pressure );
uint8_t bmp180ReadTemp( int16_t *temperature );

uint8_t bmp180Calibration( void )
uint8_t bmp180Convert( int32_t *temperature, int32_t *pressure );

//int32_t bmp180CalcAltitude(int32_t pressure);



//void BMP180_Calibration(int16_t BMP180_calibration_int16_t[],int16_t BMP180_calibration_uint16_t[], uint8_t* errorcode);
//uint16_t bmp180ReadShort(uint8_t address, uint8_t* errorcode);
//int32_t bmp180ReadTemp(uint8_t* error_code);
//int32_t bmp180ReadPressure(uint8_t* errorcode);
//void bmp180Convert(int16_t BMP180_calibration_int16_t[],int16_t BMP180_calibration_uint16_t[],int32_t* temperature, int32_t* pressure, uint8_t* error_code);

//int32_t bmp180CalcAltitude(int32_t pressure);
#endif
