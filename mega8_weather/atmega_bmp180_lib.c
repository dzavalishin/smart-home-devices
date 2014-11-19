//#################################################################################
//	Library to use BMP180 with ATMEL Atmega family
//#################################################################################

#include "atmega_bmp180_lib.h"
//#include "twi_lib.h"


/*

uint16_t bmp180ReadShort(uint8_t address, uint8_t* error_code)
{
	uint16_t msb=0;
	uint16_t lsb=0;
	uint16_t data;
	//Send Start Condition
	i2cSendStart();
	*error_code=i2cWaitForComplete();
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_START);
	}

	//Tell Salve go in receive mode
	if (*error_code==0){
		i2cSendByte(BMP180_W);	// write 0xEE
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_SLA_ACK);
	}

	//Send read address to Salve
	if (*error_code==0){
		i2cSendByte(address);	// write register address
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Send Restart to Slave
	if (*error_code==0){
		i2cSendStart();
		*error_code=i2cWaitForComplete();
	}
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_REP_START);
	}

	//Tell Salve go in send mode
	if (*error_code==0){
		i2cSendByte(BMP180_R);	// read 0xEF
		*error_code=i2cWaitForComplete();
	}

	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_SLA_ACK);
	}

	//Get MSB from Salve
	if (*error_code==0){
		i2cReceiveByteACK();
		*error_code=i2cWaitForComplete();
	}

	if (*error_code==0){
		msb = i2cGetReceivedByte();	// Get MSB result
		*error_code=i2cWaitForComplete();
	}

	//ACKM
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_DATA_ACK);
	}

	//Get LSB from Slave
	if (*error_code==0){
		i2cReceiveByteNACK();
		*error_code=i2cWaitForComplete();
	}

	if (*error_code==0){
		lsb = i2cGetReceivedByte();	// Get LSB result
		*error_code=i2cWaitForComplete();
	}

	//NACKM
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_DATA_NACK);
	}

	//Send Stop on the Bus
	if (*error_code==0){
		*error_code=i2cSendStop();
	}

	data = (msb << 8) + lsb;
	return data;
}



//----------------------------------------

uint32_t bmp180ReadLong(uint8_t address, uint8_t* error_code)
{
	uint32_t msb=0;
	uint32_t lsb=0;
	uint32_t xlsb=0;
	uint32_t data;
	//Send Start Condition
	i2cSendStart();
	*error_code=i2cWaitForComplete();
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_START);
	}

	//Tell Salve go in receive mode
	if (*error_code==0){
		i2cSendByte(BMP180_W);	// write 0xEE
		*error_code=i2cWaitForComplete();
	}

	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_SLA_ACK);
	}

	//Send read address to Salve
	if (*error_code==0){
		i2cSendByte(address);	// write register address
		*error_code=i2cWaitForComplete();
	}

	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Send Restart to Slave
	if (*error_code==0){
		i2cSendStart();
		*error_code=i2cWaitForComplete();
	}
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_REP_START);
	}

	//Tell Salve go in send mode
	if (*error_code==0){
		i2cSendByte(BMP180_R);	// read 0xEF
		*error_code=i2cWaitForComplete();
	}

	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_SLA_ACK);
	}

	//Get MSB from Salve
	if (*error_code==0){
		i2cReceiveByteACK();
		*error_code=i2cWaitForComplete();
	}

	if (*error_code==0){
		msb = i2cGetReceivedByte();	// Get MSB result
		*error_code=i2cWaitForComplete();
	}

	//ACKM
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_DATA_ACK);
	}

	//Get LSB from Slave
	if (*error_code==0){
		i2cReceiveByteNACK();
		*error_code=i2cWaitForComplete();
	}

	if (*error_code==0){
		lsb = i2cGetReceivedByte();	// Get LSB result
		*error_code=i2cWaitForComplete();
	}

	//Get XLSB from Slave
	if (*error_code==0){
		i2cReceiveByteNACK();
		*error_code=i2cWaitForComplete();
	}

	if (*error_code==0){
		xlsb = i2cGetReceivedByte();	// Get XLSB result
		*error_code=i2cWaitForComplete();
	}

	//NACKM
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MR_DATA_NACK);
	}

	//Send Stop on the Bus
	if (*error_code==0){
		*error_code=i2cSendStop();
	}

	data = ((msb << 16) + (lsb << 8) + xlsb) >> (8-OSS);
	return data;
}
//----------------------------------------

int32_t bmp180ReadTemp(uint8_t* error_code)
{
	int32_t temperature = 0;
	//Send Start Condition
	i2cSendStart();
	*error_code=i2cWaitForComplete();

	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_START);
	}

	//Tell Salve go in receive mode
	if (*error_code==0){
		i2cSendByte(BMP180_W);	// write 0xEE
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_SLA_ACK);
	}

	//Write control register address
	if (*error_code==0){
		i2cSendByte(0xF4);	// write 0xF4
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Write control register value
	if (*error_code==0){
		i2cSendByte(0x2E);	// write 0x2E
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Send Stop on the Bus
	if (*error_code==0){
		*error_code=i2cSendStop();
	}

	_delay_ms(5);	// min time is 4.5ms

	if (*error_code==0){
		temperature = bmp180ReadShort(0xF6,error_code);
	}
	return temperature;
}
//----------------------------------------

int32_t bmp180ReadPressure(uint8_t* error_code)
{
	int32_t pressure = 0;
	//Send Start Condition
	i2cSendStart();
	*error_code=i2cWaitForComplete();
	if (error_code==0){
		*error_code=checki2cReturnCode(TW_START);
	}

	//Tell Salve go in receive mode
	if (*error_code==0){
		i2cSendByte(BMP180_W);	// write 0xEE
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_SLA_ACK);
	}

	//Write control register address
	if (*error_code==0){
		i2cSendByte(0xF4);	// write 0xF4
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Write control register value
	if (*error_code==0){
		i2cSendByte(0x34+(OSS<<6));	// write 0x34+(OSS<<6)
		*error_code=i2cWaitForComplete();
	}
	//ACKS
	if (*error_code==0){
		*error_code=checki2cReturnCode(TW_MT_DATA_ACK);
	}

	//Send Stop on the Bus
	if (*error_code==0){
		*error_code=i2cSendStop();
	}

	_delay_ms(25);

	if (*error_code==0){
		pressure = bmp180ReadLong(0xF6,error_code);
	}

	return pressure;

}
*/













