
#include "i2c.h"


void init_all( void )
{
    uint8_t rc = 0;

    // TODO ports DDR

    // TODO rs485 & modbus



    // I2C

    i2cInit();
    i2cSetBitrate( 100 ); // We're ok with 100K

    // BMP180 (uses I2C)

    rc = bmp180Calibration();

    // TODO report BMP180 failure

}

