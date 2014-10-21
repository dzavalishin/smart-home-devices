#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"

#include "temperature.h"
#include "modbus.h"
#include "uart.h"

#include "eeprom.h"

#include "ui_menu.h"



#define SERIAL_SZ 6

#if N_TEMP_SENS_EEPROM_RECORDS < N_TEMPERATURE_IN
#error N_TEMP_SENS_EEPROM_RECORDS < N_TEMPERATURE_IN
#endif


static void temp_sens_read_record( int nRec, uint8_t record[EEPROM_MAP_REC_SZ] );
static void temp_sens_write_record( int nRec, uint8_t record[EEPROM_MAP_REC_SZ] );

static uint8_t same_serial( uint8_t record[], uint8_t gTempSensorID[] );
static void get_serial( uint8_t record[], uint8_t gTempSensorID[] );


/**
 *
 * EEPROM 1wire sensors id storage
 *
 * 64 records max, 8 bytes each
 *
**/


void temp_sens_load_logical_numbers()
{
    uint8_t record[EEPROM_MAP_REC_SZ];
    uint8_t i,s;


    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        gTempSensorLogicalNumber[s] = 0xFF; // unknown
    }

    for(i = 0; i < N_TEMP_SENS_EEPROM_RECORDS; i++)
    {
        temp_sens_read_record( i, record );


        for(s = 0; s < N_TEMPERATURE_IN; s++)
            if( same_serial( record, gTempSensorIDs[s] ) )
            {
                gTempSensorLogicalNumber[s] = record[SENS_ID_BYTE];
            }

    }


}


static void do_temp_sens_save_logical_numbers()
{
    uint8_t record[EEPROM_MAP_REC_SZ];
    uint8_t s;


    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        get_serial( record, gTempSensorIDs[s] );
        uint8_t logNumber = gTempSensorLogicalNumber[s];
        record[SENS_ID_BYTE] = logNumber;
        temp_sens_write_record( logNumber, record );
    }
}




void remove_mapping_by_id( uint8_t id )
{
    uint8_t s;

    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        if( gTempSensorLogicalNumber[s] == id )
            gTempSensorLogicalNumber[s] = 0xFF; // unknown
    }
}


void remove_mapping_by_rom( uint8_t rom[] )
{
#if 1
    add_mapping( rom, 0xFF );
#else
    uint8_t i,s;

    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        for (i=0;i<OW_ROMCODE_SIZE;i++)
            if( gTempSensorIDs[s][i] != rom[i] ) goto next;


        gTempSensorLogicalNumber[s] = 0xFF; // unknown

    next: ;
    }
#endif
}

void add_mapping( uint8_t rom[], uint8_t id )
{
    uint8_t i,s;

    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        for (i=0;i<OW_ROMCODE_SIZE;i++)
            if( gTempSensorIDs[s][i] != rom[i] ) goto next;


        gTempSensorLogicalNumber[s] = id; // unknown

    next: ;
    }
}


int8_t find_pos_by_persistent_id( uint8_t id )
{
    uint8_t s;

    for(s = 0; s < N_TEMPERATURE_IN; s++)
    {
        if( gTempSensorLogicalNumber[s] == id )
            return s; // unknown
    }

    return -1;
}








static uint8_t same_serial( uint8_t record[], uint8_t gTempSensorID[] )
{
    uint8_t i;
    for( i = 0; i < SERIAL_SZ; i++ )
    {
        if( record[i] != gTempSensorID[i+1] ) return 0;
    }

    return 0xFF; // true
}

static void get_serial( uint8_t record[], uint8_t gTempSensorID[] )
{
    uint8_t i;
    for( i = 0; i < SERIAL_SZ; i++ )
    {
        record[i] = gTempSensorID[i+1];
    }
}



void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEWE))
        ;

    /* Set up address and data registers */
    EEAR = uiAddress;
    EEDR = ucData;

    /* Write logical one to EEMWE */
    EECR |= (1<<EEMWE);
    /* Start eeprom write by setting EEWE */
    EECR |= (1<<EEWE);
}



unsigned char EEPROM_read(unsigned int uiAddress)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEWE))
        ;

    /* Set up address register */
    EEAR = uiAddress;

    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);

    /* Return data from data register */
    return EEDR;
}



static void temp_sens_read_record( int nRec, uint8_t record[EEPROM_MAP_REC_SZ] )
{
    uint8_t i;
    for( i = 0; i < EEPROM_MAP_REC_SZ; i++ )
    {
        record[i] = EEPROM_read( EEPROM_MAP_BASE + (nRec*EEPROM_MAP_REC_SZ) + i );
    }

}

static void temp_sens_write_record( int nRec, uint8_t record[EEPROM_MAP_REC_SZ] )
{
    if( (nRec >= N_TEMP_SENS_EEPROM_RECORDS) || (nRec < 0) ) return;

    uint8_t i;
    for( i = 0; i < EEPROM_MAP_REC_SZ; i++ )
    {
        EEPROM_write( EEPROM_MAP_BASE + (nRec*EEPROM_MAP_REC_SZ) + i, record[i] );
    }

}

// ---------------------------------------------------------------
//
// general config
//
// ---------------------------------------------------------------


void load_eeprom_settings(void)
{
    // Default contents are FF
    if( EEPROM_read( EEPROM_CONF_VALID ) ) return;

    modbus_our_address = EEPROM_read( EEPROM_CONF_MODBUS_ADDR );

    uint16_t baud = EEPROM_read( EEPROM_CONF_UART_SPEED_HI ) << 8;
    baud |= EEPROM_read( EEPROM_CONF_UART_SPEED_LO );

    modbus_set_baud( baud );
}


static void do_save_eeprom_settings(void)
{
    EEPROM_write( EEPROM_CONF_MODBUS_ADDR, modbus_our_address );

    EEPROM_write( EEPROM_CONF_UART_SPEED_LO, uart_speed & 0xFF );
    EEPROM_write( EEPROM_CONF_UART_SPEED_HI, (uart_speed >> 8) & 0xFF );

    EEPROM_write( EEPROM_CONF_VALID, 0 );
}






// ---------------------------------------------------------------
//
// EEPROM write protection/timer
//
// We shouldn't write EEPROM too frequently. Additionally, several
// sequential config modifications should be written as one EEPROM
// operation to minimize impact.
//
// That's why we:
//
// - delay each write operation for 60 sec to wait for next settings
//   to come
//
// - prevent writes to happen too frequently
//
// ---------------------------------------------------------------



static uint8_t  eeprom_write_request;
static uint8_t  eeprom_timer_sec;


void eeprom_request_write( uint8_t what )
{
    ui_report_unsaved(1);

    eeprom_write_request |= what;
    eeprom_timer_sec = EEPROM_TIMEOUT_SEC;
}

// Called once in 5 sec
void eeprom_timer_5sec(void)
{
    if( eeprom_timer_sec > 0 ) eeprom_timer_sec -= 5;

    if( eeprom_write_request == 0 ) return;

    cli();
    uint8_t eeprom_write_request_copy = eeprom_write_request;
    eeprom_write_request = 0;
    sei();

    if( eeprom_write_request_copy & EEPROM_WR_485 )
        do_save_eeprom_settings();

    if( eeprom_write_request_copy & EEPROM_WR_MAP )
        do_temp_sens_save_logical_numbers();

    ui_report_unsaved( eeprom_write_request );
}












