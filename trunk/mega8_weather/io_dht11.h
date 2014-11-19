/*
 DHT-11 Library
 (c) Created by Charalampos Andrianakis on 18/12/11.
 
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
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

// DHT Pin and Port
#define DHT_PORT    PORTC
#define DHT_PIN     0

// The packet size is 40bit but each bit consists of low and high state
// so 40 x 2 = 80 transitions. Also we have 2 transistions DHT response
// and 2 transitions which indicates End Of Frame. In total 84
#define MAXTIMINGS  84

// Select between Temp and Humidity Read
#define DHT_Temp    0
#define DHT_RH      1

#define DHT_Read_Pin    (PIN(DHT_PORT) & _BV(DHT_PIN))

// This is the main function which requests and reads the packet
uint8_t DHT_Bus_IO(voud);

#define DHT_OK 		0
#define DHT_ERR_CSUM 	1 // Checksum error
#define DHT_ERR_BUS     2 // Bus error - no hardware


// Get results
uint8_t DHT_Read_RH(voud);
uint8_t DHT_Read_Temp(voud);



