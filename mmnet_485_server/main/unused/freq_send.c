
#include <avr/io.h>

#include "icp.h"
#include "freq.h"
#include "net_io.h"
#include "packet_types.h"

#if SERVANT_NFREQ > 8
#error no more than 8 freq channels. Send mask is byte.
#endif


#if SERVANT_NFREQ > 0


unsigned int freq_outs[SERVANT_NFREQ];
unsigned int freq_prev[SERVANT_NFREQ];

/*
void
freq_meter_init(void)
{
    unsigned char i;
    for( i = 0; i < SERVANT_NFREQ; i++ )
    {
        freq_outs[i] = 0;
    }


}
*/











unsigned volatile char freq_send_mask = 0; // bit 0 == 1 -> need to send ch 0

void request_freq_data_send(unsigned char channel)
{
    freq_send_mask |= _BV(channel);
}

//#define FREQ_PWM_SHIFT 4

void send_changed_freq_data(void)
{
            // PD4
        unsigned int duty_cycle = icp_get_duty();
        unsigned int frequency = icp_get_freq();


        freq_outs[0] = frequency;
        freq_outs[1] = duty_cycle;

    unsigned char i;
    for( i = 0; i < SERVANT_NFREQ; i++ )
    {
        //cl i();

	// TODO send if changed for > 1? or not?
        if( (freq_outs[i] != freq_prev[i] ) || (freq_send_mask & _BV(i)) )
        {
            freq_prev[i] = freq_outs[i];
            freq_send_mask &= ~_BV(i);
            //sei();
            send_pack(TOHOST_FREQ_VALUE, i, freq_prev[i] );

            //printf("icp freq = %d, duty = %d  ", frequency, duty_cycle );
        }
    }
    //sei();

#if 0
    double val = pwm_in_out;

    //val *= 90; // 90 degrees
    val /= 128; // out for 90 deg is about 128

    val = asin(val); // radians
    val *= 180;
    val /= M_PI;


    int diff = val - pwm_in_prev;
    if( diff < 0 ) diff = -diff;

    // TODO send if changed for > 1? or not?
    if( ( diff > 0 ) || (freq_send_mask & _BV(FREQ_PWM_SHIFT)) )
    {
        pwm_in_prev = val;
        freq_send_mask &= ~_BV(FREQ_PWM_SHIFT);

        // TODO hack - using freq outs!
        send_pack(TOHOST_FREQ_VALUE, FREQ_PWM_SHIFT, val );

        uart_puts("\rpwm in ");            //uart_puti(i);
        uart_puts(" = ");            uart_puti(val);            uart_puts("  ");
    }

#endif

}


#endif // SERVANT_NFREQ > 0

