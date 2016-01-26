/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * I2C EEPROM io.
 *
**/

#include <dev/at24c.h>



// http://www.ethernut.de/nutwiki/I2C_EEPROM_Access

static struct at24c m24c08 =
{
    .PageSize = 16,             /* Size of one page */
    .EepromSize = 16*64,	/* Size of the chip */
    .SlaveAddress = 0xA0,	/* Slave address of the chip on the bus */
    .IAddrW = 1,                /* Width of the IADDR register */
    .Timeout = 20,              /* Failure timeout for chip access */
};
 
void EEInit( void )
{
}





int EEWriteData( uint16_t addr, void *data, uint16_t len )
{
    return At24cWrite( &m24c08, (uint8_t *)data, len, addr );
}
 
int EEReadData( uint16_t addr, void *data, uint16_t len )
{
    return At24cRead( &m24c08, (uint8_t *)data, len, addr );
}

