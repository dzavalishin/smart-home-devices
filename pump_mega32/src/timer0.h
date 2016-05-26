#include "defs.h"

void timer0_init(void);

#if HALF_DUPLEX
// Called by data reception code if non-echo reception is ongoing
void mark_halfduplex_timeout();
// called by sender to wait for others to stop sending
void wait_for_halfduplex_timeout();
#endif
void pump_every_second(void);


// Turn on frequency output on OC0, or turn off if 0
//void timer0_freq( int freq );

