

#include "io_dht11.h"
#include "atmega_bmp180_lib.h"



int32_t bmp180_temperature = 0;
int32_t bmp180_pressure = 0;

uint8_t dht11_rhumidity = 0;
uint8_t dht11_temperature = 0;


void main_loop( void )
{
    uint8_t rc;

    // Read BMP180 pressure sensor

    rc = bmp180Convert( &bmp180_temperature, &bmp180_pressure );

    // TODO report failure

    // Read DHT11 humidity sensor

    rc = DHT_Read_Data();

    // TODO report failure

    if( !rc )
    {
        dht11_rhumidity = DHT_Read_RH();
        dht11_temperature = DHT_Read_Temp();
    }

}

