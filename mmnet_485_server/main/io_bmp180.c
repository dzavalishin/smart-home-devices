/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * BMP180 pressure and temperature meter.
 *
**/


#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_bmp180.h"

#include <dev/twif.h>
#include <sys/timer.h>




#if SERVANT_BMP180



#define OSS 3

#define BMP180_R 0xEF
#define BMP180_W 0xEE

#define BMP180_A7 (0xEE >> 1)

#define BMP180_REG_CR 0xF4


int bmp180ReadPressure( int32_t *pressure );
int bmp180ReadTemp( int16_t *temperature );


// ------------------------------------------------------------------------
// BMP180 I2C IO
// ------------------------------------------------------------------------



static int
bmp180_write_CR( uint8_t value )
{
#if 1
    uint8_t tx [2] = { 0xF4, value };
    NutTwiMasterTranceive( &DEF_TWIBUS, BMP180_A7, tx, 2, tx, 0, 100 );
    return 0;
#else
    int rc = NutTwiMasterRegWrite( &DEF_TWIBUS, BMP180_W,
                                 BMP180_REG_CR, 1,              // Addr and addr len
                                 &value, 1,                     // Data and data len
                                 100 ); 			// wait 100 msec
    return (rc == 1) ? 0 : -1;
#endif
}


static int
bmp180_read(uint8_t reg, void *data, int len )
{
#if 1
    uint8_t tx [1] = { reg };
    int rc = NutTwiMasterTranceive( &DEF_TWIBUS, BMP180_A7, tx, 1, data, len, 100 );
#else
    int rc = NutTwiMasterRegRead( &DEF_TWIBUS, BMP180_R, reg, 1, data, len, 100 );
#endif
    return (rc == len) ? 0 : -1;
}

/*
static int
bmp180_write(uint8_t reg, void *data, int len )
{
    int rc = NutTwiMasterRegWrite( &DEF_TWIBUS, BMP180_W,
                                reg, 1,
                                data, len,
                                  100 );
    return (rc == len) ? 0 : -1;
}
*/


static uint8_t
bmp180ReadShort(uint16_t *data, uint8_t address)
{
    uint8_t buf[2];
    int rc = bmp180_read( address, buf, sizeof(buf) );

    *data = (buf[0] << 8) + (buf[1]);

    return rc;
}


static uint8_t
bmp180ReadLong(uint32_t *data, uint8_t address)
{
    uint8_t buf[3];
    int rc = bmp180_read( address, buf, sizeof(buf) );

    *data = ( (((uint32_t)buf[0]) << 16) + (buf[1] << 8) + buf[2]) >> (8-OSS);

    return rc;
}



// ------------------------------------------------------------------------
// Calibration
// ------------------------------------------------------------------------

static int16_t calib_int16_t[8];
static uint16_t calib_uint16_t[3];


#define READ_ONE_I( __index, __addr ) rc |= bmp180ReadShort( (uint16_t *)(calib_int16_t + (__index)), __addr )
#define READ_ONE_U( __index, __addr ) rc |= bmp180ReadShort( calib_uint16_t + (__index), __addr )


int bmp180_calibration( void )
{
    if(!RT_IO_ENABLED(IO_BMP)) return -1;

    uint8_t rc = 0;

    READ_ONE_I( 0, 0xAA ); // ac1
    READ_ONE_I( 1, 0xAC ); // ac2
    READ_ONE_I( 2, 0xAE ); // ac3


    READ_ONE_U( 0, 0xB0 ); // ac4
    READ_ONE_U( 1, 0xB2 ); // ac5
    READ_ONE_U( 2, 0xB4 ); // ac6


    READ_ONE_I( 3, 0xB6 ); // b1
    READ_ONE_I( 4, 0xB8 ); // b2
    READ_ONE_I( 5, 0xBA ); // mb
    READ_ONE_I( 6, 0xBC ); // mc
    READ_ONE_I( 7, 0xBE ); // md


    return rc;
}

// ------------------------------------------------------------------------
// Conversion using calibration data
// ------------------------------------------------------------------------

static int bmp180Convert( int32_t* temperature, int32_t* pressure )
{
    int rc;

//    int8_t i;

    int16_t ut=0;
    int32_t up=0;

    int32_t x1, x2, b5, b6, x3, b3, p;
    uint32_t b4, b7;
    int16_t ac1=calib_int16_t[0];
    int16_t ac2=calib_int16_t[1];
    int16_t ac3=calib_int16_t[2];
    uint16_t ac4=calib_uint16_t[0];
    uint16_t ac5=calib_uint16_t[1];
    uint16_t ac6=calib_uint16_t[2];
    int16_t b1=calib_int16_t[3];
    int16_t b2=calib_int16_t[4];
    //int16_t mb=calib_int16_t[5];
    int16_t mc=calib_int16_t[6];
    int16_t md=calib_int16_t[7];


    rc = bmp180ReadTemp( &ut );
    if( rc ) return rc;


    rc = bmp180ReadPressure( &up );
    if( rc ) return rc;


    x1 = ((int32_t)ut - (int32_t)ac6) * (int32_t)ac5 >> 15;
    x2 = ((int32_t)mc << 11) / (x1 + md);

    b5 = x1 + x2;
    *temperature = (b5 + 8) >> 4;

    b6 = b5 - 4000;
    x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((int32_t) ac1) * 4 + x3)<<OSS) + 2)>> 2;
    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
    b7 = ((uint32_t) (up - b3) * (50000 >> OSS));
    //p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

    if (b7 < 0x80000000)
    {
        p = (b7 << 1) / b4;
    }
    else
    {
        p = (b7 / b4) << 1;
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;

    *pressure = p + ((x1 + x2 + 3791) >> 4);

    return 0;
}


// ------------------------------------------------------------------------
// Actual data read
// ------------------------------------------------------------------------


int bmp180ReadTemp( int16_t *temperature )
{
    if(!RT_IO_ENABLED(IO_BMP)) return -1;

    uint8_t rc = bmp180_write_CR( 0x2E );
    if( rc ) return rc;

    NutSleep(5);	// min time is 4.5ms

    rc = bmp180ReadShort( (uint16_t *)temperature, 0xF6 );
    return rc;
}







int bmp180ReadPressure( int32_t *pressure )
{
    if(!RT_IO_ENABLED(IO_BMP)) return -1;

    uint8_t rc = bmp180_write_CR( 0x34 + (OSS<<6) );
    if( rc ) return rc;

    NutSleep(25);

    rc = bmp180ReadLong( (uint32_t *)pressure, 0xF6 );
    return rc;
}


int32_t bmp180_temperature	= ERROR_VALUE_32;
int32_t bmp180_pressure		= ERROR_VALUE_32;
uint16_t bmp180_pressure_mmHg	= ERROR_VALUE_16;

//int16_t bmp180_temperature_raw;
//int32_t bmp180_pressure_raw;

int bmp180_getdata( void )
{
    if(!RT_IO_ENABLED(IO_BMP)) return -1;

    //bmp180ReadPressure( &bmp180_pressure_raw );
    //bmp180ReadTemp( &bmp180_temperature_raw );

    int rc = bmp180Convert( &bmp180_temperature, &bmp180_pressure );

    // Pa is too big to pass in modbus 16 bit register, convert to mm of Hg *10
    double d = bmp180_pressure / 13.3322;
    bmp180_pressure_mmHg = (uint16_t) d;

    return rc;
}

#endif // SERVANT_BMP180


