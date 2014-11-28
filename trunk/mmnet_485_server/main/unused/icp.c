/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : icp.c
* - Compiler          : IAR EWAAVR 3.20
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : All devices with an Input Capture Unit can be used.
*                       The example is written for ATmega64
*
* - AppNote           : AVR135 - Pulse-Width Demodulation
*
* - Description       : Routines for use Pulse-Width Demodulation
*
* Originally authored by Bruce McKenny
* $Revision: 1.5 $
* $Date: Wednesday, November 02, 2005 13:20:02 UTC $
*****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "icp.h"

/*
 * Definitions for the ICP pin; for this example we use timer 1
 */
#define	ICP_PIN		PIND			/* ICP1 GPIO value	*/
#define	ICP_PORT	PORTD			/* ICP1 GPIO port	*/
#define	ICP_DDR		DDRD			/* ICP1 GPIO DDR	*/
#define	ICP_BIT		PD4				/* ICP1 GPIO pin	*/

/*
 * Definitions for ICP timer (1) setup.
 */
#define	ICP_OCR		OCR1A			/* ICP1 Output Compare register		*/
#define	ICP_OC_IE	OCIE1A			/* ICP1 timer Output Compare enable */
#define	ICP_OC_IF	OCF1A			/* ICP1 timer Output Compare flag	*/
#define	ICP_IE		TICIE1			/* ICP1 interrupt enable			*/
#define	ICP_IF		ICF1			/* ICP1 interrupt flag				*/
#define	ICP_CTL_A	TCCR1A			/* ICP1 timer control				*/
#define	ICP_CTL		TCCR1B			/* ICP1 interrupt control			*/
#define	ICP_SENSE	ICES1			/* ICP1 interrupt sense (rising/falling) */
//#define	ICP_PRESCALE ((0 << CS12) | (0 << CS11) | (1 << CS10))	/* prescale /1 */
#define	ICP_PRESCALE ((0 << CS12) | (1 << CS11) | (1 << CS10))	/* prescale /64 */
//#define	ICP_PRESCALE ((1 << CS12) | (0 << CS11) | (0 << CS10))	/* prescale /256 */

#define	ICP_START_SENSE	(1 << ICP_SENSE)	/* start with rising edge	*/

/**
 * icp_start_time, icp_stop_time, icp_period
 *
 * State variables for the Demodulator.
 *
 * start_time, stop_time and period all have the same type as the
 * respective TCNT register.
 */
typedef unsigned int icp_timer_t;			/* same as TCNT1		*/

icp_timer_t icp_start_time, icp_stop_time;
icp_timer_t icp_period;

/**
 * icp_rx_q[]
 *
 * Stores demodulated samples.
 *
 * The rx_q elements have the same type as the duty-cycle result.
 */
icp_sample_t icp_rx_q[ICP_RX_QSIZE];

/**
 * icp_rx_tail, icp_rx_head
 *
 * Queue state variables for icp_rx_q.
 *
 * The rx_head and rx_tail indices need to be wide enough to
 * accomodate [0:ICP_RX_QSIZE). Since QSIZE should generally
 * not be very large, these are hard-coded as single bytes,
 * which gets around certain atomicity concerns.
 */
unsigned char icp_rx_tail;		/* icp_rx_q insertion index */
#if	!ICP_ANALOG	/* ICP_DIGITAL */
unsigned char icp_rx_head;		/* icp_rx_q retrieval index */
#endif

#if	ICP_ANALOG
/**
 * icp_total
 *
 * Used in computing a moving average for ICP_ANALOG samples.
 *
 * icp_total needs to be wide enough to acommodate the sum of
 * RX_QSIZE icp_sample_t's.
 */
icp_total_t icp_total;
#endif	/* ICP_ANALOG */

static unsigned char icp_disabled = 0;
void disable_icp( unsigned char d )
{
    icp_disabled = d;
    if(icp_disabled)
        TIMSK	&= (~(1 << ICP_IE)) & (~(1 << ICP_OC_IE));
    else
        TIMSK	|= (1 << ICP_IE) | (1 << ICP_OC_IE);

}


/**
 *	icp_duty_compute()
 *
 *	This function computes the value of (ICP_SCALE*pulsewidth)/period.
 *	It is effectively a divide function, but uses a successive-approximation
 *	(moral equivalent of long division) method which:
 *	1) Doesn't require 32-bit arithmetic (the numerator is 24+ bits nominal).
 *	2) For an 8-bit result, only needs 8 loops (instead of 16 for general 16/16).
 *	3) Compiles nicely from C.
 *	We get away with this because we know that pulsewidth <= period, so
 *	no more than log2(ICP_SCALE) bits are relevant.
 */

__inline__
static icp_sample_t icp_duty_compute(icp_timer_t pulsewidth, icp_timer_t period)
{
	icp_sample_t r, mask;

	mask = ICP_SCALE >> 1;
	r = 0;
	do
	{
		period >>= 1;
		if (pulsewidth >= period)
		{
			r |= mask;
			pulsewidth -= period;
		}
		mask >>= 1;
	} while (pulsewidth != 0 && mask != 0);
	return(r);
}

/**
 * icp_enq()
 *
 * Stores a new sample into the Rx queue.
 */
__inline__
static void icp_enq(icp_sample_t sample)
{
	unsigned char t;

	t = icp_rx_tail;
#if	ICP_ANALOG
	icp_total += sample - icp_rx_q[t];
#endif
	icp_rx_q[t] = sample;
	if (++t >= ICP_RX_QSIZE)
		t = 0;
#if	!ICP_ANALOG
	if (t != icp_rx_head)		/* digital: Check for Rx overrun */
#endif
		icp_rx_tail = t;
	return;
}

