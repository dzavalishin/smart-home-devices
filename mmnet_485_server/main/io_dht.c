/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * DHT11/22 interface code.
 *
**/


#define DEBUG 0

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#include "io_dht.h"

#if SERVANT_DHT11

int16_t dht_temperature	= ERROR_VALUE_16;
int16_t dht_humidity	= ERROR_VALUE_16;

#define DHT_READ (DHT_PIN & (1<<DHT_INPUTPIN))

#define DHT_DDR_OUT (DHT_DDR |= (1<<DHT_INPUTPIN))
#define DHT_DDR_IN  (DHT_DDR &= ~(1<<DHT_INPUTPIN))

#define DHT_OUT_1 (DHT_PORT |= (1<<DHT_INPUTPIN))
#define DHT_OUT_0 (DHT_PORT &= ~(1<<DHT_INPUTPIN))



#if 0





int8_t dht_getdata(int16_t *temperature, int16_t *humidity)
{
    uint8_t loopCnt;

    if(!RT_IO_ENABLED(IO_DHT)) return -1;

    // TEST - pull-up
    DHT_DDR_IN;
    cli();
    _delay_us(50);

    if ( !DHT_READ )
    {
        sei();
        DPUTS("no pullup");
        return -1; //NO_PULLUP;
    }

    // send DHT START
    DHT_DDR_OUT;
    //DHT_pin = 0;
    DHT_OUT_0;
    //waitMS( 18 );
    _delay_ms(20);
    DHT_DDR_IN;
    DHT_OUT_1; // need?
#if 1
    _delay_us(50);
#else
    loopCnt = 58;
    while( !DHT_READ )     // wait max. 100us
    {
        _delay_us(5);
        if ( --loopCnt == 0 )
        {
            sei();
            DPUTS("no pre ack 1");
            return -1; //NO_ACKNOWLEDGE_1;
        }
    }
#endif

    // test for ACKNOWLEDGE 0 and 1
    if( DHT_READ )
    {
        sei();
        DPUTS("no ack 0");
        return -1; //NO_ACKNOWLEDGE_0;
    }

    //uint8_t loopCnt = 18;
    loopCnt = 38;
    while( !DHT_READ )     // wait max. 100us
    {
        _delay_us(5);
        if ( --loopCnt == 0 )
        {
            sei();
            DPUTS("no ack 1");
            return -1; //NO_ACKNOWLEDGE_1;
        }
    }

    // wait for end of ACKNOWLEDGE seqence
    loopCnt = 18;
    while( DHT_READ )     // wait max. 100us
    {
        _delay_us(5);
        if ( --loopCnt == 0 )
        {
            sei();
            DPUTS("no prev data 0");
            return -1; //NO_DATA_0;
        }
    }

    // init
    uint8_t value = 0;
    uint8_t buf[5];
    uint8_t *p = buf;

    // read data 40bits
    uint8_t loopBits = 5 * 8;
    do
    {
        loopCnt = 11;
        while( !DHT_READ )     // wait max. 55us
        {
            _delay_us(5);
            if ( --loopCnt == 0 )
            {
                sei();
                DPUTS("no data 0");
                return -1; //NO_DATA_0;
            }
        }

        loopCnt = 15;
        while( DHT_READ )     // wait max. 75us
        {
            _delay_us(5);
            if ( --loopCnt == 0 )
            {
                sei();
                DPUTS("no data 1");
                return -1; //NO_DATA_1;
            }
        }

        value <<= 1;
        if ( loopCnt < 10 )
            value |= 1;  // if DHT_pin=1 is longer than 30us it's data 1

        if ( ( loopBits & 0b111 ) == 1 )   // next byte?
        {
            *p++ = value;
            value = 0;
        }
    } while ( --loopBits != 0 );

    sei();

    // in buf is 40bit from sensor DHT11
    //  1st byte Humidity integer data
    //  2nd byte Humidity decimal data - in DTH11 not used (read 0x00)
    //  3rd byte temperature integer data
    //  4th byte fractional temperature data - in DTH11 not used (read 0x00)
    //  5th byte parity bits (the sum of 1, 2, 3, 4 byte)

    // test CRC
    if ( buf[0] + buf[1] + buf[2] + buf[3] != buf[4] )
    {
        DPUTS("crc");
        return -1; //BAD_CRC;
    }

    *temperature = 	buf[2];
    *humidity = 	buf[0];

    return 0; //OK;
}
























#else // code


/*
 DHT Library 0x03

 copyright (c) Davide Gironi, 2012

 Released under GPLv3.
 Please refer to LICENSE file for licensing information.

 (dz: modofied a lot)

 **/



