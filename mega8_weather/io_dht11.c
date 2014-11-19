/*
DHT-11 Library
(c) Created by Charalampos Andrianakis on 18/12/11.

Some cleanup from dz@dz.ru

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "io_dht11.h"

// If data line does not change for so long, bus is assumed to be dead
#define MAX_US 200
#define US_STEP 4


// dht11_data[5] is byte table where data come from DHT are stored

static uint8_t dht11_data[5];


uint8_t DHT_Read_Data(void)
{

    //laststate holds laststate value
    //counter is used to count microSeconds
    uint8_t laststate = 0, counter, j = 0, i = 0;
    
    // Clear array
    dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;
    
    cli();                              // Disable interrupts
    
    // Prepare the bus

    DDR(DHT_PORT)   |= _BV(DHT_PIN);    // Set pin Output
    DHT_PORT        |= _BV(DHT_PIN);    // Pin High
    _delay_ms(250);                     // Wait for 250mS
    
    // Send Request Signal

    DHT_PORT        &=~_BV(DHT_PIN);    // Pin Low
    _delay_ms(20);                      // 20ms Low
    
    DHT_PORT        |= _BV(DHT_PIN);    // Pin High
    _delay_us(40);                      // 40us High
    
    // Set pin Input to read Bus
    DDR(DHT_PORT)   &=~_BV(DHT_PIN);    // Set pin Input

    laststate = DHT_Read_Pin;           // Read Pin value
    
    // Repeat for each Transistions

    for( i=0; i < MAXTIMINGS; i++ )
    {
        counter=0;                  //reset counter

        // While state is the same count microseconds
        // [dz] do in 4 uS steps for better accuracy
        while( laststate == DHT_Read_Pin )
        {
            _delay_us(US_STEP);
            counter += US_STEP;

            if( counter > MAX_US ) goto dead_end;
        }
        
        // laststate==_BV(DHT_PIN) checks if laststate was High
        // ignore the first 2 transitions which are the DHT Response
        if (laststate==_BV(DHT_PIN) && (i > 2))
        {
            //Save bits 

            dht11_data[ j/8 ] <<= 1;

            if (counter >= 40)     	// If it was high for more than 40uS
                dht11_data[j/8] | = 1;  // it means it is bit '1'

            j++;
        }

        laststate=DHT_Read_Pin;     // Save current state
    }

    sei();                          // Enable interrupts
    
    // Check if DHT-11 is connected

    if( dht11_data[0] == 0 && dht11_data[1] == 0 && dht11_data[2] == 0 && dht11_data[3] == 0 )
        return DHT_ERR_BUS;


    // Check if data received are correct by checking the CheckSum

    if( dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3] != dht11_data[4] )
        return DHT_ERR_CSUM;

    return DHT_OK;

dead_end:
    return DHT_ERR_BUS;
}


uint8_t DHT_Read_RH(voud)    { return dht11_data[0]; }
uint8_t DHT_Read_Temp(voud)  { return dht11_data[2]; }