/**
 * TIMER1_COMPA()
 *
 * ICP timer Output Compare ISR.
 *
 * The OC interrupt indicates that some edge didn't arrive as
 * expected. This happens when the duty cycle is either 0% (no
 * pulse at all) or 100% (pulse encompasses the entire period).
 */

SIGNAL(SIG_OUTPUT_COMPARE1A)
{
    if(icp_disabled) return; // we are turned off

    icp_sample_t sample;

    ICP_OCR += icp_period;		/* slide timeout window forward */
    sample = 0;					/* assume 0%	*/
    if ((ICP_CTL & (1 << ICP_SENSE)) != ICP_START_SENSE)
        sample = ICP_SCALE - 1;	/* 100%	*/

    icp_enq(sample);

    return;
}

/**
 * TIMER1_CAPT()
 *
 * ICP capture interrupt.
 */
SIGNAL(SIG_INPUT_CAPTURE1)
{
	icp_timer_t icr, delta;
	unsigned char tccr1b;

        if(icp_disabled) 
        {
            // Reset
            ICP_CTL	= ICP_START_SENSE | ICP_PRESCALE;
            return;
        }

	/*
	 * Capture the ICR and then reverse the sense of the capture.
	 * These must be done in this order, since as soon as the
	 * sense is reversed it is possible for ICR to be updated again.
	 */
	icr = ICR1;							/* capture timestamp	*/

	do
	{
		tccr1b = ICP_CTL;
		ICP_CTL = tccr1b ^ (1 << ICP_SENSE);		/* reverse sense		*/
	
		/*
		 * If we were waiting for a start edge, then this is the
		 * end/beginning of a period.
		 */
		if ((tccr1b & (1 << ICP_SENSE)) == ICP_START_SENSE)
		{
			/*
			 * Beginning of pulse: Compute length of preceding period,
			 * and thence the duty cycle of the preceding pulse.
			 */
			icp_period = icr - icp_start_time;	/* Length of previous period */
			delta = icp_stop_time - icp_start_time; /* Length of previous pulse */
			icp_start_time = icr;				/* Start of new pulse/period */

			/*
			 * Update the timeout based on the new period. (The new period
			 * is probably the same as the old, give or take clock drift.)
			 * We add 1 to make fairly sure that, in case of competition,
			 * the PWM edge takes precedence over the timeout.
			 */
			ICP_OCR = icr + icp_period + 1;		/* Move timeout window		*/
			TIFR = (1 << ICP_OC_IF);			/* Clear in case of race	*/

			/*
			 * Compute the duty cycle, and store the new reading.
			 */
			icp_enq(icp_duty_compute(delta,icp_period));
	
			/*
			 * Check for a race condition where a (very) short pulse
			 * ended before we could reverse the sense above.
			 * If the ICP pin is still high (as expected) OR the IF is
			 * set (the falling edge has happened, but we caught it),
			 * then we won the race, so we're done for now.
			 */
			if ((ICP_PIN & (1 << ICP_BIT)) || (TIFR & (1 << ICP_IF)))
				break;
		}
		else
		{
			/*
			 * Falling edge detected, so this is the end of the pulse.
			 * The time is simply recorded here; the final computation
			 * will take place at the beginning of the next pulse.
			 */
			icp_stop_time = icr;		/* Capture falling-edge time */

			/*
			 * If the ICP pin is still low (as expected) OR the IF is
			 * set (the transition was caught anyway) we won the race,
			 * so we're done for now.
			 */
			if ((!(ICP_PIN & (1 << ICP_BIT))) || (TIFR & (1 << ICP_IF)))
				break;
		}
		/*
		 * If we got here, we lost the race with a very short/long pulse.
		 * We now loop, pretending (as it were) that we caught the transition.
		 * The Same ICR value is used, so the effect is that we declare
		 * the duty cycle to be 0% or 100% as appropriate.
		 */
	} while (1);

	return;
}

// Return frequency

unsigned int
icp_get_freq(void)
{
    //return 0xFFFF/icp_period;
    return 0x8000/icp_period;
}


/**
 * icp_get_duty()
 *
 * Fetch a sample from the queue. For analog mode, this is a moving
 * average of the last QSIZE readings. For digital, it is the oldest
 * reading.
 */


unsigned int
icp_get_duty(void)
{
	icp_sample_t r;

#if	ICP_ANALOG
	r = icp_total / ICP_RX_QSIZE;		/* moving average of last QSIZE samples */
#else	/* ICP_DIGITAL */
	unsigned char h;

	h = icp_rx_head;
	if (h == icp_rx_tail)				/* if head == tail, queue is empty */
		r = (icp_sample_t)-1;
	else
	{
		r = icp_rx_q[h];				/* fetch next entry				*/
		if (++h >= ICP_RX_QSIZE)		/* increment head, modulo QSIZE	*/
			h = 0;
		icp_rx_head = h;
	}
#endif	/* ICP_DIGITAL */

	return(r);
}

/**
 * icp_init()
 *
 * Set up the ICP timer.
 */
void
icp_init(void)
{
	/*
	 * Nothing interesting to set in TCCR1A
	 */
	ICP_CTL_A = 0;

	/*
	 * Setting the OCR (timeout) to 0 allows the full TCNT range for
	 * the initial period.
	 */
	ICP_OCR	= 0;

	/*
	 * Set the interrupt sense and the prescaler
	 */
	ICP_CTL	= ICP_START_SENSE | ICP_PRESCALE;

	/*
	 *	Enable both the Input Capture and the Output Capture interrupts.
	 *	The latter is used for timeout (0% and 100%) checking.
	 */
	TIMSK	|= (1 << ICP_IE) | (1 << ICP_OC_IE);

	return;
}