//----------------------------------------

static int16_t calib_int16_t[8];
static int16_t calib_uint16_t[3];


uint8_t bmp180Convert( int32_t* temperature, int32_t* pressure )
{
    uint8_t rc;

    int8_t i;

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
//----------------------------------------



/*
void BMP180_Calibration(int16_t calib_int16_t[],int16_t calib_uint16_t[], uint8_t* errorcode)
{
    if (*errorcode==0){
        calib_int16_t[0] = bmp180ReadShort(0xAA,errorcode);//ac1
    }
    if (*errorcode==0){
        calib_int16_t[1] = bmp180ReadShort(0xAC,errorcode);//ac2
    }
    if (*errorcode==0){
        calib_int16_t[2] = bmp180ReadShort(0xAE,errorcode);//ac3
    }
    if (*errorcode==0){
        calib_uint16_t[0] = bmp180ReadShort(0xB0,errorcode);//ac4
    }
    if (*errorcode==0){
        calib_uint16_t[1] = bmp180ReadShort(0xB2,errorcode);//ac5
    }
    if (*errorcode==0){
        calib_uint16_t[2] = bmp180ReadShort(0xB4,errorcode);//ac6
    }
    if (*errorcode==0){
        calib_int16_t[3] = bmp180ReadShort(0xB6,errorcode);//b1
    }
    if (*errorcode==0){
        calib_int16_t[4] = bmp180ReadShort(0xB8,errorcode);//b2
    }
    if (*errorcode==0){
        calib_int16_t[5] = bmp180ReadShort(0xBA,errorcode);//mb
    }
    if (*errorcode==0){
        calib_int16_t[6] = bmp180ReadShort(0xBC,errorcode);//mc
    }
    if (*errorcode==0){
        calib_int16_t[7] = bmp180ReadShort(0xBE,errorcode);//md
    }
}
*/


#define READ_ONE_I( __index, __addr ) rc |= bmp180ReadShort( calib_int16_t + (__index), __addr )
#define READ_ONE_U( __index, __addr ) rc |= bmp180ReadShort( calib_uint16_t + (__index), __addr )

//uint8_t BMP180_Calibration(int16_t calib_int16_t[],int16_t calib_uint16_t[], uint8_t* errorcode)
uint8_t bmp180Calibration( void )
{
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








//----------------------------------------
#if 0
int32_t bmp180CalcAltitude( int32_t pressure )
{
    float temp;
    int32_t altitude;
    temp = (float) pressure/101325;
    temp = 1-pow(temp, 0.19029);
    //altitude = round(44330*temp*10);
    altitude = 44330*temp*100;
    //get altitude in dm
    return altitude;
}

#endif


