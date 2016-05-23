#include "defs.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//#include <avr/eeprom.h>

#include "util.h"
#include "main.h"

void 		EEPROM_write( uint8_t uiAddress, uint8_t ucData );
uint8_t		EEPROM_read( uint8_t uiAddress );
void		EEPROM_write_block( uint8_t addr, const uint8_t *data, uint8_t size );
void		EEPROM_read_block( uint8_t addr, uint8_t *data, uint8_t size );



// We save 0-2 of main pwms and usart pwm

struct eeprom_data
{
	uint8_t		main_pwm[3]; 
	uint8_t		usart_pwm; 
};


static uint8_t csum( uint8_t *data, uint8_t size )
{
	uint8_t		csum = 0;

	while(size--)
		csum += *data++;

	return 1-csum;
}

void eeprom_load(void)
{
	struct eeprom_data e;

	EEPROM_read_block( 1, (void *)&e, sizeof(e) );
	uint8_t rcsum = EEPROM_read( 0 );

	uint8_t ccsum = csum( (void *)&e, sizeof(e) );

	if( rcsum != ccsum )
		return;

	main_pwm[0] = e.main_pwm[0];
	main_pwm[1] = e.main_pwm[1];
	main_pwm[2] = e.main_pwm[2];

	usart_pwm = e.usart_pwm;
}


void eeprom_save(void)
{
	struct eeprom_data e;

	e.main_pwm[0] = main_pwm[0];
	e.main_pwm[1] = main_pwm[1];
	e.main_pwm[2] = main_pwm[2];

	e.usart_pwm = usart_pwm;

	uint8_t wcsum = csum( (void *)&e, sizeof(e) );

	EEPROM_write_block( 1, (void *)&e, sizeof(e) );
	EEPROM_write( 0, wcsum );

}



void
init_eeprom(void)
{
	EECR = 0;
}




















// EEARH, EEARL - registri adressov danih v EEPROM 
// EEDR - Register Dannih v EEPROM 
// EECR - Register kontrolya sostoyaniya EEPROM
// EERIE - Register ожидания доступности EECR
// EEMWE - Register монопольного разрешения            записи EECR

void 
EEPROM_write( uint8_t uiAddress, uint8_t ucData )
{
	while(EECR & (1<<EEPE))
		;

	cli();

	EEAR = uiAddress;
	EEDR = ucData;

	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);

	sei();
}
 
uint8_t 
EEPROM_read( uint8_t uiAddress )
{
	while(EECR & (1<<EEPE))
		;

	EEAR = uiAddress;
	EECR |= (1<<EERE);
	return EEDR;
}



void
EEPROM_write_block( uint8_t addr, const uint8_t *data, uint8_t size )
{
	while( size-- )
	{
		uint8_t old = EEPROM_read( addr );

		if( old == *data )
			continue;

		EEPROM_write( addr++, *data++ );
	}
}

void
EEPROM_read_block( uint8_t addr, uint8_t *data, uint8_t size )
{
	while( size-- )
		*data++ = EEPROM_read( addr++ );
}

	