static void dht11_start(void)
{
    //reset port
    DHT_DDR |= (1<<DHT_INPUTPIN); //output
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    //_delay_ms(100);
    _delay_ms(250);

    //send request
    DHT_PORT &= ~(1<<DHT_INPUTPIN); //low
#if DHT_TYPE == DHT_DHT11
    _delay_ms(18);
    //_delay_ms(30);
#elif DHT_TYPE == DHT_DHT22
    _delay_us(500);
#endif
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    DHT_DDR &= ~(1<<DHT_INPUTPIN); //input
    _delay_us(40);
    //_delay_us(30);
}

uint8_t dht11_check_response(void)
{
    uint8_t k;

    k = 150;
    while(DHT_READ)
    {
        _delay_us(2);
        k--;
        if( k < 1 )
        {
            DPUTS("no start condition 1");
            return 1;  // time out
        }
    }

    k = 150;
    while(!DHT_READ)
    {
        _delay_us(2);
        k--;
        if( k < 1 )
        {
            DPUTS("no start condition 2");
            return 2;  // time out
        }
    }

    return 0;
}

/*
 * get data from sensor
 */
#if DHT_FLOAT == 1
int8_t dht_getdata(float *temperature, float *humidity) {
#elif DHT_FLOAT == 0
int8_t dht_getdata(int16_t *temperature, int16_t *humidity) {
#endif
    uint8_t bits[5];
    uint8_t i,j = 0;

    if(!RT_IO_ENABLED(IO_DHT)) return -1;

    memset(bits, 0, sizeof(bits));
#if 1
    dht11_start();
#else
    //reset port
    DHT_DDR |= (1<<DHT_INPUTPIN); //output
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    _delay_ms(100);

    //send request
    DHT_PORT &= ~(1<<DHT_INPUTPIN); //low
#if DHT_TYPE == DHT_DHT11
    //_delay_ms(18);
    _delay_ms(20);
#elif DHT_TYPE == DHT_DHT22
    _delay_us(500);
#endif
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    DHT_DDR &= ~(1<<DHT_INPUTPIN); //input
    _delay_us(40);
    //_delay_us(70);
#endif

#if 0
    if(dht11_check_response()) return -1;
#else
    //check start condition 1
    if((DHT_PIN & (1<<DHT_INPUTPIN))) {
        DPUTS("no start condition 1");
        return -1;
    }
    _delay_us(80);
    //check start condition 2
    if(!(DHT_PIN & (1<<DHT_INPUTPIN))) {
        DPUTS("no start condition 2");
        return -1;
    }
    _delay_us(80);
    //_delay_us(40);
#endif

    //read the data
    uint16_t timeoutcounter = 0;
    for (j=0; j<5; j++) { //read 5 byte
        uint8_t result=0;
        for(i=0; i<8; i++) {//read every bit
            timeoutcounter = 0;
            while(!(DHT_PIN & (1<<DHT_INPUTPIN))) { //wait for an high input (non blocking)
                timeoutcounter++;
                if(timeoutcounter > DHT_TIMEOUT) {
                    DPUTS("tmo 1");
                    return -1; //timeout
                }
            }
            _delay_us(30);
            if(DHT_PIN & (1<<DHT_INPUTPIN)) //if input is high after 30 us, get result
                result |= (1<<(7-i));
            timeoutcounter = 0;
            while(DHT_PIN & (1<<DHT_INPUTPIN)) { //wait until input get low (non blocking)
                timeoutcounter++;
                if(timeoutcounter > DHT_TIMEOUT) {
                    DPUTS("tmo 2");
                    return -1; //timeout
                }
            }
        }
        bits[j] = result;
    }

#if 1
    // reset port
    DHT_DDR |= (1<<DHT_INPUTPIN); //output
    DHT_PORT |= (1<<DHT_INPUTPIN); //low
    _delay_ms(100);
#endif

    // check checksum
    if((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) != bits[4])
    {
        DPUTS("checksum");
        return -1;
    }

    // return temperature and humidity
#if DHT_TYPE == DHT_DHT11
    *temperature = bits[2];
    *humidity = bits[0];
#elif DHT_TYPE == DHT_DHT22
    uint16_t rawhumidity = bits[0]<<8 | bits[1];
    uint16_t rawtemperature = bits[2]<<8 | bits[3];
    if(rawtemperature & 0x8000) {
        *temperature = (float)((rawtemperature & 0x7FFF) / 10.0) * -1.0;
    } else {
        *temperature = (float)(rawtemperature)/10.0;
    }
    *humidity = (float)(rawhumidity)/10.0;
#endif
    return 0;


}

#endif // all code

#endif // SERVANT_DHT11

