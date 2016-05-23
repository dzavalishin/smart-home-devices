#include "defs.h"

#include <avr/io.h>

#include "util.h"
#include "main.h"



typedef struct enc_state
{
	uint8_t		new_a;
	uint8_t		new_b;

	uint8_t		prev_a;
	uint8_t		prev_b;

	uint8_t		enc_val;

	uint8_t		changed;

	void		(*read)(struct enc_state *e);
	void		(*process)(uint8_t val);
} enc_state;

void init_encoder( enc_state *e, void (*read)(struct enc_state *) );
void process_encoder( enc_state *e );



#define ENC0_PORT PORTB
#define ENC0_PIN  PINB
#define ENC0_DDR  DDRB

#define ENC0_PIN_A PB0
#define ENC0_PIN_B PB1


#define GET_A0() (ENC0_PIN & _BV(ENC0_PIN_A))
#define GET_B0() (ENC0_PIN & _BV(ENC0_PIN_B))

//static int enc_val = 127;


enc_state e0;

static void process_enc_0(uint8_t val);
static void read_enc_0(struct enc_state *e);



void
init_encoders(void)
{
	// Inputs
	ENC0_DDR  &= ~_BV(ENC0_PIN_A);
	ENC0_DDR  &= ~_BV(ENC0_PIN_B);

#if 0
	// outs for test
	ENC0_DDR  |= _BV(ENC0_PIN_A);
	ENC0_DDR  |= _BV(ENC0_PIN_B);
#endif
	// Pull up
	ENC0_PORT |= _BV(ENC0_PIN_A) | _BV(ENC0_PIN_B);

	init_encoder( &e0, read_enc_0 );
	e0.process = process_enc_0;
	//e0.read    = read_enc_0;
}


void read_encoders( void )
{
//	uint8_t a;
//	uint8_t b;

//main_pwm[0] = a;
//main_pwm[1] = b;
//main_pwm[2] = ;
	process_encoder( &e0 );

}




void process_encoder( enc_state *e )
{
	if( e->read ) e->read( e );

	uint8_t a = e->new_a;
	uint8_t b = e->new_b;

	if( e->prev_a && (!a) )
	{
		activity++;

		if( b )		{ if( e->enc_val < 0xFF ) e->enc_val++; }
		else		{ if( e->enc_val > 0 )    e->enc_val--; }

		e->changed = 1;

		if( e->process ) 
			e->process( e->enc_val );
	}

	e->prev_a = e->new_a;
	e->prev_b = e->new_b;

}


void init_encoder( enc_state *e, void (*read)(struct enc_state *) )
{
	e->changed = 0;
	e->enc_val = 127;

	e->process = 0;
	e->read = read;

	if( e->read ) e->read( e );

	e->prev_a = e->new_a;
	e->prev_b = e->new_b;
}


void process_enc_0(uint8_t val)
{
	main_pwm[0] = val;
}


void read_enc_0(struct enc_state *e)
{
	e->new_a = GET_A0();
	e->new_b = GET_B0();
}



