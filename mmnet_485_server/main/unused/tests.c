#include <string.h>
#include <io.h>



#include <dev/twif.h>


#include <sys/nutconfig.h>
#include <sys/version.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/confnet.h>
#include <sys/socket.h>
#include <sys/event.h>

#include <arpa/inet.h>

#include <pro/dhcp.h>

#include "defs.h"
#include "util.h"
#include "net_io.h"

#include "runtime_cfg.h"

#include "adc.h"
#include "dports.h"
#include "temperature.h"
#include "icp.h"
#include "freq.h"

#include "util.h"
#include "dallas.h"
#include "ds2482.h"

#include "spi.h"

#include "oscgi.h"
#include "cgi.h"
















#if TEST_PWM
// temp

/**
 * pwm_init()
 *
 * Start up PWM (timer2) for demonstration purposes.
 * It drives a non-inverted PWM train on the OC2 pin, which will be
 * Demodulated by the ICP1 ISR (icp.c).
 * A patch wire should be used to connect OC2 (PB7) with ICP1 (PD4).
 */
void
pwm_init(void)
{
	/*
	 * Set up PWM using timer2
	 */
	DDRB |= (1 << PB7);		/* enable PWM (OC2) output pin */
	OCR2 = 0;				/* start at 0% */
	TCCR2 = (1 << WGM21) | (1 << WGM20) 	/* fast PWM */
		  |	(1 << COM21) | (0 << COM20)		/* non-inverted PWM */
		  |	(0 << CS22)  | (1 << CS21) | (1 << CS20)  /* prescale /64	*/
			;
	return;
}
#endif







#if 0

void onewire_test(void)
{
    if(0) {
        printf("Fisrt 1w device: ");
        uint8_t data[16];
        uint8_t result;

        dallasPrintROM(gTempSensorIDs[0]);

        char r[] = {0,0,0,0,0,0,0,0};

        printf("Match ROM: ");
        result = dallasMatchROM(gTempSensorIDs[0]);
        //result = dallasMatchROM(r);
        dallasPrintError(result);
        printf(" (0x%02X)\n", result);

        dallasWriteByte(0xBE); //rd scratch

        int i;
        for ( i=0 ; i < 9; i++ )
            printf(" %02X", dallasReadByte() );



        //printf("Read RAM: ");        result = dallasReadRAM(gTempSensorIDs[0], 0, 1, data);        dallasPrintError(result);

    }

}

#endif



