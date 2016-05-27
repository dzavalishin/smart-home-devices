
#include <avr/interrupt.h>

#include "defs.h"
#include "errlog.h"
#include "pump.h"
#include "eeprom.h"

#include "util.h"


static int log_pos = 0; // TODO save in EEPROM too?

void log_event( char log_id )
{
    int p = EEPROM_LOG_BASE + (log_pos*EEPROM_REC_SIZE);
    long u = uptime;

    cli();

    EEPROM_write( p + 0, log_id );

    EEPROM_write( p+1, (u >> 0) & 0xFF );
    EEPROM_write( p+2, (u >> 8) & 0xFF );
    EEPROM_write( p+3, (u >> 16) & 0xFF );
    EEPROM_write( p+4, (u >> 24) & 0xFF );

    log_pos++;
    if( log_pos >= EEPROM_LOG_N_RECORS )
        log_pos = 0;

    sei(); // TODO save/restore int flg state
}

void lcd_put_time( long time_sec )
{
    
    lcd_puti( (int) time_sec / (24L*60L*60) );
    lcd_puts("d ");

    lcd_puti( (int) ((time_sec / (60L*60)) % 24) );
    lcd_puts(":");
    lcd_puti( (int) ((time_sec / 60) % 60) );
    lcd_puts(":");
    lcd_puti( (int) (time_sec % 60) );


}


char *event_name( char id )
{
    switch(id)
    {
    default:     return "?";

    case 0:     return "restart";

    case 1:     return "no pressure";
    case 2:     return "long run";
    case 3:     return "retry";
    }
}





void lcd_put_event( int pos )
{
    pos %= EEPROM_LOG_N_RECORS;

    int p = EEPROM_LOG_BASE + (log_pos*EEPROM_REC_SIZE);

    long u = uptime;
    char id;

    id = EEPROM_read( p + 0 );

    u = EEPROM_read( p+1 );
    u |= EEPROM_read( p+2 ) << 8;
    u |= ((long)EEPROM_read( p+3 )) << 16;
    u |= ((long)EEPROM_read( p+4 )) << 24;

    lcd_puti( pos );
    lcd_puts(": ");

    lcd_put_time( u );
    lcd_puts(" ");
    lcd_puts( event_name( id ) );
}





