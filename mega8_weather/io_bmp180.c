
#include "atmega_bmp180_lib.h"
#include "i2c.h"


/*static uint8_t checki2cReturnCode( uint8_t code )
 {
 i2cWaitForComplete();

 uint8_t st = i2cGetStatus();
 if( st == code ) return 0;
 return st;
 }*/


#define checki2cReturnCode( __code ) \
    do { \
    i2cWaitForComplete();            \
    \
    __status = i2cGetStatus();       \
    if( st != __code ) goto fail;    \
    } while(0)







uint8_t bmp180ReadShort(uint16_t *data, uint8_t address)
{
    uint8_t __status;

    uint16_t msb = 0; // TODO 16?
    uint16_t lsb = 0;


    // Send Start Condition
    i2cSendStart();
    checki2cReturnCode(TW_START);


    // Tell Slave to go to receive mode
    i2cSendByte(BMP180_W);	// write 0xEE
    checki2cReturnCode(TW_MT_SLA_ACK);


    // Send read address to Slave
    i2cSendByte(address);	// write register address
    checki2cReturnCode(TW_MT_DATA_ACK);


    // Send Restart to Slave
    i2cSendStart();
    checki2cReturnCode(TW_REP_START);


    // Tell Slave go in send mode
    i2cSendByte(BMP180_R);	// read 0xEF
    checki2cReturnCode(TW_MR_SLA_ACK);


    // Get MSB from Slave
    i2cReceiveByte(1);
    i2cWaitForComplete();

    msb = i2cGetReceivedByte();	// Get MSB result
    checki2cReturnCode(TW_MR_DATA_ACK);


    // Get LSB from Slave
    i2cReceiveByte(0); // NACK
    i2cWaitForComplete();

    lsb = i2cGetReceivedByte();	// Get LSB result
    checki2cReturnCode(TW_MR_DATA_NACK);


    i2cSendStop();

    *data = (msb << 8) + lsb;

    return 0;

fail:
    i2cSendStop();
    return __status;

}









uint8_t bmp180ReadLong( uint32_t *data, uint8_t address )
{
    uint8_t msb = 0;
    uint8_t lsb = 0;
    uint8_t xlsb = 0;


    // Send Start Condition
    i2cSendStart();
    checki2cReturnCode(TW_START);


    // Tell Slave to go to receive mode
    i2cSendByte(BMP180_W);	// write 0xEE
    checki2cReturnCode(TW_MT_SLA_ACK);


    // Send read address to Slave
    i2cSendByte(address);	// write register address
    checki2cReturnCode(TW_MT_DATA_ACK);


    // Send Restart to Slave
    i2cSendStart();
    checki2cReturnCode(TW_REP_START);


    // Tell Slave go in send mode
    i2cSendByte(BMP180_R);	// read 0xEF
    checki2cReturnCode(TW_MR_SLA_ACK);




    // Get MSB from Slave
    i2cReceiveByte(1);
    i2cWaitForComplete();

    msb = i2cGetReceivedByte();	// Get MSB result
    checki2cReturnCode(TW_MR_DATA_ACK);


    // Get LSB from Slave
    i2cReceiveByte(1); // ACK TODO was NAK!?
    i2cWaitForComplete();

    lsb = i2cGetReceivedByte();	// Get LSB result
    checki2cReturnCode(TW_MR_DATA_ACK);


    // Get XLSB from Slave
    i2cReceiveByte(0); // NACK
    i2cWaitForComplete();

    xlsb = i2cGetReceivedByte();	// Get LSB result
    checki2cReturnCode(TW_MR_DATA_NACK);


    i2cSendStop();



    *data = ((msb << 16) + (lsb << 8) + xlsb) >> (8-OSS);
    return 0;

fail:
    i2cSendStop();
    return __status;
}




uint8_t bmp180_write_CR( uint8_t value )
{


    // Send Start Condition
    i2cSendStart();
    checki2cReturnCode(TW_START);


    // Tell Slave to go to receive mode
    i2cSendByte(BMP180_W);	// write 0xEE
    checki2cReturnCode(TW_MT_SLA_ACK);


    // Send write address to Slave
    i2cSendByte(0xF4);	// write 0xF4 (register address)
    checki2cReturnCode(TW_MT_DATA_ACK);


    //Write control register value
    i2cSendByte(value);
    checki2cReturnCode(TW_MT_DATA_ACK);

    i2cSendStop();

    return 0;

fail:
    i2cSendStop();
    return __status;
}











//----------------------------------------

uint8_t bmp180ReadTemp( int16_t *temperature )
{
    // int32_t temperature = 0;

/*
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
*/

    uint8_t rc = bmp180_write_CR( 0x2E );
    if( rc ) return rc;

    _delay_ms(5);	// min time is 4.5ms


    rc = bmp180ReadShort( temperature, 0xF6 );
    return rc;
}





//----------------------------------------

uint8_t bmp180ReadPressure( int32_t* pressure )
{
    /*
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
    */

    uint8_t rc = bmp180_write_CR( 0x34 + (OSS<<6) );
    if( rc ) return rc;


    _delay_ms(25);

    rc = bmp180ReadLong( pressure, 0xF6 );
    return rc;
}


